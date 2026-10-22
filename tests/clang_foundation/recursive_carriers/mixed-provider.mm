#include "mixed-boundary.h"

extern "C" id class_createInstance(Class, __SIZE_TYPE__)
    __attribute__((ns_returns_retained));
extern "C" id object_dispose(__unsafe_unretained id);

int mixed_objects_destroyed, mixed_constructions, mixed_copies;
int mixed_moves, mixed_destructions, mixed_live, mixed_borrows;
bool mixed_throw_copy, mixed_throw_move, mixed_throw_destruction;
static int events[32], event_count;

void mixed_reset() {
  mixed_objects_destroyed = mixed_constructions = mixed_copies = 0;
  mixed_moves = mixed_destructions = mixed_live = mixed_borrows = 0;
  mixed_throw_copy = mixed_throw_move = mixed_throw_destruction = false;
  mixed_reset_events();
}
void mixed_event(int value) noexcept { if (event_count < 32) events[event_count++] = value; }
int mixed_events() { return event_count; }
int mixed_event_at(int index) { return index < event_count ? events[index] : -1; }
void mixed_reset_events() { event_count = 0; }

@implementation MixedProbe
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc {
  ++mixed_objects_destroyed;
  mixed_event(9);
  object_dispose(self);
}
@end

MixedItem::MixedItem(int value) : value(value) {
  ++mixed_constructions;
  ++mixed_live;
}
MixedItem::MixedItem(const MixedItem &source) : value(source.value) {
  if (mixed_throw_copy) throw 81;
  ++mixed_copies;
  ++mixed_live;
}
MixedItem::MixedItem(MixedItem &&source) : value(source.value) {
  if (mixed_throw_move) throw 82;
  source.value = -1;
  ++mixed_moves;
  ++mixed_live;
}
MixedItem::~MixedItem() noexcept(false) {
  ++mixed_destructions;
  --mixed_live;
  mixed_event(3);
  if (mixed_throw_destruction) {
    mixed_throw_destruction = false;
    throw 83;
  }
}

const MixedTree &mixed_borrow(const MixedTree &source) {
  ++mixed_borrows;
  return source;
}
MixedTree &mixed_mutable_borrow(MixedTree &source) {
  ++mixed_borrows;
  return source;
}
void mixed_move_and_drop(MixedTree &source) noexcept {
  MixedTree moved = moveValue(source);
}
