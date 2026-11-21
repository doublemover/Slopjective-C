#ifndef RECURSIVE_CARRIERS_MIXED_BOUNDARY_H
#define RECURSIVE_CARRIERS_MIXED_BOUNDARY_H

extern int mixed_objects_destroyed, mixed_constructions, mixed_copies;
extern int mixed_moves, mixed_destructions, mixed_live, mixed_borrows;
extern bool mixed_throw_copy, mixed_throw_move, mixed_throw_destruction;
void mixed_reset();
void mixed_event(int) noexcept;
int mixed_events();
int mixed_event_at(int);
void mixed_reset_events();

__attribute__((objc_root_class))
@interface MixedProbe { Class isa; }
+ (id)new;
@end

struct MixedItem {
  int value;
  explicit MixedItem(int);
  MixedItem(const MixedItem &);
  MixedItem(MixedItem &&);
  ~MixedItem() noexcept(false);
};
using MixedManaged = Optional(id);
using MixedNative = Optional(MixedItem);
using MixedTree = Result(MixedManaged, MixedNative);
using MixedOuter = Optional(MixedTree);

const MixedTree &mixed_borrow(const MixedTree &);
MixedTree &mixed_mutable_borrow(MixedTree &);
void mixed_move_and_drop(MixedTree &) noexcept;

#endif
