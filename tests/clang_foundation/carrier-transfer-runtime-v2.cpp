extern "C" void transfer_event(int);

struct Tracked {
  int value;
  explicit Tracked(int value) : value(value) { transfer_event(100 + value); }
  Tracked(const Tracked &other) : value(other.value) {
    transfer_event(200 + value);
  }
  Tracked(Tracked &&other) : value(other.value) {
    other.value = 99;
    transfer_event(300 + value);
  }
  ~Tracked() { transfer_event(400 + value); }
};
struct Failed {
  int value;
  explicit Failed(int value) : value(value) { transfer_event(500 + value); }
  Failed(const Failed &other) : value(other.value) {
    transfer_event(600 + value);
  }
  Failed(Failed &&other) : value(other.value) {
    other.value = 199;
    transfer_event(700 + value);
  }
  ~Failed() { transfer_event(800 + value); }
};
struct ThrowOnMove {
  static bool shouldThrow;
  int value;
  explicit ThrowOnMove(int value) : value(value) {}
  ThrowOnMove(const ThrowOnMove &other) : value(other.value) {}
  ThrowOnMove(ThrowOnMove &&other) : value(other.value) {
    transfer_event(900 + value);
    if (shouldThrow)
      throw 73;
    other.value = 199;
  }
  ~ThrowOnMove() { transfer_event(1000 + value); }
};
bool ThrowOnMove::shouldThrow = false;

using Maybe = __objc3_optional(Tracked);
using Outcome = __objc3_result(Tracked, Failed);
using ThrowMaybe = __objc3_optional(ThrowOnMove);
using VoidOutcome = __objc3_result(void, Tracked);
using VoidErrorOutcome = __objc3_result(Tracked, void);

static int Events[64];
static int EventCount;
static int SourceEvaluations;
extern "C" void transfer_event(int value) {
  if (EventCount < 64)
    Events[EventCount++] = value;
}

static void clear_events() { EventCount = 0; }
static bool is_event(int index, int value) {
  return index < EventCount && Events[index] == value;
}
static Maybe &source_ref(Maybe &source) {
  ++SourceEvaluations;
  return source;
}

int main() {
  {
    Maybe source = __objc3_some(Tracked, Tracked(1));
    clear_events();
    {
      Maybe copied = __objc3_copy(source);
      if (!__objc3_has_value(copied)) return 1;
    }
    if (EventCount != 2 || !is_event(0, 201) || !is_event(1, 401)) return 2;

    clear_events();
    SourceEvaluations = 0;
    {
      Maybe copied = __objc3_copy(source_ref(source));
      if (!__objc3_has_value(copied)) return 15;
    }
    if (SourceEvaluations != 1 || EventCount != 2 ||
        !is_event(0, 201) || !is_event(1, 401))
      return 16;

    clear_events();
    {
      Maybe moved = __objc3_move(source);
      if (!__objc3_has_value(moved) || !__objc3_has_value(source)) return 3;
    }
    if (EventCount != 2 || !is_event(0, 301) || !is_event(1, 401)) return 4;
  }
  if (EventCount != 3 || !is_event(2, 499)) return 5;

  {
    Outcome source = __objc3_err(Tracked, Failed, Failed(2));
    clear_events();
    {
      Outcome copied = __objc3_copy(source);
      if (__objc3_is_ok(copied)) return 6;
    }
    if (EventCount != 2 || !is_event(0, 602) || !is_event(1, 802)) return 7;

    clear_events();
    {
      Outcome moved = __objc3_move(source);
      if (__objc3_is_ok(moved) || __objc3_is_ok(source)) return 8;
    }
    if (EventCount != 2 || !is_event(0, 702) || !is_event(1, 802)) return 9;
  }
  if (EventCount != 3 || !is_event(2, 999)) return 10;

  {
    ThrowMaybe source = __objc3_some(ThrowOnMove, ThrowOnMove(3));
    clear_events();
    ThrowOnMove::shouldThrow = true;
    try {
      ThrowMaybe destination = __objc3_move(source);
      (void)destination;
      return 11;
    } catch (int value) {
      if (value != 73) return 12;
    }
    ThrowOnMove::shouldThrow = false;
    if (EventCount != 1 || !is_event(0, 903) ||
        !__objc3_has_value(source))
      return 13;
  }
  if (EventCount != 2 || !is_event(1, 1003)) return 14;
  {
    Maybe absent = __objc3_none(Tracked);
    clear_events();
    {
      Maybe copied = __objc3_copy(absent);
      if (__objc3_has_value(copied)) return 17;
    }
    if (EventCount != 0) return 18;
    {
      Maybe moved = __objc3_move(absent);
      if (__objc3_has_value(moved) || __objc3_has_value(absent)) return 19;
    }
    if (EventCount != 0) return 20;
  }

  {
    Outcome source = __objc3_ok(Tracked, Failed, Tracked(5));
    clear_events();
    {
      Outcome copied = __objc3_copy(source);
      if (!__objc3_is_ok(copied) || !__objc3_is_ok(source)) return 21;
    }
    if (EventCount != 2 || !is_event(0, 205) || !is_event(1, 405))
      return 22;

    clear_events();
    {
      Outcome moved = __objc3_move(source);
      if (!__objc3_is_ok(moved) || !__objc3_is_ok(source)) return 23;
    }
    if (EventCount != 2 || !is_event(0, 305) || !is_event(1, 405))
      return 24;
  }
  if (EventCount != 3 || !is_event(2, 499)) return 35;

  {
    clear_events();
    VoidOutcome source =
        __objc3_ok(void, Tracked, (void)transfer_event(1100));
    if (EventCount != 1 || !is_event(0, 1100)) return 25;
    clear_events();
    {
      VoidOutcome copied = __objc3_copy(source);
      if (!__objc3_is_ok(copied) || !__objc3_is_ok(source)) return 26;
    }
    if (EventCount != 0) return 27;
    {
      VoidOutcome moved = __objc3_move(source);
      if (!__objc3_is_ok(moved) || !__objc3_is_ok(source)) return 28;
    }
    if (EventCount != 0) return 29;
  }

  {
    clear_events();
    VoidErrorOutcome source =
        __objc3_err(Tracked, void, (void)transfer_event(1101));
    if (EventCount != 1 || !is_event(0, 1101)) return 30;
    clear_events();
    {
      VoidErrorOutcome copied = __objc3_copy(source);
      if (__objc3_is_ok(copied) || __objc3_is_ok(source)) return 31;
    }
    if (EventCount != 0) return 32;
    {
      VoidErrorOutcome moved = __objc3_move(source);
      if (__objc3_is_ok(moved) || __objc3_is_ok(source)) return 33;
    }
    if (EventCount != 0) return 34;
  }
  return 0;
}
