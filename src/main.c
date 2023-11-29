#include <stdio.h>
#include <zephyr.h>
#include <arch/cpu.h>
#include <sys/printk.h>
#include "thread.h"


#define STACKSIZE 2000
#define SLEEPTIME 5000

struct k_thread coop_thread;
K_THREAD_STACK_DEFINE(coop_stack, STACKSIZE);

struct k_sem semaphore;
int counter;


void thread_entry(void)
{
	struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);

	while (1) {
        update_counter(&counter,&semaphore,"thread",K_FOREVER);
        k_timer_start(&timer, K_MSEC(SLEEPTIME), K_NO_WAIT);
		k_timer_status_sync(&timer);
        
        
	}
}

int main(void)
{
    counter = 0;
    k_sem_init(&semaphore, 1, 1);
    k_thread_create(&coop_thread,
                    coop_stack,
                    STACKSIZE,
                    (k_thread_entry_t) thread_entry,
                    NULL,
                    NULL,
                    NULL,
                    K_PRIO_COOP(7),
                    0,
                    K_NO_WAIT);

	struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);

	while (1) {
        update_counter(&counter,&semaphore,"main",K_FOREVER);
        k_timer_start(&timer, K_MSEC(SLEEPTIME), K_NO_WAIT);
		k_timer_status_sync(&timer);
        
	}

	return 0;
}
