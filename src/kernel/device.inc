// includes
#include<linux/moduleparam.h> // for module_param, MODULE_PARM_DESC
#include<linux/init.h> // for __init, __exit
#include<linux/slab.h> // for kmalloc
#include<linux/cdev.h> // for struct cdev, cdev_init, cdev_add, cdev_del
#include<linux/fs.h> // for fops
#include<linux/device.h> // for device support
#include<linux/version.h> /* for LINUX_VERSION_CODE, KERNEL_VERSION */

// structures
struct kern_dev {
	// pointer to the first device number allocated to us
	dev_t first_dev;
	// cdev structures for the char devices we expose to user space
	struct cdev cdev;
};

// static data
static struct kern_dev* pdev;
static struct class* my_class;

// parameters
static bool chrdev_alloc_dynamic = 1;
module_param(chrdev_alloc_dynamic, bool, 0444);
MODULE_PARM_DESC(chrdev_alloc_dynamic, "Allocate the device number dynamically?");

static int first_minor = 0;
module_param(first_minor, int, 0444);
MODULE_PARM_DESC(first_minor, "first minor to allocate in dynamic mode (usually best to keep at 0)");

static int kern_major = 253;
module_param(kern_major, int, 0444);
MODULE_PARM_DESC(kern_major, "major to allocate in static mode");

static int kern_minor = 0;
module_param(kern_minor, int, 0444);
MODULE_PARM_DESC(kern_minor, "minor to allocate in static mode");

static int minors_count = 1;
module_param(minors_count, int, 0444);
MODULE_PARM_DESC(minors_count, "how many minors to allocate");

// registration/unregistration functions
static int __init register_dev(void)
{
	int ret;
	// create a class
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,5,0)
	my_class = class_create(THIS_MODULE, THIS_MODULE->name);
#else
	my_class = class_create(THIS_MODULE->name);
#endif
	if (IS_ERR(my_class)) {
		ret=PTR_ERR(my_class);
		PR_ERROR("cannot class_create");
		goto goto_nothing;
	}
	PR_DEBUG("created the class");
	// alloc and zero
	pdev = kmalloc(sizeof(struct kern_dev), GFP_KERNEL);
	if (IS_ERR(pdev)) {
		PR_ERROR("cannot kmalloc");
		ret=PTR_ERR(pdev);
		goto goto_destroy;
	}
	memset(pdev, 0, sizeof(struct kern_dev));
	PR_DEBUG("set up the structure");
	if (chrdev_alloc_dynamic) {
		ret=alloc_chrdev_region(&pdev->first_dev, first_minor, minors_count, THIS_MODULE->name);
		if(ret) {
			PR_ERROR("cannot alloc_chrdev_region");
			goto goto_dealloc;
		}
	} else {
		pdev->first_dev = MKDEV(kern_major, kern_minor);
		ret=register_chrdev_region(pdev->first_dev, minors_count, THIS_MODULE->name);
		if(ret) {
			PR_ERROR("cannot register_chrdev_region");
			goto goto_dealloc;
		}
	}
	PR_DEBUG("allocated the device");
	// create the add the sync device
	cdev_init(&pdev->cdev, &my_fops);
	pdev->cdev.owner = THIS_MODULE;
	pdev->cdev.ops = &my_fops;
	kobject_set_name(&pdev->cdev.kobj, THIS_MODULE->name);
	ret=cdev_add(&pdev->cdev, pdev->first_dev, minors_count);
	if(ret) {
		PR_ERROR("cannot cdev_add");
		goto goto_deregister;
	}
	PR_DEBUG("added the device");
	// now register it in /dev
	my_device = device_create(
		my_class,/* our class */
		NULL,/* device we are subdevices of */
		pdev->first_dev,
		NULL,
		"%s",
		THIS_MODULE->name
	);
	if (IS_ERR(my_device)) {
		PR_ERROR("cannot create device");
		ret=PTR_ERR(my_device);
		goto goto_create_device;
	}
	PR_DEBUG("did device_create");
	return(0);

	//goto_all:
	//	device_destroy(my_class,pdev->first_dev);
	goto_create_device:
		cdev_del(&pdev->cdev);
	goto_deregister:
		unregister_chrdev_region(pdev->first_dev, minors_count);
	goto_dealloc:
		kfree(pdev);
	goto_destroy:
		class_destroy(my_class);
	goto_nothing:
		return ret;
}

static void __exit unregister_dev(void) {
	device_destroy(my_class, pdev->first_dev);
	cdev_del(&pdev->cdev);
	unregister_chrdev_region(pdev->first_dev, minors_count);
	kfree(pdev);
	class_destroy(my_class);
}

// init/exit functions
static int __init mod_init(void) {
	return(register_dev());
}

static void __exit mod_exit(void) {
	unregister_dev();
}

// declaration of init/exit functions of this module
module_init(mod_init);
module_exit(mod_exit);
