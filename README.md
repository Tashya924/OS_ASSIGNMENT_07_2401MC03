# OS_ASSIGNMENT_07_2401MC03
This project extends xv6-RISC-V for Operating Systems Lab Assignment 7. It explores thread creation and shared address spaces, kernel synchronization primitives, deadlock detection and avoidance, and priority-based preemptive CPU scheduling with aging.
Note: Run the build and test instructions on the actual source tree before describing any result as verified. The behaviors below are the assignment's expected outcomes, not a claim that every test has passed in every environment.

Assignment Overview
Q1 — Threads
Adds clone() and join() to support kernel-managed threads that share an address space. The threadtest program creates three threads, each incrementing a shared counter 100,000 times, and prints the final value.
The test illustrates that sharing memory does not automatically make concurrent updates safe: without synchronization, increments can be lost, so the final value may be less than 300,000.
Q2 — Synchronization with Semaphores
Adds counting semaphore operations:
- sem_init(sem_id, value) — initialize a semaphore.
- sem_wait(sem_id) — acquire a resource or block while it is unavailable.
- sem_signal(sem_id) — release a resource and wake a waiter when appropriate.
The synctest program protects the shared counter with a mutex semaphore and exercises the Producer–Consumer problem using a fixed-size buffer of five items, with empty, full, and mutex semaphores. The synchronized counter is expected to finish at 300,000.
Q3 — Deadlock Detection and Avoidance
The deadlock program simulates resource allocation using allocation, request/need, and available-resource arrays. It demonstrates a deadlock-detection algorithm and the Banker's safety algorithm.
The required test scenarios are:
1. A deadlocked state.
2. A safe state.
3. A state that becomes unsafe after a hypothetical resource request.
The assignment also asks for a circular-wait demonstration using xv6 processes and pipes, or shared memory using clone(), where supported by the implementation.
Q4 — Priority-Based Preemptive Scheduling
Adds process priorities and system calls to set and query them. A lower numeric value represents a higher priority:
Priority	Meaning
1	Highest
10	Default
20	Lowest


The scheduler selects runnable processes by priority, uses round-robin order to break ties, and applies aging to runnable processes that have waited more than 30 ticks. The priotest program runs CPU-bound processes with different priorities to demonstrate scheduling behavior and reduced starvation.
Repository Layout
The files must be integrated into a complete xv6-RISC-V source tree using the normal directory structure. A typical layout is:
.
├── kernel/
│   ├── proc.c
│   ├── proc.h
│   ├── sysproc.c
│   ├── syscall.c
│   ├── syscall.h
│   ├── defs.h
│   └── ...                 # Other original xv6 kernel files
├── user/
│   ├── threadtest.c
│   ├── synctest.c
│   ├── deadlock.c
│   ├── priotest.c
│   ├── user.h
│   ├── usys.pl
│   └── ...                 # Other original xv6 user files
├── Makefile
├── report.pdf              # Assignment report, when prepared
└── screenshots/             # Genuine test-output screenshots, when captured
Depending on the implementation, additional files may need updates, including kernel/trap.c, user declarations, syscall stubs, and the Makefile. Keep the original xv6 files needed to build the kernel; do not flatten kernel/ and user/ into the repository root.
Build and Run
1. Install the required tools
Use an environment supported by xv6-RISC-V, such as Linux or WSL with a RISC-V cross-compiler, QEMU, and make. Package names vary by distribution. Confirm the toolchain expected by the checked-out xv6 version before installing or changing compiler settings.
2. Build and boot xv6
From the root of the complete xv6-RISC-V source tree:
make clean
make qemu
If your environment uses a different RISC-V toolchain prefix, configure TOOLPREFIX for that toolchain as required by the xv6 Makefile.
3. Add the test programs to the xv6 build
Ensure the Makefile includes all four programs in its UPROGS list, using the format expected by the version of xv6 being used (for a typical xv6-RISC-V tree, entries resemble $U/_threadtest\). Rebuild after changing the Makefile.
4. Run the tests
At the xv6 shell prompt, run:
threadtest
synctest
deadlock
priotest
Capture the actual terminal output for each program. If any program fails to compile, boot, or run, fix the issue and repeat the test before recording the result.
What to Verify
- Threads: all three threads run, share the counter, and are joined; observe the unsynchronized counter result rather than assuming it must always be exactly the same.
- Semaphores: protected increments produce exactly 300,000; Producer–Consumer neither overflows nor underflows the buffer.
- Deadlock: the deadlocked, safe, and unsafe-after-request cases produce the expected classifications and show the relevant resource state.
- Scheduling: the priority API returns valid values, higher-priority work is favored, and aging eventually improves the priority of waiting runnable processes.
- Integration: every new syscall is declared and registered consistently, and all four programs are included in the build.
Submission Deliverables
The assignment submission should include:
- The modified xv6 kernel and syscall-integration source files.
- threadtest.c, synctest.c, deadlock.c, and priotest.c.
- Genuine terminal screenshots demonstrating the four questions.
- A consolidated 2–3 page report.pdf covering design decisions and data structures, problems and resolutions, the Q1–Q2 race-condition comparison, Q3 test results, and a Q4 completion-time-versus-priority table or graph.
Do not include temporary code-generation scripts, unrelated files, or unnecessary compiled artifacts in the submission folder.
