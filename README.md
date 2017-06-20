# `get-pfn`

*Warning: You probably do not want this tool!* This program grants any user access to the physical addresses mapped into arbitrary executables, which is almost certainly a security vulnerability. This is meant to be used as a utility to grant non-privileged users access to the Linux pagemap files, not a tool for general use.

## Building and Installing
The `get-pfn` tool is installed to `/usr/local/bin` by default. To build and install, run:

    make all && sudo make install

## Usage
There are two ways to use this tool: via the command line, or through a C++ wrapper defined in pagemap.hh.

### Command Line Usage
To get the physical address for a program's virtual address, run `get-pfn`:

    get-pfn <pid> 0x<virtual address>

For example, to get the physical address for the virtual address `0x400017` in process `457`, run:

    get-pfn 457 0x400017

The `get-pfn` tool will output the physical address if it is mapped, or zero if it is not mapped. If you do not provide the virtual address as a command line argument, the tool will run in interactive mode. In interactive mode, the tool expects to receive a virtual address *in binary form* via standard in. It well then write the physical address out to standard out, again in binary form. This is primarily useful for access via the C++ wrapper.

### Using the C++ Wrapper
To look up physical addresses programmatically, use the C++ wrapper to start a get-pfn process in interactive mode. This wrapper sets up a communication channel with the child and hides the communication details behind a simple API.
To use the wrapper, just include the `<pagemap.hh>` include file and build with C++11 or later. The following example shows a simple lookup:

    #include <pagemap.hh>
    #include <stdio.h>
    #include <sys/types.h>
    #include <unistd.h>
    
    int main() {
      int x;
      pagemap map(getpid());
      printf("x is at virtual address %p\n", &x);
      printf("x is at physical address %p\n", map[&x]);
      return 0;
    }

You can perform lookups using the `lookup` member function, which takes a `uintptr_t`, or using array-style indexing with a `uintptr_t`. In both cases, the pagemap will return a `uintptr_t` for the physical address. There is also a template member function that allows you to perform a lookup using at pointer type, with the result returned as a `void*`. The example above uses this method.

