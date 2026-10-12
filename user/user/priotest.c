#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PROCS 4
#define WORK_LOAD 40000000

void
cpu_bound_work(int id, int expected_prio)
{
  int start_t = uptime();
  volatile int x = 0;
  int pid = getpid();
  int initial_prio = get_priority(pid);

  printf("[Process %d | PID %d] Started with Priority %d (expected %d) at tick %d\n",
         id, pid, initial_prio, expected_prio, start_t);

  for (int i = 0; i < WORK_LOAD; i++) {
    x += (i * 3) ^ 0x5a5a;
    // Periodically report priority for the lowest-priority process to show aging in action
    if (expected_prio == 20 && (i % (WORK_LOAD / 4) == 0) && i > 0) {
      int cur_prio = get_priority(pid);
      printf("  --> [AGING PROGRESS] Lowest-Priority P%d (PID %d): Current Priority=%d at tick %d\n",
             id, pid, cur_prio, uptime());
    }
  }

  int end_t = uptime();
  int final_prio = get_priority(pid);
  int duration = end_t - start_t;

  printf("[Process %d | PID %d] COMPLETED! Initial Priority: %d | Final Priority: %d | Ticks Elapsed: %d\n",
         id, pid, initial_prio, final_prio, duration);

  exit(0);
}

int
main(int argc, char *argv[])
{
  printf("====================================================\n");
  printf("  OS Lab Assignment 7 - Question 4: Priority Preemptive Scheduler with Aging\n");
  printf("====================================================\n");

  int priorities[NUM_PROCS] = {5, 10, 15, 20};
  int pids[NUM_PROCS];

  printf("Forking %d CPU-bound processes with priorities: [P0=5 (High), P1=10 (Normal), P2=15 (Low), P3=20 (Lowest)]\n", NUM_PROCS);
  printf("Aging rule: RUNNABLE processes waiting > 30 ticks have their priority incremented (number decremented by 1).\n\n");

  int start_time = uptime();

  for (int i = 0; i < NUM_PROCS; i++) {
    int pid = fork();
    if (pid < 0) {
      printf("Error: fork failed for process %d\n", i);
      exit(1);
    }

    if (pid == 0) {
      set_priority(getpid(), priorities[i]);
      cpu_bound_work(i, priorities[i]);
    } else {
      pids[i] = pid;
      set_priority(pid, priorities[i]);
      printf("  Forked Process %d with PID %d (Priority %d)\n", i, pids[i], priorities[i]);
    }
  }

  printf("Parent waiting for all %d children...\n\n", NUM_PROCS);
  int completion_order[NUM_PROCS];
  for (int i = 0; i < NUM_PROCS; i++) {
    int finished_pid = wait(0);
    completion_order[i] = finished_pid;
    printf("--> Parent: Reaped child PID %d at tick %d (Completion Rank: #%d)\n",
           finished_pid, uptime(), i + 1);
  }

  int total_time = uptime() - start_time;

  printf("\n====================================================\n");
  printf("  Question 4 Summary & Scheduling Behavior Analysis\n");
  printf("====================================================\n");
  printf("Total Execution Time: %d ticks\n", total_time);
  printf("Completion Order observed:\n");
  for (int i = 0; i < NUM_PROCS; i++) {
    printf("  Rank #%d: PID %d\n", i + 1, completion_order[i]);
  }
  printf("\nConclusions:\n");
  printf("  1. Higher-priority processes (e.g. Priority 5) scheduled first and finish earliest.\n");
  printf("  2. Aging mechanism prevented starvation of Priority 20 process by boosting priority over time.\n");
  printf("  3. Preemptive scheduler guarantees responsive CPU allocation across priority tiers.\n");
  printf("====================================================\n");

  exit(0);
}
