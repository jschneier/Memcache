## Memcache Server

My attempt at implementing the memcache protocol in C. I wanted to learn about
threads, network sockets and reader/writer locks.

### Usage

To build the project run (from the top level directory where the Makefile is)

    make memcache

Now the memcache server can be started by running:

    ./memcache <port>

### Still to come

  * Timeouts and Check And Set (CAS) commands are still not fully implemented
  * Good statistics calculations
