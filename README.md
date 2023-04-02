# kernbuf
A Linux kernel module design to store and retrieve data from a queue.

# Setup
make
make test

# Make the directory entry
The make setup command will report the device major number:

```kernbuf module loaded with device major number 236```

Make a directory entry using the major number:
```
sudo mknod /dev/kernbuf c 236 0
```

Then set the permissions so the device is writable:
```
sudo chmod a+w /dev/kernbuf
```

# Try is out:
```
cat >> /dev/kernbuf
This is a test
^d
cat /dev/kernbuf
This is a test
```

# Run the acceptance test
```
cd test
make
./test_kernbuf 
Output size: 1  Match
Output size: 2  Match
Output size: 3  Match
Output size: 4  Match
Output size: 5  Match
Output size: 6  Match
Output size: 7  Match
Output size: 8  Match
Output size: 9  Match
Output size: 10  Match
Output size: 11  Match
Output size: 12  Match
Output size: 13  Match
Output size: 14  Match
Output size: 15  Match
Output size: 16  Match
Output size: 17  Match
Output size: 18  Match
Output size: 19  Match
Output size: 20  Match
Output size: 21  Match
Output size: 22  Match
Output size: 23  Match
Output size: 24  Match
Output size: 25  Match
Output size: 26  Match
Output size: 27  Match
Output size: 28  Match
Output size: 29  Match
Output size: 30  Match
Output size: 31  Match
Output size: 32  Match
Output size: 33  Match
Output size: 34  Match
Output size: 35  Match
Output size: 36  Match
Output size: 37  Match
Output size: 38  Match
Output size: 39  Match
Output size: 40  Match
Output size: 41  Match
Output size: 42  Match
Output size: 43  Match
Output size: 44  Match
Output size: 45  Match
Output size: 46  Match
Output size: 47  Match
Output size: 48  Match
Output size: 49  Match
Output size: 50  Match
Output size: 51  Match
Output size: 52  Match
Output size: 53  Match
Output size: 54  Match
Output size: 55  Match
Output size: 56  Match
Output size: 57  Match
Output size: 58  Match
Output size: 59  Match
Output size: 60  Match
Output size: 61  Match
Output size: 62  Match
Output size: 63  Match
Output size: 64  Match
Output size: 65  Match
Output size: 66  Match
Output size: 67  Match
Output size: 68  Match
Output size: 69  Match
Output size: 70  Match
Output size: 71  Match
Output size: 72  Match
```
