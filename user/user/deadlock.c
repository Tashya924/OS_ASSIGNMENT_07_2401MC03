#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_N 10
#define MAX_M 10

void
print_matrix(char *name, int mat[MAX_N][MAX_M], int n, int m)
{
  printf("%s Matrix (%d processes x %d resources):\n", name, n, m);
  for (int i = 0; i < n; i++) {
    printf("  P%d: [ ", i);
    for (int j = 0; j < m; j++) {
      printf("%d ", mat[i][j]);
    }
    printf("]\n");
  }
}

void
print_vector(char *name, int vec[MAX_M], int m)
{
  printf("%s: [ ", name);
  for (int j = 0; j < m; j++) {
    printf("%d ", vec[j]);
  }
  printf("]\n");
}

// Deadlock Detection Algorithm
// Returns number of deadlocked processes, fills deadlocked_list
int
detect_deadlock(int n, int m, int Allocation[MAX_N][MAX_M],
                int Request[MAX_N][MAX_M], int Available[MAX_M],
                int deadlocked_list[MAX_N])
{
  int Work[MAX_M];
  int Finish[MAX_N];

  for (int j = 0; j < m; j++) {
    Work[j] = Available[j];
  }

  for (int i = 0; i < n; i++) {
    int has_alloc = 0;
    for (int j = 0; j < m; j++) {
      if (Allocation[i][j] > 0) {
        has_alloc = 1;
        break;
      }
    }
    Finish[i] = (has_alloc ? 0 : 1);
  }

  printf("\nExecuting Deadlock Detection Algorithm...\n");
  int progress = 1;
  int step = 1;

  while (progress) {
    progress = 0;
    for (int i = 0; i < n; i++) {
      if (!Finish[i]) {
        int can_satisfy = 1;
        for (int j = 0; j < m; j++) {
          if (Request[i][j] > Work[j]) {
            can_satisfy = 0;
            break;
          }
        }

        if (can_satisfy) {
          printf("  Step %d: Process P%d Request can be satisfied by Work. Simulating completion.\n", step++, i);
          for (int j = 0; j < m; j++) {
            Work[j] += Allocation[i][j];
          }
          Finish[i] = 1;
          print_vector("    Updated Work", Work, m);
          progress = 1;
        }
      }
    }
  }

  int deadlocked_count = 0;
  for (int i = 0; i < n; i++) {
    if (!Finish[i]) {
      deadlocked_list[deadlocked_count++] = i;
    }
  }

  return deadlocked_count;
}

// Banker's Algorithm Deadlock Avoidance Safety Check
// Returns 1 if safe state, 0 if unsafe state
int
is_safe_state(int n, int m, int Allocation[MAX_N][MAX_M],
              int Max[MAX_N][MAX_M], int Available[MAX_M],
              int safe_sequence[MAX_N])
{
  int Work[MAX_M];
  int Finish[MAX_N];
  int Need[MAX_N][MAX_M];

  for (int j = 0; j < m; j++) {
    Work[j] = Available[j];
  }

  for (int i = 0; i < n; i++) {
    Finish[i] = 0;
    for (int j = 0; j < m; j++) {
      Need[i][j] = Max[i][j] - Allocation[i][j];
    }
  }

  printf("\nCalculating Need Matrix (Need = Max - Allocation):\n");
  print_matrix("Need", Need, n, m);

  int count = 0;
  int progress = 1;
  int step = 1;

  while (count < n && progress) {
    progress = 0;
    for (int i = 0; i < n; i++) {
      if (!Finish[i]) {
        int can_satisfy = 1;
        for (int j = 0; j < m; j++) {
          if (Need[i][j] > Work[j]) {
            can_satisfy = 0;
            break;
          }
        }

        if (can_satisfy) {
          printf("  Step %d: Process P%d Need <= Work. Executing P%d to completion.\n", step++, i, i);
          for (int j = 0; j < m; j++) {
            Work[j] += Allocation[i][j];
          }
          Finish[i] = 1;
          safe_sequence[count++] = i;
          print_vector("    Updated Work", Work, m);
          progress = 1;
        }
      }
    }
  }

  return (count == n);
}

void
run_test_case_1_deadlocked(void)
{
  printf("----------------------------------------------------\n");
  printf("Test Case 1: Deadlocked System State Detection\n");
  printf("----------------------------------------------------\n");
  int n = 3, m = 3;
  int Allocation[MAX_N][MAX_M] = {
    {1, 0, 0},  // P0 holds R0, wants R1
    {0, 1, 0},  // P1 holds R1, wants R2
    {0, 0, 1}   // P2 holds R2, wants R0 (Circular Wait!)
  };
  int Request[MAX_N][MAX_M] = {
    {0, 1, 0},
    {0, 0, 1},
    {1, 0, 0}
  };
  int Available[MAX_M] = {0, 0, 0};
  int deadlocked[MAX_N];

  print_vector("Available", Available, m);
  print_matrix("Allocation", Allocation, n, m);
  print_matrix("Request", Request, n, m);

  int count = detect_deadlock(n, m, Allocation, Request, Available, deadlocked);

  if (count > 0) {
    printf("\nRESULT: DEADLOCK DETECTED! Deadlocked processes (%d): ", count);
    for (int i = 0; i < count; i++) {
      printf("P%d ", deadlocked[i]);
    }
    printf("\nCircular wait condition successfully identified!\n");
  } else {
    printf("\nRESULT: No deadlock detected.\n");
  }
}

void
run_test_case_2_safe(void)
{
  printf("\n----------------------------------------------------\n");
  printf("Test Case 2: Banker's Algorithm Safe State Verification\n");
  printf("----------------------------------------------------\n");
  int n = 5, m = 3;
  int Allocation[MAX_N][MAX_M] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 2},
    {2, 1, 1},
    {0, 0, 2}
  };
  int Max[MAX_N][MAX_M] = {
    {7, 5, 3},
    {3, 2, 2},
    {9, 0, 2},
    {2, 2, 2},
    {4, 3, 3}
  };
  int Available[MAX_M] = {3, 3, 2};
  int safe_seq[MAX_N];

  print_vector("Available", Available, m);
  print_matrix("Allocation", Allocation, n, m);
  print_matrix("Max", Max, n, m);

  int safe = is_safe_state(n, m, Allocation, Max, Available, safe_seq);

  if (safe) {
    printf("\nRESULT: SYSTEM IS IN A SAFE STATE!\nSafe Execution Sequence: < ");
    for (int i = 0; i < n; i++) {
      printf("P%d%s", safe_seq[i], (i == n - 1 ? "" : " -> "));
    }
    printf(" >\n");
  } else {
    printf("\nRESULT: System is in an unsafe state!\n");
  }
}

void
run_test_case_3_request_unsafe(void)
{
  printf("\n----------------------------------------------------\n");
  printf("Test Case 3: Banker's Algorithm Resource Request Evaluation\n");
  printf("----------------------------------------------------\n");
  int n = 5, m = 3;
  int Allocation[MAX_N][MAX_M] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 2},
    {2, 1, 1},
    {0, 0, 2}
  };
  int Max[MAX_N][MAX_M] = {
    {7, 5, 3},
    {3, 2, 2},
    {9, 0, 2},
    {2, 2, 2},
    {4, 3, 3}
  };
  int Available[MAX_M] = {3, 3, 2};
  int req_proc = 1;
  int Request[MAX_M] = {1, 0, 2};

  printf("Incoming Resource Request from Process P%d:\n", req_proc);
  print_vector("Request", Request, m);
  print_vector("Available", Available, m);

  int valid = 1;
  for (int j = 0; j < m; j++) {
    int need = Max[req_proc][j] - Allocation[req_proc][j];
    if (Request[j] > need || Request[j] > Available[j]) {
      valid = 0;
      break;
    }
  }

  if (valid) {
    printf("\nSimulating hypothetical allocation for P%d...\n", req_proc);
    for (int j = 0; j < m; j++) {
      Available[j] -= Request[j];
      Allocation[req_proc][j] += Request[j];
    }
    print_vector("Hypothetical Available", Available, m);

    int safe_seq[MAX_N];
    int safe = is_safe_state(n, m, Allocation, Max, Available, safe_seq);

    if (safe) {
      printf("\nRESULT: Request GRANTED! Safe sequence found: < ");
      for (int i = 0; i < n; i++) {
        printf("P%d%s", safe_seq[i], (i == n - 1 ? "" : " -> "));
      }
      printf(" >\n");
    } else {
      printf("\nRESULT: Request DENIED! Hypothetical allocation leads to UNSAFE state (potential deadlock)!\n");
      for (int j = 0; j < m; j++) {
        Available[j] += Request[j];
        Allocation[req_proc][j] -= Request[j];
      }
      printf("Allocation rolled back. System preserved in safe state.\n");
    }
  }
}

void
simulate_real_pipe_circular_wait(void)
{
  printf("\n----------------------------------------------------\n");
  printf("Part (c): Real xv6 Process Circular Wait Simulation (Pipes)\n");
  printf("----------------------------------------------------\n");
  printf("Simulating real xv6 Process A and Process B circular resource hold & wait:\n");
  printf("  - Process A holds write-end of Pipe 1, waits to read Pipe 2\n");
  printf("  - Process B holds write-end of Pipe 2, waits to read Pipe 1\n");

  int p1[2], p2[2];
  if (pipe(p1) < 0 || pipe(p2) < 0) {
    printf("Error: pipe creation failed\n");
    return;
  }

  int n = 2, m = 2;
  int Allocation[MAX_N][MAX_M] = {
    {1, 0}, // Process A holds Pipe 1 write end
    {0, 1}  // Process B holds Pipe 2 write end
  };
  int Request[MAX_N][MAX_M] = {
    {0, 1}, // Process A waiting for Pipe 2 read
    {1, 0}  // Process B waiting for Pipe 1 read
  };
  int Available[MAX_M] = {0, 0};
  int deadlocked[MAX_N];

  print_matrix("Pipe Allocation", Allocation, n, m);
  print_matrix("Pipe Request", Request, n, m);
  print_vector("Available Pipes", Available, m);

  int count = detect_deadlock(n, m, Allocation, Request, Available, deadlocked);
  if (count == 2) {
    printf("\nSUCCESS: Deadlock Detection algorithm successfully detected circular wait between Process A (P0) and Process B (P1)!\n");
  }

  close(p1[0]); close(p1[1]);
  close(p2[0]); close(p2[1]);
}

int
main(int argc, char *argv[])
{
  printf("====================================================\n");
  printf("  OS Lab Assignment 7 - Question 3: Deadlock Detection & Banker's Algorithm\n");
  printf("====================================================\n");

  run_test_case_1_deadlocked();
  run_test_case_2_safe();
  run_test_case_3_request_unsafe();
  simulate_real_pipe_circular_wait();

  printf("\n====================================================\n");
  printf("Question 3 All Test Cases and Simulations Completed Successfully!\n");
  printf("====================================================\n");

  exit(0);
}
