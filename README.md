# Memory Pool

Thread Safe Memory Pool

1. Memory Pool to be used by any number of thread for alloc and free of a particular kind of object
2. Size of memory pool is given by constructor parameter

# Compile Steps
1. ./compile.sh

# Run Steps
2. ./run.sh

# Example verbose output 
Memory Pool Size :: 30 Number of threads :: 3 Iteration in Thread :: 10 Verbose :: 1

>>>Unique Allocation Test

Thread #0
Used Space Locations
29 26 23 22 18 15 12 11 7 5 

Thread #1
Used Space Locations
28 24 20 17 14 10 6 3 1 0 

Thread #2
Used Space Locations
27 25 21 19 16 13 9 8 4 2 
>>>PASS

Remaining indexes in MemoryPool 0


Remaining indexes in MemoryPool after recycle 30
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 

>>>Alloc Free Space Consistency Check

Thread #0
Used Space Locations
13 16 24 28 
Freed Space Location
14 17 19 21 23 26 

Thread #1
Used Space Locations
14 18 20 23 26 
Freed Space Location
15 17 18 19 29 

Thread #2
Used Space Locations
12 15 17 19 21 22 25 27 29 
Freed Space Location
17 
>>>PASS

Remaining indexes in MemoryPool 12
0 1 2 3 4 5 6 7 8 9 10 11 

Memory Pool Size :: 30 Number of threads :: 3 Iteration in Thread :: 10 Verbose :: 0

>>>Unique Allocation Test
>>>PASS

>>>Alloc Free Space Consistency Check
>>>PASS

Memory Pool Size :: 300 Number of threads :: 3 Iteration in Thread :: 100 Verbose :: 0

>>>Unique Allocation Test
>>>PASS

>>>Alloc Free Space Consistency Check
>>>PASS

Memory Pool Size :: 3000 Number of threads :: 3 Iteration in Thread :: 1000 Verbose :: 0

>>>Unique Allocation Test
>>>PASS

>>>Alloc Free Space Consistency Check
>>>PASS

# TODO
1. Make Memory Pool resizable
2. Use other indexing technique to manage memory
