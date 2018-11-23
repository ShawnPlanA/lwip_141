

#include "lwip/debug.h"
#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"

#include <stdlib.h>
#include <string.h>

static int num_sem = 0;                 // Number of semaphores created
static int num_mbox = 0;                // Number of mailboxes created
static int num_thread = 0;              // Number of threads created
static int num_hisr = 0;                // Number of hisrs created
//static struct sys_thread *threads = NULL;
//static struct sys_hisr *hisrs = NULL;

#define TICKS_PER_SECOND 10000
#define MS_TO_TICKS(MS) (MS * (TICKS_PER_SECOND / 1000))
#define TICKS_TO_MS(TICKS) ((unsigned long)((1000ULL * TICKS) / TICKS_PER_SECOND))
#define TICKS_TO_HUNDMICROSEC(TICKS) TICKS

#define SYS_MBOX_SIZE 128               // Number of elements in mbox queue
#define SYS_STACK_SIZE 2048             // A minimum Nucleus stack for coldfire
#define SYS_HISR_STACK_SIZE 2048             // A minimum Nucleus stack for coldfire

/*---------------------------------------------------------------------------------*/
void sys_init(void)
{
	return;
}

//  lib_mips/time.c
u32_t sys_now(void)
{
	return get_timer(0) / 1000;
}
