#include <libmuu/mman.h>
#include <libmuu/util.h>

#include <stdint.h>
#include <stdbool.h>

#define REGIONS_LENGTH (MEM_MANAGER_MAX_REGIONS+2)

static void align(void) {}

struct MemManager *MemManagerInit(struct MemRegion region) {
  struct MemManager *mman = (void*) region.start;
  
  mman->entire_region = region;
  
  mman->current_start_idx = 0;
  mman->regions[mman->current_start_idx] = (struct MemRegion) {
    .start = region.start + sizeof(struct MemManager),
    .end   = region.start + sizeof(struct MemManager),

    .grow_direction = MEM_GROW_NONE
  };
  
  mman->current_end_idx = REGIONS_LENGTH - 1;
  mman->regions[mman->current_end_idx] = (struct MemRegion) {
    .start = region.end,
    .end   = region.end,

    .grow_direction = MEM_GROW_NONE
  };

  return mman;
}

static void MemManagerEnsureTopOfStack(
  struct MemManager *mman,
  struct MemRegion *region
) {
  switch (region->grow_direction) {
    case MEM_GROW_UP:
      if (mman->regions + mman->current_start_idx == region) {
        return;
      }
      break;

    case MEM_GROW_DOWN:
      if (mman->regions + mman->current_end_idx == region) {
        return;
      }
      break;

    default:;
  }
  fatal("region must be at top of stack to be valid");
}

struct MemRegion *MemManagerPushRegion(
  struct MemManager *mman, 
  enum MemGrowDirection grow_dir
) {
  if (mman->current_start_idx + mman->current_end_idx + 1 > REGIONS_LENGTH) {
    fatal("ran out of regions to allocate into");
  }

  switch (grow_dir) {
    case MEM_GROW_UP: {
      uintptr_t base_address = mman->regions[mman->current_start_idx].end;
      mman->current_start_idx++;

      mman->regions[mman->current_start_idx] = (struct MemRegion) {
        .start = base_address,
        .end   = base_address,

        .grow_direction = grow_dir
      };

      return &mman->regions[mman->current_start_idx];
  }
    
  case MEM_GROW_DOWN: {
      uintptr_t base_address = mman->regions[mman->current_end_idx].start;
      mman->current_end_idx--;

      mman->regions[mman->current_end_idx] = (struct MemRegion) {
        .start = base_address,
        .end   = base_address,

        .grow_direction = grow_dir
      };

      return &mman->regions[mman->current_end_idx];
  }

    default:
      fatal("can only push a region of direction MEM_GROW_UP or MEM_GROW_DOWN");
      // should never reach beyond fatal
      return NULL;
  }
}

// removes the region from the stack and invalidates it
//void MemManagerPopRegion(struct MemManager*, struct MemRegion*);

void MemManagerResizeRegion(
  struct MemManager *mman, 
  struct MemRegion *region,
  size_t size
) {
  MemManagerEnsureTopOfStack(mman, region);

  if (region->grow_direction == MEM_GROW_UP) {
    region->end = region->start + size;
  } else {
    region->start = region->end - size;
  }
}

/*

struct AllocBump *AllocBumpInit(struct MemManager*);
struct AllocBumpDestroy(struct AllocBump*);

// -1 align uses last alignment
void *AllocBumpInsert(struct AllocBump*, size_t size, ssize_t align);
// only the most recently inserted element can be resized
void AllocBumpResize(struct AllocBump*, void*, size_t size);

void AllocPoolDestroy(struct AllocPool*);

void *AllocPoolInsert(struct AllocBumpPool*);
void *AllocPoolRemove(struct AllocBumpPool*);
*/
