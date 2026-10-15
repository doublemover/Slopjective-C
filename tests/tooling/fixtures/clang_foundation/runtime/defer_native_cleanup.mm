// Native cleanup execution through the canonical Clang DeferStmt pipeline.
static int events[32];
static unsigned event_count;
static void record(int value) { events[event_count++] = value; }

struct Guard {
  ~Guard() { record(2); }
};

static int early_return() {
  int value = 1;
  __objc3_defer { record(value); }
  Guard guard;
  __objc3_defer { record(3); }
  value = 4;
  return 7;
}

static void loop_exits() {
  for (int index = 0; index != 2; ++index) {
    __objc3_defer { record(index + 10); }
    if (index == 0) continue;
    break;
  }
}

static void native_cpp_unwind() {
  try {
    __objc3_defer { record(30); }
    Guard guard;
    throw 5;
  } catch (int value) {
    record(value);
  }
}

static void goto_exit() {
  {
    __objc3_defer { record(40); }
    goto outside;
  }
outside:
  record(41);
}

static void normal_scope_exit() {
  {
    __objc3_defer { record(50); }
    __objc3_defer { record(51); }
  }
  record(52);
}

int main() {
  if (early_return() != 7) return 1;
  loop_exits();
  native_cpp_unwind();
  goto_exit();
  normal_scope_exit();
  const int expected[] = {3, 2, 4, 10, 11, 2, 30, 5, 40, 41, 51, 50, 52};
  if (event_count != sizeof(expected) / sizeof(*expected)) return 2;
  for (unsigned index = 0; index != event_count; ++index)
    if (events[index] != expected[index]) return 3;
  return 0;
}
