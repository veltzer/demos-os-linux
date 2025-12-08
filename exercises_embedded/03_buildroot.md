# Exercise: Building an ARM64 System with Buildroot

## Introduction

In the previous exercise, we built an ARM64 Linux system by hand — compiling the kernel, building BusyBox, manually creating device nodes, writing init scripts, and assembling the root filesystem piece by piece. While educational, this process was tedious and error-prone.

**Buildroot** automates all of this. It's a complete build system that generates:

- Cross-compilation toolchain
- Linux kernel
- Root filesystem with packages
- Bootloader (optional)
- Complete disk images ready to flash

All from a single `make` command.

---

## Learning Objectives

By the end of this exercise, you will be able to:

1. Understand what Buildroot is and when to use it
2. Configure a Buildroot project for ARM64
3. Customize the kernel, packages, and root filesystem
4. Generate bootable images for QEMU and real hardware
5. Add custom packages and configuration
6. Debug common build issues

---

## What is Buildroot?

Buildroot is a **simple, efficient, and easy-to-use tool** to generate embedded Linux systems through cross-compilation. It is:

| Feature | Description |
|---------|-------------|
| **Makefile-based** | Familiar build system, no complex abstractions |
| **Self-contained** | Downloads and builds its own toolchain |
| **Reproducible** | Same config always produces same output |
| **Lightweight** | Minimal dependencies, fast builds |
| **Well-documented** | Excellent manual and active community |

### Buildroot vs. The Manual Approach

| Task | Manual Approach | Buildroot |
|------|-----------------|-----------|
| Toolchain | Install from apt | Auto-generated or downloaded |
| Kernel config | Manual `make defconfig` | Integrated, per-board defaults |
| BusyBox | Separate download/build | Built-in package |
| Root filesystem | Hand-create directories | Auto-generated |
| Init scripts | Write from scratch | Provided by packages |
| Device nodes | Manual `mknod` | Handled by devtmpfs/mdev |
| Build time | Hours of manual work | Single `make` command |

### Buildroot vs. Yocto

| Aspect | Buildroot | Yocto |
|--------|-----------|-------|
| Complexity | Simple | Complex |
| Learning curve | Hours | Weeks |
| Flexibility | Good | Excellent |
| Package count | ~2,500 | ~10,000+ |
| Build time | 30-60 min | 2-4 hours |
| Use case | Small/medium systems | Production, large teams |

**Rule of thumb**: Use Buildroot for learning and smaller projects; consider Yocto for complex production systems.

---

## Prerequisites

### System Requirements

- Ubuntu 20.04+ x86_64 (or similar Linux distribution)
- At least 10GB free disk space
- 4GB+ RAM recommended
- Internet connection

### Install Dependencies

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    libncurses-dev \
    git \
    wget \
    unzip \
    bc \
    cpio \
    rsync \
    qemu-system-arm
```

---

## Step 1: Download Buildroot

```bash
# Create working directory
mkdir -p ~/buildroot-lab
cd ~/buildroot-lab

# Clone Buildroot (use a release tag for stability)
git clone --branch 2024.02 https://github.com/buildroot/buildroot.git
cd buildroot
```

Alternatively, download a release tarball:

```bash
wget https://buildroot.org/downloads/buildroot-2024.02.tar.gz
tar xf buildroot-2024.02.tar.gz
cd buildroot-2024.02
```

---

## Step 2: Explore the Directory Structure

Before building, let's understand what's inside:

```
buildroot/
├── arch/           # Architecture-specific configs
├── board/          # Board-specific files (scripts, overlays)
├── boot/           # Bootloader packages (U-Boot, GRUB, etc.)
├── configs/        # Pre-made defconfig files
├── docs/           # Documentation
├── fs/             # Filesystem image generators
├── linux/          # Linux kernel package
├── package/        # All 2,500+ packages
├── support/        # Helper scripts
├── system/         # System-level configs (init, users, etc.)
├── toolchain/      # Toolchain packages
├── Makefile        # Main build entry point
└── Config.in       # Top-level Kconfig
```

### Key Insight: Everything is a Package

In Buildroot, everything — including the toolchain, kernel, and BusyBox — is a "package" with a consistent build interface.

---

## Step 3: List Available Configurations

Buildroot includes pre-made configurations for many boards:

```bash
# List all defconfigs
ls configs/

# Filter for ARM64/QEMU
ls configs/ | grep -E "(aarch64|qemu.*arm)"
```

You'll see `qemu_aarch64_virt_defconfig` — this is exactly what we need.

---

## Step 4: Configure for QEMU ARM64

```bash
# Apply the QEMU ARM64 configuration
make qemu_aarch64_virt_defconfig
```

This single command configures:
- ARM64 architecture
- Toolchain (downloads or builds one)
- Linux kernel with appropriate options
- BusyBox-based root filesystem
- Virtio drivers for QEMU
- Serial console support

### Examine the Configuration

```bash
# Open the interactive configuration menu
make menuconfig
```

Navigate the menus to explore:

```
┌─────────────────── Buildroot Configuration ───────────────────┐
│  Target options          → Architecture, CPU, ABI             │
│  Toolchain               → Compiler, C library selection      │
│  Build options           → Parallel jobs, download directory  │
│  System configuration    → Hostname, init system, /dev        │
│  Kernel                  → Version, config, custom patches    │
│  Target packages         → BusyBox, networking, libraries     │
│  Filesystem images       → ext4, squashfs, cpio, etc.         │
│  Bootloaders             → U-Boot, GRUB, etc.                 │
└───────────────────────────────────────────────────────────────┘
```

**Don't change anything yet** — exit without saving to use the defaults.

---

## Step 5: Build the System

```bash
# Build everything
make -j$(nproc)
```

This will take **30-60 minutes** on first build (downloading and compiling toolchain, kernel, packages). Subsequent builds are much faster due to caching.

### What Happens During Build

1. **Toolchain**: Downloads or builds cross-compiler
2. **Host utilities**: Builds tools needed on build machine
3. **Packages**: Compiles selected target packages
4. **Root filesystem**: Assembles everything into a filesystem
5. **Images**: Generates final disk/initrd images

### Monitor Progress

Buildroot shows each package being built:

```
>>> toolchain-buildroot >>>  Downloading
>>> linux 6.6.x >>>  Extracting
>>> busybox 1.36.x >>>  Building
>>> linux 6.6.x >>>  Installing to target
>>> host-genimage >>>  Building
```

---

## Step 6: Examine Build Output

After build completes:

```bash
ls -la output/images/
```

You'll see:

```
output/images/
├── Image           # Linux kernel (uncompressed)
├── rootfs.ext2     # Root filesystem (ext2 format)
├── rootfs.ext4     # Root filesystem (ext4 format, symlink)
└── start-qemu.sh   # Helper script to launch QEMU
```

### Output Directory Structure

```
output/
├── build/          # Package build directories
├── host/           # Tools for the build machine
├── images/         # Final images (kernel, rootfs)
├── staging/        # Development sysroot (headers, libs)
└── target/         # Root filesystem tree (before imaging)
```

---

## Step 7: Boot with QEMU

### Option A: Use the Provided Script

```bash
./output/images/start-qemu.sh
```

### Option B: Manual QEMU Command

```bash
qemu-system-aarch64 \
    -M virt \
    -cpu cortex-a72 \
    -m 1024 \
    -kernel output/images/Image \
    -drive file=output/images/rootfs.ext4,if=virtio,format=raw \
    -append "root=/dev/vda console=ttyAMA0" \
    -nographic \
    -netdev user,id=net0 \
    -device virtio-net-pci,netdev=net0
```

### Login

- Username: `root`
- Password: (empty, just press Enter)

### Explore the System

```bash
# Check architecture
uname -a

# List available commands
busybox --list | head -20

# Check filesystem
df -h

# View running processes
ps aux

# Check network
ip addr
```

### Exit QEMU

Press `Ctrl+A` then `X`

---

## Step 8: Customizing Your Build

### 8.1 Change System Settings

```bash
make menuconfig
```

Navigate to **System configuration**:

- `System hostname`: Change to your preferred name
- `System banner`: Add a welcome message
- `Root password`: Set a password (leave empty for none)
- `Init system`: Choose BusyBox init, systemd, or OpenRC

### 8.2 Add Packages

Navigate to **Target packages** and explore:

```
Target packages
├── Audio and video applications
├── Compressors and decompressors
├── Debugging, profiling and benchmark
├── Development tools
├── Filesystem and flash utilities
├── Games
├── Hardware handling
├── Interpreter languages and scripting
├── Libraries
├── Networking applications
├── Shell and utilities
└── System tools
```

Let's add some useful packages:

```
Target packages → Networking applications
    [*] dropbear (SSH server - lightweight)
    [*] wget

Target packages → Shell and utilities
    [*] htop
    [*] file

Target packages → System tools
    [*] util-linux
```

Save and exit, then rebuild:

```bash
make -j$(nproc)
```

### 8.3 Customize the Kernel

```bash
# Open kernel configuration
make linux-menuconfig
```

Make changes, save, then rebuild:

```bash
make linux-rebuild
make -j$(nproc)
```

---

## Step 9: Adding a Root Filesystem Overlay

An **overlay** lets you add custom files to the root filesystem.

### 9.1 Create Overlay Directory

```bash
mkdir -p board/mycompany/myboard/rootfs-overlay
```

### 9.2 Add Custom Files

```bash
# Add a welcome message
mkdir -p board/mycompany/myboard/rootfs-overlay/etc
cat > board/mycompany/myboard/rootfs-overlay/etc/motd << 'EOF'

  ╔═══════════════════════════════════════╗
  ║   Welcome to My Custom ARM64 Linux!   ║
  ║   Built with Buildroot                ║
  ╚═══════════════════════════════════════╝

EOF

# Add a custom script
mkdir -p board/mycompany/myboard/rootfs-overlay/usr/bin
cat > board/mycompany/myboard/rootfs-overlay/usr/bin/sysinfo << 'EOF'
#!/bin/sh
echo "=== System Information ==="
echo "Hostname: $(hostname)"
echo "Kernel:   $(uname -r)"
echo "Arch:     $(uname -m)"
echo "Uptime:   $(cat /proc/uptime | cut -d' ' -f1)s"
echo "Memory:   $(free -m | awk '/Mem:/ {print $3"/"$2" MB"}')"
echo "=========================="
EOF
chmod +x board/mycompany/myboard/rootfs-overlay/usr/bin/sysinfo
```

### 9.3 Configure Buildroot to Use Overlay

```bash
make menuconfig
```

Navigate to:
```
System configuration
    → Root filesystem overlay directories
```

Enter: `board/mycompany/myboard/rootfs-overlay`

Save and rebuild:

```bash
make -j$(nproc)
```

---

## Step 10: Creating a Custom Defconfig

Save your configuration for reuse:

```bash
# Save current config
make savedefconfig BR2_DEFCONFIG=configs/my_arm64_defconfig

# Now you can recreate your build with:
# make my_arm64_defconfig
```

---

## Step 11: Understanding Build Targets

Useful make targets:

```bash
# Main targets
make                    # Build everything
make clean              # Remove build output (keeps downloads)
make distclean          # Remove everything (fresh start)

# Configuration
make menuconfig         # Main configuration
make linux-menuconfig   # Kernel configuration
make busybox-menuconfig # BusyBox configuration
make uboot-menuconfig   # U-Boot configuration (if enabled)

# Package operations
make <pkg>              # Build a specific package
make <pkg>-rebuild      # Force rebuild of package
make <pkg>-reconfigure  # Reconfigure and rebuild
make <pkg>-dirclean     # Clean package build directory

# Information
make show-targets       # List all enabled packages
make graph-depends      # Generate dependency graph (needs graphviz)
make legal-info         # Generate license information

# Help
make help               # Show all targets
```

---

## Student Exercises

### Exercise 1: Basic Customization (15 minutes)

1. Change the hostname to `student-arm64`
2. Set a root password
3. Add the `nano` text editor package
4. Rebuild and verify your changes

**Verification:**
```bash
hostname
nano --version
```

### Exercise 2: Add SSH Access (20 minutes)

1. Enable the `dropbear` SSH server package
2. Rebuild the system
3. Boot with network support
4. SSH into your system from the host

**Hint:** Use QEMU with port forwarding:
```bash
-netdev user,id=net0,hostfwd=tcp::2222-:22
```

Then: `ssh -p 2222 root@localhost`

### Exercise 3: Create a Custom Application (30 minutes)

1. Create a simple "Hello ARM64" C program
2. Cross-compile it using Buildroot's toolchain
3. Add it to the root filesystem via overlay
4. Verify it runs on the target

**Toolchain location:** `output/host/bin/aarch64-linux-gcc`

```bash
# Example
cat > hello.c << 'EOF'
#include <stdio.h>
int main() {
    printf("Hello from ARM64!\n");
    return 0;
}
EOF

./output/host/bin/aarch64-linux-gcc -o hello hello.c
file hello  # Should show ARM aarch64
```

### Exercise 4: Enable systemd (30 minutes)

1. Change init system from BusyBox to systemd
2. Note the additional dependencies that get selected
3. Rebuild and observe the boot process differences
4. Compare filesystem size before and after

**Path:** System configuration → Init system → systemd

### Exercise 5: Generate Documentation (15 minutes)

1. Run `make manual` to build documentation
2. Explore the generated HTML manual
3. Run `make legal-info` to generate license information
4. Review what licenses your system uses

---

## Troubleshooting Guide

### Build Fails: Missing Dependencies

```bash
# Check what's missing
make dependencies

# Common fixes
sudo apt install libncurses-dev libssl-dev
```

### Build Fails: Download Error

```bash
# Retry with verbose output
make V=1

# Or manually download to dl/ directory
```

### Package Build Fails

```bash
# Get detailed output
make <package>-rebuild V=1

# Clean and retry
make <package>-dirclean
make <package>
```

### "No space left on device"

Buildroot needs significant disk space:

```bash
# Check output size
du -sh output/

# Clean old builds
make clean
```

### QEMU Boot Fails

1. Verify kernel was built: `ls output/images/Image`
2. Check root filesystem: `ls output/images/rootfs.ext4`
3. Ensure QEMU is installed: `qemu-system-aarch64 --version`

### Changes Not Appearing

```bash
# Force full rebuild of package
make <package>-dirclean
make <package>-rebuild

# Regenerate root filesystem
make
```

---

## Best Practices

### 1. Use Out-of-Tree Builds

Keep your customizations separate from Buildroot source:

```bash
make O=/path/to/output BR2_EXTERNAL=/path/to/external
```

### 2. Version Control Your Config

```bash
# Save minimal config
make savedefconfig

# Commit to git
git add configs/my_defconfig board/mycompany/
git commit -m "Add custom ARM64 configuration"
```

### 3. Use BR2_EXTERNAL for Custom Packages

Create an external tree for your organization's packages:

```
my-external/
├── external.mk
├── external.desc
├── Config.in
└── package/
    └── my-custom-app/
        ├── Config.in
        └── my-custom-app.mk
```

### 4. Reproducible Builds

Lock package versions for production:

```bash
# In your defconfig or menuconfig:
BR2_REPRODUCIBLE=y
```

### 5. Check Licenses Before Shipping

```bash
make legal-info
cat output/legal-info/manifest.csv
```

---

## Quick Reference Card

| Task | Command |
|------|---------|
| Configure | `make menuconfig` |
| Build | `make -j$(nproc)` |
| Clean build | `make clean` |
| Full clean | `make distclean` |
| Kernel config | `make linux-menuconfig` |
| BusyBox config | `make busybox-menuconfig` |
| Rebuild package | `make <pkg>-rebuild` |
| Save config | `make savedefconfig` |
| Show packages | `make show-targets` |
| Get help | `make help` |

---

## Comparison: Manual vs. Buildroot

| Metric | Manual BusyBox Build | Buildroot |
|--------|---------------------|-----------|
| Setup time | 2-4 hours | 10 minutes |
| Build commands | 20+ | 2 (`make defconfig`, `make`) |
| Configuration | Edit multiple files | Single menuconfig |
| Package management | Manual | Built-in (2,500+ packages) |
| Reproducibility | Difficult | Automatic |
| Documentation | You write it | Generated |
| Maintenance | High effort | Low effort |

---

## Summary

Buildroot transforms what was a complex, multi-hour manual process into:

```bash
git clone https://github.com/buildroot/buildroot.git
cd buildroot
make qemu_aarch64_virt_defconfig
make -j$(nproc)
./output/images/start-qemu.sh
```

Five commands to a complete, bootable ARM64 Linux system.

---

## Further Reading

- [Buildroot Manual](https://buildroot.org/downloads/manual/manual.html)
- [Buildroot Training Materials](https://bootlin.com/doc/training/buildroot/)
- [Buildroot Mailing List](http://lists.buildroot.org/mailman/listinfo/buildroot)

---

## Next Steps

1. Try building for real hardware (Raspberry Pi 4: `make raspberrypi4_64_defconfig`)
2. Create a custom BR2_EXTERNAL tree for your packages
3. Explore advanced topics: secure boot, firmware updates, factory flashing
4. Compare with Yocto Project for larger-scale builds
