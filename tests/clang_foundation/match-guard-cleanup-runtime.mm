typedef __objc3_optional(int) MaybeInt;

static int Failure;
static int Calls;
static int Events;

struct GuardProbe {
  int Event;
  ~GuardProbe() { Events = Events * 10 + Event; }
};

static GuardProbe make_probe(int Event) { return GuardProbe{Event}; }
static bool reject_guard(const GuardProbe &) { return false; }
static bool accept_guard(const GuardProbe &) { return true; }
static bool throw_guard(const GuardProbe &) { throw 7; }

static int next_scalar() {
  ++Calls;
  return 41;
}

static void check_scalar_lvalue(int &Source) {
  const int Snapshot = Source;
  __objc3_match_stmt (Source) {
  case let Borrowed: {
    if (&Borrowed == &Source || Borrowed != Snapshot)
      Failure = 1;
    Source += 10;
    if (Borrowed != Snapshot)
      Failure = 16;
  }
  }
}

static void check_scalar_prvalue() {
  const int Before = Calls;
  __objc3_match_stmt (next_scalar()) {
  case let Borrowed: {
    if (Borrowed != 41)
      Failure = 2;
  }
  }
  if (Calls != Before + 1)
    Failure = 3;
}

static void check_guard_normal_cleanup() {
  Events = 0;
  __objc3_match_stmt (true) {
  case true where reject_guard(make_probe(1)): {
    Failure = 4;
  }
  case true: {
    if (Events != 1)
      Failure = 5;
    Events = Events * 10 + 2;
  }
  case false: {
    Failure = 6;
  }
  }
  if (Events != 12)
    Failure = 7;

  Events = 0;
  __objc3_match_stmt (true) {
  case true where accept_guard(make_probe(3)): {
    if (Events != 3)
      Failure = 8;
    Events = Events * 10 + 4;
  }
  case true: {
    Failure = 14;
  }
  case false: {
    Failure = 9;
  }
  }
  if (Events != 34)
    Failure = 10;
}

static void check_guard_eh_cleanup() {
  Events = 0;
  try {
    __objc3_match_stmt (true) {
    case true where throw_guard(make_probe(5)): {
      Failure = 11;
    }
    case true: {
      Failure = 15;
    }
    case false: {
      Failure = 12;
    }
    }
  } catch (int Value) {
    if (Value != 7 || Events != 5)
      Failure = 13;
  }
}

int main() {
  int Source = 29;
  check_scalar_lvalue(Source);
  check_scalar_prvalue();
  check_guard_normal_cleanup();
  check_guard_eh_cleanup();
  return Failure;
}
