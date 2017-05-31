# `get-pfn`

*Warning: You probably do not want this tool!* This program grants any user access to the physical addresses mapped into arbitrary executables, which is almost certainly a security vulnerability. This is meant to be used as a utility to grant non-privileged users access to the Linux pagemap files, not a tool for general use.

## Building and Installing
The `get-pfn` tool is installed to `/usr/local/bin` by default. To build and install, run:

    make all && sudo make install

## Usage
To get the physical address for a program's virtual address, run `get-pfn`:

    get-pfn <pid> 0x<virtual address>

For example, to get the physical address for the virtual address `0x400017` in process `457`, run:

    get-pfn 457 0x400017

The `get-pfn` tool will output the physical address if it is mapped, "Not present" if the page is not mapped, or "Swapped" if the page is mapped but swapped to disk.
