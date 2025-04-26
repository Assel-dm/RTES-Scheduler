# EDF Scheduler (Non-Preemptive)

## Project Description

This project implements a non-preemptive Earliest Deadline First (EDF) scheduler in C for a given set of real-time periodic tasks.
It checks the schedulability of the system, generates all jobs over the hyperperiod, schedules them, and analyzes deadline compliance and total waiting time.

## Features

- Schedulability Test based on CPU utilization.
- Hyperperiod Calculation using the Least Common Multiple (LCM) of task periods.
- Job Generation up to the hyperperiod.
- Non-Preemptive EDF Scheduling:
  - Execute job with the earliest deadline first.
  - Task 5 is allowed to miss deadlines in a second scenario.
- Metrics Computed:
  - Response times,
  - Waiting times,
  - Deadline compliance,
  - Total waiting time across all jobs.
- Execution Order printed in the form Tij (Task i, Job j).

## How to Compile and Run

1. Compile the program:

   gcc main.c -o scheduler

2. Run the program:

   ./scheduler

You will be prompted to input:
- Number of tasks,
- Computation time (C) and period (T) for each task.

The program will then:
- Test if the task set is schedulable,
- If schedulable, simulate the execution over the hyperperiod and print detailed results.

## Example

Select the number of tasks to schedule: 3
Enter C and T for task 1: 2 10
Enter C and T for task 2: 3 20
Enter C and T for task 3: 2 40

✅ Tasks are schedulable!
Hyperperiod: 40

===== EDF Scheduling =====
...
Execution Order:
T11 -> T21 -> T12 -> T31 -> T22 -> ...

Total waiting time across all jobs: 5 time units

## Project Author

- Name: Axel Fouchard
- Date: April 2025
- School: IPSA Toulouse

