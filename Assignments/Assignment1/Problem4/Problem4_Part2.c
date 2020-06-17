#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/pid.h>


int p_id = -1;
module_param(p_id, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(p_id, "An integer");


int iterate_init(void) {
    struct pid *pid_struct;
    struct task_struct *task;        /*    Structure defined in sched.h for tasks/processes    */
    struct task_struct *task_child;        /*    Structure needed to iterate through task children    */
    struct list_head *list;            /*    Structure needed to iterate through the list in each task->children struct    */
    printk(KERN_INFO "%s", "LOADING MODULE\n");
    pid_struct = find_get_pid(p_id);
    task = pid_task(pid_struct,PIDTYPE_PID);
    struct task_struct *parent = task->parent;
    if(task == NULL) {
            if(p_id == -1){
                printk(KERN_INFO "%s", "NOT ENTERED PID!!!\n");
            }else{
                printk(KERN_INFO "ENTERED INVALID PID %d\n", p_id);
            }
        
    }else{
        long int stat = task->state;
        long int stat_exit = task->exit_state;
        if (stat == TASK_RUNNING){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_RUNNING", parent->pid); 
        }else if (stat == TASK_INTERRUPTIBLE){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_INTERRUPTIBLE", parent->pid);
        }else if (stat == TASK_UNINTERRUPTIBLE){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_UNINTERRUPTIBLE", parent->pid);
        }else if (stat == __TASK_STOPPED){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "__TASK_STOPPED", parent->pid);
        }else if (stat == __TASK_TRACED){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "__TASK_TRACED", parent->pid);
        }else if (stat == TASK_DEAD){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_DEAD", parent->pid);
        }else if (stat == TASK_WAKEKILL){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_WAKEKILL", parent->pid);
        }else if (stat == TASK_WAKING){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_WAKING", parent->pid);
        }else if (stat == TASK_PARKED){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_WAKING", parent->pid);
        }else if (stat_exit == EXIT_DEAD){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "EXIT_DEAD", parent->pid);
        }else if (stat_exit == EXIT_ZOMBIE){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "EXIT_ZOMBIE", parent->pid);
        }else if (stat_exit == EXIT_TRACE){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "EXIT_TRACE", parent->pid);
        }else if (stat == TASK_PARKED){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_PARKED", parent->pid);
        }else if (stat == TASK_DEAD){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_DEAD", parent->pid);
        }else if (stat == TASK_WAKEKILL){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_WAKEKILL", parent->pid);
        }else if (stat == TASK_WAKING){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_WAKING", parent->pid);
        }else if (stat == TASK_NOLOAD){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_NOLOAD", parent->pid);
        }else if (stat == TASK_NEW){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_NEW", parent->pid);
        }else if (stat == TASK_STATE_MAX){
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %s AND HAS PARENT PID: %d",task->comm, "TASK_STATE_MAX", parent->pid);
        }else {
                printk(KERN_INFO "\nGIVEN PROCESS: %s IS IN STATE: %ld AND HAS PARENT PID: %d ",task->comm, stat, parent->pid);
        }


        list_for_each(list, &parent->children){
                task_child = list_entry( list, struct task_struct, sibling );
                if(task->pid != task_child->pid){
                        printk(KERN_INFO "\nSIBLING OF %s[%d] PID: %d PROCESS: %s",task->comm, task->pid,task_child->pid, task_child->comm);
                }      
        }
    }
    return 0;
}
    


void cleanup_exit(void) {

    printk(KERN_INFO "Removing module\n");


}

module_init(iterate_init);
module_exit(cleanup_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Process Status");
MODULE_AUTHOR("Utku Noyan");

