typedef __objc3_optional(int) MaybeInt;
typedef __objc3_optional(bool) MaybeBool;
typedef __objc3_result(int, long long) IntOrWide;

// Count pointer-producing calls, then match the local carrier lvalue. This
// exercises scrutinee evaluation without requiring a C++ carrier call ABI.
extern "C" {
static int calls;
static int guard_calls;
static int failure;
static int binding_order;
static bool next_bool(void) {
  ++calls;
  return true;
}

static MaybeInt *next_some_zero(MaybeInt *source) {
  ++calls;
  return source;
}

static MaybeInt *next_none(MaybeInt *source) {
  ++calls;
  return source;
}

static MaybeBool *next_some_false(MaybeBool *source) {
  ++calls;
  return source;
}

static IntOrWide *next_ok_zero(IntOrWide *source) {
  ++calls;
  return source;
}

static IntOrWide *next_err_zero(IntOrWide *source) {
  ++calls;
  return source;
}

static bool reject_payload(int value) {
  ++guard_calls;
  if (value != 0) failure = 17;
  binding_order = binding_order * 10 + 1;
  return false;
}

static void check_optional(void) {
  const int before = calls;
  MaybeInt some_zero = __objc3_some(int, 0);
  __objc3_match_stmt (*next_some_zero(&some_zero)) {
  case __objc3_case_some(let value) where reject_payload(value): {
    failure = 1;
  }
  case __objc3_case_some(let value): {
    if (value != 0) failure = 2;
    binding_order = binding_order * 10 + 2;
    calls += 1; // body runs only after the false guard falls through
  }
  case __objc3_case_none: { failure = 3; }
  }
  if (calls != before + 2 || guard_calls != 1 || binding_order != 12)
    failure = 4;

  const int noneBefore = calls;
  MaybeInt none = __objc3_none(int);
  __objc3_match_stmt (*next_none(&none)) {
  case __objc3_case_some(let value): { (void)value; failure = 5; }
  case __objc3_case_none: { calls += 1; }
  }
  if (calls != noneBefore + 2) failure = 6;

  const int falseBefore = calls;
  MaybeBool some_false = __objc3_some(bool, false);
  __objc3_match_stmt (*next_some_false(&some_false)) {
  case __objc3_case_some(let value): { if (value) failure = 18; }
  case __objc3_case_none: { failure = 19; }
  }
  if (calls != falseBefore + 1) failure = 20;
}

static void check_result(void) {
  const int before = calls;
  IntOrWide ok_zero = __objc3_ok(int, long long, 0);
  __objc3_match_stmt (*next_ok_zero(&ok_zero)) {
  case .Ok(let value): { if (value != 0) failure = 7; }
  case .Err(let error): { (void)error; failure = 8; }
  }
  if (calls != before + 1) failure = 9;

  const int errorBefore = calls;
  IntOrWide err_zero = __objc3_err(int, long long, 0LL);
  __objc3_match_stmt (*next_err_zero(&err_zero)) {
  case .Ok(let value): { (void)value; failure = 10; }
  case .Err(let error): { if (error != 0LL) failure = 11; }
  }
  if (calls != errorBefore + 1) failure = 12;
}

static int check_expression(void) {
  const int before = calls;
  MaybeInt value = __objc3_some(int, 0);
  int result = __objc3_match (value) {
    case __objc3_case_some(let payload) => payload;
    case __objc3_case_none => 13;
  };
  if (calls != before || result != 0) failure = 14;

  const int boolBefore = calls;
  int boolResult = __objc3_match (next_bool()) {
    case true => 0;
    case false => 15;
  };
  if (calls != boolBefore + 1 || boolResult != 0) failure = 16;
  return failure;
}
}

int main(void) {
  check_optional();
  check_result();
  if (check_expression() != 0) return failure;
  return failure;
}
