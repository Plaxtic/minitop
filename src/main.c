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
        psafter = sort(psafter);
        ptr = psafter;

        printf("%-6s %-40s %4s\n","PID", "Name", "%CPU");
        puts("-----------------------------------------------------");

        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int t_rows = w.ws_row-4;
        while (ptr != NULL && t_rows-- > 0) {
            printf("%-6d %-40s %5.2f\n", ptr->pid, ptr->comm, ptr->cpu_usage);
            ptr = ptr->next;
        }
        printf("total: %f%%\n", cpuratio*100);

        free_list(psbefore);
        free_list(psafter);
    }
    return 0;
}

