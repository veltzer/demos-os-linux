# Exercise: Building an ARM64 System with the Yocto Project

## Introduction

In previous exercises, we built ARM64 Linux systems manually with BusyBox and automatically with Buildroot. Now we'll explore the **Yocto Project** — the industry-standard build system for production embedded Linux.

Yocto is more complex than Buildroot, but offers unparalleled flexibility, scalability, and is the choice of major companies (Intel, AMD, NXP, TI, Automotive Grade Linux, etc.) for commercial embedded products.

---

## Learning Objectives

By the end of this exercise, you will be able to:

1. Understand Yocto Project architecture and terminology
2. Set up a Yocto build environment
3. Build a minimal ARM64 image for QEMU
4. Navigate and understand layers, recipes, and BitBake
5. Customize images with additional packages
6. Create custom layers and recipes
7. Debug common build issues

---

## What is the Yocto Project?

The Yocto Project is an **open-source collaboration project** that provides:

- **BitBake** — The task execution engine (like make, but for distributions)
- **OpenEmbedded-Core (OE-Core)** — Core metadata (recipes, classes)
- **Poky** — Reference distribution combining BitBake + OE-Core
- **Extensive BSP ecosystem** — Board Support Packages for hundreds of boards

### Key Characteristics

| Feature | Description |
|---------|-------------|
| **Layer-based** | Modular architecture separates concerns |
| **Recipe-driven** | Each package defined by a recipe (.bb file) |
| **Highly configurable** | Override anything at any level |
| **Reproducible** | Shared State cache enables binary reproducibility |
| **Scalable** | Used for tiny IoT devices to automotive systems |
| **Industry standard** | Backed by Linux Foundation, used by major vendors |

---

## Yocto vs. Buildroot vs. Manual

| Aspect | Manual | Buildroot | Yocto |
|--------|--------|-----------|-------|
| Learning curve | Medium | Low | High |
| Setup time | Hours | Minutes | Hour+ |
| Build time | Minutes | 30-60 min | 2-4 hours |
| Packages | DIY | ~2,500 | ~10,000+ |
| Customization | Total control | Good | Excellent |
| Scalability | Poor | Medium | Excellent |
| Commercial support | None | Limited | Extensive |
| Layer ecosystem | None | None | Vast |
| License compliance | Manual | Basic | Comprehensive |
| Use case | Learning | Small/medium projects | Production systems |

---

## Yocto Terminology

Before we begin, let's understand the key terms:

| Term | Definition |
|------|------------|
| **Poky** | Reference distribution; starting point for custom distros |
| **BitBake** | Build engine that parses recipes and executes tasks |
| **Recipe (.bb)** | Instructions to build a single package |
| **Layer** | Collection of related recipes (starts with `meta-`) |
| **Image** | Root filesystem specification (special recipe) |
| **Machine** | Hardware definition (CPU, features, bootloader) |
| **Distro** | Distribution policy (init system, libc, features) |
| **BSP** | Board Support Package — machine configs + drivers |
| **Class (.bbclass)** | Shared build logic inherited by recipes |
| **Conf files** | Configuration (local.conf, bblayers.conf) |
| **Sstate** | Shared State cache — binary build artifacts |
| **Task** | Build step (fetch, unpack, configure, compile, etc.) |

### The Layer Model

```
┌─────────────────────────────────────────────────────────┐
│                    Your Custom Layer                     │
│                    (meta-mylayer)                        │
├─────────────────────────────────────────────────────────┤
│           BSP Layer              Distro Layer           │
│         (meta-raspberrypi)      (meta-poky)             │
├─────────────────────────────────────────────────────────┤
│              OpenEmbedded-Core (meta)                   │
│         Base recipes, classes, core packages            │
└─────────────────────────────────────────────────────────┘
```

Layers stack on top of each other. Higher layers can override lower layers.

---

## Prerequisites

### System Requirements

- Ubuntu 20.04 or 22.04 LTS (x86_64) — **strongly recommended**
- At least **50GB free disk space** (100GB recommended)
- At least **8GB RAM** (16GB recommended)
- Fast internet connection
- Multi-core CPU (build is highly parallel)

### Install Dependencies

```bash
sudo apt update
sudo apt install -y \
    gawk wget git diffstat unzip texinfo gcc \
    build-essential chrpath socat cpio python3 \
    python3-pip python3-pexpect xz-utils debianutils \
    iputils-ping python3-git python3-jinja2 \
    python3-subunit zstd liblz4-tool file locales \
    libacl1 libsdl1.2-dev pylint

# Ensure locale is set (Yocto requirement)
sudo locale-gen en_US.UTF-8
```

### Verify Git Configuration

```bash
git config --global user.email "you@example.com"
git config --global user.name "Your Name"
```

---

## Step 1: Download Poky (Reference Distribution)

```bash
# Create working directory
mkdir -p ~/yocto-lab
cd ~/yocto-lab

# Clone Poky (use a release branch for stability)
git clone -b scarthgap git://git.yoctoproject.org/poky
cd poky
```

**Release branches** (named after releases):
- `scarthgap` — Yocto 5.0 (2024, LTS)
- `nanbield` — Yocto 4.3 (2023)
- `kirkstone` — Yocto 4.0 (2022, LTS)

---

## Step 2: Explore the Directory Structure

```bash
ls -la
```

```
poky/
├── bitbake/          # BitBake build engine
├── documentation/    # Documentation source
├── meta/             # OpenEmbedded-Core layer
├── meta-poky/        # Poky distro layer
├── meta-yocto-bsp/   # Reference BSP layer
├── meta-selftest/    # Testing layer
├── scripts/          # Helper scripts
├── oe-init-build-env # Environment setup script
└── LICENSE.*         # License files
```

### Inside the meta/ Layer (OE-Core)

```bash
ls meta/
```

```
meta/
├── classes/          # Shared build classes (.bbclass)
├── conf/             # Layer configuration
├── files/            # Shared files
├── lib/              # Python libraries for BitBake
├── recipes-bsp/      # Board support recipes
├── recipes-core/     # Core system (busybox, init, base-files)
├── recipes-devtools/ # Development tools (gcc, make, etc.)
├── recipes-extended/ # Extended utilities
├── recipes-gnome/    # GNOME components
├── recipes-graphics/ # Graphics stack
├── recipes-kernel/   # Linux kernel recipe
├── recipes-multimedia/ # Audio/video
├── recipes-rt/       # Real-time patches
├── recipes-sato/     # Sato UI
├── recipes-support/  # Support libraries
└── COPYING.MIT       # License
```

---

## Step 3: Initialize the Build Environment

```bash
cd ~/yocto-lab/poky

# Initialize build environment
source oe-init-build-env build-arm64
```

This script:
1. Creates the `build-arm64/` directory
2. Sets up environment variables
3. Adds BitBake to your PATH
4. Changes to the build directory

You'll see:

```
You had no conf/local.conf file. This configuration file has therefore been
created for you from /home/user/yocto-lab/poky/meta-poky/conf/templates/default/local.conf.sample
...
You can now run 'bitbake <target>'
```

---

## Step 4: Explore the Build Directory

```bash
ls build-arm64/
```

```
build-arm64/
└── conf/
    ├── bblayers.conf   # Defines which layers to use
    ├── local.conf      # Local build configuration
    └── templateconf.cfg
```

### bblayers.conf — Layer Configuration

```bash
cat conf/bblayers.conf
```

```bitbake
BBLAYERS ?= " \
  /home/user/yocto-lab/poky/meta \
  /home/user/yocto-lab/poky/meta-poky \
  /home/user/yocto-lab/poky/meta-yocto-bsp \
"
```

### local.conf — Build Configuration

Key settings in `conf/local.conf`:

```bash
# View and understand the configuration
cat conf/local.conf
```

Important variables:

```bitbake
# Target machine (default is qemux86-64)
MACHINE ??= "qemux86-64"

# Distribution
DISTRO ?= "poky"

# Package format
PACKAGE_CLASSES ?= "package_rpm"

# Parallel build settings (auto-detected)
BB_NUMBER_THREADS ?= "${@oe.utils.cpu_count()}"
PARALLEL_MAKE ?= "-j ${@oe.utils.cpu_count()}"

# Download directory (shared across builds)
DL_DIR ?= "${TOPDIR}/../downloads"

# Shared state cache
SSTATE_DIR ?= "${TOPDIR}/../sstate-cache"
```

---

## Step 5: Configure for ARM64 (QEMU)

Edit `conf/local.conf` to target ARM64:

```bash
# Open the configuration file
nano conf/local.conf
```

Find the `MACHINE` line and change it:

```bitbake
# Change from:
MACHINE ??= "qemux86-64"

# To:
MACHINE ??= "qemuarm64"
```

Also add these recommended settings:

```bitbake
# Accept all licenses (for educational purposes)
LICENSE_FLAGS_ACCEPTED = "commercial"

# Remove old images to save space
RM_OLD_IMAGE = "1"

# Useful additions for QEMU
EXTRA_IMAGE_FEATURES += "debug-tweaks"
```

Save and exit (`Ctrl+X`, `Y`, `Enter` in nano).

---

## Step 6: Explore Available Machines and Images

### List Available Machines

```bash
# Find all machine configurations
find ../meta* -name "*.conf" -path "*/machine/*" | xargs -I{} basename {} .conf
```

Common machines:
- `qemuarm64` — QEMU ARM64 (our target)
- `qemuarm` — QEMU ARM 32-bit
- `qemux86-64` — QEMU x86_64
- `genericarm64` — Generic ARM64 hardware
- `beaglebone-yocto` — BeagleBone

### List Available Images

```bash
# Find all image recipes
find ../meta* -name "*-image-*.bb" | head -20
```

Common images:

| Image | Description |
|-------|-------------|
| `core-image-minimal` | Tiny boot-capable image |
| `core-image-base` | Console image with hardware support |
| `core-image-full-cmdline` | Full command-line tools |
| `core-image-sato` | Graphical UI image |
| `core-image-weston` | Wayland/Weston compositor |

---

## Step 7: Build the Minimal Image

```bash
# Ensure you're in the build directory with environment set
cd ~/yocto-lab/poky
source oe-init-build-env build-arm64

# Build the minimal image
bitbake core-image-minimal
```

### What to Expect

First build takes **2-4 hours** depending on:
- Internet speed (downloading sources)
- CPU cores (parallel compilation)
- Disk speed (many small files)

BitBake shows progress:

```
Loading cache: 100% |#####################################| Time: 0:00:00
Loaded 1662 entries from dependency cache.
NOTE: Resolving any missing task queue dependencies

Build Configuration:
BB_VERSION           = "2.7.3"
BUILD_SYS            = "x86_64-linux"
NATIVELSBSTRING      = "ubuntu-22.04"
TARGET_SYS           = "aarch64-poky-linux"
MACHINE              = "qemuarm64"
DISTRO               = "poky"
DISTRO_VERSION       = "5.0"
TUNE_FEATURES        = "aarch64 armv8a crc"

Initialising tasks: 100% |################################| Time: 0:00:01
Sstate summary: Wanted 1234 Local 0 Mirrors 0 Missed 1234 Current 0 (0% match)
NOTE: Executing Tasks
```

### Monitor Disk Usage

Yocto builds use significant disk space:

```bash
# In another terminal
watch -n 30 "du -sh ~/yocto-lab/poky/build-arm64/tmp"
```

---

## Step 8: Examine Build Output

After successful build:

```bash
ls -la tmp/deploy/images/qemuarm64/
```

```
tmp/deploy/images/qemuarm64/
├── core-image-minimal-qemuarm64.rootfs.ext4
├── core-image-minimal-qemuarm64.rootfs.manifest
├── core-image-minimal-qemuarm64.rootfs.tar.bz2
├── core-image-minimal-qemuarm64.testdata.json
├── Image                          # Kernel
├── Image--6.6.*-qemuarm64-*       # Kernel (versioned)
├── modules--6.6.*-qemuarm64-*.tgz # Kernel modules
└── u-boot.bin                     # Bootloader (if enabled)
```

### Image Sizes

```bash
ls -lh tmp/deploy/images/qemuarm64/*.ext4
```

The minimal image is typically 10-20MB.

---

## Step 9: Boot with QEMU

Yocto provides a convenient wrapper script:

```bash
# Boot the image
runqemu qemuarm64 nographic
```

This automatically:
- Locates the kernel and rootfs
- Sets up networking with TAP/TUN
- Configures serial console

### Manual QEMU Command

If `runqemu` has issues, use QEMU directly:

```bash
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a57 \
    -m 512 \
    -kernel tmp/deploy/images/qemuarm64/Image \
    -drive file=tmp/deploy/images/qemuarm64/core-image-minimal-qemuarm64.rootfs.ext4,if=virtio,format=raw \
    -append "root=/dev/vda console=ttyAMA0" \
    -nographic \
    -netdev user,id=net0 \
    -device virtio-net-pci,netdev=net0
```

### Login

- Username: `root`
- Password: (none — due to `debug-tweaks`)

### Explore

```bash
uname -a
cat /etc/os-release
df -h
```

### Exit QEMU

Press `Ctrl+A` then `X`

---

## Step 10: Understanding BitBake

### BitBake Task Execution

BitBake breaks each recipe into tasks:

```
do_fetch      → Download source
do_unpack     → Extract source
do_patch      → Apply patches
do_configure  → Run configure
do_compile    → Compile source
do_install    → Install to staging
do_package    → Create packages
do_rootfs     → Assemble root filesystem (images only)
```

### Useful BitBake Commands

```bash
# Build a specific recipe
bitbake busybox

# Build a specific task
bitbake busybox -c compile

# Force rebuild
bitbake busybox -c compile -f

# Clean a recipe
bitbake busybox -c clean

# Deep clean (including sstate)
bitbake busybox -c cleansstate

# Show recipe environment
bitbake busybox -e | less

# Show recipe dependencies
bitbake busybox -g
cat recipe-depends.dot

# List all tasks for a recipe
bitbake busybox -c listtasks

# Open devshell (interactive build environment)
bitbake busybox -c devshell

# Search for packages
bitbake-layers show-recipes "*ssh*"

# Show layers
bitbake-layers show-layers
```

---

## Step 11: Examining a Recipe

Let's look at the BusyBox recipe:

```bash
# Find it
find ../meta -name "busybox*.bb"

# View it
cat ../meta/recipes-core/busybox/busybox_*.bb
```

### Recipe Structure

```bitbake
SUMMARY = "Tiny versions of many common UNIX utilities"
DESCRIPTION = "BusyBox combines tiny versions of many common UNIX utilities..."
HOMEPAGE = "https://www.busybox.net"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://LICENSE;md5=..."

SECTION = "base"
DEPENDS = "virtual/libc"

# Source location
SRC_URI = "https://busybox.net/downloads/busybox-${PV}.tar.bz2 \
           file://defconfig \
           file://init.cfg \
           "
SRC_URI[sha256sum] = "..."

# Inherit the autotools class for standard build
inherit autotools update-rc.d

# Configure options
EXTRA_OEMAKE = "..."

# Installation
do_install() {
    install -d ${D}${base_bindir}
    install -m 0755 busybox ${D}${base_bindir}/
    ...
}
```

---

## Step 12: Customizing Your Image

### Method 1: Add Packages via local.conf

Edit `conf/local.conf`:

```bitbake
# Add packages to the image
IMAGE_INSTALL:append = " htop vim nano wget dropbear openssh-sftp-server"

# Or use CORE_IMAGE_EXTRA_INSTALL for core images
CORE_IMAGE_EXTRA_INSTALL += "htop nano"
```

Rebuild:

```bash
bitbake core-image-minimal
```

### Method 2: Create a Custom Image Recipe

Create a new layer first (see Step 13), then add an image recipe:

```bash
# meta-mylayer/recipes-core/images/my-image.bb

SUMMARY = "My Custom ARM64 Image"

# Start from minimal image
require recipes-core/images/core-image-minimal.bb

# Add packages
IMAGE_INSTALL += " \
    htop \
    nano \
    vim \
    wget \
    curl \
    dropbear \
    i2c-tools \
    python3 \
    "

# Image features
IMAGE_FEATURES += " \
    ssh-server-dropbear \
    package-management \
    "

# Set root password (hashed)
# Generate with: openssl passwd -6 yourpassword
EXTRA_USERS_PARAMS = " \
    usermod -p '\$6\$xyz...' root; \
    "
```

---

## Step 13: Creating a Custom Layer

### Generate Layer Skeleton

```bash
cd ~/yocto-lab/poky

# Create layer
bitbake-layers create-layer ../meta-mylayer
```

This creates:

```
meta-mylayer/
├── conf/
│   └── layer.conf       # Layer configuration
├── recipes-example/
│   └── example/
│       └── example_0.1.bb
├── COPYING.MIT
└── README
```

### Add Layer to Build

```bash
cd build-arm64
bitbake-layers add-layer ../../meta-mylayer

# Verify
bitbake-layers show-layers
```

### Layer Configuration (layer.conf)

```bash
cat ../meta-mylayer/conf/layer.conf
```

```bitbake
# Layer configuration
BBPATH .= ":${LAYERDIR}"
BBFILES += "${LAYERDIR}/recipes-*/*/*.bb \
            ${LAYERDIR}/recipes-*/*/*.bbappend"
BBFILE_COLLECTIONS += "meta-mylayer"
BBFILE_PATTERN_meta-mylayer = "^${LAYERDIR}/"
LAYERSERIES_COMPAT_meta-mylayer = "scarthgap"
```

---

## Step 14: Adding a Custom Recipe

Create a simple "Hello World" application:

### 14.1 Create the Source Code

```bash
mkdir -p ~/yocto-lab/hello-src
cat > ~/yocto-lab/hello-src/hello.c << 'EOF'
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Hello from Yocto ARM64!\n");
    printf("Built with love using the Yocto Project\n");
    return 0;
}
EOF

cat > ~/yocto-lab/hello-src/Makefile << 'EOF'
CC ?= gcc
CFLAGS ?= -Wall -O2

hello: hello.c
	$(CC) $(CFLAGS) -o $@ $<

install: hello
	install -d $(DESTDIR)/usr/bin
	install -m 0755 hello $(DESTDIR)/usr/bin/

clean:
	rm -f hello

.PHONY: install clean
EOF
```

### 14.2 Create the Recipe

```bash
mkdir -p ~/yocto-lab/meta-mylayer/recipes-apps/hello

cat > ~/yocto-lab/meta-mylayer/recipes-apps/hello/hello_1.0.bb << 'EOF'
SUMMARY = "Hello World Application"
DESCRIPTION = "A simple hello world application for Yocto learning"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

# Source location
SRC_URI = "file://hello.c \
           file://Makefile"

# Source is in a subdirectory
S = "${WORKDIR}"

# Use the standard make class
inherit pkgconfig

# Compile
do_compile() {
    oe_runmake
}

# Install
do_install() {
    oe_runmake install DESTDIR=${D}
}
EOF
```

### 14.3 Add Source Files to Recipe

```bash
mkdir -p ~/yocto-lab/meta-mylayer/recipes-apps/hello/files
cp ~/yocto-lab/hello-src/* ~/yocto-lab/meta-mylayer/recipes-apps/hello/files/
```

### 14.4 Build and Test

```bash
# Build just the recipe
bitbake hello

# Add to image
echo 'IMAGE_INSTALL:append = " hello"' >> conf/local.conf

# Rebuild image
bitbake core-image-minimal
```

---

## Step 15: Using .bbappend Files

`.bbappend` files modify existing recipes without editing them directly.

### Example: Add a Custom File to BusyBox

```bash
mkdir -p ~/yocto-lab/meta-mylayer/recipes-core/busybox

cat > ~/yocto-lab/meta-mylayer/recipes-core/busybox/busybox_%.bbappend << 'EOF'
# This appends to any version of the busybox recipe

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://my-busybox.cfg"
EOF
```

### Example: Add Custom Kernel Configuration

```bash
mkdir -p ~/yocto-lab/meta-mylayer/recipes-kernel/linux/files

cat > ~/yocto-lab/meta-mylayer/recipes-kernel/linux/linux-yocto_%.bbappend << 'EOF'
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://my-kernel.cfg"
EOF

# Create kernel config fragment
cat > ~/yocto-lab/meta-mylayer/recipes-kernel/linux/files/my-kernel.cfg << 'EOF'
CONFIG_IKCONFIG=y
CONFIG_IKCONFIG_PROC=y
EOF
```

---

## Step 16: Useful Development Features

### Enable Package Management

In `local.conf`:

```bitbake
# Enable runtime package management
EXTRA_IMAGE_FEATURES += "package-management"

# Choose package format
PACKAGE_CLASSES = "package_deb"  # or package_rpm, package_ipk
```

### Enable SSH Server

```bitbake
# In local.conf or image recipe
IMAGE_FEATURES += "ssh-server-dropbear"

# Or for OpenSSH
IMAGE_FEATURES += "ssh-server-openssh"
```

### Add Development Tools

```bitbake
# Tools for development
IMAGE_FEATURES += "tools-sdk tools-debug"
```

### Generate SDK

Build a complete cross-compilation SDK:

```bash
bitbake core-image-minimal -c populate_sdk
```

The SDK installer will be at:
`tmp/deploy/sdk/poky-glibc-x86_64-core-image-minimal-aarch64-qemuarm64-toolchain-*.sh`

---

## Student Exercises

### Exercise 1: Add Packages to Image (20 minutes)

1. Add these packages to your image: `htop`, `nano`, `tree`, `file`
2. Rebuild and boot
3. Verify the packages are installed

**Verification:**
```bash
htop --version
nano --version
tree /etc
file /bin/busybox
```

### Exercise 2: Create a Custom Recipe (45 minutes)

1. Write a simple C program that prints system information
2. Create a Yocto recipe to build and install it
3. Add it to your image
4. Boot and run it

**Hints:**
- Use `uname()` syscall for system info
- Recipe should inherit nothing special (simple Makefile project)
- Install to `/usr/bin`

### Exercise 3: Create a Custom Image (30 minutes)

1. Create a new image recipe in your layer
2. Base it on `core-image-minimal`
3. Add networking tools: `iproute2`, `iputils`, `net-tools`
4. Add your custom application from Exercise 2
5. Build and test

### Exercise 4: Modify Kernel Configuration (30 minutes)

1. Create a `.bbappend` for the kernel
2. Add a kernel config fragment that enables `CONFIG_MAGIC_SYSRQ`
3. Rebuild the kernel
4. Verify: `cat /proc/sys/kernel/sysrq`

**Hint:** Use `bitbake virtual/kernel -c menuconfig` to explore options.

### Exercise 5: Build the SDK (30 minutes)

1. Build the SDK: `bitbake core-image-minimal -c populate_sdk`
2. Install it on your host
3. Source the environment script
4. Cross-compile a program outside of Yocto
5. Copy it to the target and run it

---

## Troubleshooting Guide

### Build Fails: Fetch Error

```bash
# Check network
ping git.yoctoproject.org

# Retry with verbose
bitbake -v core-image-minimal

# Check/create download directory
mkdir -p ~/yocto-lab/downloads
```

### Build Fails: Do_compile Error

```bash
# See the log
cat tmp/work/aarch64-poky-linux/<package>/<version>/temp/log.do_compile

# Open devshell to debug interactively
bitbake <package> -c devshell
```

### Build Fails: License Issue

```bash
# Accept specific license
LICENSE_FLAGS_ACCEPTED += "commercial_packagename"

# Accept all (for development only!)
LICENSE_FLAGS_ACCEPTED = "commercial"
```

### Out of Disk Space

```bash
# Check usage
du -sh tmp/
du -sh sstate-cache/

# Clean old builds
bitbake -c cleanall <package>

# Or use rm_work to delete work directories after build
# Add to local.conf:
INHERIT += "rm_work"
```

### runqemu Fails

```bash
# Check images exist
ls tmp/deploy/images/qemuarm64/

# Run with debug
runqemu qemuarm64 nographic slirp

# Or use QEMU directly (see Step 9)
```

### Layer Incompatibility

Check `LAYERSERIES_COMPAT` in your layer.conf matches your Yocto version:

```bitbake
# For Yocto 5.0 (scarthgap)
LAYERSERIES_COMPAT_meta-mylayer = "scarthgap"
```

---

## Best Practices

### 1. Never Edit Poky Directly

Always use layers and `.bbappend` files. Keep poky pristine.

### 2. Use Shared Downloads and Sstate

In `local.conf`:

```bitbake
DL_DIR = "/opt/yocto/downloads"
SSTATE_DIR = "/opt/yocto/sstate-cache"
```

Share these across builds and machines to save time.

### 3. Use INHERIT += "rm_work"

Saves disk space by removing work directories after packaging:

```bitbake
INHERIT += "rm_work"
RM_WORK_EXCLUDE += "linux-yocto busybox"  # Keep some for debugging
```

### 4. Version Control Your Layers

```bash
cd ~/yocto-lab/meta-mylayer
git init
git add .
git commit -m "Initial layer setup"
```

### 5. Document Your Configuration

Create a README with build instructions:

```markdown
# My Yocto Build

## Setup
```bash
git clone git://git.yoctoproject.org/poky -b scarthgap
git clone <your-layer-repo> meta-mylayer
source poky/oe-init-build-env build
bitbake-layers add-layer ../meta-mylayer
```

## Build
```bash
bitbake my-custom-image
```
```

---

## Quick Reference Card

| Task | Command |
|------|---------|
| Initialize environment | `source oe-init-build-env <build-dir>` |
| Build image | `bitbake core-image-minimal` |
| Build package | `bitbake <recipe-name>` |
| Clean package | `bitbake <recipe> -c clean` |
| Deep clean | `bitbake <recipe> -c cleansstate` |
| Kernel menuconfig | `bitbake virtual/kernel -c menuconfig` |
| BusyBox menuconfig | `bitbake busybox -c menuconfig` |
| Show recipe env | `bitbake <recipe> -e \| less` |
| List layers | `bitbake-layers show-layers` |
| Add layer | `bitbake-layers add-layer <path>` |
| Find recipe | `bitbake-layers show-recipes "*name*"` |
| Create layer | `bitbake-layers create-layer <path>` |
| Run QEMU | `runqemu qemuarm64 nographic` |
| Build SDK | `bitbake <image> -c populate_sdk` |

---

## Comparison Summary

| Metric | Manual Build | Buildroot | Yocto |
|--------|--------------|-----------|-------|
| Time to first boot | 2-4 hours | 1 hour | 3-5 hours |
| Lines of config | 100+ | 10-20 | 20-50 |
| Learning investment | Days | Hours | Weeks |
| Production readiness | Low | Medium | High |
| Team scalability | Poor | Medium | Excellent |
| Package ecosystem | None | 2,500 | 10,000+ |
| Commercial support | None | Limited | Extensive |
| License compliance | Manual | Basic | Comprehensive |

---

## Further Reading

- [Yocto Project Documentation](https://docs.yoctoproject.org/)
- [Yocto Mega Manual](https://docs.yoctoproject.org/singleindex.html)
- [BitBake User Manual](https://docs.yoctoproject.org/bitbake/)
- [Bootlin Yocto Training](https://bootlin.com/doc/training/yocto/)
- [Yocto Project Wiki](https://wiki.yoctoproject.org/)

---

## Summary

The Yocto Project is powerful but complex. Key takeaways:

1. **Everything is a layer** — Modular, stackable configuration
2. **Everything is a recipe** — Consistent package definition
3. **BitBake orchestrates** — Parallel, cached, reproducible builds
4. **Never modify upstream** — Use layers and appends
5. **Sstate is your friend** — Binary caching saves hours

While the learning curve is steep, Yocto provides unmatched flexibility and is the industry standard for production embedded Linux systems.

---

## Next Steps

1. Explore BSP layers for real hardware (meta-raspberrypi, meta-ti, etc.)
2. Learn about devtool for rapid recipe development
3. Study AUTOREV and kernel development workflows
4. Explore toaster (web-based build management)
5. Investigate kas for build configuration management
6. Learn about wic for custom image layouts
