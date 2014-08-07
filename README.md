There are two main components in the repository: FlashGraph and SAFS. NOTE: FlashGraph is still under heavy development.

FlashGraph
===========

FlashGraph is a semi-external memory graph processing engine, optimized for a high-speed
SSD array. FlashGraph provides flexible programming interface to help users implement
graph algorithms. In FlashGraph, users write serial code that reads data in memory
and FlashGraph executes users' code in parallel and out of core.
It enables us to process a billion-node graph in a single machine
and has performance comparable to or exceed in-memory graph engines such as
[PowerGraph](http://graphlab.org/).

SAFS
========

`SAFS` is an open-source library that provides a filesystem-like interface
in the userspace to help users access a large SSD array in a
[NUMA](http://en.wikipedia.org/wiki/Non-uniform_memory_access) machine.
It is designed to eliminate overhead in the block subsystem in Linux, without modifying the kernel,
and achieves the maximal performance of a large SSD array in a NUMA machine.
FlashGraph is an application to demonstrate the power of SAFS.

Documentation
========

[FlashGraph Quick start guide](https://github.com/icoming/FlashGraph/wiki/FlashGraph-Quick-Start-Guide)

[FlashGraph User manual](https://github.com/icoming/FlashGraph/wiki/FlashGraph-User-Manual).

[FlashGraph performance](https://github.com/icoming/FlashGraph/wiki/Performance-of-FlashGraph)

[SAFS user manual](https://github.com/icoming/FlashGraph/wiki/SAFS-user-manual).

Publications
========

Da Zheng, Disa Mhembere, Randal Burns, Alexander S. Szalay, FlashGraph: Processing Billion-Node Graphs on an Array of Commodity SSDs, arXiv:1408.0500

Da Zheng, Randal Burns, Alexander S. Szalay, Toward Millions of File System IOPS on Low-Cost, Commodity Hardware, in Proceeding SC '13 Proceedings of the International Conference on High Performance Computing, Networking, Storage and Analysis

Contact
========

Mailing list: flashgraph-dev@googlegroups.com
