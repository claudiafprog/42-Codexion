*This project has been created as part of the 42 curriculum by clferrei.*

# Codexion

## Description

**Codexion** is a concurrent programming project developed at 42 that models the classical Dining Philosophers problem extended with real-time scheduling constraints. 

The project simulates a group of programmers (*coders*) sitting at a round table, sharing adjacent hardware keys (*dongles*) to perform continuous cycles of **compiling**, **debugging**, and **refactoring**. To compile, a coder must hold both their left and right dongles.

The goal of this project is to implement a robust multi-threaded system capable of scheduling resource allocation under strict timing constraints, avoiding starvation, preventing deadlocks, and respecting two customizable scheduling algorithms:
- **FIFO (First-In, First-Out)**: Resource requests are handled according to arrival tickets.
- **EDF (Earliest Deadline First)**: Resource requests prioritize coders closest to their burnout threshold.

---

## Instructions

### Compilation

The project compiles with standard 42 flags (`-Wall -Wextra -Werror -pthread`) using the provided `Makefile`.

```bash
# Standard compilation
make

# Compile with ThreadSanitizer enabled for race detection
make tsan

# Clean object files
make clean

# Clean object files and binary
make fclean

# Recompile project
make re
```

```bash
./codexion [number_of_coders] [time_to_burnout] [time_to_compile] [time_to_debug] [time_to_refactor] [number_of_compiles_required] [dongle_cooldown] [scheduler]
```

## Parameter Breakdown:

  1.  number_of_coders: Number of coders and matching dongles (≥ 1).

  2.  time_to_burnout (in ms): Time limit a coder can wait without compiling before burning out.

  3.  time_to_compile (in ms): Duration of the compilation phase.

  4.  time_to_debug (in ms): Duration of the debugging phase.

  5.  time_to_refactor (in ms): Duration of the refactoring phase.

  6.  number_of_compiles_required (optional, 0 for infinite): Total compilations required per coder to complete the simulation.

  7.  dongle_cooldown (in ms): Time a dongle remains unavailable after being released.

  8.  scheduler: Scheduling strategy (fifo or edf).

## Example Usage:

```bash
./codexion 5 3000 200 200 200 10 800 fifo
./codexion 5 3000 200 200 200 10 800 edf
```

## Blocking Cases Handled

The implementation addresses several concurrency challenges to ensure deterministic execution and clean termination:

1.    Deadlock Prevention (Coffman Conditions & Total Order):

    -    Deadlocks occur when circular waiting occurs. In dongles.c (assign_dongles), locking order is determined by comparing memory addresses (coder->left_dongle > coder->right_dongle).

    -    By locking the lower memory address pointer first and the higher address second across all threads, a strict global hierarchy (total ordering) is enforced, breaking Coffman's circular wait condition.

2.    Starvation Prevention & Cooldown Handling:

    -    Dongles enforce a last_release_time timestamp. Before acquiring dongles, coders check are_dongles_available() under scheduler_mutex to respect active cooldowns.

    -    Local priority checks (has_local_priority()) ensure that adjacent waiting coders with earlier deadlines (EDF) or tickets (FIFO) are served before lower-priority coders claim available resources.

3.    Precise Burnout Detection:

    -    The dedicated simulation_monitor thread checks coder deadlines using check_burnout() with a high-resolution sleep interval (usleep(10)).

    -    Timestamp comparisons (get_time_in_ms() >= last_compile_time + time_to_burnout) allow burnout events to be detected and logged almost instantaneously.

4.    Log Serialization:

    -    Thread safety during output printing is maintained via print_lock mutex in print_status(). This guarantees that log timestamps and state transitions are properly serialized without interleaved character output.

5.    Single Coder Edge Case:

    -    When number_of_coders == 1, a coder cannot acquire two dongles. Handled gracefully via one_coder(), taking the available single dongle and waiting until burnout without crashing or hanging indefinitely.

## Thread Synchronization Mechanisms

The implementation coordinates worker threads (coders) and the supervisor thread (simulation_monitor) using POSIX threading primitives:

### Primitives Used

*    pthread_mutex_t:

    -    dongle_lock (per dongle): Guarantees mutual exclusion when taking hardware dongles.

    -    scheduler_mutex: Protects global queue state (wait_queue), heap manipulations (heap_append_scheduled, heap_prio_remove), ticket allocation, and dongle status flags (is_taken).

    -    simulation_lock: Protects shared state reads and updates, including last_compile_time, compiles_count, and simulation_over.

    -    print_lock: Ensures atomic console output for state changes.

*    pthread_cond_t:

    -    node->cond & dongle->cond: Initialized per node/dongle to support condition-based sleeping and clean thread waking during simulation termination (finish_simulation()).

### Communication & Race Condition Prevention

*    Thread-Safe State Reads:

    -    check_simulation_over() wraps access to simulation_over inside simulation_lock to prevent data races between worker coders and the monitor.

*    Atomic Dongle Claiming:

    -    In dongles_check.c, try_to_claim_dongles() checks conditions and updates state (atomic_take_dongles()) inside scheduler_mutex. Once state flags are locked, scheduler_mutex is released before acquiring the physical dongle_lock mutexes to avoid lock-order inversion and long critical sections.

*    Clean Destruction & Shutdown:

    -    When a simulation finishes (either by burnout or completion), finish_simulation() signals all waiting threads while holding scheduler_mutex, ensuring no thread remains blocked on a condition variable or locked mutex during cleanup (free_all()).

## Resources
### References

*    POSIX Threads Programming: IEEE Std 1003.1-2017 / Linux man pages (pthread_mutex_lock, pthread_cond_wait).

*    Operating System Concepts (Silberschatz, Galvin, Gagne): Deadlock conditions, Earliest Deadline First (EDF) scheduling theory, and priority queues.

*    The Dining Philosophers Problem: Edsger W. Dijkstra (1965) - Resource allocation and deadlock prevention techniques.

### AI Usage Disclaimer

Artificial Intelligence (LLM assistance) was utilized during the development of this project for specific tasks:

*    Debugging Concurrency & ThreadSanitizer Warnings: Analyzing race condition traces reported by ThreadSanitizer (-fsanitize=thread), specifically resolving destroy of a locked mutex warnings and lock-order inversion scenarios between simulation_lock and scheduler_mutex.

*    Refactoring Strategy & Edge Case Analysis: Reviewing theoretical edge cases (e.g., high cooldown contention under EDF vs. FIFO) and validating heap priority comparison logic.

*    Documentation: Structuring and proofreading project documentation and this README.md to align with 42 curriculum standards.

All core code implementation, architecture, structural code decisions, and validation tests were written, reviewed, and tested by the author.
