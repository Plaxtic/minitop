#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>


#define UPTIME "/proc/uptime"
#define STAT "/proc/stat"
#define WAITTIME 1
#define UL unsigned long 

struct pstat {
    int pid;                        // (1)
    char comm[255];                 // (2) The filename of the executable, in parentheses.
    char state;                     // (3) One of the following characters, indicating process state:
#if 0 
                                                R Running
                                                S Sleeping in an interruptible wait
                                                D Waiting in uninterruptible disk sleep
                                                Z Zombie
                                                T Stopped (on a signal) or (before Linux 2.6.33) trace stopped
                                                t Tracing stop (Linux 2.6.33 onward)
                                                W Paging (only before Linux 2.6.0)
                                                X Dead (from Linux 2.6.0 onward)
                                                x Dead (Linux 2.6.33 to 3.13 only)
                                                K Wakekill (Linux 2.6.33 to 3.13 only)
                                                W Waking (Linux 2.6.33 to 3.13 only)
                                                P Parked (Linux 3.9 to 3.13 only)
#endif
    int ppid;                       // (4) The PID of the parent of this process.
    int pgrp;                       // (5) The process group ID of the process.
    int session;                    // (6) The session ID of the process.
    int tty_nr;                     // (7) The controlling terminal of the process. 
    int tpgid;                      // (8) The ID of the foreground process group of the controlling terminal of the process.
    unsigned flags;                 // (9) The kernel flags word of the process. For bit meanings, see the PF_* defines in include/linux/sched.h.
    long unsigned minflt;           // (10) The number of minor faults the process has made which have not required loading a memory page from disk.
    long unsigned cminflt;          // (11) The number of minor faults that the process's waited-for children have made.
    long unsigned majflt;           // (12) The number of major faults the process has made which have required loading a memory page from disk.
    long unsigned cmajflt;          // (13) The number of major faults that the process's waited-for children have made.
    long unsigned utime;            // (14) Amount of time this process has been scheduled in user mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
    long unsigned stime;            // (15) Amount of time this process has been scheduled in kernel mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
    long int cutime;                // (16) Amount of time this process's waited-for children have been scheduled in user mode, measured in clock ticks  (divide  by
    long int cstime;                // (17) Amount of time this process's waited-for children have been scheduled in kernel mode, measured in clock ticks (divide by
    long int priority;              // (18)
    long int nice;                  // (19) The nice value (see setpriority(2)), a value in the range 19 (low priority) to -20 (high priority).
    long int num_threads;           // (20) Number of threads in this process
    long int itrealvalue;           // (21) The time in jiffies before the next SIGALRM is sent to the process due to an interval timer. 
    long long unsigned starttime;   // (22) The time the process started after system boot. The value is expressed in clock ticks (divide by sysconf(_SC_CLK_TCK)).
    long unsigned vsize;            // (23) Virtual memory size in bytes.
    long int rss;                   // (24)
    long unsigned rsslim;           // (25) Current soft limit in bytes on the rss of the process; see the description of RLIMIT_RSS in getrlimit(2).
    long unsigned startcode;        // (26) The address above which program text can run.
    long unsigned endcode;          // (27) The address below which program text can run.
    long unsigned startstack;       // (28) The address of the start (i.e., bottom) of the stack.
    long unsigned kstkesp;          // (29) The current value of ESP (stack pointer), as found in the kernel stack page for the process.
    long unsigned kstkeip;          // (30) The current EIP (instruction pointer).
    long unsigned signal;           // (31) OBSOLETE 
    long unsigned blocked;          // (32) OBSOLETE 
    long unsigned sigignore;        // (33) OBSOLETE 
    long unsigned sigcatch;         // (34) OBSOLETE 
    long unsigned wchan;            // (35) This is the "channel" in which the process is waiting. 
    long unsigned nswap;            // (36) Number of pages swapped (not maintained).
    long unsigned cnswap;           // (37) Cumulative nswap for child processes (not maintained).
    int exit_signal;                // (38) Signal to be sent to parent when we die.
    int processor;                  // (39) CPU number last executed on.
    unsigned rt_priority;           // (40) Real-time scheduling priority, a number in the range 1 to 99 for processes scheduled under a real-time policy, or 0, for
    unsigned policy;                // (41) Scheduling policy (see sched_setscheduler(2)). Decode using the SCHED_* constants in linux/sched.h.
    long long unsigned delayacct_blkio_ticks;       // (42) Aggregated block I/O delays, measured in clock ticks (centiseconds).
    long unsigned guest_time;                       // (43) Guest time of the process, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
    long int cguest_time;                           // (44) Guest time of the process's children, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
    long unsigned start_data;                       // (45) Address above which program initialized and uninitialized (BSS) data are placed.
    long unsigned end_data;                         // (46) Address below which program initialized and uninitialized (BSS) data are placed.
    long unsigned start_brk;                        // (47) Address above which program heap can be expanded with brk(2).
    long unsigned arg_start;                        // (48) Address above which program command-line arguments (argv) are placed.
    long unsigned arg_end;                          // (49) Address below program command-line arguments (argv) are placed.
    long unsigned env_start;                        // (50) Address above which program environment is placed.
    long unsigned env_end;                          // (51) Address below which program environment is placed.
    int exit_code;                                  // (52) The thread's exit status in the form reported by waitpid(2).
    double cpu_usage;
    struct pstat *next;
};

extern double hertz;

void free_list(struct pstat *);
int read_stat_by_increment(FILE*, char *, int, void*);
double get_total_cpu_work();
double get_total_cpu_jiffies();
double get_curr_cpu(double, double, double);
struct pstat *find_entry_by_pid(struct pstat*, int);
struct pstat *sort(struct pstat *);
struct pstat *get_all_ps();
struct pstat *get_pstat(char*);


