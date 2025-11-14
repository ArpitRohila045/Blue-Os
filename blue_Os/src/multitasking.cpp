#include <multitasking.h>
#include <common/print.h>

using namespace blueOs;
using namespace blueOs::common;

/* --------------------------------------------------------
   TASK IMPLEMENTATION
-------------------------------------------------------- */

Task::Task(void entrypoint(), uint32_t burst, uint32_t prio)
{
    burstTime      = burst;
    remainingTime  = burst;
    priority       = prio;

    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));

    cpustate->eax = 0;
    cpustate->ebx = 0;
    cpustate->ecx = 0;
    cpustate->edx = 0;

    cpustate->esi = 0;
    cpustate->edi = 0;
    cpustate->ebp = 0;

    cpustate->eip = (uint32_t)entrypoint;
    cpustate->cs  = 0x08;
    cpustate->eflags = 0x202;   // interrupts enabled
}

Task::~Task() {}



/* --------------------------------------------------------
   TASK MANAGER IMPLEMENTATION
-------------------------------------------------------- */

TaskManager::TaskManager() {
    numTasks = 0;
    currentTask = -1;
}

TaskManager::~TaskManager() {}

bool TaskManager::addTask(Task* task) {
    if (numTasks < 256) {
        tasks[numTasks++] = task;
        return true;
    }
    return false;
}



/* ------------------------------
   ROUND ROBIN (DEFAULT)
-------------------------------- */

CPUState* TaskManager::schedule(CPUState* cpustate) {
    if (numTasks == 0)
        return cpustate;

    if (currentTask >= 0)
        tasks[currentTask]->cpustate = cpustate;

    currentTask = (currentTask + 1) % numTasks;

    return tasks[currentTask]->cpustate;
}



/* ------------------------------
   FCFS (NON-PREEMPTIVE)
-------------------------------- */

CPUState* TaskManager::fcfs(CPUState* cpustate) {
    if (numTasks == 0)
        return cpustate;

    if (currentTask == -1)
        currentTask = 0;

    tasks[currentTask]->cpustate = cpustate;
    return tasks[currentTask]->cpustate;
}



/* ------------------------------
   SJF (NON-PREEMPTIVE)
-------------------------------- */

CPUState* TaskManager::sjf(CPUState* cpustate) {
    if (numTasks == 0)
        return cpustate;

    if (currentTask >= 0)
        tasks[currentTask]->cpustate = cpustate;

    // If a task is running and not finished, continue it
    if (currentTask >= 0 && tasks[currentTask]->remainingTime > 0)
        return tasks[currentTask]->cpustate;

    // Choose shortest burst task
    int best = -1;
    for (int i = 0; i < numTasks; i++) {
        if (tasks[i]->remainingTime > 0) {
            if (best == -1 || tasks[i]->burstTime < tasks[best]->burstTime)
                best = i;
        }
    }

    if (best == -1)
        return cpustate;

    currentTask = best;
    return tasks[currentTask]->cpustate;
}



/* ------------------------------
   SRTF (PREEMPTIVE)
-------------------------------- */

CPUState* TaskManager::srtf(CPUState* cpustate) {
    if (numTasks == 0)
        return cpustate;

    if (currentTask >= 0) {
        tasks[currentTask]->cpustate = cpustate;
        tasks[currentTask]->remainingTime--;
    }

    int best = 0;
    for (int i = 1; i < numTasks; i++) {
        if (tasks[i]->remainingTime < tasks[best]->remainingTime)
            best = i;
    }

    currentTask = best;
    return tasks[currentTask]->cpustate;
}



/* ------------------------------
   PRIORITY SCHEDULING
-------------------------------- */

CPUState* TaskManager::prioritySchedule(CPUState* cpustate) {
    if (numTasks == 0)
        return cpustate;

    if (currentTask >= 0)
        tasks[currentTask]->cpustate = cpustate;

    int best = 0;
    for (int i = 1; i < numTasks; i++) {
        if (tasks[i]->priority < tasks[best]->priority)
            best = i;
    }

    currentTask = best;
    return tasks[currentTask]->cpustate;
}
