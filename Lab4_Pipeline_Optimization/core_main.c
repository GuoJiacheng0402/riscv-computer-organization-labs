#include "coremark.h"
#include <stdint.h>
#include <stdio.h>

/* ========================================================================== */
/* [MODIFIED] RISC-V Hardware Counter Access Functions                        */
/* ========================================================================== */

/**
 * @brief Reads the Machine Cycle Counter (mcycle)
 *
 * This function retrieves the number of clock cycles executed by the processor.
 * It handles both 32-bit and 64-bit RISC-V architectures.
 *
 * @return uint64_t Current cycle count
 */
static inline uint64_t get_mcycles(void) {
#if __riscv_xlen == 32
    volatile uint32_t hi, lo, hi2;
    /*
     * For 32-bit architecture, the 64-bit counter is split into two 32-bit CSRs.
     * We must read high -> low -> high to ensure no rollover occurred during the read.
     * If hi != hi2, a rollover happened, so we repeat the read.
     */
    do {
        asm volatile ("csrr %0, mcycleh" : "=r"(hi));
        asm volatile ("csrr %0, mcycle"  : "=r"(lo));
        asm volatile ("csrr %0, mcycleh" : "=r"(hi2));
    } while (hi != hi2);
    return ((uint64_t)hi << 32) | lo;
#else
    /* For 64-bit architecture, we can read the full 64-bit register directly. */
    uint64_t val;
    asm volatile ("csrr %0, mcycle" : "=r"(val));
    return val;
#endif
}

/**
 * @brief Reads the Machine Instruction Retired Counter (minstret)
 *
 * This function retrieves the number of instructions retired (completed) by the processor.
 *
 * @return uint64_t Current retired instruction count
 */
static inline uint64_t get_minstret(void) {
#if __riscv_xlen == 32
    volatile uint32_t hi, lo, hi2;
    /* Same double-read protection logic as mcycle for 32-bit systems */
    do {
        asm volatile ("csrr %0, minstreth" : "=r"(hi));
        asm volatile ("csrr %0, minstret"  : "=r"(lo));
        asm volatile ("csrr %0, minstreth" : "=r"(hi2));
    } while (hi != hi2);
    return ((uint64_t)hi << 32) | lo;
#else
    uint64_t val;
    asm volatile ("csrr %0, minstret" : "=r"(val));
    return val;
#endif
}
/* ========================================================================== */


/* Function: iterate
Run the benchmark for a specified number of iterations.
*/
static ee_u16 list_known_crc[]   =      {(ee_u16)0xd4b0,(ee_u16)0x3340,(ee_u16)0x6a79,(ee_u16)0xe714,(ee_u16)0xe3c1};
static ee_u16 matrix_known_crc[] =      {(ee_u16)0xbe52,(ee_u16)0x1199,(ee_u16)0x5608,(ee_u16)0x1fd7,(ee_u16)0x0747};
static ee_u16 state_known_crc[]  =      {(ee_u16)0x5e47,(ee_u16)0x39bf,(ee_u16)0xe5a4,(ee_u16)0x8e3a,(ee_u16)0x8d84};

void *iterate(void *pres) {
    ee_u32 i;
    ee_u16 crc;
    core_results *res=(core_results *)pres;
    ee_u32 iterations=res->iterations;
    res->crc=0;
    res->crclist=0;
    res->crcmatrix=0;
    res->crcstate=0;

    for (i=0; i<iterations; i++) {
        crc=core_bench_list(res,1);
        res->crc=crcu16(crc,res->crc);
        crc=core_bench_list(res,-1);
        res->crc=crcu16(crc,res->crc);
        if (i==0) res->crclist=res->crc;
    }
    return NULL;
}

#if (SEED_METHOD==SEED_ARG)
ee_s32 get_seed_args(int i, int argc, char *argv[]);
#define get_seed(x) (ee_s16)get_seed_args(x,argc,argv)
#define get_seed_32(x) get_seed_args(x,argc,argv)
#else /* via function or volatile */
ee_s32 get_seed_32(int i);
#define get_seed(x) (ee_s16)get_seed_32(x)
#endif

#if (MEM_METHOD==MEM_STATIC)
ee_u8 static_memblk[TOTAL_DATA_SIZE];
#endif
char *mem_name[3] = {"Static","Heap","Stack"};

/* ========================================================================== */
/* Main Application Entry                                                     */
/* ========================================================================== */
#if MAIN_HAS_NOARGC
MAIN_RETURN_TYPE main(void) {
    int argc=0;
    char *argv[1];
#else
MAIN_RETURN_TYPE main(int argc, char *argv[]) {
#endif

    ee_u16 i,j=0,num_algorithms=0;
    ee_s16 known_id=-1,total_errors=0;
    ee_u16 seedcrc=0;
    CORE_TICKS total_time;
    core_results results[MULTITHREAD];
#if (MEM_METHOD==MEM_STACK)
    ee_u8 stack_memblock[TOTAL_DATA_SIZE*MULTITHREAD];
#endif

    /* 1. Initialization Phase */
    portable_init(&(results[0].port), &argc, argv);
    
    /* Sanity check for data structures */
    if (sizeof(struct list_head_s)>128) {
        ee_printf("list_head structure too big for comparable data!\r\n");
        return MAIN_RETURN_VAL;
    }

    /* 2. Setup Benchmark Parameters (Seeds & Iterations) */
    results[0].seed1=get_seed(1);
    results[0].seed2=get_seed(2);
    results[0].seed3=get_seed(3);
    results[0].iterations=get_seed_32(4);

#if CORE_DEBUG
    results[0].iterations=1;
#endif

    // [MODIFICATION] Adjust iterations based on simulation vs real hardware
#ifdef CFG_SIMULATION
    results[0].iterations=2; // Short run for simulation speed
#else
    results[0].iterations=ITERATIONS; // Full run for real hardware
#endif

    ee_printf ("Start to run coremark for %d iterations\r\n", results[0].iterations);

    results[0].execs=get_seed_32(5);
    if (results[0].execs==0) { /* if not supplied, execute all algorithms */
        results[0].execs=ALL_ALGORITHMS_MASK;
    }

    /* Set default seeds if none provided */
    if ((results[0].seed1==0) && (results[0].seed2==0) && (results[0].seed3==0)) { /* validation run */
        results[0].seed1=0;
        results[0].seed2=0;
        results[0].seed3=0x66;
    }
    if ((results[0].seed1==1) && (results[0].seed2==0) && (results[0].seed3==0)) { /* performance run */
        results[0].seed1=0x3415;
        results[0].seed2=0x3415;
        results[0].seed3=0x66;
    }

    /* 3. Memory Allocation for Benchmark Data */
#if (MEM_METHOD==MEM_STATIC)
    results[0].memblock[0]=(void *)static_memblk;
    results[0].size=TOTAL_DATA_SIZE;
    results[0].err=0;
#if (MULTITHREAD>1)
#error "Cannot use a static data area with multiple contexts!"
#endif
#elif (MEM_METHOD==MEM_MALLOC)
    for (i=0 ; i<MULTITHREAD; i++) {
        ee_s32 malloc_override=get_seed(7);
        if (malloc_override != 0)
            results[i].size=malloc_override;
        else
            results[i].size=TOTAL_DATA_SIZE;
        results[i].memblock[0]=portable_malloc(results[i].size);
        results[i].seed1=results[0].seed1;
        results[i].seed2=results[0].seed2;
        results[i].seed3=results[0].seed3;
        results[i].err=0;
        results[i].execs=results[0].execs;
    }
#elif (MEM_METHOD==MEM_STACK)
    for (i=0 ; i<MULTITHREAD; i++) {
        results[i].memblock[0]=stack_memblock+i*TOTAL_DATA_SIZE;
        results[i].size=TOTAL_DATA_SIZE;
        results[i].seed1=results[0].seed1;
        results[i].seed2=results[0].seed2;
        results[i].seed3=results[0].seed3;
        results[i].err=0;
        results[i].execs=results[0].execs;
    }
#else
#error "Please define a way to initialize a memory block."
#endif

    /* 4. Data Initialization */
    /* Distribute memory among algorithms */
    for (i=0; i<NUM_ALGORITHMS; i++) {
        if ((1<<(ee_u32)i) & results[0].execs)
            num_algorithms++;
    }
    for (i=0 ; i<MULTITHREAD; i++)
        results[i].size=results[i].size/num_algorithms;

    /* Assign pointers */
    for (i=0; i<NUM_ALGORITHMS; i++) {
        ee_u32 ctx;
        if ((1<<(ee_u32)i) & results[0].execs) {
            for (ctx=0 ; ctx<MULTITHREAD; ctx++)
                results[ctx].memblock[i+1]=(char *)(results[ctx].memblock[0])+results[0].size*j;
            j++;
        }
    }

    /* Initialize data structures (Linked List, Matrix, State Machine) */
    for (i=0 ; i<MULTITHREAD; i++) {
        if (results[i].execs & ID_LIST) {
            results[i].list=core_list_init(results[0].size,results[i].memblock[1],results[i].seed1);
        }
        if (results[i].execs & ID_MATRIX) {
            core_init_matrix(results[0].size, results[i].memblock[2], (ee_s32)results[i].seed1 | (((ee_s32)results[i].seed2) << 16), &(results[i].mat) );
        }
        if (results[i].execs & ID_STATE) {
            core_init_state(results[0].size,results[i].seed1,results[i].memblock[3]);
        }
    }

    /* Automatic time adjustment if iterations not set */
    if (results[0].iterations==0) {
        secs_ret secs_passed=0;
        ee_u32 divisor;
        results[0].iterations=1;
        while (secs_passed < (secs_ret)1) {
            results[0].iterations*=10;
            start_time();
            iterate(&results[0]);
            stop_time();
            secs_passed=time_in_secs(get_time());
        }
        divisor=(ee_u32)secs_passed;
        if (divisor==0)
            divisor=1;
        results[0].iterations*=1+10/divisor;
    }

    /* 5. Perform Actual Benchmark */

    /* [MODIFIED] Capture Start Snapshot of Hardware Counters (Cycles & Instructions) */
    uint64_t my_start_cyc  = get_mcycles();
    uint64_t my_start_inst = get_minstret();

    start_time();
    
#if (MULTITHREAD>1)
    if (default_num_contexts>MULTITHREAD) {
        default_num_contexts=MULTITHREAD;
    }
    for (i=0 ; i<default_num_contexts; i++) {
        results[i].iterations=results[0].iterations;
        results[i].execs=results[0].execs;
        core_start_parallel(&results[i]);
    }
    for (i=0 ; i<default_num_contexts; i++) {
        core_stop_parallel(&results[i]);
    }
#else
    iterate(&results[0]);
#endif
    
    stop_time();

    /* [MODIFIED] Capture End Snapshot of Hardware Counters */
    uint64_t my_end_cyc    = get_mcycles();
    uint64_t my_end_inst   = get_minstret();

    total_time=get_time();

    /* 6. Verify Results */
    seedcrc=crc16(results[0].seed1,seedcrc);
    seedcrc=crc16(results[0].seed2,seedcrc);
    seedcrc=crc16(results[0].seed3,seedcrc);
    seedcrc=crc16(results[0].size,seedcrc);

    switch (seedcrc) { /* test known output for common seeds */
        case 0x8a02: known_id=0; ee_printf("6k performance run parameters for coremark.\n"); break;
        case 0x7b05: known_id=1; ee_printf("6k validation run parameters for coremark.\n"); break;
        case 0x4eaf: known_id=2; ee_printf("Profile generation run parameters for coremark.\n"); break;
        case 0xe9f5: known_id=3; ee_printf("2K performance run parameters for coremark.\n"); break;
        case 0x18f2: known_id=4; ee_printf("2K validation run parameters for coremark.\n"); break;
        default: total_errors=-1; break;
    }

    if (known_id>=0) {
        for (i=0 ; i<default_num_contexts; i++) {
            results[i].err=0;
            if ((results[i].execs & ID_LIST) &&
                (results[i].crclist!=list_known_crc[known_id])) {
                ee_printf("[%u]ERROR! list crc 0x%04x - should be 0x%04x\n",i,results[i].crclist,list_known_crc[known_id]);
                results[i].err++;
            }
            if ((results[i].execs & ID_MATRIX) &&
                (results[i].crcmatrix!=matrix_known_crc[known_id])) {
                ee_printf("[%u]ERROR! matrix crc 0x%04x - should be 0x%04x\n",i,results[i].crcmatrix,matrix_known_crc[known_id]);
                results[i].err++;
            }
            if ((results[i].execs & ID_STATE) &&
                (results[i].crcstate!=state_known_crc[known_id])) {
                ee_printf("[%u]ERROR! state crc 0x%04x - should be 0x%04x\n",i,results[i].crcstate,state_known_crc[known_id]);
                results[i].err++;
            }
            total_errors+=results[i].err;
        }
    }
    total_errors+=check_data_types();

    /* 7. Report Standard CoreMark Results */
    ee_printf("CoreMark Size      : %lu\n",(ee_u32)results[0].size);
    ee_printf("Total ticks        : %lu\n",(ee_u32)total_time);
#if HAS_FLOAT
    ee_printf("Total time (secs): %f\n",time_in_secs(total_time));
    if (time_in_secs(total_time) > 0)
        ee_printf("Iterations/Sec   : %f\n",default_num_contexts*results[0].iterations/time_in_secs(total_time));
#else
    ee_printf("Total time (secs): %d\n",time_in_secs(total_time));
    if (time_in_secs(total_time) > 0)
        ee_printf("Iterations/Sec   : %d\n",default_num_contexts*results[0].iterations/time_in_secs(total_time));
#endif

#ifdef CFG_SIMULATION
    // Bob: for simulation we just comment this out to avoid error on short runs
#else
    if (time_in_secs(total_time) < 10) {
        ee_printf("ERROR! Must execute for at least 10 secs for a valid result!\n");
        total_errors++;
    }
#endif

    ee_printf("Iterations       : %lu\n",(ee_u32)default_num_contexts*results[0].iterations);
    ee_printf("Compiler version : %s\n",COMPILER_VERSION);
    ee_printf("Compiler flags   : %s\n",COMPILER_FLAGS);
#if (MULTITHREAD>1)
    ee_printf("Parallel %s : %d\n",PARALLEL_METHOD,default_num_contexts);
#endif
    ee_printf("Memory location  : %s\n",MEM_LOCATION);
    
    /* Output CRCs for verification */
    ee_printf("seedcrc          : 0x%04x\n",seedcrc);
    if (results[0].execs & ID_LIST)
        for (i=0 ; i<default_num_contexts; i++)
            ee_printf("[%d]crclist      : 0x%04x\n",i,results[i].crclist);
    if (results[0].execs & ID_MATRIX)
        for (i=0 ; i<default_num_contexts; i++)
            ee_printf("[%d]crcmatrix    : 0x%04x\n",i,results[i].crcmatrix);
    if (results[0].execs & ID_STATE)
        for (i=0 ; i<default_num_contexts; i++)
            ee_printf("[%d]crcstate     : 0x%04x\n",i,results[i].crcstate);
    for (i=0 ; i<default_num_contexts; i++)
        ee_printf("[%d]crcfinal     : 0x%04x\n",i,results[i].crc);

    if (total_errors==0) {
        ee_printf("Correct operation validated. See readme.txt for run and reporting rules.\n");
#if HAS_FLOAT
        if (known_id==3) {
            ee_printf("CoreMark 1.0 : %f / %s %s",default_num_contexts*results[0].iterations/time_in_secs(total_time),COMPILER_VERSION,COMPILER_FLAGS);
#if defined(MEM_LOCATION) && !defined(MEM_LOCATION_UNSPEC)
            ee_printf(" / %s",MEM_LOCATION);
#else
            ee_printf(" / %s",mem_name[MEM_METHOD]);
#endif
#if (MULTITHREAD>1)
            ee_printf(" / %d:%s",default_num_contexts,PARALLEL_METHOD);
#endif
            ee_printf("\n");
        }
#endif
    }
    if (total_errors>0)
        ee_printf("Errors detected\n");
    if (total_errors<0)
        ee_printf("Cannot validate operation for these seed values, please compare with results on a known platform.\n");

#if (MEM_METHOD==MEM_MALLOC)
    for (i=0 ; i<MULTITHREAD; i++)
        portable_free(results[i].memblock[0]);
#endif

    /* Cleanup platform specific resources */
    portable_fini(&(results[0].port));

    /* 8. Additional Performance Metrics (Custom) */
    
    // Calculate DMIPS based on normalized 1MHz estimation
    float coremark_dmips = ((uint64_t)results[0].iterations*1000000)/(float)total_time;

#if HAS_FLOAT
    ee_printf ("\n");
    ee_printf ("\n");
    ee_printf ("Print Personal Added Additional Info to Easy Visual Analysis\n");
    ee_printf ("\n");
    ee_printf ("     (Iterations is: %u\n", results[0].iterations);
    ee_printf ("     (total_ticks is: %u\n", (ee_u32)total_time);
    ee_printf (" (*) Assume the core running at 1 MHz\n");
    ee_printf ("     So the CoreMark/MHz can be calculated by: \n");
    ee_printf ("     (Iterations*1000000/total_ticks) = %2.6f CoreMark/MHz\n", coremark_dmips);
    ee_printf ("\n");
#endif

    /* [MODIFIED] Calculate and Print IPC and CPI */
    uint64_t my_total_cyc  = my_end_cyc - my_start_cyc;
    uint64_t my_total_inst = my_end_inst - my_start_inst;

    ee_printf ("\n[Experiment Result] --------------------------\n");
    // Cast to 32-bit for printing. Safe for short runs, be careful with long runs overflowing 32-bit.
    ee_printf ("Total Cycles       : %lu\n", (ee_u32)my_total_cyc);
    ee_printf ("Total Instructions : %lu\n", (ee_u32)my_total_inst);

    if (my_total_cyc > 0 && my_total_inst > 0) {
        // IPC = Instructions / Cycles
        ee_printf ("IPC (Inst/Cycle)   : %f\n", (double)my_total_inst / my_total_cyc);
        // CPI = Cycles / Instructions
        ee_printf ("CPI (Cycle/Inst)   : %f\n", (double)my_total_cyc / my_total_inst);
    } else {
        ee_printf ("Error: Counters read 0. Please check privilege mode or HW support.\n");
    }
    ee_printf ("----------------------------------------------\n");

    return MAIN_RETURN_VAL;
}
