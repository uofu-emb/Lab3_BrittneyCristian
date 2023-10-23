#pragma once

#include <zephyr.h>
#include <kernel.h>
#include <arch/cpu.h>
#include <sys/printk.h>

int update_counter(int *counter_, struct k_sem *semaphore_,char * thread,k_timeout_t timeout);
void thread1(void *p1, void *p2, void *p3);
void thread2(void *p1, void *p2, void *p3);