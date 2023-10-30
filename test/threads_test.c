#include "unity.h"
#include <stdio.h>
#include <zephyr.h>
#include <arch/cpu.h>
#include <sys/printk.h>
#include "thread.h"

#define STACKSIZE 2000
#define SLEEPTIME 5000
K_THREAD_STACK_DEFINE(coop_stack, STACKSIZE);
struct k_thread coop_thread;
struct k_sem semaphore_a;
struct k_sem semaphore_b;




void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_semaphore_unlocked(void) {
  
struct k_sem semaphore;
int counter;
counter = 0;
k_sem_init(&semaphore, 1, 1);

int semaphore_value = update_counter(&counter, &semaphore,"test semaphore", K_NO_WAIT);
TEST_ASSERT_EQUAL_INT(0, semaphore_value);
TEST_ASSERT_EQUAL_INT(1, counter);

}

void test_semaphore_locked(void) {
  
struct k_sem semaphore;
int counter;
counter = 0;
k_sem_init(&semaphore, 1, 1);
k_sem_take(&semaphore, K_FOREVER);
int semaphore_value = update_counter(&counter, &semaphore,"test semaphore", K_MSEC(2));
TEST_ASSERT_EQUAL_INT(-EAGAIN, semaphore_value);
TEST_ASSERT_EQUAL_INT(0, counter);

}

void test_semaphore_deadlocked(void) {
    k_tid_t tid1, tid2;
    k_sem_init(&semaphore_a, 1, 1);
    k_sem_init(&semaphore_b, 1, 1);


    tid1 =  k_thread_create(&coop_thread,
                    coop_stack,
                    STACKSIZE,
                    (k_thread_entry_t) thread1,
                    &semaphore_a,
                    &semaphore_b,
                    NULL,
                    K_PRIO_COOP(7),
                    0,
                    K_NO_WAIT);
    tid2 =  k_thread_create(&coop_thread,
                    coop_stack,
                    STACKSIZE,
                    (k_thread_entry_t) thread2,
                    &semaphore_a,
                    &semaphore_b,
                    NULL,
                    K_PRIO_COOP(7),
                    0,
                    K_NO_WAIT);

    int left_done = k_thread_join(tid1, K_MSEC(1000));
    printf("- Waited left %d\n", left_done);
    int right_done = k_thread_join(tid2, K_MSEC(1000));
    printf("- Waited right %d\n", right_done);
    TEST_ASSERT_EQUAL_INT(-EAGAIN, left_done);
    TEST_ASSERT_EQUAL_INT(-EAGAIN, right_done);
    printf("- Killing threads\n");
    k_thread_abort(tid1);
    k_thread_abort(tid2);
    printf("- Killed threads\n");

  
}

void test_orphaned(void)
{
    int counter = 1;
    struct k_sem semaphore;
    k_sem_init(&semaphore, 1, 1);

    orphaned_lock(&semaphore, K_MSEC(500),&counter);
    TEST_ASSERT_EQUAL_INT(2, counter);
    // Note that reading and using the value of the semaphore
    // isn't usually a good idea in real situations.
    TEST_ASSERT_EQUAL_INT(1, k_sem_count_get(&semaphore));

    orphaned_lock(&semaphore, K_MSEC(500), &counter);
    TEST_ASSERT_EQUAL_INT(3, counter);
    TEST_ASSERT_EQUAL_INT(0, k_sem_count_get(&semaphore));

    orphaned_lock(&semaphore, K_MSEC(500), &counter);
    TEST_ASSERT_EQUAL_INT(3, counter);
    TEST_ASSERT_EQUAL_INT(0, k_sem_count_get(&semaphore));
}

void test_unorphaned(void)
{
    int counter = 1;
    struct k_sem semaphore;
    k_sem_init(&semaphore, 1, 1);

    int result;
    result = unorphaned_lock(&semaphore, K_MSEC(500), &counter);
    TEST_ASSERT_EQUAL_INT(2, counter);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(1, k_sem_count_get(&semaphore));

    result = unorphaned_lock(&semaphore, K_MSEC(500), &counter);
    TEST_ASSERT_EQUAL_INT(3, counter);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(1, k_sem_count_get(&semaphore));
}


int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_semaphore_unlocked);
  RUN_TEST(test_semaphore_deadlocked);
  RUN_TEST(test_semaphore_locked);
  RUN_TEST(test_orphaned);
  RUN_TEST(test_unorphaned);


  return UNITY_END();
}

// WARNING!!! PLEASE REMOVE UNNECESSARY MAIN IMPLEMENTATIONS //

/**
  * For native dev-platform or for some embedded frameworks
  */
int main(void) {
  return runUnityTests();
}