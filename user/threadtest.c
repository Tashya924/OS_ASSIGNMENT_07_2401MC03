#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NTHREADS 3
#define ITERATIONS 100000

volatile int counter = 0;

void
worker(void *arg)
{
  int id = *(int *)arg;
  printf("[Thread %d] Started (PID: %d). Incrementing counter %d times...\n", id, getpid(), ITERATIONS);

  for (int i = 0; i < ITERATIONS; i++) {
    counter++;
  }

  printf("[Thread %d] Completed.\n", id);
  exit(0);
}

int
main(int argc, char *argv[])
{
  printf("====================================================\n");
  printf("  OS Lab Assignment 7 - Question 1: User-Level Threads\n");
  printf("====================================================\n");
  printf("Spawning %d threads sharing address space via clone()...\n", NTHREADS);
  printf("Each thread performs %d increments without synchronization.\n", ITERATIONS);

  int ids[NTHREADS];
  void *stacks[NTHREADS];

  counter = 0;

  for (int i = 0; i < NTHREADS; i++) {
    ids[i] = i + 1;
    stacks[i] = malloc(4096);
    if (stacks[i] == 0) {
      printf("Error: malloc failed for thread stack\n");
      exit(1);
    }
    int tid = clone(worker, &ids[i], (char *)stacks[i] + 4096);
    if (tid < 0) {
      printf("Error: clone failed for thread %d\n", i + 1);
      exit(1);
    }
    printf("Created thread %d with PID %d\n", i + 1, tid);
  }

  for (int i = 0; i < NTHREADS; i++) {
    int joined_pid = join();
    printf("Reaped/Joined thread child PID: %d\n", joined_pid);
  }

  printf("\n--- Question 1 Verification ---\n");
  printf("Expected Counter Value (Theoretical): %d\n", NTHREADS * ITERATIONS);
  printf("Actual Final Counter Value:           %d\n", counter);

  if (counter < NTHREADS * ITERATIONS) {
    int lost = (NTHREADS * ITERATIONS) - counter;
    printf("RACE CONDITION CONFIRMED: Lost %d updates due to concurrent execution!\n", lost);
  } else {
    printf("All increments accounted for (%d).\n", counter);
  }
  printf("====================================================\n");

  exit(0);
}
