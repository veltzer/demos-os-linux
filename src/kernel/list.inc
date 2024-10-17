/*
 * List handling functions
 */

#include<linux/list.h> // for list handling functions
#include<linux/slab.h> // for kmalloc, kfree

/*
 * private structure to hold void pointers
 */
typedef struct _list_struct {
	struct list_head list;
	void* ptr; /* this is my specific data */
} list_struct;

/*
 * Create a list
 */
void *api_list_create(void);
void *api_list_create(void) {
	struct list_head *lp = kmalloc(sizeof(struct list_head), GFP_KERNEL);
	INIT_LIST_HEAD(lp);
	return(lp);
}

/*
 * Destroy a list
 * TODO: need to clean the list...
 */
void api_list_destroy(void*);
void api_list_destroy(void *lptr) {
	struct list_head *lp;
	if(lptr==NULL) {
		pr_err("destroy on null list");
		return;
	}
	lp=(struct list_head *)lptr;
	kfree(lp);
}

/*
 * Is empty call
 */
int api_list_isempty(void *lptr) {
	struct list_head *lp;
	if(lptr==NULL) {
		pr_err("isempty on null list");
		return 0;
	}
	lp=(struct list_head *)lptr;
	return(list_empty(lp));
}


/*
 * Init an iterator
 */
void *api_list_iter_init(void *lptr) {
	struct list_head *lp;
	if(lptr==NULL) {
		pr_err("iter_init on null list");
		return NULL;
	}
	lp=(struct list_head *)lptr;
	return(lp->next);
}

/*
 * Is the iterator over
 */
int api_list_iter_isover(void *lptr, void *iter) {
	struct list_head *lp;
	struct list_head *li;
	if(lptr==NULL) {
		pr_err("iter_isover on null list");
		return 0;
	}
	lp=(struct list_head *)lptr;
	li=(struct list_head *)iter;
	return(li == lp);
}

/*
 * Get the current value from the current iterator
 */
void *api_list_iter_getval(void *lptr, void *iter) {
	struct list_head* lp;
	list_struct* entry;
	if(lptr==NULL) {
		pr_err("iter_getval on null list");
		return NULL;
	}
	lp=(struct list_head *)iter;
	entry=list_entry(lp, struct _list_struct, list);
	return(entry->ptr);
}


/*
 * Advance the iterator one step
 */
void *api_list_iter_next(void *lptr, void *iter) {
	struct list_head* lp;
	if(lptr==NULL) {
		pr_err("iter_next on null list");
		return NULL;
	}
	lp=(struct list_head *)iter;
	return(lp->next);
}

/*
 * Delete an element at the current position of the iterator
 */
void api_list_iter_del(void *lptr, void *iter) {
	struct list_head *lp;
	list_struct* entry;
	if(lptr==NULL) {
		pr_err("iter_del on null list");
		return;
	}
	lp=(struct list_head *)iter;
	entry=list_entry(lp,struct _list_struct,list);
	kfree(entry);
}

/*
 * Pop an element and return its pointer
 */
void *api_list_del(void *lptr) {
	struct list_head *lp;
	struct list_head *prev;
	list_struct* entry;
	void* ret;
	if(lptr==NULL) {
		pr_err("del on null list");
		return NULL;
	}
	lp = (struct list_head *)lptr;
	prev = lp->prev;

	if (list_empty(lp)) {
		pr_err("ERROR - pop should not be called on an empty list");
		return NULL;
	} else {
		entry = list_entry(prev, struct _list_struct, list);
		ret = entry->ptr;
		list_del(prev);
		kfree(entry);
		return(ret);
	}
}


/*
 * Add a pointer
 */
void api_list_add(void *lptr, void *ptr) {
	struct list_head *lp = (struct list_head *)lptr;
	list_struct* entry;
	if(lptr==NULL) {
		pr_err("add on null list");
		return;
	}
	entry=kmalloc(sizeof(list_struct), GFP_KERNEL);

	entry->ptr = ptr;
	list_add(&entry->list, lp);
}


/*
 * Print the list
 */
void api_list_print(void *lptr) {
	struct list_head *lp = (struct list_head *)lptr;
	void* item;
	void* iter = api_list_iter_init(lp);
	if(lptr==NULL) {
		pr_err("print on null list");
		return;
	}

	while (!api_list_iter_isover(lp, iter)) {
		item = api_list_iter_getval(lp, iter);
		pr_info("item is %ld", (int)item);
		iter = api_list_iter_next(lp, iter);
	}
}
