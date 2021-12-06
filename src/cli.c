#include <libmuu/mman.h>
#include <libmuu/fs.h>
#include <libmuu/util.h>
#include <stdlib.h>

#define MEM_SIZE (64*1024)

static struct MemManager *InitializeMemorySystem() {
  uintptr_t backing_memory_ptr = (uintptr_t) malloc(MEM_SIZE);
  assert_fatal(backing_memory_ptr, NULL);

  struct MemRegion region = {
    .start = backing_memory_ptr,
    .end   = backing_memory_ptr + MEM_SIZE,

    .grow_direction = MEM_GROW_NONE
  };

  return MemManagerInit(region);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fatal("expected exactly one argument");
  }
  
  struct MemManager *mman = InitializeMemorySystem();
  struct MemRegion *file = MemManagerPushRegion(mman, MEM_GROW_DOWN);

  FSLoadFile(mman, file, argv[1]);

  write(1, (char*) file->start, file->end - file->start);
  
  return 0;
}
