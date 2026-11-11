#include "mixed-boundary.h"

static int strong_leaf() {
  mixed_reset();
  __weak id observer = 0;
  {
    id object = [MixedProbe new];
    observer = object;
    MixedTree source = Ok(MixedManaged, MixedNative, Some(id, object));
    object = 0;
    {
      MixedTree copied = copyValue(source);
      MixedTree moved = moveValue(source);
      const MixedTree &constant = copied;
      MixedTree constMoved = moveValue(constant);
      if (!isOk(source) || !isOk(copied) || !isOk(moved) ||
          !isOk(constMoved) || !observer || mixed_live || mixed_objects_destroyed)
        return 1;
    }
    if (observer || mixed_objects_destroyed != 1 || mixed_destructions)
      return 2;
  }
  return mixed_objects_destroyed != 1 ? 3 : 0;
}

static int native_leaf() {
  mixed_reset();
  {
    MixedTree source = Err(MixedManaged, MixedNative,
                            Some(MixedItem, MixedItem(1)));
    MixedTree copied = copyValue(mixed_borrow(source));
    MixedTree moved = moveValue(mixed_mutable_borrow(source));
    if (isOk(source) || isOk(copied) || isOk(moved) || mixed_live != 3 ||
        mixed_copies != 1 || mixed_moves != 1 || mixed_borrows != 2 ||
        mixed_objects_destroyed)
      return 4;
  }
  return mixed_live || mixed_destructions != 3 ? 5 : 0;
}

static int empty_nested() {
  mixed_reset();
  {
    MixedOuter absent = None(MixedTree);
    MixedOuter present = Some(
        MixedTree, Err(MixedManaged, MixedNative, None(MixedItem)));
    MixedOuter copied = copyValue(present);
    MixedOuter moved = moveValue(absent);
    if (hasValue(absent) || hasValue(moved) || !hasValue(present) ||
        !hasValue(copied))
      return 6;
  }
  return mixed_destructions || mixed_objects_destroyed ? 7 : 0;
}

static int weak_and_unsafe() {
  mixed_reset();
  using Weak = Optional(id __weak);
  using WeakTree = Result(Weak, MixedNative);
  using Unsafe = Optional(id __unsafe_unretained);
  using UnsafeTree = Result(Unsafe, MixedNative);
  __weak id observer = 0;
  {
    id object = [MixedProbe new];
    observer = object;
    WeakTree source = Ok(Weak, MixedNative, Some(id __weak, object));
    WeakTree copied = copyValue(source);
    WeakTree moved = moveValue(source);
    UnsafeTree unsafe = Ok(
        Unsafe, MixedNative, Some(id __unsafe_unretained, object));
    object = 0;
    if (observer || mixed_objects_destroyed != 1 || !isOk(source) ||
        !isOk(copied) || !isOk(moved) || !isOk(unsafe))
      return 8;
  }
  return mixed_objects_destroyed != 1 || mixed_destructions ? 9 : 0;
}

static int block_leaf() {
  mixed_reset();
  using Action = void (^)(void);
  using ManagedAction = Optional(Action);
  using ActionTree = Result(ManagedAction, MixedNative);
  __weak id observer = 0;
  {
    id object = [MixedProbe new];
    observer = object;
    Action action = ^{ if (!object) __builtin_trap(); };
    ActionTree source = Ok(ManagedAction, MixedNative, Some(Action, action));
    ActionTree copied = copyValue(source);
    ActionTree moved = moveValue(source);
    action = 0;
    object = 0;
    if (!observer || mixed_objects_destroyed || !isOk(copied) || !isOk(moved))
      return 10;
  }
  return observer || mixed_objects_destroyed != 1 ? 11 : 0;
}

struct ThrowingTemporary {
  ~ThrowingTemporary() noexcept(false) { throw 84; }
};

static int rollback_and_unwind() {
  mixed_reset();
  __weak id observer = 0;
  try {
    id object = [MixedProbe new];
    observer = object;
    MixedTree value = Ok(
        MixedManaged, MixedNative, (ThrowingTemporary{}, Some(id, object)));
    return 12;
  } catch (int code) {
    if (code != 84 || observer || mixed_objects_destroyed != 1)
      return 13;
  }
  try {
    MixedTree value = Err(
        MixedManaged, MixedNative,
        (ThrowingTemporary{}, Some(MixedItem, MixedItem(2))));
    return 14;
  } catch (int code) {
    if (code != 84 || mixed_live || mixed_destructions != 1)
      return 15;
  }
  {
    MixedTree source = Err(MixedManaged, MixedNative,
                            Some(MixedItem, MixedItem(3)));
    mixed_throw_copy = true;
    try {
      MixedTree failed = copyValue(source);
      return 16;
    } catch (int code) {
      if (code != 81 || mixed_live != 1 || mixed_destructions != 1)
        return 17;
    }
    mixed_throw_copy = false;
    mixed_throw_move = true;
    try {
      MixedTree failed = moveValue(source);
      return 18;
    } catch (int code) {
      if (code != 82 || mixed_live != 1 || mixed_destructions != 1)
        return 19;
    }
    mixed_throw_move = false;
  }
  return mixed_live || mixed_destructions != 2 ? 20 : 0;
}

static int source_ordering() {
  mixed_reset();
  __weak id observer = 0;
  int failures = 0;
  {
    defer { mixed_event(5); if (!observer) ++failures; }
    MixedTree value = Ok(MixedManaged, MixedNative, Some(id, [MixedProbe new]));
    // A weak view observes that source-owned ARC payloads outlive every
    // source action, even though the same static type can own a C++ action.
    MixedTree borrowedCopy = copyValue(value);
    mixed_move_and_drop(borrowedCopy);
    // Capture a separately created strong object only through source storage.
    id object = [MixedProbe new];
    observer = object;
    MixedTree observed = Ok(MixedManaged, MixedNative, Some(id, object));
    object = 0;
    defer { mixed_event(7); if (!observer) ++failures; }
    mixed_reset_events();
  }
  if (failures || observer || mixed_objects_destroyed != 2 ||
      mixed_event_at(0) != 7 || mixed_event_at(1) != 5 ||
      mixed_event_at(2) != 9 || mixed_event_at(3) != 9)
    return 21;
  mixed_reset();
  {
    defer { mixed_event(5); }
    MixedTree value = Err(MixedManaged, MixedNative, Some(MixedItem, MixedItem(4)));
    defer { mixed_event(7); }
  }
  if (mixed_events() != 3 || mixed_event_at(0) != 7 ||
      mixed_event_at(1) != 3 || mixed_event_at(2) != 5)
    return 22;
  mixed_reset();
  try {
    defer { mixed_event(5); }
    MixedTree object = Ok(MixedManaged, MixedNative, Some(id, [MixedProbe new]));
    MixedTree native = Err(MixedManaged, MixedNative, Some(MixedItem, MixedItem(5)));
    mixed_throw_destruction = true;
  } catch (int code) {
    if (code != 83 || mixed_live || mixed_destructions != 1 ||
        mixed_objects_destroyed != 1 || mixed_events() != 3 ||
        mixed_event_at(0) != 3 || mixed_event_at(1) != 5 ||
        mixed_event_at(2) != 9)
      return 23;
  }
  return 0;
}

static int implicit_block_captures() {
  mixed_reset();
  {
    MixedTree source = Err(MixedManaged, MixedNative, Some(MixedItem, MixedItem(7)));
    {
      int (^__unsafe_unretained capture)(void) = ^{ return (int)isOk(source); };
      if (capture() || mixed_copies != 1 || mixed_live != 2) return 24;
    }
    if (mixed_live != 1 || mixed_destructions != 1) return 25;
  }
  return mixed_live || mixed_destructions != 2 ? 26 : 0;
}

int main() {
  if (int code = strong_leaf()) return code;
  if (int code = native_leaf()) return code;
  if (int code = empty_nested()) return code;
  if (int code = weak_and_unsafe()) return code;
  if (int code = block_leaf()) return code;
  if (int code = rollback_and_unwind()) return code;
  if (int code = source_ordering()) return code;
  if (int code = implicit_block_captures()) return code;
  return 0;
}
