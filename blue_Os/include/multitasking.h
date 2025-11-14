#ifndef __BLUE_OS__MULTITASKING_H
#define __BLUE_OS__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>


namespace blueOs{
    struct CPUState
    {
        blueOs::common::uint32_t eax;
        blueOs::common::uint32_t ebx;
        blueOs::common::uint32_t ecx;
        blueOs::common::uint32_t edx;

        blueOs::common::uint32_t esi;
        blueOs::common::uint32_t edi;
        blueOs::common::uint32_t ebp;

        /*
        commmon::uint32_t gs;
        commmon::uint32_t fs;
        commmon::uint32_t es;
        commmon::uint32_t ds;
        */

        blueOs::common::uint32_t error;

        blueOs::common::uint32_t eip;
        blueOs::common::uint32_t cs;
        blueOs::common::uint32_t eflags;
        blueOs::common::uint32_t esp;
        blueOs::common::uint32_t ss;        
    } __attribute__((packed));

    class Task{
        friend class TaskManager;
        private:
            blueOs::common::uint8_t stack[4096];
            CPUState* cpustate;
        public:
            Task(void entrypoint());
            ~Task();
    };

    class TaskManager{
        private:
            Task* tasks[256];
            int numTasks;
            int currentTask;
        public:
            TaskManager();
            ~TaskManager();
            bool addTask(Task* task);
            CPUState* schedule(CPUState* cpustate); 
    };
}

#endif
