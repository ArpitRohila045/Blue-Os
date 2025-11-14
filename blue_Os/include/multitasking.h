#ifndef __BLUE_OS__MULTITASKING_H
#define __BLUE_OS__MULTITASKING_H

#include <common/types.h>

namespace blueOs {

    struct CPUState {
        blueOs::common::uint32_t eax;
        blueOs::common::uint32_t ebx;
        blueOs::common::uint32_t ecx;
        blueOs::common::uint32_t edx;

        blueOs::common::uint32_t esi;
        blueOs::common::uint32_t edi;
        blueOs::common::uint32_t ebp;

        blueOs::common::uint32_t error;

        blueOs::common::uint32_t eip;
        blueOs::common::uint32_t cs;
        blueOs::common::uint32_t eflags;
        blueOs::common::uint32_t esp;
        blueOs::common::uint32_t ss;
    } __attribute__((packed));


    class Task {
        friend class TaskManager;

    private:
        blueOs::common::uint8_t stack[4096];
        CPUState* cpustate;

    public:
        blueOs::common::uint32_t burstTime;
        blueOs::common::uint32_t remainingTime;
        blueOs::common::uint32_t priority;

        Task(void entrypoint(),
             blueOs::common::uint32_t burst = 10,
             blueOs::common::uint32_t prio = 1);

        ~Task();
    };


    class TaskManager {
    private:
        Task* tasks[256];
        int numTasks;
        int currentTask;

    public:
        TaskManager();
        ~TaskManager();

        bool addTask(Task* task);

        CPUState* schedule(CPUState* cpustate);

        // additional scheduling methods:
        CPUState* fcfs(CPUState* cpustate);
        CPUState* sjf(CPUState* cpustate);
        CPUState* srtf(CPUState* cpustate);
        CPUState* prioritySchedule(CPUState* cpustate);
    };

}

#endif
