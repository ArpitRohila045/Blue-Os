#include <multitasking.h>
#include <common/print.h>


using namespace blueOs;
using namespace blueOs::common;

/*
 * ----------------------------
 *  Task Class Implementation
 * ----------------------------
 * Each Task object represents a single running task (or thread).
 * It has its own stack and CPU state, so the scheduler can switch
 * between them safely.
 */
Task::Task(void entrypoint()) {
    /*
     * Every task gets a 4 KB stack (4096 bytes).
     * We initialize the CPU state at the very top of the stack,
     * because in x86, the stack grows downward.
     */
    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));

    // Initialize general-purpose registers to 0
    cpustate->eax = 0;
    cpustate->ebx = 0;
    cpustate->ecx = 0;
    cpustate->edx = 0;

    cpustate->esi = 0;
    cpustate->edi = 0;
    cpustate->ebp = 0;

    /*
     * These segment registers are usually set by the kernel or
     * restored during context switching, so they’re commented out here.
     */
    // cpustate->gs = 0;
    // cpustate->fs = 0;
    // cpustate->es = 0;
    // cpustate->ds = 0;

    /*
     * The instruction pointer (EIP) tells the CPU where to start executing
     * when this task is scheduled. We set it to the address of the
     * entry function passed to the constructor.
     */
    cpustate->eip = (uint32_t)entrypoint;

    /*
     * Code segment selector (CS) — defines which privilege level
     * and memory segment this task runs in.
     * For kernel-level multitasking, this usually points to SEG_KCODE.
     */
    cpustate->cs = 0x08; // Kernel code segment selector

    /*
     * EFLAGS is set with bit 9 (the interrupt enable flag) = 1.
     * 0x202 means:
     *   Bit 1: always set
     *   Bit 9: IF = 1 (interrupts enabled)
     */
    cpustate->eflags = 0x202;
}

/*
 * Destructor for Task — currently does nothing.
 * You could later free the stack or other resources here.
 */
Task::~Task() {}


/*
 * ----------------------------
 *  TaskManager Implementation
 * ----------------------------
 * The TaskManager keeps track of all running tasks,
 * performs context switches, and schedules the next task.
 */
TaskManager::TaskManager() {
    numTasks = 0;
    currentTask = -1;
}

TaskManager::~TaskManager() {}


/*
 * Adds a new task to the scheduler.
 * Returns true if successful, false if task limit (256) reached.
 */
bool TaskManager::addTask(Task* task) {
    if (numTasks < 256) {
        tasks[numTasks++] = task;
        return true;
    }
    return false;
}


/*
 * The core of the multitasking system.
 * Called from the timer interrupt (scheduler tick).
 *
 * - Saves the current CPU state into the currently running task.
 * - Selects the next task in a round-robin fashion.
 * - Returns the next task's CPU state pointer, which the interrupt
 *   handler will use to restore registers and resume execution.
 */
CPUState* TaskManager::schedule(CPUState* cpustate) {
    // No tasks yet — continue running the same one.
    if (numTasks == 0) {
        return cpustate;
    }

    // Save current task state
    if (currentTask >= 0) {
        tasks[currentTask]->cpustate = cpustate;
    }

    // Pick next task (round-robin scheduling)
    currentTask = (currentTask + 1) % numTasks;

    // Return the CPU state of the next task
    return tasks[currentTask]->cpustate;
}
