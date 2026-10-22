struct Counters {
  int constructions, copies, moves, destructions, live;
  bool throwConstruction, throwCopy, throwMove;
};
static Counters counts[2];

template <int Kind> struct Tracked {
  int value;
  explicit Tracked(int value) : value(value) {
    if (counts[Kind].throwConstruction)
      throw 10 + Kind;
    ++counts[Kind].constructions;
    ++counts[Kind].live;
  }
  Tracked(const Tracked &other) : value(other.value) {
    if (counts[Kind].throwCopy)
      throw 20 + Kind;
    ++counts[Kind].copies;
    ++counts[Kind].live;
  }
  Tracked(Tracked &&other) : value(other.value) {
    if (counts[Kind].throwMove)
      throw 30 + Kind;
    other.value = -1;
    ++counts[Kind].moves;
    ++counts[Kind].live;
  }
  ~Tracked() {
    ++counts[Kind].destructions;
    --counts[Kind].live;
  }
};
using Item = Tracked<0>;
using Problem = Tracked<1>;
using Inner = Optional(Item);
using Outer = Optional(Inner);
using Failure = Optional(Problem);
using Tree = Result(Outer, Failure);

static int sourceEvaluations;
static Outer &source_once(Outer &source) {
  ++sourceEvaluations;
  return source;
}

static void reset_counts() {
  counts[0] = {};
  counts[1] = {};
}

static int empty_and_inactive() {
  reset_counts();
  {
    Outer absent = None(Inner);
    Outer presentEmpty = Some(Inner, None(Item));
    Tree failureEmpty = Err(Outer, Failure, None(Problem));
    Outer copiedAbsent = copyValue(absent);
    Outer movedEmpty = moveValue(presentEmpty);
    Tree copiedFailure = copyValue(failureEmpty);
    if (hasValue(absent) || hasValue(copiedAbsent) ||
        !hasValue(presentEmpty) || !hasValue(movedEmpty) ||
        isOk(failureEmpty) || isOk(copiedFailure))
      return 1;
  }
  if (counts[0].constructions || counts[0].copies || counts[0].moves ||
      counts[0].destructions || counts[1].destructions)
    return 2;
  return 0;
}

static int recursive_copy_and_move() {
  reset_counts();
  sourceEvaluations = 0;
  {
    Outer source = Some(Inner, Some(Item, Item(3)));
    {
      Outer copied = copyValue(source_once(source));
      Outer moved = moveValue(source);
      const Outer &constant = copied;
      Outer constMoved = moveValue(constant);
      if (sourceEvaluations != 1 || counts[0].constructions != 1 ||
          counts[0].copies != 2 || counts[0].moves != 1 ||
          counts[0].live != 4 || counts[0].destructions != 0 ||
          !hasValue(source) || !hasValue(moved) ||
          !hasValue(constMoved))
        return 3;
    }
    if (counts[0].live != 1 || counts[0].destructions != 3)
      return 4;
  }
  if (counts[0].live || counts[0].destructions != 4)
    return 5;
  return 0;
}

static int recursive_result_arms() {
  reset_counts();
  {
    Tree success = Ok(
        Outer, Failure, Some(Inner, Some(Item, Item(4))));
    Tree failure = Err(
        Outer, Failure, Some(Problem, Problem(5)));
    Tree copiedSuccess = copyValue(success);
    Tree copiedFailure = copyValue(failure);
    Tree movedSuccess = moveValue(success);
    Tree movedFailure = moveValue(failure);
    if (!isOk(success) || !isOk(copiedSuccess) ||
        !isOk(movedSuccess) || isOk(failure) ||
        isOk(copiedFailure) || isOk(movedFailure) ||
        counts[0].constructions != 1 || counts[1].constructions != 1 ||
        counts[0].copies != 1 || counts[1].copies != 1 ||
        counts[0].moves != 1 || counts[1].moves != 1 ||
        counts[0].live != 3 || counts[1].live != 3)
      return 6;
  }
  if (counts[0].live || counts[1].live ||
      counts[0].destructions != 3 || counts[1].destructions != 3)
    return 7;
  return 0;
}

static int recursive_failed_transfer() {
  reset_counts();
  {
    Outer source = Some(Inner, Some(Item, Item(6)));
    counts[0].throwCopy = true;
    try {
      Outer destination = copyValue(source);
      return 8;
    } catch (int code) {
      if (code != 20)
        return 9;
    }
    counts[0].throwCopy = false;
    counts[0].throwMove = true;
    try {
      Outer destination = moveValue(source);
      return 10;
    } catch (int code) {
      if (code != 30)
        return 11;
    }
    counts[0].throwMove = false;
    if (!hasValue(source) || counts[0].live != 1 ||
        counts[0].destructions || counts[0].copies || counts[0].moves)
      return 12;
  }
  if (counts[0].live || counts[0].destructions != 1)
    return 13;
  return 0;
}

struct ThrowingTemporary {
  ~ThrowingTemporary() noexcept(false) { throw 47; }
};

static int nested_initialization_rollback() {
  reset_counts();
  counts[0].throwConstruction = true;
  try {
    Outer value = Some(Inner, Some(Item, Item(7)));
    return 14;
  } catch (int code) {
    if (code != 10 || counts[0].destructions || counts[0].live)
      return 15;
  }
  counts[0].throwConstruction = false;
  try {
    Outer value = Some(
        Inner, (ThrowingTemporary{}, Some(Item, Item(8))));
    return 16;
  } catch (int code) {
    if (code != 47 || counts[0].constructions != 1 ||
        counts[0].destructions != 1 || counts[0].live)
      return 17;
  }
  try {
    Tree value = Err(
        Outer, Failure,
        (ThrowingTemporary{}, Some(Problem, Problem(9))));
    return 18;
  } catch (int code) {
    if (code != 47 || counts[1].constructions != 1 ||
        counts[1].destructions != 1 || counts[1].live)
      return 19;
  }
  return 0;
}

static int recursive_native_unwind() {
  reset_counts();
  try {
    Outer source = Some(Inner, Some(Item, Item(10)));
    Tree failure = Err(
        Outer, Failure, Some(Problem, Problem(11)));
    Outer copied = copyValue(source);
    Tree moved = moveValue(failure);
    throw 59;
  } catch (int code) {
    if (code != 59 || counts[0].live || counts[1].live ||
        counts[0].destructions != 2 || counts[1].destructions != 2)
      return 20;
  }
  return 0;
}

int main() {
  if (int code = empty_and_inactive()) return code;
  if (int code = recursive_copy_and_move()) return code;
  if (int code = recursive_result_arms()) return code;
  if (int code = recursive_failed_transfer()) return code;
  if (int code = nested_initialization_rollback()) return code;
  if (int code = recursive_native_unwind()) return code;
  return 0;
}
