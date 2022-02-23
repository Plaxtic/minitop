#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>

#include "pstat/pstat.h"

#define UL unsigned long 
#define WAITTIME 1

void free_list(struct pstat *head);
int read_stat_by_increment(FILE*, char *, int, void*);
double get_total_cpu_work();
double get_total_cpu_jiffies();
double get_curr_cpu(double, double, double);
struct pstat *find_entry_by_pid(struct pstat *head, int pid);
struct pstat *sort(struct pstat *);
struct pstat *get_all_ps();

int main (int argc, char **argv) {
    
    // get system clock hertz
    hertz = sysconf(_SC_CLK_TCK);

    while (1) {
        struct pstat *psbefore = get_all_ps();
        double totalcpuwork = get_total_cpu_work();
        double totalcpu = get_total_cpu_jiffies();
        sleep(WAITTIME);
        struct pstat *psafter = get_all_ps();
        totalcpuwork = get_total_cpu_work() - totalcpuwork;
        totalcpu = get_total_cpu_jiffies() - totalcpu;

        double cpuratio = totalcpuwork/totalcpu;

        double pstotal;
        struct pstat *ptr;
        for (ptr = psbefore, pstotal = 0; ptr != NULL; ptr = ptr->next) {
            struct pstat *update = find_entry_by_pid(psafter, ptr->pid);

            if (update == NULL)
                continue;

            double pscpu  = (update->utime-ptr->utime);
            pscpu += (update->stime-ptr->stime);
            pscpu += (update->cutime-ptr->cutime);
            pscpu += (update->cstime-ptr->cstime);
            pscpu += (update->delayacct_blkio_ticks-ptr->delayacct_blkio_ticks);
            pscpu += (update->guest_time-ptr->guest_time);
            pscpu += (update->cguest_time-ptr->cguest_time);

            double cpu = 100 * (pscpu / totalcpu);

            pstotal += cpu;
            update->cpu_usage = cpu; 
        }

        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        psafter = sort(psafter);
        ptr = psafter;
        printf("%-6s %-40s %4s\n","PID", "Name", "%CPU");
        puts("-----------------------------------------------------");

        int t_rows = w.ws_row-4;
        while (ptr != NULL && t_rows--) {
            printf("%-6d %-40s %5.2f\n", ptr->pid, ptr->comm, ptr->cpu_usage);
            ptr = ptr->next;
        }
        printf("total: %f%%\n", cpuratio*100);
        free_list(psbefore);
        free_list(psafter);
    }

    return 0;
}

