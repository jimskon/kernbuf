# kernbuf
A lunix kernal module design to store and retrieve data from a queue.

# Setup
make
make test

# Make the directory entry
The make setup command will report the device major number:

```kernbuf module loaded with device major number 236```

Make a directory entry using he major number:
```sudo mknod /dev/kernbuf c 236 0```

Then set the permissions so the device is writable:
```sudo chmod a+w /dev/kernbuf```

# Try is out:
```cat >> /dev/kernbuf
This is a test
^d
cat /dev/kernbuf
This is a test```
