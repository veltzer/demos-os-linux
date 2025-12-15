# Exercise: Building an ARM64 System on Ubuntu x86_64

This exercise walks through setting up a cross-compilation environment and building a minimal ARM64 Linux system from an x86_64 Ubuntu host.

---

## Prerequisites

- Ubuntu 20.04+ x86_64 host system
- At least 20GB free disk space
- Internet connection
- sudo privileges

---

## Step 1: Install the Cross-Compilation Toolchain

Install the ARM64 cross-compiler and related tools:

```bash
sudo apt update
sudo apt install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu \
    binutils-aarch64-linux-gnu crossbuild-essential-arm64
```

Verify installation:

```bash
aarch64-linux-gnu-gcc --version
```

Expected output should show the GCC version targeting `aarch64-linux-gnu`.

---

## Step 2: Install Build Dependencies and QEMU

Install kernel build dependencies and QEMU for testing:

```bash
sudo apt install -y build-essential libncurses-dev bison flex libssl-dev \
    libelf-dev bc git fakeroot rsync kmod cpio \
    qemu-system-arm qemu-user-static debootstrap
```

Verify QEMU installation:

```bash
qemu-system-aarch64 --version
```

---

## Step 3: Build the Linux Kernel for ARM64

### 3.1 Clone the Kernel Source

```bash
# Clone a stable kernel release (use --depth 1 for faster download)
git clone --depth 1 --branch v6.6 \
    https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
cd linux
```

### 3.2 Set Up Cross-Compile Environment

```bash
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
```

### 3.3 Configure the Kernel

```bash
# Use the default ARM64 configuration
make defconfig

# Optional: customize the configuration
# make menuconfig
```

### 3.4 Build the Kernel

```bash
# Build kernel image, modules, and device trees
make -j$(nproc) Image modules dtbs
```

Build artifacts:
- Kernel image: `arch/arm64/boot/Image`
- Device trees: `arch/arm64/boot/dts/`
- Modules: throughout the source tree

### 3.5 Prepare Modules for Installation

```bash
# Create a temporary directory for modules
mkdir -p ../modules_install
make modules_install INSTALL_MOD_PATH=../modules_install
```

---

## Step 4: Create a Root Filesystem with Debootstrap

### 4.1 Bootstrap the Base System

```bash
cd ~

# Create rootfs directory
mkdir -p ~/arm64-rootfs

# Bootstrap a minimal Debian arm64 system
sudo debootstrap --arch=arm64 --foreign bookworm \
    ~/arm64-rootfs http://deb.debian.org/debian
```

### 4.2 Set Up QEMU User Emulation

```bash
# Copy QEMU static binary for chroot emulation
sudo cp /usr/bin/qemu-aarch64-static ~/arm64-rootfs/usr/bin/
```

### 4.3 Complete the Bootstrap

```bash
# Complete the second stage of debootstrap
sudo chroot ~/arm64-rootfs /debootstrap/debootstrap --second-stage
```

---

## Step 5: Configure the Root Filesystem

### 5.1 Enter the Chroot Environment

```bash
sudo chroot ~/arm64-rootfs /bin/bash
```

### 5.2 Basic System Configuration

Run these commands inside the chroot:

```bash
# Set root password
passwd root

# Set hostname
echo "arm64-test" > /etc/hostname

# Configure hosts file
cat > /etc/hosts << EOF
127.0.0.1   localhost
127.0.1.1   arm64-test
EOF

# Configure apt sources
cat > /etc/apt/sources.list << EOF
deb http://deb.debian.org/debian bookworm main contrib non-free non-free-firmware
deb http://deb.debian.org/debian bookworm-updates main contrib non-free non-free-firmware
deb http://security.debian.org/debian-security bookworm-security main contrib non-free non-free-firmware
EOF

# Update and install essential packages
apt update
apt install -y systemd systemd-sysv udev kmod sudo vim net-tools iputils-ping

# Enable serial console for QEMU
systemctl enable serial-getty@ttyAMA0.service

# Create a non-root user (optional)
useradd -m -s /bin/bash -G sudo developer
passwd developer

# Exit chroot
exit
```

### 5.3 Install Kernel Modules

```bash
# Copy kernel modules to rootfs
sudo cp -a ~/linux/../modules_install/lib/modules ~/arm64-rootfs/lib/
```

---

## Step 6: Create a Bootable Disk Image

### 6.1 Create the Image File

```bash
# Create a 2GB sparse image
dd if=/dev/zero of=${HOME}/arm64-disk.img bs=1M count=2048
```

### 6.2 Format the Image

```bash
# Format as ext4
mkfs.ext4 ~/arm64-disk.img
```

### 6.3 Copy the Root Filesystem

```bash
# Mount the image
mkdir -p /tmp/arm64-mnt
sudo mount -o loop ~/arm64-disk.img /tmp/arm64-mnt

# Copy rootfs contents
sudo cp -a ~/arm64-rootfs/* /tmp/arm64-mnt/

# Unmount
sudo umount /tmp/arm64-mnt
```

---

## Step 7: Boot with QEMU

### 7.1 Basic Boot Command

```bash
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a72 \
    -m 2048 \
    -kernel ~/linux/arch/arm64/boot/Image \
    -append "root=/dev/vda rw console=ttyAMA0" \
    -drive if=virtio,file=${HOME}/arm64-disk.img,format=raw \
    -nographic
```

### 7.2 Boot with Networking

```bash
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a72 \
    -m 2048 \
    -kernel ~/linux/arch/arm64/boot/Image \
    -append "root=/dev/vda rw console=ttyAMA0" \
    -drive if=virtio,file=${HOME}/arm64-disk.img,format=raw \
    -netdev user,id=net0,hostfwd=tcp::2222-:22 \
    -device virtio-net-pci,netdev=net0 \
    -nographic
```

SSH access: `ssh -p 2222 developer@localhost`

### 7.3 Exit QEMU

Press `Ctrl+A` then `X` to exit QEMU.

---

## Step 8: Verification Exercises

Once booted, verify your ARM64 system:

```bash
# Check architecture
uname -m
# Expected: aarch64

# Check kernel version
uname -r

# View CPU info
cat /proc/cpuinfo

# Check loaded modules
lsmod

# Test package installation
apt update && apt install -y htop
htop
```

---

## Alternative: Using Buildroot

For a more minimal/embedded system, use Buildroot:

```bash
# Clone Buildroot
git clone https://git.buildroot.net/buildroot
cd buildroot

# Use QEMU ARM64 defconfig
make qemu_aarch64_virt_defconfig

# Optional: customize
# make menuconfig

# Build (takes 30-60 minutes)
make -j$(nproc)
```

Boot the Buildroot system:

```bash
qemu-system-aarch64 \
    -M virt \
    -cpu cortex-a72 \
    -m 1024 \
    -kernel output/images/Image \
    -drive file=output/images/rootfs.ext4,if=virtio,format=raw \
    -append "root=/dev/vda console=ttyAMA0" \
    -nographic
```

---

## Alternative: Using Yocto Project

For production-grade embedded systems:

```bash
# Install dependencies
sudo apt install -y gawk wget git diffstat unzip texinfo gcc \
    build-essential chrpath socat cpio python3 python3-pip \
    python3-pexpect xz-utils debianutils iputils-ping \
    python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev \
    pylint xterm python3-subunit mesa-common-dev zstd liblz4-tool

# Clone Poky (Yocto reference distribution)
git clone -b scarthgap git://git.yoctoproject.org/poky
cd poky

# Initialize build environment
source oe-init-build-env build-arm64

# Configure for ARM64 QEMU
# Edit conf/local.conf and set:
# MACHINE = "qemuarm64"

# Build minimal image
bitbake core-image-minimal

# Run in QEMU
runqemu qemuarm64 nographic
```

---

## Troubleshooting

### Kernel Panic: VFS Unable to Mount Root

- Ensure the disk image path is correct
- Verify the root filesystem was copied correctly
- Check that ext4 filesystem support is built into the kernel

### Debootstrap Fails

- Ensure `qemu-user-static` is installed and registered with binfmt
- Try: `sudo update-binfmts --enable qemu-aarch64`

### Slow QEMU Performance

- Enable KVM if available: add `-enable-kvm` (requires ARM64 host or TCG)
- Reduce memory: `-m 512`
- Use fewer CPU cores in guest

### Network Not Working in QEMU

Inside the guest:

```bash
# Bring up interface
ip link set eth0 up
dhclient eth0

# Or configure statically
ip addr add 10.0.2.15/24 dev eth0
ip route add default via 10.0.2.2
echo "nameserver 8.8.8.8" > /etc/resolv.conf
```

---

## Quick Reference: Environment Variables

```bash
# Add to ~/.bashrc for convenience
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
export KERNEL_SRC=${HOME}/linux
export ROOTFS=${HOME}/arm64-rootfs
```

---

## Summary

| Step | Action | Output |
|------|--------|--------|
| 1 | Install toolchain | `aarch64-linux-gnu-gcc` |
| 2 | Install dependencies | QEMU, build tools |
| 3 | Build kernel | `arch/arm64/boot/Image` |
| 4 | Create rootfs | `~/arm64-rootfs/` |
| 5 | Configure rootfs | Users, networking, services |
| 6 | Create disk image | `arm64-disk.img` |
| 7 | Boot with QEMU | Running ARM64 system |

---

## Next Steps

- Deploy to real hardware (e.g., Raspberry Pi 4, SolidRun boards)
- Add custom kernel modules
- Build a custom device tree
- Create a reproducible build with Yocto or Buildroot
- Set up automated CI/CD for your embedded system
