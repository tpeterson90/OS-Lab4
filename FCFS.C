#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int process_id;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
    int remaining_time;
    int completion_time;
} Process;


//Function to manually sort processes by arrival time
void sortProcessesByArrival(Process proc[], int n) {
    for (int i = 0; i < n - 1; i++){
        for (int j = 0; j<n-i-1; j++){
            if (proc[j].arrival_time > proc[j+1].arrival_time || (proc[j].arrival_time == proc[j+1].arrival_time && proc[j].process_id > proc[j + 1].process_id)){
                //swap the process
                Process temp = proc[j];
                proc[j] = proc[j+1];
                proc[j+1] = temp;
            }
        }
    }
}
static void resetRuntimeFields(Process proc[], int n){
    for (int i = 0; i < n; i++) {
        proc[i].remaining_time = proc[i].burst_time;
        proc[i].completion_time = 0;
        proc[i].waiting_time = 0;
        proc[i].turnaround_time = 0;
    }
}
static void computeWaitTurn(Process proc[], int n){
    for (int i = 0; i < n; i++) {
        proc[i].turnaround_time = proc[i].completion_time - proc[i].arrival_time;
        proc[i].waiting_time = proc[i].turnaround_time - proc[i].burst_time;
    }
}

static void printAverages(Process proc[], int n){
    double sumW = 0.0, sumT = 0.0;
    for (int i = 0; i < n; i++){
        sumW += proc[i].waiting_time;
        sumT += proc[i].turnaround_time;
    }
    printf("Average Waiting Time: %.2f\n", sumW / n);
    printf("Average Turnaround Time: %.2f\n", sumT / n);
}

 void printProcessesTable(Process proc[], int n){
    printf("Process ID\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\n");
    for (int i = 0; i < n; i++){
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n", proc[i].process_id, proc[i].arrival_time, proc[i].burst_time, proc[i].waiting_time, proc[i].turnaround_time);
    }
 }

/* ---------- Preemptive SJF (SRTF) ---------- */
void sjf_preemptive(Process proc[], int n) {
    sortProcessesByArrival(proc, n);
    resetRuntimeFields(proc, n);

    int time = 0;
    int completed = 0;
    int last_running_pid = -1;

    printf("=== Preemptive SJF (SRTF) ===\n");
    printf("Order of execution: ");

    while (completed < n) {
        int best = -1;

        // pick ready process with smallest remaining time
        for (int i = 0; i < n; i++) {
            if (proc[i].arrival_time <= time && proc[i].remaining_time > 0) {
                if (best == -1 || proc[i].remaining_time < proc[best].remaining_time) {
                    best = i;
                }
            }
        }

        // CPU idle
        if (best == -1) {
            time++;
            last_running_pid = -1;
            continue;
        }

        // print only when process changes 
        if (proc[best].process_id != last_running_pid) {
            printf("P%d ", proc[best].process_id);
            last_running_pid = proc[best].process_id;
        }

        // run 1 time unit (preemptive)
        proc[best].remaining_time--;
        time++;

        if (proc[best].remaining_time == 0) {
            proc[best].completion_time = time;
            completed++;
        }
    }

    printf("\n");
    computeWaitTurn(proc, n);
    printProcessesTable(proc, n);
    printAverages(proc, n);
    printf("\n");
}

/* ---------- Round Robin (minimal queue) ---------- */
void rr_preemptive(Process proc[], int n, int quantum) {
    sortProcessesByArrival(proc, n);
    resetRuntimeFields(proc, n);

    // queue using array indices
    int q[1024];
    int front = 0, back = 0;

    int time = 0;
    int completed = 0;
    int next_arrival = 0;

    printf("=== Round Robin (Quantum = %d) ===\n", quantum);
    printf("Order of execution: ");

    // jump to first arrival if needed
    if (n > 0 && proc[0].arrival_time > time) time = proc[0].arrival_time;

    // enqueue all arrivals up to current time
    while (next_arrival < n && proc[next_arrival].arrival_time <= time) {
        q[back++] = next_arrival;
        next_arrival++;
    }

    while (completed < n) {
    // if queue empty, CPU idle until next arrival
        if (front == back) {
            if (next_arrival < n) {
                time = proc[next_arrival].arrival_time;
                while (next_arrival < n && proc[next_arrival].arrival_time <= time) {
                    q[back++] = next_arrival;
                    next_arrival++;
            }
        }
        continue;
    }

    int idx = q[front++]; // pop
    printf("P%d ", proc[idx].process_id);

    int slice = quantum;
    if (proc[idx].remaining_time < slice) slice = proc[idx].remaining_time;

    // advance time 1-by-1 so new arrivals get enqueued during the slice
        for (int t = 0; t < slice; t++) {
            proc[idx].remaining_time--;
            time++;

            while (next_arrival < n && proc[next_arrival].arrival_time <= time) {
                q[back++] = next_arrival;
                next_arrival++;
            }

            if (proc[idx].remaining_time == 0) break;
        }

        if (proc[idx].remaining_time == 0) {
            proc[idx].completion_time = time;
            completed++;
        } else {
            q[back++] = idx; // re-enqueue
        }
    }

    printf("\n");
    computeWaitTurn(proc, n);
    printProcessesTable(proc, n);
    printAverages(proc, n);
    printf("\n");
}

 int main() {
    //Initialize processes with their IDs, arrival times, and burst times
    //Process proc[] = {{1, 0, 8}, {2, 1, 4}, {3, 2, 9}, {4, 3, 5}};
    Process original[] = {{1, 0, 24}, {2, 0, 3}, {3, 0, 3}};
    int n = (int)sizeof(original) / sizeof(original[0]);

    //Run SJF
    Process sjf_proc[64];
    for (int i = 0; i < n; i++) sjf_proc[i] = original[i];
    sjf_preemptive(sjf_proc, n);

    Process rr_proc[64];
    for (int i = 0; i < n; i++) rr_proc[i] = original[i];
    rr_preemptive(rr_proc, n, 4);

    return 0;

 }