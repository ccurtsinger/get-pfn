#if !defined(PAGEMAP_HH)
#define PAGEMAP_HH

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

class pagemap {
public:
  /// Set up a channel to a pfn lookup process for a given pid
  pagemap(pid_t pid) : _pid(pid) {
    // Set up the output pipe
    if(pipe(_output) != 0) {
      perror("failed to set up output pipe");
      abort();
    }

    // Set up the input pipe
    if(pipe(_input) != 0) {
      perror("failed to set up input pipe");
      abort();
    }

    // Fork
    _child = fork();

    // Check for failure
    if(_child == -1) {
      perror("fork failed");
      abort();
    }

    // Are we in the child?
    if(_child == 0) {
      // Replace standard input with the parent's output pipe
      if(dup2(_output[0], STDIN_FILENO) == -1) {
        perror("failed to set up standard input");
        abort();
      }

      // Replace standard output with the parent's input pipe
      if(dup2(_input[1], STDOUT_FILENO) == -1) {
        perror("failed to set up standard output");
        abort();
      }

      // Generate a string of the target's PID
      char buf[32];
      snprintf(buf, 32, "%d", _pid);

      // Exec the get-pfn utility
      execlp("get-pfn", "get-pfn", buf, NULL);

      // Unreachable, unless something goes wrong
      perror("failed to execute get-pfn");
      abort();
    }

    // No action is necessary in the parent process. Just return.
  }

  /// Clean up
  ~pagemap() {
    if(_child != -1) {
      close(_output[1]);
      close(_input[0]);
      kill(_child, SIGKILL);
    }
  }

  /// Look up the physical address for a given virtual address
  uintptr_t lookup(uintptr_t vaddr) {
    // Send the virtual address bytes to the get-pfn process
    if(write(_output[1], &vaddr, sizeof(uintptr_t)) != sizeof(uintptr_t)) {
      perror("failed to send address to get-pfn");
      abort();
    }

    // Read the physical address from the get-pfn process
    uintptr_t paddr;
    if(read(_input[0], &paddr, sizeof(uintptr_t)) != sizeof(uintptr_t)) {
      perror("failed to read result from get-pfn");
      abort();
    }

    return paddr;
  }

  /// Useful wrapper for array-style lookups
  uintptr_t operator[](uintptr_t vaddr) {
    return this->lookup(vaddr);
  }

  /// Useful wrapper for pointer-type lookups
  template<typename T> void* operator[](T* vaddr) {
    return (void*)this->lookup((uintptr_t)vaddr);
  }

private:
  /// Disallow copying
  pagemap(const pagemap& other) = delete;

  /// Disallow copying assignment
  pagemap& operator=(const pagemap& other) = delete;

  pid_t _pid;         //< PID of the process being examined
  pid_t _child = -1;  //< PID of the pfn lookup process
  int _output[2];     //< Pipe to send input to pfn lookup process
  int _input[2];      //< Pipe to receive output from pfn lookup process
};

#endif

