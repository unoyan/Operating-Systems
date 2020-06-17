#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/types.h>
#include <linux/slab.h>
struct birthday {
    int day;
    int month;
    int year;
    struct list_head list;
};

static LIST_HEAD(birthday_list);

/* Module entry point */

int LList_init(void) {

/* Creating each birthday struct in list */

    printk(KERN_INFO "Creating birthday list\n");
    

    struct birthday *utku;
    utku = kmalloc(sizeof(*utku), GFP_KERNEL);
    utku->day = 17;
    utku->month = 4;
    utku->year = 1998;
    INIT_LIST_HEAD(&utku->list);
    list_add_tail(&utku->list, &birthday_list);

    struct birthday *onat;
    onat = kmalloc(sizeof(*onat), GFP_KERNEL);
    onat->day = 23;
    onat->month = 7;
    onat->year = 1999;
    INIT_LIST_HEAD(&onat->list);
    list_add_tail(&onat->list, &birthday_list);

    struct birthday *merve;
    merve = kmalloc(sizeof(*merve), GFP_KERNEL);
    merve->day = 15;
    merve->month = 10;
    merve->year = 1998;
    INIT_LIST_HEAD(&merve->list);
    list_add_tail(&merve->list, &birthday_list);

    struct birthday *beyza;
    beyza = kmalloc(sizeof(*beyza), GFP_KERNEL);
    beyza->day = 19;
    beyza->month = 12;
    beyza->year = 1997;
    INIT_LIST_HEAD(&beyza->list);
    list_add_tail(&beyza->list, &birthday_list);

    struct birthday *can;
    can = kmalloc(sizeof(*can), GFP_KERNEL);
    can->day = 17;
    can->month = 4;
    can->year = 1998;
    INIT_LIST_HEAD(&can->list);
    list_add_tail(&can->list, &birthday_list);

/* Traversing the list */

	printk(KERN_INFO "Traversing birthday list\n");

	struct birthday *birthdayPtr;

	list_for_each_entry(birthdayPtr, &birthday_list, list) {
		printk(KERN_INFO "BIRTHDAY: Month: %d Day: %d Year: %d\n", 
			birthdayPtr->month, 
			birthdayPtr->day,
			birthdayPtr->year);
	}

    return 0;
}

/* Module exit point */

void LList_exit(void) {

    printk(KERN_INFO "Removing module\n");

/* Removes each struct from list */
    struct birthday *ptr, *next;
	list_for_each_entry_safe(ptr, next, &birthday_list, list) {
		printk(KERN_INFO "REMOVING BIRTHDAY: Month: %d Day: %d Year: %d\n",
			ptr->month,
			ptr->day,
			ptr->year);

		list_del(&ptr->list);
		kfree(ptr);
	}
}

/* Register module entry/exit pts */

module_init(LList_init);
module_exit(LList_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Friend Birthday List");
MODULE_AUTHOR("Utku Noyan");
