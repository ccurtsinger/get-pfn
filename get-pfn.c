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

FILE* pagemap;
size_t page_size;

uint64_t get_pfn(uint64_t address);

int main(int argc, char** argv) {
  if(argc != 2 && argc != 3) {
    fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
    fprintf(stderr, "       %s <pid> <virtual address>\n", argv[0]);
    return 1;
  }

  // Read in the process id
  pid_t pid;
  if(sscanf(argv[1], "%d", &pid) != 1) {
    fprintf(stderr, "Invalid pid %s\n", argv[1]);
    return 1;
  }

  // Get the system page size
  page_size = getpagesize();

  // Generate the filename for the pagemap
  char filename[256];
  snprintf(filename, 256, "/proc/%d/pagemap", pid);

  // Open the pagemap file
  pagemap = fopen(filename, "rb");
  if(pagemap == NULL) {
    fprintf(stderr, "Failed to open pagemap for process %d. Does the process exist?\n", pid);
    return 2;
  }
  
  // Are we running in single address mode?
  if(argc == 3) {
    // Read in the virtual address
    uint64_t address;
    if(sscanf(argv[2], "0x%lx\n", &address) != 1) {
      fprintf(stderr, "Invalid address %s\n", argv[2]);
      return 1;
    }
    uint64_t pfn = get_pfn(address);
    printf("0x%016lx\n", pfn);
    
  } else {
    // Running in multi-address mode. Read addresses from stdin
    uint64_t address;
    while(fread(&address, sizeof(uint64_t), 1, stdin) == 1) {
      fprintf(stderr, "0x%016lx\n", address);
      uint64_t pfn = get_pfn(address);
      fwrite(&pfn, sizeof(uint64_t), 1, stdout);
    }
  }

  return 0;
}

uint64_t get_pfn(uint64_t address) {
  // Compute page index and offset
  size_t page_index = address / page_size;
  size_t page_offset = address % page_size;

  // Seek to the entry for the specified virtual page
  if(fseek(pagemap, page_index * sizeof(pfn_info_t), SEEK_SET) == -1) {
    fprintf(stderr, "Failed to seek to requested entry in pagemap.\n");
    exit(2);
  }

  // Read the entry
  pfn_info_t entry;
  if(fread(&entry, sizeof(pfn_info_t), 1, pagemap) != 1) {
    fprintf(stderr, "Failed to read pagemap entry.\n");
    exit(2);;
  }
  
  if(entry.present) return entry.pfn * page_size + page_offset;
  else return 0;
}

