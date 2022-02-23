#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "pstat.h"

double hertz;

void free_list(struct pstat *head) {
    if (head == NULL)
        return;
    
    free_list(head->next);
    free(head);
}

struct pstat *get_all_ps() {
    struct dirent *dent;

    // open proc to get processes
    DIR *proc = opendir("/proc/");
    if (proc == NULL)
        return NULL;

    struct pstat *pslst = NULL;
    char statpath[275] = {0};
    while((dent = readdir(proc)) != NULL) {
        if (dent->d_type == DT_DIR && isdigit(dent->d_name[0])) {

            snprintf(statpath, 274, "/%s/%s/%s", "proc", dent->d_name, "stat");
            struct pstat *ps = get_pstat(statpath);

            if (ps == NULL) 
                perror("get_pstat");
            else {
                ps->next = pslst, pslst = ps;
            }

            memset(statpath, 0, 20);
        }
    }
    closedir(proc);

    return pslst;
}

struct pstat *sort(struct pstat *head) {

    struct pstat *curr = head;
    struct pstat *idx = NULL;
    struct pstat *tmp = NULL;
    struct pstat *prev1 = NULL;

    if (curr == NULL)
        return 0;

    else {
        while (curr != NULL)  {
            idx = curr->next;
            struct pstat *prev2 = NULL;

            while (idx != NULL) {
                if (curr->cpu_usage < idx->cpu_usage) {
                    if (prev1 != NULL)
                        prev1->next = idx;
                    else 
                        head = idx;

                    if (prev2 != NULL) {
                        prev2->next = curr;

                        tmp = idx->next;
                        idx->next = curr->next;
                        curr->next = tmp;
                    }
                    else {
                        // prevent loop with idx->idx
                        curr->next = idx->next;  
                        idx->next = curr;
                    }
                    // swap pointers
                    tmp = idx;
                    idx = curr;
                    curr = tmp;
                }
                prev2 = idx;
                idx = idx->next;
            }
            prev1 = curr;
            curr = curr->next;
        }
    }
    return head;
}

double get_curr_cpu(double utime, double stime, double seconds) {
    double total_time = utime + stime;
    return 100 * ((total_time / hertz) / seconds);
}

double get_total_cpu_work() {
    UL umode, nice, smode;

    FILE *fp = fopen(STAT, "r");

    fscanf(fp, "cpu %lu %lu %lu", &umode, &nice, &smode);
    fclose(fp);

    return umode + nice + smode;
}

double get_total_cpu_jiffies() {
    UL all[10];

    FILE *fp = fopen(STAT, "r");

    fscanf(fp, "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", 
            &all[0],
            &all[1],
            &all[2],
            &all[3],
            &all[4],
            &all[5],
            &all[6],
            &all[7],
            &all[8],
            &all[9]);

    fclose(fp);

    // sum jiffies
    UL total = 0;
    for (int i = 0; i < 10; i++)
        total += all[i];

    return total;
}

struct pstat *find_entry_by_pid(struct pstat *head, int pid) {
    for (struct pstat *idx = head; idx != NULL; idx = idx->next)
        if (idx->pid == pid) 
            return idx;

    return NULL;
}

struct pstat *get_pstat(char *path) {

    struct pstat *pst = malloc(sizeof(struct pstat));
    FILE *fp = fopen(path, "r");

    if (fp == NULL)
        return NULL;

    fscanf(fp, "%d (%255[^)])", &pst->pid, pst->comm);

    char c;
    if ((c = fgetc(fp)) != ' ') {
        int i = strlen(pst->comm);

        do {
            pst->comm[i] = c;
        } while ((c = fgetc(fp)) != ' ');
    }

    fscanf(fp, "%c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d",
            &pst->state,
            &pst->ppid,
            &pst->pgrp,
            &pst->session,
            &pst->tty_nr,
            &pst->tpgid,
            &pst->flags,
            &pst->minflt,
            &pst->cminflt,
            &pst->majflt,
            &pst->cmajflt,
            &pst->utime,
            &pst->stime, 
            &pst->cutime,
            &pst->cstime, 
            &pst->priority, 
            &pst->nice, 
            &pst->num_threads, 
            &pst->itrealvalue, 
            &pst->starttime, 
            &pst->vsize,
            &pst->rss,
            &pst->rsslim,
            &pst->startcode, 
            &pst->endcode,
            &pst->startstack, 
            &pst->kstkesp,
            &pst->kstkeip, 
            &pst->signal, 
            &pst->blocked, 
            &pst->sigignore, 
            &pst->sigcatch, 
            &pst->wchan,
            &pst->nswap,
            &pst->cnswap, 
            &pst->exit_signal,
            &pst->processor,
            &pst->rt_priority,
            &pst->policy,
            &pst->delayacct_blkio_ticks, 
            &pst->guest_time, 
            &pst->cguest_time, 
            &pst->start_data,
            &pst->end_data, 
            &pst->start_brk, 
            &pst->arg_start, 
            &pst->arg_end,
            &pst->env_start, 
            &pst->env_end, 
            &pst->exit_code);

    fclose(fp);

    return pst;
}
#endif
