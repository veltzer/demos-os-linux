# QEMU ARM64 Emulation Exercise

Bring up an emulated ARM64 Linux system from scratch using QEMU. By the end, you'll have a bootable virtual machine with emulated hardware you can inspect and interact with.

---

## Prerequisites

You need an Ubuntu host system (20.04+ recommended). All commands assume you're running on x86_64 but emulating ARM64.

---

## Part 1: Install QEMU

### Step 1.1: Install packages

```bash
sudo apt update
sudo apt install -y qemu-system-arm qemu-efi-aarch64 qemu-utils
```

### Step 1.2: Verify installation

```bash
qemu-system-aarch64 --version
```

You should see something like `QEMU emulator version 6.2.0` or newer.

---

## Part 2: Obtain an ARM64 Cloud Image

We'll use Ubuntu's official cloud images — these are minimal and boot quickly.

### Step 2.1: Create a working directory

```bash
mkdir -p ~/qemu-arm64-lab && cd ~/qemu-arm64-lab
```

### Step 2.2: Download the Ubuntu cloud image

```bash
wget https://cloud-images.ubuntu.com/jammy/current/jammy-server-cloudimg-arm64.img
```

### Step 2.3: Resize the image (optional but recommended)

The cloud image is minimal. Expand it to give yourself room to work:

```bash
qemu-img resize jammy-server-cloudimg-arm64.img 10G
```

---

## Part 3: Prepare Cloud-Init for Login Access

Cloud images require cloud-init configuration to set up a user account.

### Step 3.1: Create a cloud-init seed image

Create the user-data file:

```bash
cat > user-data <<EOF
#cloud-config
password: ubuntu
chpasswd:
  expire: false
ssh_pwauth: true
EOF
```

Create an empty meta-data file:

```bash
touch meta-data
```

### Step 3.2: Build the seed ISO

```bash
sudo apt install -y cloud-image-utils
cloud-localds seed.img user-data meta-data
```

This creates `seed.img` which QEMU will attach as a virtual CD-ROM for cloud-init.

---

## Part 4: Obtain UEFI Firmware

ARM64 VMs typically boot via UEFI. Copy the firmware files:

```bash
cp /usr/share/qemu-efi-aarch64/QEMU_EFI.fd .
```

Create a writable pflash for UEFI variables:

```bash
truncate -s 64M varstore.img
```

---

## Part 5: Boot the Virtual Machine

### Step 5.1: Launch QEMU

```bash
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a72 \
    -smp 2 \
    -m 2048 \
    -drive if=pflash,format=raw,readonly=on,file=QEMU_EFI.fd \
    -drive if=pflash,format=raw,file=varstore.img \
    -drive if=virtio,format=qcow2,file=jammy-server-cloudimg-arm64.img \
    -drive if=virtio,format=raw,file=seed.img \
    -netdev user,id=net0,hostfwd=tcp::2222-:22 \
    -device virtio-net-pci,netdev=net0 \
    -nographic
```

**Explanation of key options:**

| Option | Purpose |
|--------|---------|
| `-machine virt` | Use the generic ARM virtual machine platform |
| `-cpu cortex-a72` | Emulate a Cortex-A72 (ARMv8-A) |
| `-smp 2` | 2 virtual CPUs |
| `-m 2048` | 2GB RAM |
| `-nographic` | Console output to terminal (no GUI window) |
| `hostfwd=tcp::2222-:22` | Forward host port 2222 to guest port 22 for SSH |

### Step 5.2: Wait for boot

You'll see UEFI initialization, then GRUB, then the kernel booting. First boot takes 1-3 minutes as cloud-init runs. Look for:

```
Ubuntu 22.04.x LTS ubuntu ttyAMA0

ubuntu login:
```

### Step 5.3: Log in

```
Username: ubuntu
Password: ubuntu
```

---

## Part 6: Explore the Emulated Hardware

Now that you're inside the VM, verify what QEMU has emulated.

### Step 6.1: Check CPU info

```bash
lscpu
```

You should see `aarch64` architecture with Cortex-A72 features.

### Step 6.2: Examine the device tree

```bash
ls /sys/firmware/devicetree/base/
cat /sys/firmware/devicetree/base/compatible
```

This shows the `linux,dummy-virt` machine compatible string.

### Step 6.3: List PCI devices

```bash
lspci
```

You'll see the virtio-net and virtio-blk devices.

### Step 6.4: Check network interfaces

```bash
ip addr
```

You should see `enp0s1` or similar with a DHCP address from QEMU's user-mode networking (10.0.2.x range).

### Step 6.5: Test network connectivity

```bash
ping -c 3 8.8.8.8
curl -I https://ubuntu.com
```

---

## Part 7: SSH Access

From another terminal on your host:

```bash
ssh -p 2222 ubuntu@localhost
```

This is useful for copy/paste and running multiple sessions.

---

## Part 8: Clean Shutdown

Inside the VM:

```bash
sudo poweroff
```

Or from the QEMU console, press `Ctrl-A` then `X` to force quit.

---

## Exercises

1. **Modify kernel parameters:** Edit GRUB inside the VM to add `earlycon` and observe the difference in boot messages.

2. **Dump the device tree:** Inside the VM, run:
   ```bash
   sudo apt install device-tree-compiler
   dtc -I fs /sys/firmware/devicetree/base > virt.dts
   ```
   Examine the structure and compare it to device trees you've worked with on real hardware.

3. **Create a writable overlay:** Instead of modifying the base image, use a QCOW2 overlay:
   ```bash
   qemu-img create -f qcow2 -b jammy-server-cloudimg-arm64.img -F qcow2 overlay.qcow2
   ```
   Then boot from `overlay.qcow2`. This lets you snapshot and revert easily.

4. **Add a second network interface:** Add another `-netdev` and `-device virtio-net-pci` pair and configure both interfaces inside the guest.

5. **Emulate different CPUs:** Try `-cpu max` to see all features QEMU can emulate, or `-cpu cortex-a53` for a simpler core. Compare `lscpu` output.

6. **Attach a second disk:** Create a new image and attach it:
   ```bash
   qemu-img create -f qcow2 data.qcow2 5G
   ```
   Add `-drive if=virtio,format=qcow2,file=data.qcow2` to your QEMU command, then partition and mount it inside the guest.

---

## Quick Reference

**Start VM:**
```bash
cd ~/qemu-arm64-lab && qemu-system-aarch64 \
    -machine virt -cpu cortex-a72 -smp 2 -m 2048 \
    -drive if=pflash,format=raw,readonly=on,file=QEMU_EFI.fd \
    -drive if=pflash,format=raw,file=varstore.img \
    -drive if=virtio,format=qcow2,file=jammy-server-cloudimg-arm64.img \
    -drive if=virtio,format=raw,file=seed.img \
    -netdev user,id=net0,hostfwd=tcp::2222-:22 \
    -device virtio-net-pci,netdev=net0 \
    -nographic
```

**SSH in:**
```bash
ssh -p 2222 ubuntu@localhost
```

**Credentials:**
- Username: `ubuntu`
- Password: `ubuntu`

---

## Next Steps

Once you're comfortable with basic emulation, consider:

- Building a custom kernel and booting it with `-kernel` and `-initrd`
- Passing a custom device tree with `-dtb`
- Using QEMU's GDB stub (`-s -S`) to debug kernel boot
- Exploring different machine types with `qemu-system-aarch64 -machine help`
