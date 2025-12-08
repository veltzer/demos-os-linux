# Using buildroot to build a system from scratch

git clone https://git.buildroot.net/buildroot
cd buildroot
make qemu_aarch64_virt_defconfig
make -j$(nproc)

# Boot the result
qemu-system-aarch64 -M virt -cpu cortex-a72 -m 1024 \
    -kernel output/images/Image \
    -drive file=output/images/rootfs.ext4,if=virtio,format=raw \
    -append "root=/dev/vda console=ttyAMA0" \
    -nographic
