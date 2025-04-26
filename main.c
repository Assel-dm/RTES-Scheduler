#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

//Global variables for computation times (C) and periods (T)
int* C_global = NULL;
int* T_global = NULL;


//Structure for Tasks and Jobs
typedef struct {
    int id; 
    int C;
    int T;
} Task;

typedef struct {
    int task_id;
    int release_time;
    int deadline;
    int remaining_time;
    int C;
} Job;

//Function to determine the number of tasks and their parameters
int nbr_task(int* n) {
    printf("Select a number of tasks to schedule: ");
    scanf("%d", n);

    C_global = malloc(*n * sizeof(int));
    T_global = malloc(*n * sizeof(int));

    for (int k = 0; k < *n; k++) {
        printf("Enter C and T for task %d: ", k + 1);
        scanf("%d %d", &C_global[k], &T_global[k]);
    }
    return 0; 
}

//Function to check the schedulability of the task set
int schedulability(int n) {
    double out = 0.0;
    for(int i=0; i<n; i++) {
        out +=  (double)C_global[i] / (double)T_global[i];
    }
    printf("Total utilisation of the processus : %.3f\n", out);
    if (out<1) {
        printf("✅ Tasks schedulable ! \n");
        return 1;
    }
    if (out>1) {
        printf("⚠️ Tasks not schedulable ! \n");
        return 0;
    }
    
}

//Get the current time in seconds
double current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

//Loop simulating an active function
void busy_wait(int seconds) {
    double start = current_time();
    while (current_time() - start < seconds) {
        // We do useless things to occupied the CPU
        for (volatile int i = 0; i < 1000; i++) {} 
    }
}

//Simulate a task
void task(int C) {
    busy_wait(C); 
}


// Functions for calculating GCD, LCM, and LCM of an array (for hyperperiod)
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int lcm(int a, int b) {
    return (a * b) / gcd(a, b);
}

int lcm_array(int arr[], int n) {
    int result = arr[0];
    for (int i = 1; i < n; i++) {
        result = lcm(result, arr[i]);
    }
    return result;
}

//Generation of all jobs for the hyperperiode
int generate_jobs(Task* tasks, int n, Job* job_list, int hyperperiod) {
    int count = 0;
    for (int i = 0; i < n; i++) {
        for (int t = 0; t < hyperperiod; t += tasks[i].T) {
            job_list[count].task_id = tasks[i].id;
            job_list[count].release_time = t;
            job_list[count].deadline = t + tasks[i].T;
            job_list[count].remaining_time = tasks[i].C;
            job_list[count].C = tasks[i].C;
            count++;
        }
    }
    return count;
}

//EDF Scheduler
void edf_scheduler(Task* tasks, int task_count, int hyperperiod) {
    char exec_order[1000][10]; // Table of de jobs "Tij"
    int exec_index = 0;
    int allowed_task_to_miss = 5; // task 5 allowed to miss it deadlines
    int total_wait_time = 0;//Accumulate total waiting time

    Job job_list[200];
    int job_count = generate_jobs(tasks, task_count, job_list, hyperperiod);

    int current_time =0;
    printf("\n===== EDF Scheduling =====\n");

    while (current_time < hyperperiod) {
        // Find the ready job with the earliest deadline
        int selected = -1;
        int min_deadline = 99999;

        for (int i = 0; i < job_count; i++) {
            if (job_list[i].release_time <= current_time && job_list[i].remaining_time > 0) {
                if (job_list[i].deadline < min_deadline) {
                    min_deadline = job_list[i].deadline;
                    selected = i;
                }
            }
        }

        if (selected != -1) {
            printf("[t=%d] Task %d started, deadline = %d\n", current_time, job_list[selected].task_id, job_list[selected].deadline);

            int wait = current_time - job_list[selected].release_time;
            total_wait_time += wait;
            int finish_time = current_time + job_list[selected].C;
            int response_time = finish_time - job_list[selected].release_time;

            task(job_list[selected].C);
            job_list[selected].remaining_time = 0; 

            // Store job execution order in the format: T<task_id><job_index>
            int job_num = (job_list[selected].release_time) / tasks[job_list[selected].task_id - 1].T + 1;
            sprintf(exec_order[exec_index], "T%d%d", job_list[selected].task_id, job_num);
            exec_index++;

            printf("   → finished at t=%d | response = %d | wait = %d ", finish_time, response_time, wait);
            if (finish_time > job_list[selected].deadline) {
                if (job_list[selected].task_id == allowed_task_to_miss) {
                    printf("⚠️ Task %d missed its deadline (allowed)\n", job_list[selected].task_id);
                } else {
                    printf("❌ Task %d missed its deadline\n", job_list[selected].task_id);
                }
            } else {
                printf("✅ deadline met\n");
            }
            current_time = finish_time;
            
        } else {
            // None job ready → CPU idle
            printf("[t=%d] CPU idle...\n", current_time);
            current_time++;
        }
    }
    // Print execution order
    printf("\nExecution Order:\n");
    for (int i = 0; i < exec_index; i++) {
        printf("%s", exec_order[i]);
        if (i < exec_index - 1)
            printf(" → ");
    }
    printf("\n");
    // Print total waiting time
    printf("\nTotal waiting time across all jobs: %d time units\n", total_wait_time);

}


// Simple testing function simulating tasks (used in early debugging)
void time_task(int n) {
    clock_t start, end;
    double TimeUsed;double release_time, finish_time, response_time, delay;


    for (int i = 0; i < n; i++) {
        printf("\n[Task %d]\n", i + 1);

        release_time = (double)clock() / CLOCKS_PER_SEC;
        printf("Release time : %.2f s\n", release_time);

        start = clock();
        task(C_global[i]);
        end = clock();

        finish_time = (double)end / CLOCKS_PER_SEC;
        response_time = finish_time - release_time;
        delay = response_time - T_global[i];

        printf("Finish time  : %.2f s\n", finish_time);
        printf("Response time: %.2f s\n", response_time);
        
        if (delay > 0) {
            printf("⚠️ Deadline missed by %.2f s\n", delay);
        } else {
            printf("✅ Deadline met (%.2f s remaining)\n", -delay);
        }
    }
}

int main() {
    int n;
    nbr_task(&n); // Input tasks

    if (schedulability(n)) {
        int hyperperiod = lcm_array(T_global, n);
        printf("Hyperperiod : %d\n", hyperperiod);

        // Creat de tasks
        Task task_list[10];
        for (int i = 0; i < n; i++) {
            task_list[i].id = i + 1;
            task_list[i].C = C_global[i];
            task_list[i].T = T_global[i];
        }

        edf_scheduler(task_list, n, hyperperiod);
    }
    
    // Free dynamically allocated memory
    free(C_global);
    free(T_global);

    return 0;
}

