# Loopback filesystem exercise

In this exercise we want to create a file to be used as a disk,
format the file, mount it, put some files on it, umount it,
mount it again, see the files are still there, umount it,
and destroy the file.

Create the big file:
```text
sudo dd if=/dev/zero of=big_file count=2097152
```

Format the file to ext4:
```text
sudo mkfs.ext4 big_file
```

Create a folder to mount the file at:
```text
sudo mkdir /mnt/my_file
```

Mount the file:
```text
sudo mount big_file /mnt/my_file -o loop
```

Put some file on the new disk:
- go to `/mnt/my_file`
- and create some files there...

Umount the file:
```text
sudo umount /mnt/my_file
```

Mount again:
```text
sudo mount big_file /mnt/my_file -o loop
```

See that files are still there.

Umount:
```text
sudo umount /mnt/my_file
```

Destroy the big file:
```text
sudo rm big_file
```
