# Exercise: Building a Linux Kernel Module for a Memory-Based Block Device

In this exercise, you'll create a simple Linux kernel module that implements a block device using system memory. This type of device is often called a RAM disk.

## Objectives

* Create a kernel module that registers a block device
* Implement read and write operations using memory as storage
* Handle basic I/O requests
* Learn about Linux kernel module development and block device drivers

## Prerequisites

* A Linux development environment
* Basic knowledge of C programming
* Familiarity with Linux kernel concepts

## Steps

### 1. Set up the development environment

Ensure you have the necessary tools installed:

```bash
sudo apt-get update
sudo apt-get install build-essential linux-headers-$(uname -r)
```

### 2. Create the module source file

Create a new file named `memory_block.c` with the following content:

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/vmalloc.h>

#define DEVICE_NAME "memory_block"
#define SECTOR_SIZE 512
#define DEVICE_SIZE (1024 * 1024) // 1MB

static int major_num = 0;
static u8 *device_data;
static struct gendisk *memory_disk = NULL;

static int memory_open(struct block_device *bdev, fmode_t mode)
{
    return 0;
}

static void memory_release(struct gendisk *disk, fmode_t mode)
{
}

static struct block_device_operations memory_ops = {
    .owner = THIS_MODULE,
    .open = memory_open,
    .release = memory_release,
};

static void memory_transfer(struct request *req)
{
    int dir = rq_data_dir(req);
    sector_t start_sector = blk_rq_pos(req);
    unsigned long nsect = blk_rq_sectors(req);
    struct bio_vec bvec;
    struct req_iterator iter;
    unsigned long offset;

    offset = start_sector * SECTOR_SIZE;

    rq_for_each_segment(bvec, req, iter) {
        void *buffer = page_address(bvec.bv_page) + bvec.bv_offset;
        unsigned len = bvec.bv_len;

        if (dir == WRITE)
            memcpy(device_data + offset, buffer, len);
        else
            memcpy(buffer, device_data + offset, len);

        offset += len;
    }
}

static void memory_request(struct request_queue *q)
{
    struct request *req;

    while ((req = blk_fetch_request(q)) != NULL) {
        if (req->cmd_type != REQ_TYPE_FS) {
            __blk_end_request_all(req, -EIO);
            continue;
        }

        memory_transfer(req);
        __blk_end_request_all(req, 0);
    }
}

static int __init memory_block_init(void)
{
    device_data = vmalloc(DEVICE_SIZE);
    if (!device_data)
        return -ENOMEM;

    major_num = register_blkdev(major_num, DEVICE_NAME);
    if (major_num <= 0) {
        vfree(device_data);
        return -EBUSY;
    }

    memory_disk = alloc_disk(1);
    if (!memory_disk) {
        unregister_blkdev(major_num, DEVICE_NAME);
        vfree(device_data);
        return -ENOMEM;
    }

    memory_disk->major = major_num;
    memory_disk->first_minor = 0;
    memory_disk->fops = &memory_ops;
    memory_disk->private_data = NULL;
    sprintf(memory_disk->disk_name, DEVICE_NAME);
    set_capacity(memory_disk, DEVICE_SIZE / SECTOR_SIZE);

    memory_disk->queue = blk_init_queue(memory_request, NULL);
    if (!memory_disk->queue) {
        put_disk(memory_disk);
        unregister_blkdev(major_num, DEVICE_NAME);
        vfree(device_data);
        return -ENOMEM;
    }

    add_disk(memory_disk);

    printk(KERN_INFO "Memory block device initialized\n");
    return 0;
}

static void __exit memory_block_exit(void)
{
    if (memory_disk) {
        del_gendisk(memory_disk);
        put_disk(memory_disk);
    }

    if (memory_disk->queue)
        blk_cleanup_queue(memory_disk->queue);

    unregister_blkdev(major_num, DEVICE_NAME);
    vfree(device_data);

    printk(KERN_INFO "Memory block device unloaded\n");
}

module_init(memory_block_init);
module_exit(memory_block_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple memory-based block device");
```

### 3. Create a Makefile

Create a `Makefile` in the same directory with the following content:

```makefile
obj-m += memory_block.o

all:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

### 4. Build the module

Compile the module by running:

```bash
make
```

### 5. Load the module

Load the module into the kernel:

```bash
sudo insmod memory_block.ko
```

### 6. Test the module

Create a filesystem on the device and mount it:

```bash
sudo mkfs.ext4 /dev/memory_block
sudo mkdir /mnt/memory_block
sudo mount /dev/memory_block /mnt/memory_block
```

Now you can use the mounted directory to read and write files, which will be stored in memory.

### 7. Unload the module

When you're done, unmount the filesystem and unload the module:

```bash
sudo umount /mnt/memory_block
sudo rmmod memory_block
```

## Conclusion

This exercise introduces you to creating a simple block device driver as a Linux kernel module. It demonstrates key concepts such as registering a block device, handling I/O requests, and managing memory for storage. Remember that this is a basic implementation and lacks many features of a production-ready driver, such as proper error handling and optimizations.
