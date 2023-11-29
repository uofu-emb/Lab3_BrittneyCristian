#include "thread.h"
#include <stdio.h>

K_SEM_DEFINE(semaphore1, 0, 1);  // Initialize semaphore1 with a count of 0 and a maximum count of 1
K_SEM_DEFINE(semaphore2, 0, 1);  // Initialize semaphore2 with a count of 0

int update_counter(int *counter_, struct k_sem *semaphore_,char * thread, k_timeout_t timeout){
      printf("counter value: %d",*counter_);

     int semaphore_return_value = k_sem_take(semaphore_, timeout);
     if(semaphore_return_value ==  -EAGAIN) return semaphore_return_value;
     printf("counter value: %d",semaphore_return_value);
     (*counter_) = (*counter_) + 1;
	   printk("hello world from %s! Count %d\n", thread, *counter_);
	   k_sem_give(semaphore_);
     return semaphore_return_value;
}

void thread1(void *semaphore1_, void *semaphore2_, void *p3) {
    
        // Wait for semaphore1
        k_sem_take(semaphore1_, K_FOREVER);

        // Sleep for a while to make sure thread2 has a chance to run
        k_sleep(K_MSEC(100));

        // Try to acquire semaphore2
        k_sem_take(semaphore2_, K_NO_WAIT);

        // Critical section
        printf("Thread 1: In critical section\n");

        // Release semaphore2
        k_sem_give(semaphore2_);

        // Release semaphore1
        k_sem_give(semaphore1_);

        k_sleep(K_SECONDS(1));
    
}

void thread2(void *semaphore1_, void *semaphore2_, void *p3) {
    
        // Wait for semaphore1
        k_sem_take(semaphore2_, K_FOREVER);

        // Sleep for a while to make sure thread2 has a chance to run
        k_sleep(K_MSEC(100));

        // Try to acquire semaphore2
        k_sem_take(semaphore1_, K_NO_WAIT);

        // Critical section
        printf("Thread 2: In critical section\n");

        // Release semaphore2
        k_sem_give(semaphore1_);

        // Release semaphore1
        k_sem_give(semaphore2_);

        k_sleep(K_SECONDS(1));
    
}

int orphaned_lock(struct k_sem *semaphore, k_timeout_t timeout, int *counter)
{
    if (k_sem_take(semaphore, timeout)) return 1;
    (*counter)++;
    if (*counter % 2) {
        return 0;
    }
    printk("Count %d\n", *counter);
    k_sem_give(semaphore);
    return 0;
}

int unorphaned_lock(struct k_sem *semaphore, k_timeout_t timeout, int *counter)
{
    if (k_sem_take(semaphore, timeout))
        return 1;
    {
        (*counter)++;
        if (!(*counter % 2)) {
            printk("Count %d\n", *counter);
        }
    }
    k_sem_give(semaphore);
    return 0;
}

void deadlock(struct k_sem *a, struct k_sem *b, int *counter)
{
    int id = *counter;
    printk("\tinside deadlock %d\n", id);
    (*counter)++;
    struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);
    k_sem_take(a, K_FOREVER);
    {
        (*counter)++;
        printk("\tinside first lock %d\n", id);
        k_yield();
        printk("\tpost-yield %d\n", id);
        k_timer_start(&timer, K_MSEC(100), K_NO_WAIT);
        k_timer_status_sync(&timer);
        k_sem_take(b, K_FOREVER);
        {
            printk("\tinside second lock %d\n", id);
            (*counter)++;
        }
        k_sem_give(b);
    }
    k_sem_give(a);
}