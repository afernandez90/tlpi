# Exercise 6.1
**Compile the program in Listing 6-1 (`mem_segments.c`), and list its size 
using `ls -l`. Although the program contains an array (`mbuf`) that is 
around 10 MB in size, the executable file is much smaller than this. Why
is this?**

Because the uninitialized data segment is not explicitly stored on disk.
Instead, only its size is persisted in the binary program. When the kernel
loads the program into a process, it allocates the uninitialized data
segment using the stored size value and initializes it to zero.
