# SlidingBloomfilter-Sketch

A Count-Min sketch with Bloom-filter for summarizing streaming data over sliding windows.

The file stream.dat is the real IP trace from the main gateway at our campus. This small dataset contains 1M items totally and 193,894 distinct items.



## How to run

Suppose you've already cloned the respository and start from the `SlidingBloomfilter-Sketch` directory.

You just need:

```
$ make 
$ ./main
```