#define _LARGEFILE64_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

typedef struct pfn_info {
  uint64_t pfn : 55;
  bool soft_dirty : 1;
  bool exclusive : 1;
  uint8_t padding : 4;
  bool file_or_shared_anon : 1;
  bool swapped : 1;
  bool present : 1;
} __attribute__((packed)) pfn_info_t;

int main(int argc, char** argv) {
  if(argc != 3) {
    fprintf(stderr, "Usage: %s <pid> <virtual address>\n", argv[0]);
    return 1;
  }

  // Read in the process id
  pid_t pid;
  if(sscanf(argv[1], "%d", &pid) != 1) {
    fprintf(stderr, "Invalid pid %s\n", argv[1]);
    return 1;
  }
  
  // Read in the virtual address
  uint64_t address;
  if(sscanf(argv[2], "0x%lx", &address) != 1) {
    fprintf(stderr, "Invalid address %s\n", argv[2]);
    return 1;
  }

  // Compute page index and offset
  size_t page_size = getpagesize();

  size_t page_index = address / page_size;
  size_t page_offset = address % page_size;

  // Generate the filename for the pagemap
  char filename[256];
  snprintf(filename, 256, "/proc/%d/pagemap", pid);

  // Open the pagemap file
  FILE* f = fopen(filename, "rb");
  if(f == NULL) {
    fprintf(stderr, "Failed to open procmap for process %d. Does the process exist?\n", pid);
    return 2;
  }

  // Seek to the entry for the specified virtual page
  if(fseek(f, page_index * sizeof(pfn_info_t), SEEK_SET) == -1) {
    fprintf(stderr, "Failed to seek to requested entry in pagemap.\n");
    return 2;
  }

  // Read the entry
  pfn_info_t entry;
  if(fread(&entry, sizeof(pfn_info_t), 1, f) != 1) {
    fprintf(stderr, "Failed to read pagemap entry.\n");
    return 2;
  }

  if(!entry.present) {
    printf("Not present\n");
  } else if(entry.swapped) {
    printf("Swapped\n");
  } else {
    printf("0x%016lx\n", entry.pfn * page_size + page_offset);
  }

  return 0;
}

