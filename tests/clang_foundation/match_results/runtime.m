#ifndef OBJC3_MATCH_RUNTIME_WITNESS
#define OBJC3_MATCH_RUNTIME_WITNESS
#include <assert.h>
#include <objc/runtime.h>

#if __has_feature(objc_arc)
static int created, destroyed;
@protocol Error @end
__attribute__((objc_root_class))
@interface MatchObject <Error> { Class isa; @public int number; }
+ (instancetype)make:(int)value __attribute__((ns_returns_retained));
@end
@implementation MatchObject
+ (instancetype)make:(int)value {
  MatchObject *object = class_createInstance(self, 0);
  object->number = value;
  ++created;
  return object;
}
- (void)dealloc { ++destroyed; }
@end
#endif

struct Pair { int first, second; };
static int selected_calls, other_calls;
static struct Pair pair(int value) { ++selected_calls; return (struct Pair){value, 42}; }
static struct Pair other(void) { ++other_calls; return (struct Pair){-1, -1}; }

static void plain(void) {
  Optional(int) source = Some(int, 8);
  Optional(int) value = match(source) {
    case .Some(let payload) => (source = None(int), Some(int, payload + 34));
    case .None => None(int);
  };
  assert(!hasValue(source));
  match(value) { case .Some(let payload): { assert(payload == 42); } case .None: { assert(0); } }
  int input = 5;
  int snapshot = match(input) {
    case let bound => ((input = 99), bound);
  };
  assert(snapshot == 5 && input == 99);
  source = Some(int, 8);
  match(source) {
    case .Some(let payload) where ((source = None(int)), (__typeof__(hasValue(source)))0): { (void)payload; assert(0); }
    case .Some(let payload): { assert(payload == 8); }
    case .None: { assert(0); }
  }
  assert(!hasValue(source));
  struct Pair record = match(value) {
    case .Some(let payload) => pair(payload);
    case .None => other();
  };
  assert(record.first == 42 && record.second == 42);
  assert(selected_calls == 1 && other_calls == 0);
  Result(int, int) result = match(value) {
    case .Some(let payload) => Ok(int, int, payload);
    case .None => Err(int, int, -1);
  };
  assert(isOk(result));
  // Native scalar representations include vectors and complex values.
  typedef int IntVector __attribute__((vector_size(16)));
  IntVector vector = match(value) {
    case .Some(let payload) => ((IntVector){payload, 2, 3, 4});
    case .None => ((IntVector){-1, -2, -3, -4});
  };
  assert(vector[0] == 42 && vector[3] == 4);
  _Complex double left, right;
  __real__ left = 12.5; __imag__ left = 3.25;
  __real__ right = -8.0; __imag__ right = 9.0;
  _Complex double complex_value = match(value) {
    case .Some(_) => left;
    case .None => right;
  };
  assert(__real__ complex_value == 12.5 && __imag__ complex_value == 3.25);
  complex_value = match(None(int)) {
    case .Some(_) => left;
    case .None => right;
  };
  assert(__real__ complex_value == -8.0 && __imag__ complex_value == 9.0);
  int (^block)(void) = match(value) {
    case .Some(_) => ^{ return 42; };
    case .None => ^{ return -1; };
  };
  assert(block() == 42);
}

#if __has_feature(objc_arc)
static Optional(id) fail_native(void) {
  @throw [MatchObject make:71];
}
static Optional(id) fail_checked(void) throws(id<Error>) {
  throwError((id<Error>)[MatchObject make:72]);
}
static void owned_values(void) {
  id __weak witness = (id)0;
  @autoreleasepool {
    MatchObject *object = match(Some(id, [MatchObject make:81])) {
      case .Some(let payload) => (MatchObject *)payload;
      case .None => (MatchObject *)(id)0;
    };
    witness = object;
    assert(object->number == 81 && witness != (id)0);
    object = (id)0;
    assert(witness == (id)0);

    // Keep an ARC-loaded object alive after its strong arm temporary dies.
    Optional(id) source = Some(id, [MatchObject make:82]);
    MatchObject *selected = match(source) {
      case .Some(let payload) => (source = None(id), (MatchObject *)payload);
      case .None => (MatchObject *)(id)0;
    };
    witness = selected;
    assert(selected->number == 82 && !hasValue(source));
    selected = (id)0;
    assert(witness == (id)0);

    MatchObject *original = [MatchObject make:89];
    MatchObject *snapshot = match(original) {
      case (id)0 => (MatchObject *)(id)0;
      case let bound => (original = (id)0, bound);
    };
    witness = snapshot;
    assert(original == (id)0 && snapshot->number == 89);
    snapshot = (id)0;
    assert(witness == (id)0);
    assert(match((id)0) { case (id)0 => 7; default => 0; } == 7);
    MatchObject *owner = [MatchObject make:91];
    MatchObject *__weak weak_source = owner;
    MatchObject *weak_snapshot = match(weak_source) {
      case (id)0 => (MatchObject *)(id)0;
      case let bound => (owner = (id)0, bound);
    };
    assert(weak_snapshot->number == 91 && weak_source != (id)0);
    weak_snapshot = (id)0;
    assert(weak_source == (id)0);

    int (^original_block)(void) = ^{ return 90; };
    int (^block_snapshot)(void) = match(original_block) {
      case (void *)0 => (int (^)(void))0;
      case let bound => (original_block = (int (^)(void))0, bound);
    };
    assert(original_block == (int (^)(void))0 && block_snapshot() == 90);
    block_snapshot = (int (^)(void))0;

    int (^copied)(void) = match(Some(id, [MatchObject make:83])) {
      case .Some(let payload) => ^{ return ((MatchObject *)payload)->number; };
      case .None => ^{ return -1; };
    };
    assert(copied() == 83);
    copied = (int (^)(void))0;
  }
  assert(created == destroyed);
  @autoreleasepool {
    // The +0 match result survives to the surrounding full-expression.
    assert(((MatchObject *)match(Some(id, [MatchObject make:84])) {
      case .Some(let payload) => payload;
      case .None => (id)0;
    })->number == 84);
    (void)match(Some(id, [MatchObject make:85])) {
      case .Some(let payload) => payload;
      case .None => (id)0;
    };
    assert(created == destroyed);
    int side = 0;
    if (side && match(Some(id, [MatchObject make:86])) {
        case .Some(let payload) => payload;
        case .None => (id)0;
      }) ++side;
    assert(side == 0 && created == destroyed);
  }
  assert(created == destroyed);
}

static void owned(void) {
  id __weak witness = (id)0;
  @autoreleasepool {
    MatchObject *object = [MatchObject make:42];
    witness = object;
    Optional(id) source = Some(id, object);
    object = (id)0;
    Optional(id) value = match(source) {
      case .Some(let payload) => (source = None(id), Some(id, payload));
      case .None => None(id);
    };
    assert(!hasValue(source) && hasValue(value) && witness != (id)0);
    match(value) {
      case .Some(let payload): { assert(((MatchObject *)payload)->number == 42); }
      case .None: { assert(0); }
    }
    value = None(id);
    assert(witness == (id)0);
  }
  assert(created == destroyed);
  @autoreleasepool {
    (void)match(Some(id, [MatchObject make:43])) {
      case .Some(let payload) => Some(id, payload);
      case .None => None(id);
    };
    assert(created == destroyed);
    assert(hasValue(match(None(id)) {
      case .Some(let payload) => Some(id, payload);
      case .None => Some(id, [MatchObject make:44]);
    }));
    assert(created == destroyed);
    Optional(id) source = Some(id, [MatchObject make:45]);
    Optional(id) destination = Some(id, [MatchObject make:46]);
    int caught = 0;
    @try {
      destination = match(source) {
        case .Some(_) => fail_native();
        case .None => None(id);
      };
      assert(0);
    } @catch(MatchObject *error) {
      assert(error->number == 71);
      caught = 1;
    }
    assert(caught && hasValue(destination) && hasValue(source));
    caught = 0;
    handle {
      destination = match(source) {
        case .Some(_) => tryValue(fail_checked());
        case .None => None(id);
      };
      assert(0);
    } catch(id<Error> error) {
      assert(((MatchObject *)error)->number == 72);
      caught = 1;
    }
    assert(caught && hasValue(destination) && hasValue(source));
    match(destination) {
      case .Some(let payload): { assert(((MatchObject *)payload)->number == 46); }
      case .None: { assert(0); }
    }
  }
  assert(created == destroyed);
}
#endif

#ifdef __cplusplus
struct Lifetime {
  static int live, made, removed;
  int value;
  explicit Lifetime(int v) : value(v) { if (v == 99) throw 99; ++live; ++made; }
  Lifetime(const Lifetime &other) : value(other.value) { ++live; ++made; }
  Lifetime(Lifetime &&other) : value(other.value) { ++live; ++made; }
  ~Lifetime() { --live; ++removed; }
};
int Lifetime::live, Lifetime::made, Lifetime::removed;
struct ThrowAfter {
  ~ThrowAfter() noexcept(false) { throw 91; }
};
static Optional(int) make_source(int value, const ThrowAfter &) {
  return Some(int, value);
}
template <class T> T choose(Optional(int) source, const T &left, const T &right) {
  return match(source) {
    case .Some(_) => left;
    case .None => right;
  };
}
struct MemberRecord {
  int first, second;
  int get_first() const { return first; }
  int get_second() const { return second; }
};
struct Immovable {
  int value;
  explicit Immovable(int v) : value(v) {}
  Immovable(const Immovable &) = delete;
  Immovable(Immovable &&) = delete;
  ~Immovable() {}
};
struct MoveOnly {
  int value;
  explicit MoveOnly(int v) : value(v) {}
  MoveOnly(const MoveOnly &) = delete;
  MoveOnly(MoveOnly &&other) : value(other.value) { other.value = -1; }
  ~MoveOnly() {}
};
__attribute__((noinline)) static void match_value_checkpoint(int observed) {
  __asm__ volatile("" : : "r"(observed) : "memory");
}
static void cpp(void) {
  Optional(int) source = Some(int, 42);
  MemberRecord record{17, 29};
  int MemberRecord::*data = match(source) {
    case .Some(_) => &MemberRecord::second;
    case .None => &MemberRecord::first;
  };
  assert(record.*data == 29);
  int (MemberRecord::*method)() const = match(None(int)) {
    case .Some(_) => &MemberRecord::get_first;
    case .None => &MemberRecord::get_second;
  };
  assert((record.*method)() == 29);
  {
    Lifetime value = match(source) {
      case .Some(let payload) => Lifetime(payload);
      case .None => Lifetime(-1);
    };
    assert(value.value == 42 && Lifetime::live == 1);
  }
  assert(Lifetime::live == 0 && Lifetime::made == Lifetime::removed);
  (void)match(source) {
    case .Some(let payload) => Lifetime(payload);
    case .None => Lifetime(-1);
  };
  assert(Lifetime::live == 0 && Lifetime::made == Lifetime::removed);
  try {
    Lifetime value = match(source) {
      case .Some(let payload) => ((void)ThrowAfter{}, Lifetime(payload));
      case .None => Lifetime(-1);
    };
    (void)value;
    assert(0);
  } catch (int error) { assert(error == 91); }
  assert(Lifetime::live == 0 && Lifetime::made == Lifetime::removed);
  try {
    Lifetime value = match(make_source(42, ThrowAfter{})) {
      case .Some(let payload) => Lifetime(payload);
      case .None => Lifetime(-1);
    };
    (void)value;
    assert(0);
  } catch (int error) { assert(error == 91); }
  assert(Lifetime::live == 0 && Lifetime::made == Lifetime::removed);
  try {
    (void)match(Some(int, 99)) {
      case .Some(let payload) => Lifetime(payload);
      case .None => Lifetime(-1);
    };
    assert(0);
  } catch (int error) { assert(error == 99); }
  assert(Lifetime::live == 0 && Lifetime::made == Lifetime::removed);
  {
    Lifetime left(10), right(20);
    {
      Lifetime copied = choose(source, left, right);
      assert(copied.value == 10 && Lifetime::live == 3);
      match_value_checkpoint(copied.value);
    }
    assert(Lifetime::live == 2);
  }
  assert(Lifetime::live == 0 && Lifetime::made == Lifetime::removed);
  Immovable direct = match(source) {
    case .Some(let payload) => Immovable(payload);
    case .None => Immovable(-1);
  };
  assert(direct.value == 42);
#if __has_feature(objc_arc)
  @autoreleasepool {
    int before = created - destroyed;
    try {
      MatchObject *value = match(source) {
        case .Some(_) => ((void)ThrowAfter{}, [MatchObject make:87]);
        case .None => (MatchObject *)(id)0;
      };
      (void)value;
      assert(0);
    } catch (int error) { assert(error == 91); }
    assert(created - destroyed == before);
    try {
      MatchObject *value = match(make_source(42, ThrowAfter{})) {
        case .Some(_) => [MatchObject make:88];
        case .None => (MatchObject *)(id)0;
      };
      (void)value;
      assert(0);
    } catch (int error) { assert(error == 91); }
    assert(created - destroyed == before);
  }
#endif

  MoveOnly first(17), second(23);
  MoveOnly moved = match(source) {
    case .Some(_) => static_cast<MoveOnly &&>(first);
    case .None => static_cast<MoveOnly &&>(second);
  };
  assert(moved.value == 17 && first.value == -1 && second.value == 23);
}
#endif

static int runtime_contract(void) {
  plain();
#if __has_feature(objc_arc)
  owned();
  owned_values();
#endif
#ifdef __cplusplus
  cpp();
#endif
  return 0;
}

#endif
#ifndef OBJC3_MATCH_PCH
int main(void) { return runtime_contract(); }
#endif
