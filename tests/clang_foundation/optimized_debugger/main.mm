#include <assert.h>

typedef Optional(int) DebugNumber;
static volatile int hidden_input = 17;
static int events;

extern "C" __attribute__((noinline)) int objc3_debug_hidden_value() {
  return hidden_input;
}

extern "C" __attribute__((noinline)) void
objc3_optimized_debug_checkpoint(int phase, const void *storage, int observed) {
  __asm__ volatile("" : : "r"(phase), "r"(storage), "r"(observed) : "memory");
}

struct Cleanup {
  ~Cleanup() { events = events * 10 + 1; }
};

__attribute__((noinline)) static void optimized_scope() {
  DebugNumber number = Some(int, 42);
  // This value has no observable source use. Optimized debug information may
  // retain 17 or accurately report it unavailable at the subsequent call.
  int optimized_away = objc3_debug_hidden_value();
  (void)optimized_away;
  objc3_optimized_debug_checkpoint(1, &number, 42);
  if let observed = number {
    assert(observed == 42);
  } else {
    assert(false);
  }
  number = None(int);
  objc3_optimized_debug_checkpoint(2, &number, 0);
  assert(!hasValue(number));
  try {
    Cleanup owned;
    defer { events = events * 10 + 2; }
    throw 9;
  } catch (int value) {
    assert(value == 9 && events == 21);
  }
  objc3_optimized_debug_checkpoint(3, &number, events);
}

int main() {
  optimized_scope();
  assert(events == 21);
  return 0;
}
