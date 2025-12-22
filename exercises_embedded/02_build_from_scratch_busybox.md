# Exercise: Building an ARM64 System from Scratch on Ubuntu x86_64

This exercise walks through building a complete minimal ARM64 Linux system from source using BusyBox — no pre-built distribution, no debootstrap.

---

## Prerequisites

- Ubuntu 20.04+ x86_64 host system
- At least 10GB free disk space
- Internet connection
- sudo privileges

---

## Overview

We will build:

1. **Cross-compilation toolchain** — compiles ARM64 binaries on x86_64
2. **Linux kernel** — the ARM64 kernel image
3. **BusyBox** — provides all userspace utilities in a single binary
4. **Root filesystem** — manually constructed directory structure
5. **Init system** — simple shell scripts to boot the system

---

## Step 1: Install the Cross-Compilation Toolchain

```bash
sudo apt update
sudo apt install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu \
    binutils-aarch64-linux-gnu
```

Install build dependencies:

```bash
sudo apt install -y build-essential libncurses-dev bison flex libssl-dev \
    libelf-dev bc git fakeroot cpio qemu-system-arm
```

Verify installation:

```bash
aarch64-linux-gnu-gcc --version
```

Set up environment variables (add to `~/.bashrc` for persistence):

```bash
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
```

---

## Step 2: Create Working Directory Structure

```bash
mkdir -p ~/arm64-scratch
cd ~/arm64-scratch
mkdir -p rootfs kernel busybox
```

---

## Step 3: Build the Linux Kernel

### 3.1 Download Kernel Source

```bash
cd ~/arm64-scratch/kernel
git clone --depth 1 --branch v6.6 \
    https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git .
```

### 3.2 Configure the Kernel

```bash
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-

# Start with default ARM64 config
make defconfig

# Enable useful options for our minimal system
./scripts/config --enable CONFIG_BLK_DEV_INITRD
./scripts/config --enable CONFIG_RD_GZIP
./scripts/config --enable CONFIG_DEVTMPFS
./scripts/config --enable CONFIG_DEVTMPFS_MOUNT
./scripts/config --enable CONFIG_TTY
./scripts/config --enable CONFIG_SERIAL_AMBA_PL011
./scripts/config --enable CONFIG_SERIAL_AMBA_PL011_CONSOLE
./scripts/config --enable CONFIG_VIRTIO
./scripts/config --enable CONFIG_VIRTIO_PCI
./scripts/config --enable CONFIG_VIRTIO_BLK
./scripts/config --enable CONFIG_VIRTIO_NET
./scripts/config --enable CONFIG_EXT4_FS
./scripts/config --enable CONFIG_TMPFS

# Update config
make olddefconfig
```

### 3.3 Build the Kernel

```bash
make -j$(nproc) Image modules dtbs
```

The kernel image will be at `arch/arm64/boot/Image`.

---

## Step 4: Build BusyBox

### 4.1 Download BusyBox

```bash
cd ~/arm64-scratch/busybox
git clone --depth 1 --branch 1_36_stable \
    https://git.busybox.net/busybox .
```

### 4.2 Configure BusyBox

```bash
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-

# Start with default config
make defconfig

# Enable static linking (important - no shared libraries needed)
./scripts/config --enable CONFIG_STATIC

# Optional: open menuconfig to customize
# make menuconfig
```

### 4.3 Build BusyBox

```bash
make -j$(nproc)
```

### 4.4 Install BusyBox to Rootfs

```bash
make CONFIG_PREFIX=${HOME}/arm64-scratch/rootfs install
```

This creates the basic directory structure with symlinks to BusyBox.

---

## Step 5: Create the Root Filesystem

### 5.1 Create Directory Structure

```bash
cd ~/arm64-scratch/rootfs

# Create required directories
mkdir -p proc sys dev etc etc/init.d tmp var run root home mnt

# Create device nodes (minimal set for boot)
sudo mknod -m 622 dev/console c 5 1
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 666 dev/zero c 1 5
sudo mknod -m 666 dev/tty c 5 0
sudo mknod -m 444 dev/urandom c 1 9
```

### 5.2 Create /etc/passwd

```bash
cat > etc/passwd << 'EOF'
root:x:0:0:root:/root:/bin/sh
daemon:x:1:1:daemon:/usr/sbin:/bin/false
nobody:x:65534:65534:nobody:/nonexistent:/bin/false
EOF
```

### 5.3 Create /etc/group

```bash
cat > etc/group << 'EOF'
root:x:0:
daemon:x:1:
tty:x:5:
disk:x:6:
wheel:x:10:root
nobody:x:65534:
EOF
```

### 5.4 Create /etc/shadow (optional, for password)

```bash
# Password is "root" - generated with: openssl passwd -1 root
cat > etc/shadow << 'EOF'
root:$1$xyz$X11gJHk1d1k3GmB1o3J1d1:19000:0:99999:7:::
EOF
chmod 600 etc/shadow
```

### 5.5 Create /etc/fstab

```bash
cat > etc/fstab << 'EOF'
# <device>    <mount>   <type>  <options>         <dump> <pass>
proc          /proc     proc    defaults          0      0
sysfs         /sys      sysfs   defaults          0      0
devtmpfs      /dev      devtmpfs defaults         0      0
tmpfs         /tmp      tmpfs   defaults          0      0
tmpfs         /run      tmpfs   defaults          0      0
EOF
```

### 5.6 Create /etc/hostname

```bash
echo "arm64-busybox" > etc/hostname
```

### 5.7 Create /etc/hosts

```bash
cat > etc/hosts << 'EOF'
127.0.0.1   localhost
127.0.1.1   arm64-busybox
::1         localhost ip6-localhost ip6-loopback
EOF
```

### 5.8 Create /etc/profile

```bash
cat > etc/profile << 'EOF'
export PATH=/bin:/sbin:/usr/bin:/usr/sbin
export HOME=/root
export TERM=linux
export PS1='\u@\h:\w\$ '

alias ll='ls -la'
alias la='ls -A'

echo "Welcome to ARM64 BusyBox Linux!"
EOF
```

### 5.9 Create /etc/inittab

```bash
cat > etc/inittab << 'EOF'
# Startup script
::sysinit:/etc/init.d/rcS

# Console on serial port (QEMU uses ttyAMA0)
::respawn:-/bin/sh
ttyAMA0::respawn:-/bin/sh

# Ctrl+Alt+Del
::ctrlaltdel:/sbin/reboot

# Shutdown
::shutdown:/bin/umount -a -r
::shutdown:/sbin/swapoff -a
EOF
```

### 5.10 Create the Init Script

```bash
cat > etc/init.d/rcS << 'EOF'
#!/bin/sh

# Mount virtual filesystems
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t devtmpfs devtmpfs /dev
mount -t tmpfs tmpfs /tmp
mount -t tmpfs tmpfs /run

# Create device nodes that devtmpfs might miss
mkdir -p /dev/pts
mount -t devpts devpts /dev/pts

# Set hostname
hostname -F /etc/hostname

# Bring up loopback interface
ip link set lo up

# Display boot info
echo ""
echo "========================================"
echo " ARM64 BusyBox Linux"
echo " Kernel: $(uname -r)"
echo " Arch:   $(uname -m)"
echo "========================================"
echo ""

# Drop to shell
exec /bin/sh
EOF

chmod +x etc/init.d/rcS
```

### 5.11 Create a Simple Init Wrapper (Alternative)

If you prefer a minimal `/init` at the root:

```bash
cat > init << 'EOF'
#!/bin/sh

# Emergency shell if something breaks
rescue_shell() {
    echo "Something went wrong. Dropping to rescue shell."
    exec /bin/sh
}

# Mount essentials
mount -t proc proc /proc || rescue_shell
mount -t sysfs sysfs /sys || rescue_shell
mount -t devtmpfs devtmpfs /dev || rescue_shell

# Continue to normal init
exec /sbin/init
EOF

chmod +x init
```

---

## Step 6: Create an Initramfs (Option A - Recommended for Testing)

The initramfs is a compressed cpio archive loaded into RAM at boot.

```bash
cd ~/arm64-scratch/rootfs

# Create the initramfs archive
find . | cpio -o -H newc | gzip > ../initramfs.cpio.gz

echo "Created: ~/arm64-scratch/initramfs.cpio.gz"
ls -lh ../initramfs.cpio.gz
```

---

## Step 7: Boot with QEMU Using Initramfs

```bash
cd ~/arm64-scratch

qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a72 \
    -m 512 \
    -kernel kernel/arch/arm64/boot/Image \
    -initrd initramfs.cpio.gz \
    -append "console=ttyAMA0 rdinit=/init" \
    -nographic
```

You should see the kernel boot and drop into a BusyBox shell!

Exit QEMU with: `Ctrl+A` then `X`

---

## Step 8: Create a Persistent Disk Image (Option B)

For a system with persistent storage instead of initramfs:

### 8.1 Create and Format the Disk Image

```bash
cd ~/arm64-scratch

# Create 256MB image
dd if=/dev/zero of=disk.img bs=1M count=256

# Format as ext4
mkfs.ext4 disk.img
```

### 8.2 Copy Rootfs to Disk Image

```bash
mkdir -p /tmp/arm64-mnt
sudo mount -o loop disk.img /tmp/arm64-mnt
cp -a rootfs/* /tmp/arm64-mnt/
sudo umount /tmp/arm64-mnt
```

### 8.3 Boot with Disk Image

```bash
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a72 \
    -m 512 \
    -kernel kernel/arch/arm64/boot/Image \
    -append "root=/dev/vda rw console=ttyAMA0 init=/sbin/init" \
    -drive if=virtio,file=disk.img,format=raw \
    -nographic
```

---

## Step 9: Add Networking (Optional)

### 9.1 Boot with Network Device

```bash
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a72 \
    -m 512 \
    -kernel kernel/arch/arm64/boot/Image \
    -initrd initramfs.cpio.gz \
    -append "console=ttyAMA0 rdinit=/init" \
    -netdev user,id=net0 \
    -device virtio-net-pci,netdev=net0 \
    -nographic
```

### 9.2 Configure Network Inside Guest

```bash
# Bring up interface
ip link set eth0 up

# Get IP via DHCP (if udhcpc is enabled in BusyBox)
udhcpc -i eth0

# Or set static IP
ip addr add 10.0.2.15/24 dev eth0
ip route add default via 10.0.2.2
echo "nameserver 8.8.8.8" > /etc/resolv.conf

# Test
ping -c 3 8.8.8.8
```

### 9.3 Create a Network Init Script

Add to the rootfs before creating initramfs:

```bash
cat > ~/arm64-scratch/rootfs/etc/init.d/network << 'EOF'
#!/bin/sh
ip link set lo up
ip link set eth0 up
udhcpc -i eth0 -s /etc/udhcpc.script -q
EOF
chmod +x ~/arm64-scratch/rootfs/etc/init.d/network
```

---

## Step 10: Verification Exercises

Once booted, verify your system:

```bash
# Check architecture
uname -a

# List all BusyBox applets
busybox --list

# Check mounted filesystems
mount

# View running processes
ps aux

# Check memory
free

# Explore /proc
cat /proc/cpuinfo
cat /proc/meminfo
cat /proc/version

# Test utilities
ls -la /
df -h
```

---

## Step 11: Customization Ideas

### Add More Device Nodes

```bash
# In rootfs/dev before creating initramfs
sudo mknod -m 666 dev/random c 1 8
sudo mknod -m 666 dev/tty0 c 4 0
sudo mknod -m 666 dev/tty1 c 4 1
sudo mknod -m 660 dev/loop0 b 7 0
```

### Add a Welcome Banner

```bash
cat > ~/arm64-scratch/rootfs/etc/motd << 'EOF'

    _   ___ __  __  __ _  _   
   /_\ | _ \  \/  |/ /| || |  
  / _ \|   / |\/| / _ \_  _| 
 /_/ \_\_|_\_|  |_\___/ |_|  
                              
 BusyBox Linux - Built from Scratch
 
EOF
```

### Add Custom Scripts

```bash
# A simple system info script
cat > ~/arm64-scratch/rootfs/usr/bin/sysinfo << 'EOF'
#!/bin/sh
echo "=== System Information ==="
echo "Hostname: $(hostname)"
echo "Kernel:   $(uname -r)"
echo "Arch:     $(uname -m)"
echo "Uptime:   $(uptime)"
echo "Memory:   $(free | grep Mem | awk '{print $3"/"$2}')"
echo "=========================="
EOF
chmod +x ~/arm64-scratch/rootfs/usr/bin/sysinfo
```

---

## Troubleshooting

### Kernel Panic: No init found

- Ensure `/init` or `/sbin/init` exists and is executable
- Check the `rdinit=` or `init=` kernel parameter
- Verify BusyBox was compiled with `init` applet enabled

### "sh: can't access tty"

- Add `ttyAMA0::respawn:-/bin/sh` to `/etc/inittab`
- Ensure console device nodes exist

### BusyBox: "not found" or Exec Format Error

- Verify you built BusyBox with `CONFIG_STATIC=y`
- Check you used the correct cross-compiler
- Verify with: `file rootfs/bin/busybox` — should show "ARM aarch64"

### Kernel Boots but No Shell

- Check that `/etc/inittab` has a respawn entry for the console
- Try adding `init=/bin/sh` to kernel command line to bypass init

### Network Device Not Found

- Ensure virtio modules are built into kernel
- Check `ip link` to see available interfaces

---

## Quick Reference: Complete Build Script

Save this as `build-arm64.sh`:

```bash
#!/bin/bash
set -e

WORKDIR=${HOME}/arm64-scratch
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-

echo "=== Creating directories ==="
mkdir -p $WORKDIR/{kernel,busybox,rootfs}

echo "=== Building Kernel ==="
cd $WORKDIR/kernel
if [ ! -d ".git" ]; then
    git clone --depth 1 --branch v6.6 \
        https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git .
fi
make defconfig
make -j$(nproc) Image

echo "=== Building BusyBox ==="
cd $WORKDIR/busybox
if [ ! -d ".git" ]; then
    git clone --depth 1 --branch 1_36_stable \
        https://git.busybox.net/busybox .
fi
make defconfig
sed -i 's/# CONFIG_STATIC is not set/CONFIG_STATIC=y/' .config
make -j$(nproc)
make CONFIG_PREFIX=$WORKDIR/rootfs install

echo "=== Creating rootfs ==="
cd $WORKDIR/rootfs
mkdir -p proc sys dev etc etc/init.d tmp var run root

# Create init script
cat > etc/init.d/rcS << 'INITEOF'
#!/bin/sh
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t devtmpfs devtmpfs /dev
echo "ARM64 BusyBox Linux - Kernel $(uname -r)"
INITEOF
chmod +x etc/init.d/rcS

# Create inittab
cat > etc/inittab << 'INITTAB'
::sysinit:/etc/init.d/rcS
::respawn:-/bin/sh
ttyAMA0::respawn:-/bin/sh
INITTAB

echo "=== Creating initramfs ==="
cd $WORKDIR/rootfs
find . | cpio -o -H newc | gzip > $WORKDIR/initramfs.cpio.gz

echo ""
echo "=== Build Complete ==="
echo "Kernel: $WORKDIR/kernel/arch/arm64/boot/Image"
echo "Initrd: $WORKDIR/initramfs.cpio.gz"
echo ""
echo "Run with:"
echo "qemu-system-aarch64 -machine virt -cpu cortex-a72 -m 512 \\"
echo "  -kernel $WORKDIR/kernel/arch/arm64/boot/Image \\"
echo "  -initrd $WORKDIR/initramfs.cpio.gz \\"
echo "  -append 'console=ttyAMA0' -nographic"
```

---

## Summary

| Component | Source | Output |
|-----------|--------|--------|
| Toolchain | apt packages | `aarch64-linux-gnu-gcc` |
| Kernel | kernel.org | `arch/arm64/boot/Image` |
| Userspace | BusyBox | Single static binary |
| Rootfs | Hand-crafted | Directory tree |
| Boot image | cpio + gzip | `initramfs.cpio.gz` |

Total system size: ~10-15MB (kernel + initramfs)

---

## Next Steps

- Add kernel modules to the initramfs
- Build a real init system (e.g., OpenRC, runit)
- Add a C library (musl) and compile custom programs
- Create an SD card image for real hardware
- Add a package manager (opkg)
- Set up cross-compilation for custom applications
