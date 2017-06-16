#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

bool initialized = false;
pid_t child_process;
int input_pipe[2];
int output_pipe[2];

uint64_t getpfn(uint64_t address) {
  if(!initialized) {
    // Set up pipes for parent-child communication
    if(pipe(input_pipe) != 0 || pipe(output_pipe) != 0) {
      perror("pipe");
      abort();
    }

    // Make a child process
    child_process = fork();

    // Run in the child
    if(child_process == 0) {
      // Replace stdin in the child
      if(dup2(STDIN_FILENO
    }
  }
}

