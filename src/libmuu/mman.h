#pragma once

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// TODO: zeroing
// TODO: IRAM, libmuu


enum MemGrowDirection {
  MEM_GROW_NONE,

  // standard extending behavior, no special considerations neccesary
  MEM_GROW_UP,

  // addresses decrease when extending a region, does not work for
  // resizable arrays,
  // but is suitable for graph/tree like data structures and pools
  MEM_GROW_DOWN,
};

struct MemRegion {
  uintptr_t start;
  uintptr_t end;
  
  enum MemGrowDirection grow_direction;
};

#define MEM_MANAGER_MAX_REGIONS 16
/*
  the memory allocator is implemented as two stacks, one growing upward,
  the other growing downward. as such, it is only valid to operate on a region
  at the top of either stack (ie most recently added).

  ONLY THE MEMORY MANAGER SHOULD MUTATE THE START/END OF ANY REGION IT OWNS
*/
struct MemManager {
  // the structure itself is included in this region
  struct MemRegion entire_region;
  
  // double ended (both ends grow towards the middle, sorted array)
  size_t current_start_idx;
  size_t current_end_idx;
  // two extra positions are needed to store the start and end sentinels
  struct MemRegion regions[MEM_MANAGER_MAX_REGIONS+2];
};

struct MemManager *MemManagerInit(struct MemRegion);

struct MemRegion *MemManagerPushRegion(
  struct MemManager*, 
  enum MemGrowDirection
);

// removes the region from the stack and invalidates it
void MemManagerPopRegion(struct MemManager*, struct MemRegion*);

void MemManagerResizeRegion(
  struct MemManager*, 
  struct MemRegion*, 
  size_t size
);

/*
  bump allocator
*/
struct AllocBump {
  struct MemManager *mman;
  struct MemRegion *region;
  size_t align;
};

struct AllocBump *AllocBumpInit(struct MemManager*);
void AllocBumpDestroy(struct AllocBump*);

// -1 align uses last alignment
void *AllocBumpInsert(struct AllocBump*, size_t size, ssize_t align);
// only the most recently inserted element can be resized
void AllocBumpResize(struct AllocBump*, void*, size_t size);

/*
  zero wasted space allocator for many items of the same type
  TODO: generalize to bitmap allocator w/blocksize
*/
struct AllocPool {
  struct MemManager *mman;
  struct MemRegion *region;

  size_t align;
  size_t element_size;

  size_t max_size;
  uint32_t *bitmap;
};

struct AllocPool *AllocPoolInit(
  struct MemManager*,
  size_t element_size,
  size_t align,

  // the highest number of elements the allocator will be able to handle.
  // this is used to allocate space to keep track of all allocations at the 
  // beginning. memory used will be maximum_elements/8 bytes, rounded up to a
  // multiple of 4
  size_t maximum_elements
);
void AllocPoolDestroy(struct AllocPool*);

void *AllocPoolInsert(struct AllocPool*);
void *AllocPoolRemove(struct AllocPool*);
