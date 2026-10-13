// Supplemental native runtime witness for direct local carrier initialization.
// The owned transfer runner records this source separately from LLVM fixtures.
using MaybeInt = __objc3_optional(int);
using MaybeBool = __objc3_optional(bool);
using MaybePointer = __objc3_optional(void *);
using IntResult = __objc3_result(int, int);
using BoolResult = __objc3_result(bool, bool);
using PointerResult = __objc3_result(void *, void *);

static int Evaluations;
template <class Carrier> static Carrier &evaluate_once(Carrier &source) {
  ++Evaluations;
  return source;
}

static int plain_optional_initialization() {
  MaybeInt zero = __objc3_some(int, 0);
  MaybeInt absent = __objc3_none(int);
  MaybeBool falsity = __objc3_some(bool, false);
  MaybePointer null = __objc3_some(void *, (void *)0);
  Evaluations = 0;
  MaybeInt copied_zero = evaluate_once(zero);
  if (Evaluations != 1 || !__objc3_has_value(copied_zero)) return 1;
  MaybeInt copied_absent = absent;
  MaybeBool copied_false = falsity;
  MaybePointer copied_null = null;
  if (__objc3_has_value(copied_absent) ||
      !__objc3_has_value(copied_false) || !__objc3_has_value(copied_null))
    return 2;
  int zero_payload = __objc3_match (copied_zero) {
    case __objc3_case_some(let value) => value;
    case __objc3_case_none => 1;
  };
  bool false_payload = __objc3_match (copied_false) {
    case __objc3_case_some(let value) => value;
    case __objc3_case_none => true;
  };
  void *null_payload = __objc3_match (copied_null) {
    case __objc3_case_some(let value) => value;
    case __objc3_case_none => (void *)1;
  };
  return zero_payload != 0 || false_payload || null_payload != (void *)0 ? 3 : 0;
}

static int plain_result_initialization() {
  IntResult zero_ok = __objc3_ok(int, int, 0);
  IntResult zero_error = __objc3_err(int, int, 0);
  BoolResult false_ok = __objc3_ok(bool, bool, false);
  BoolResult false_error = __objc3_err(bool, bool, false);
  PointerResult null_ok = __objc3_ok(void *, void *, (void *)0);
  PointerResult null_error = __objc3_err(void *, void *, (void *)0);
  Evaluations = 0;
  IntResult copied_ok = evaluate_once(zero_ok);
  IntResult copied_error = evaluate_once(zero_error);
  BoolResult copied_false_ok = false_ok;
  BoolResult copied_false_error = false_error;
  PointerResult copied_null_ok = null_ok;
  PointerResult copied_null_error = null_error;
  if (Evaluations != 2 || !__objc3_is_ok(copied_ok) ||
      __objc3_is_ok(copied_error) || !__objc3_is_ok(copied_false_ok) ||
      __objc3_is_ok(copied_false_error) || !__objc3_is_ok(copied_null_ok) ||
      __objc3_is_ok(copied_null_error))
    return 4;
  int ok_payload = __objc3_match (copied_ok) {
    case .Ok(let value) => value;
    case .Err(let error) => 1;
  };
  int error_payload = __objc3_match (copied_error) {
    case .Ok(let value) => 1;
    case .Err(let error) => error;
  };
  bool false_ok_payload = __objc3_match (copied_false_ok) {
    case .Ok(let value) => value;
    case .Err(let error) => true;
  };
  bool false_error_payload = __objc3_match (copied_false_error) {
    case .Ok(let value) => true;
    case .Err(let error) => error;
  };
  void *null_ok_payload = __objc3_match (copied_null_ok) {
    case .Ok(let value) => value;
    case .Err(let error) => (void *)1;
  };
  void *null_error_payload = __objc3_match (copied_null_error) {
    case .Ok(let value) => (void *)1;
    case .Err(let error) => error;
  };
  return ok_payload != 0 || error_payload != 0 || false_ok_payload ||
      false_error_payload || null_ok_payload != (void *)0 ||
      null_error_payload != (void *)0 ? 5 : 0;
}

static int ValueCopies, ValueMoves, ValueDestructions, LastValue;
static int ErrorCopies, ErrorMoves, ErrorDestructions, LastError;

struct Value {
  int Payload;
  explicit Value(int payload) : Payload(payload) {}
  Value(const Value &other) : Payload(other.Payload) {
    ++ValueCopies;
    LastValue = Payload;
  }
  Value(Value &&other) : Payload(other.Payload) {
    ++ValueMoves;
    LastValue = Payload;
    other.Payload = -1;
  }
  ~Value() { ++ValueDestructions; LastValue = Payload; }
};
struct Error {
  int Payload;
  explicit Error(int payload) : Payload(payload) {}
  Error(const Error &other) : Payload(other.Payload) {
    ++ErrorCopies;
    LastError = Payload;
  }
  Error(Error &&other) : Payload(other.Payload) {
    ++ErrorMoves;
    LastError = Payload;
    other.Payload = -2;
  }
  ~Error() { ++ErrorDestructions; LastError = Payload; }
};
using MaybeValue = __objc3_optional(Value);
using ValueResult = __objc3_result(Value, Error);

static void reset_events() {
  Evaluations = 0;
  ValueCopies = ValueMoves = ValueDestructions = LastValue = 0;
  ErrorCopies = ErrorMoves = ErrorDestructions = LastError = 0;
}

static bool no_error_events() {
  return ErrorCopies == 0 && ErrorMoves == 0 && ErrorDestructions == 0;
}
static bool no_value_events() {
  return ValueCopies == 0 && ValueMoves == 0 && ValueDestructions == 0;
}

static int explicit_optional_initialization() {
  {
    MaybeValue source = __objc3_some(Value, Value(17));
    reset_events();
    {
      MaybeValue copied = __objc3_copy(evaluate_once(source));
      if (Evaluations != 1 || !__objc3_has_value(copied) ||
          ValueCopies != 1 || ValueMoves != 0 || LastValue != 17)
        return 6;
    }
    if (ValueDestructions != 1 || LastValue != 17) return 7;
    reset_events();
    {
      MaybeValue moved = __objc3_move(evaluate_once(source));
      if (Evaluations != 1 || !__objc3_has_value(moved) ||
          !__objc3_has_value(source) || ValueCopies != 0 ||
          ValueMoves != 1 || LastValue != 17)
        return 8;
    }
    if (ValueDestructions != 1 || LastValue != 17) return 9;
  }
  if (ValueDestructions != 2 || LastValue != -1 || !no_error_events())
    return 10;
  {
    MaybeValue absent = __objc3_none(Value);
    reset_events();
    {
      MaybeValue copied = __objc3_copy(evaluate_once(absent));
      MaybeValue moved = __objc3_move(evaluate_once(absent));
      if (Evaluations != 2 || __objc3_has_value(copied) ||
          __objc3_has_value(moved) || __objc3_has_value(absent))
        return 11;
    }
  }
  return no_value_events() && no_error_events() ? 0 : 12;
}

static int explicit_result_initialization() {
  {
    ValueResult source = __objc3_ok(Value, Error, Value(23));
    reset_events();
    {
      ValueResult copied = __objc3_copy(evaluate_once(source));
      if (Evaluations != 1 || !__objc3_is_ok(copied) || ValueCopies != 1 ||
          ValueMoves != 0 || LastValue != 23 || !no_error_events())
        return 13;
    }
    if (ValueDestructions != 1 || LastValue != 23) return 14;
    reset_events();
    {
      ValueResult moved = __objc3_move(evaluate_once(source));
      if (Evaluations != 1 || !__objc3_is_ok(moved) || !__objc3_is_ok(source) ||
          ValueCopies != 0 || ValueMoves != 1 || LastValue != 23 ||
          !no_error_events())
        return 15;
    }
    if (ValueDestructions != 1 || LastValue != 23) return 16;
  }
  if (ValueDestructions != 2 || LastValue != -1 || !no_error_events())
    return 17;
  {
    ValueResult source = __objc3_err(Value, Error, Error(31));
    reset_events();
    {
      ValueResult copied = __objc3_copy(evaluate_once(source));
      if (Evaluations != 1 || __objc3_is_ok(copied) || ErrorCopies != 1 ||
          ErrorMoves != 0 || LastError != 31 || !no_value_events())
        return 18;
    }
    if (ErrorDestructions != 1 || LastError != 31) return 19;
    reset_events();
    {
      ValueResult moved = __objc3_move(evaluate_once(source));
      if (Evaluations != 1 || __objc3_is_ok(moved) || __objc3_is_ok(source) ||
          ErrorCopies != 0 || ErrorMoves != 1 || LastError != 31 ||
          !no_value_events())
        return 20;
    }
    if (ErrorDestructions != 1 || LastError != 31) return 21;
  }
  return ErrorDestructions == 2 && LastError == -2 && no_value_events() ? 0 : 22;
}

int main() {
  if (int failure = plain_optional_initialization()) return failure;
  if (int failure = plain_result_initialization()) return failure;
  if (int failure = explicit_optional_initialization()) return failure;
  return explicit_result_initialization();
}
