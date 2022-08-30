# Memory-Allocator
This is an implementation of the C memory allocation library: malloc, calloc,
realloc, and free

Right now, there is an issue with realloc not preserving memory properly, however the basic allocation and free works well. They also right now function at
roughly 1.8x the speed of the standard implementations. I hope to lower that difference and make my implementations more efficient, after I've fixed the 
reallocation issue.
