#include <objc3c/runtime/objc3_task.hpp>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
id class_createInstance(Class, size_t) __attribute__((ns_returns_retained));

static int native_result_destruction_count;

@protocol Error @end
@protocol NativeModuleError <Error> @end

__attribute__((objc_root_class))
@interface NativeModuleResult<NativeModuleError> {
  Class isa;
  int value;
}
+ (id)new;
- (void)setNumber:(int)number;
- (int)number;
@end
@implementation NativeModuleResult
+ (id)new {
  return class_createInstance(self, 0);
}
- (void)setNumber:(int)number {
  value = number;
}
- (int)number {
  return value;
}
- (void)dealloc {
  ++native_result_destruction_count;
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {
}
#endif
@end
using native_module_reader = int (^)(int);

#if !__has_feature(objc_arc)
struct module_result_owner {
  id value;
  ~module_result_owner() { objc_release(value); }
};
#endif

NativeModuleResult *native_module_object(int value) __objc3_async {
  NativeModuleResult *result = [NativeModuleResult new];
#if !__has_feature(objc_arc)
  module_result_owner owner{result};
#endif
  [result setNumber:value];
  __objc3_await objc3::task_yield;
  return result;
}

native_module_reader native_module_block(int value) __objc3_async {
  NativeModuleResult *result = [NativeModuleResult new];
#if !__has_feature(objc_arc)
  module_result_owner owner{result};
#endif
  [result setNumber:value];
  __objc3_await objc3::task_yield;
  return ^(int delta) {
    return [result number] + delta;
  };
}

int native_module_value(int value) __objc3_async {
  __objc3_await objc3::task_yield;
  return value + 1;
}

static objc3::task<void> module_scope_child() {
  co_await objc3::task_yield;
  co_return;
}
int native_module_scoped(int value) __objc3_async
    __attribute__((objc_task_group)) {
  auto group = objc3::task_group_handle::current_scope();
  (void)module_scope_child().start_child(group);
  return value + 1;
}

objc3_async_frame native_module_frame(int value) {
  return native_module_value(value).release_native();
}
objc3_async_frame native_module_object_frame(int value) {
  return native_module_object(value).release_native();
}
objc3_async_frame native_module_block_frame(int value) {
  return native_module_block(value).release_native();
}

int native_module_checked(int value, bool fail)
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  NativeModuleResult *error = [NativeModuleResult new];
#if !__has_feature(objc_arc)
  module_result_owner owner{error};
#endif
  [error setNumber:value];
  __objc3_await objc3::task_yield;
  if (fail) __objc3_throw((id<NativeModuleError>)error);
  return value;
}

struct NativeModuleWorker {
  int bias;
  int value(int) const __objc3_async;
  int checked(int, bool) const __objc3_async __objc3_throws(id<NativeModuleError>);
  int operator()(int) const __objc3_async;
  int frame_borrow(int &) const & __objc3_async_frame;
};
int NativeModuleWorker::value(int n) const __objc3_async {
  return bias + __objc3_await native_module_value(n - 1);
}
int NativeModuleWorker::checked(int n, bool fail) const
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  return __objc3_try(__objc3_await native_module_checked(n, fail));
}
int NativeModuleWorker::operator()(int n) const __objc3_async {
  return __objc3_await value(n);
}

static int native_module_borrow_leaf() __objc3_async_frame { return 0; }
int NativeModuleWorker::frame_borrow(int &extra) const & __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return bias + extra;
}
int native_module_reference(int &value) __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return value;
}

struct NativeModuleValue { int number; long padding[5]; };
using native_module_complex = __complex__ double;
using native_module_data = int NativeModuleValue::*;
NativeModuleValue native_module_record(int n, bool fail)
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  int value = __objc3_try(__objc3_await native_module_checked(n, fail));
  return NativeModuleValue{value, {1, 2, 3, 4, 5}};
}
native_module_complex native_module_complex_value(int n, bool fail)
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  int value = __objc3_try(__objc3_await native_module_checked(n, fail));
  native_module_complex result;
  __real__ result = value; __imag__ result = 2;
  return result;
}
native_module_data native_module_data_value(bool fail)
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  (void)__objc3_try(__objc3_await native_module_checked(42, fail));
  return &NativeModuleValue::number;
}

@protocol NativeModuleMethodProtocol
- (int)checked:(int)value fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeModuleError>);
@end
__attribute__((objc_root_class))
@interface NativeModuleMethods <NativeModuleMethodProtocol> { Class isa; }
+ (id)new;
- (int)checked:(int)value fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeModuleError>);
- (NativeModuleValue)record:(int)value fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeModuleError>);
- (NativeModuleResult *)object:(int)value __objc3_async;
- (int)direct:(int)value __objc3_async __attribute__((objc_direct));
@end
@implementation NativeModuleMethods
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc { object_dispose(self); }
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
- (int)checked:(int)value fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  return __objc3_try(__objc3_await native_module_checked(value, fail));
}
- (NativeModuleValue)record:(int)value fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  return __objc3_try(__objc3_await native_module_record(value, fail));
}
- (NativeModuleResult *)object:(int)value __objc3_async {
  return __objc3_await native_module_object(value);
}
- (int)direct:(int)value __objc3_async __attribute__((objc_direct)) {
  __objc3_await objc3::task_yield;
  return value;
}
@end

using native_module_async_block = int (^)(int) __objc3_async;
using native_module_checked_block = NativeModuleValue (^)(int, bool)
    __objc3_async __objc3_throws(id<NativeModuleError>);
native_module_async_block native_module_async_factory(int bias) __objc3_async {
  __objc3_await objc3::task_yield;
  return ^int(int delta) __objc3_async {
    return bias + __objc3_await native_module_value(delta - 1);
  };
}
native_module_checked_block native_module_checked_factory() __objc3_async {
  __objc3_await objc3::task_yield;
  return ^NativeModuleValue(int n, bool fail) __objc3_async __objc3_throws(id<NativeModuleError>) {
    return __objc3_try(__objc3_await native_module_record(n, fail));
  };
}

struct NativeClassStats { int live; int copied; int moved; int destroyed; };
class NativeClassValue {
  NativeClassStats *stats;
  int storage;
  mutable int reads;
  int checkedStorage() const;
public:
  NativeClassValue(NativeClassStats *, int);
  NativeClassValue(const NativeClassValue &);
  NativeClassValue(NativeClassValue &&) noexcept;
  NativeClassValue &operator=(const NativeClassValue &);
  NativeClassValue &operator=(NativeClassValue &&) noexcept;
  ~NativeClassValue() noexcept;
  explicit operator bool() const;
  int number() const;
  int asyncNumber() const __objc3_async_frame;
};
NativeClassValue::NativeClassValue(NativeClassStats *s, int n)
    : stats(s), storage(n), reads(0) { ++stats->live; }
NativeClassValue::NativeClassValue(const NativeClassValue &v)
    : stats(v.stats), storage(v.storage), reads(0) { ++stats->live; ++stats->copied; }
NativeClassValue::NativeClassValue(NativeClassValue &&v) noexcept
    : stats(v.stats), storage(v.storage), reads(0) {
  ++stats->live; ++stats->moved; v.storage = -1;
}
NativeClassValue &NativeClassValue::operator=(const NativeClassValue &v) {
  if (this != &v) {
    --stats->live; stats = v.stats; ++stats->live;
    storage = v.storage; ++stats->copied;
  }
  return *this;
}
NativeClassValue &NativeClassValue::operator=(NativeClassValue &&v) noexcept {
  if (this != &v) {
    --stats->live; stats = v.stats; ++stats->live;
    storage = v.storage; v.storage = -1; ++stats->moved;
  }
  return *this;
}
NativeClassValue::~NativeClassValue() noexcept { --stats->live; ++stats->destroyed; }
int NativeClassValue::checkedStorage() const { ++reads; return storage; }
int NativeClassValue::number() const { return checkedStorage(); }
NativeClassValue::operator bool() const { return storage >= 0; }
int NativeClassValue::asyncNumber() const __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return number();
}
NativeClassValue native_module_class(NativeClassValue value) __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return value;
}
class NativeMoveOnly {
  int storage;
public:
  explicit NativeMoveOnly(int);
  NativeMoveOnly(const NativeMoveOnly &) = delete;
  NativeMoveOnly(NativeMoveOnly &&) noexcept;
  ~NativeMoveOnly() noexcept;
  int number() const;
};
NativeMoveOnly::NativeMoveOnly(int n) : storage(n) {}
NativeMoveOnly::NativeMoveOnly(NativeMoveOnly &&v) noexcept : storage(v.storage) { v.storage = -1; }
NativeMoveOnly::~NativeMoveOnly() noexcept { storage = -1; }
int NativeMoveOnly::number() const { return storage; }
int native_module_move_only(NativeMoveOnly value) __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return value.number();
}

struct NativeDeletedMember {
  int value;
  int number() const = delete;
};

struct NativeEmptyBase {};
struct NativeFirstBase { long first; };
struct NativeAggregate : NativeEmptyBase, NativeFirstBase { int second; };
struct NativeDefaultedValue : NativeFirstBase {
  int value;
  NativeDefaultedValue() = default;
  NativeDefaultedValue(const NativeDefaultedValue &) = default;
  NativeDefaultedValue(NativeDefaultedValue &&) = default;
  NativeDefaultedValue &operator=(const NativeDefaultedValue &) = default;
  NativeDefaultedValue &operator=(NativeDefaultedValue &&) = default;
  ~NativeDefaultedValue() = default;
};
class NativeDerivedValue : public NativeEmptyBase, public NativeFirstBase, public NativeClassValue {
  int tail;
public:
  NativeDerivedValue(NativeClassStats *, int);
  NativeDerivedValue(const NativeDerivedValue &) = default;
  NativeDerivedValue(NativeDerivedValue &&) = default;
  NativeDerivedValue &operator=(const NativeDerivedValue &) = default;
  NativeDerivedValue &operator=(NativeDerivedValue &&) = default;
  ~NativeDerivedValue() = default;
  int sum() const;
  int asyncTotal(const int &) const __objc3_async_frame;
};
NativeDerivedValue::NativeDerivedValue(NativeClassStats *stats, int n)
    : NativeEmptyBase{}, NativeFirstBase{20}, NativeClassValue(stats, n), tail(2) {}
int NativeDerivedValue::sum() const { return (int)first + number() + tail; }
int NativeDerivedValue::asyncTotal(const int &extra) const __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return sum() + extra;
}
class NativeAccessValue : private NativeFirstBase, protected NativeEmptyBase {
public:
  explicit NativeAccessValue(int);
  int total() const;
};
NativeAccessValue::NativeAccessValue(int n) : NativeFirstBase{n}, NativeEmptyBase{} {}
int NativeAccessValue::total() const { return (int)first; }
NativeDerivedValue native_module_derived(NativeDerivedValue value) __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return value;
}
int native_module_aggregate(NativeAggregate value) __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return (int)value.first + value.second;
}


struct NativeDispatchStats { int live; int destroyed; };
struct NativeVirtualRoot {
  NativeDispatchStats *stats;
  int number;
  NativeVirtualRoot(NativeDispatchStats *, int);
  NativeVirtualRoot(const NativeVirtualRoot &);
  virtual ~NativeVirtualRoot();
  virtual int score() const = 0;
  virtual NativeVirtualRoot *self() noexcept;
  virtual int frame(int &) const & __objc3_async_frame;
};
struct NativeVirtualLeft : virtual NativeVirtualRoot {
  NativeVirtualLeft(NativeDispatchStats *, int);
  int score() const override;
};
struct NativeVirtualRight : virtual NativeVirtualRoot {
  NativeVirtualRight(NativeDispatchStats *, int);
  virtual int right() const;
};
struct NativeVirtualDiamond final : NativeVirtualLeft, NativeVirtualRight {
  int tail;
  NativeVirtualDiamond(NativeDispatchStats *, int);
  NativeVirtualDiamond(const NativeVirtualDiamond &) = default;
  NativeVirtualDiamond(NativeVirtualDiamond &&) = default;
  ~NativeVirtualDiamond() override = default;
  static void operator delete(void *) noexcept;
  int score() const override final;
  NativeVirtualDiamond *self() noexcept override;
  int frame(int &) const & __objc3_async_frame override;
};
static int native_virtual_freed=0;
int native_module_virtual_deallocations() { return native_virtual_freed; }
void NativeVirtualDiamond::operator delete(void *storage) noexcept {
  ++native_virtual_freed;
  ::operator delete(storage);
}
NativeVirtualRoot::NativeVirtualRoot(NativeDispatchStats *s, int n)
    : stats(s), number(n) { ++stats->live; }
NativeVirtualRoot::NativeVirtualRoot(const NativeVirtualRoot &v)
    : stats(v.stats), number(v.number) { ++stats->live; }
NativeVirtualRoot::~NativeVirtualRoot() { --stats->live; ++stats->destroyed; }
NativeVirtualRoot *NativeVirtualRoot::self() noexcept { return this; }
int NativeVirtualRoot::frame(int &extra) const & __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return number + extra;
}
NativeVirtualLeft::NativeVirtualLeft(NativeDispatchStats *s, int n)
    : NativeVirtualRoot(s, n) {}
int NativeVirtualLeft::score() const { return number; }
NativeVirtualRight::NativeVirtualRight(NativeDispatchStats *s, int n)
    : NativeVirtualRoot(s, n) {}
int NativeVirtualRight::right() const { return number; }
NativeVirtualDiamond::NativeVirtualDiamond(NativeDispatchStats *s, int n)
    : NativeVirtualRoot(s, n), NativeVirtualLeft(s, n),
      NativeVirtualRight(s, n), tail(22) {}
int NativeVirtualDiamond::score() const { return number + tail; }
NativeVirtualDiamond *NativeVirtualDiamond::self() noexcept { return this; }
int NativeVirtualDiamond::frame(int &extra) const & __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return score() + extra;
}
NativeVirtualDiamond native_module_virtual(NativeVirtualDiamond value)
    __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return value;
}

bool native_module_virtual_is(NativeVirtualRoot *value) {
  return dynamic_cast<NativeVirtualDiamond *>(value) != nullptr;
}

namespace NativePublished {
inline namespace v1 {
  using Scalar = long;
  struct Value { int number; };
  extern int bias;
  int add(Value);
  int frame(Value) __objc3_async_frame;
}
}
namespace NativeOther { using Scalar = int; }
int NativePublished::v1::bias = 2;
int NativePublished::v1::add(Value value) { return value.number + bias; }
int NativePublished::v1::frame(Value value) __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return add(value);
}

namespace NativeAlias = NativePublished;

class NativeFamily {
  struct Hidden { int token; };
protected:
  using Counter=long;
public:
  enum class Kind : unsigned { First=20, Answer=42 };
  class Value : public NativeClassValue {
    int extra;
  public:
    Value(NativeClassStats *, int);
    Value(const Value &) = default;
    Value(Value &&) = default;
    ~Value() = default;
    int sum() const;
    int frame() const __objc3_async_frame;
  };
  using Result=Value;
  using PublicHidden=Hidden;
  Value stored;
  NativeFamily(NativeClassStats *, int);
};
NativeFamily::Value::Value(NativeClassStats *stats, int n)
    : NativeClassValue(stats,n), extra(2) {}
int NativeFamily::Value::sum() const { return number()+extra; }
int NativeFamily::Value::frame() const __objc3_async_frame {
  __objc3_await native_module_borrow_leaf(); return sum();
}
NativeFamily::NativeFamily(NativeClassStats *stats,int n) : stored(stats,n) {}
NativeFamily::Result native_module_nested(NativeFamily::Result value)
    __objc3_async_frame {
  __objc3_await native_module_borrow_leaf(); return value;
}

namespace NativeUsingApi {
  struct NativeUsingItem { int n; };
  int using_calculate(NativeUsingItem);
  int using_calculate(int);
}
namespace NativeUsing {
  using NativeUsingApi::NativeUsingItem;
  using NativeUsingApi::using_calculate;
}
using NativeUsing::using_calculate;
using namespace NativeUsingApi;
int NativeUsingApi::using_calculate(NativeUsingItem value) { return value.n+2; }
int NativeUsingApi::using_calculate(int value) { return value+2; }
struct NativeInherited : NativeClassValue {
  using NativeClassValue::NativeClassValue;
  using NativeClassValue::number;
  using NativeClassValue::asyncNumber;
};

using enum NativeFamily::Kind;

namespace NativeDefaults {
  extern int evaluations;
  int seed();
  int accept(int n=seed());
  int accept(int n);
  struct Aggregate { int value=seed(); int extra{2}; };
  struct Object {
    int value=seed();
    Object()=default;
    Object(int n, int delta=seed());
    int get(int delta=seed()) const;
  };
  int frame(int n=seed()) __objc3_async_frame;
}
int NativeDefaults::evaluations=0;
int NativeDefaults::seed() { ++evaluations; return 40; }
int NativeDefaults::accept(int n) { return n+2; }
NativeDefaults::Object::Object(int n,int delta) : value(n+delta) {}
int NativeDefaults::Object::get(int delta) const { return value+delta; }
int NativeDefaults::frame(int n) __objc3_async_frame {
  __objc3_await native_module_borrow_leaf(); return n+2;
}

namespace NativeInline {
  inline int total=40;
  inline int client_total=40;
  inline int dynamic=NativeDefaults::seed();
  inline int ordered=dynamic+2;
  inline int client_only(int n) { return n+2; }
  inline constexpr int extra=2;
  constexpr int compute(int n) {
    int result=0;
    for (int i=0;i<n;++i) result+=2;
    return result;
  }
  consteval int immediate(int n) { return n+2; }
  struct Value {
    int n;
    inline static int updates=0;
    static constexpr int offset=2;
    constexpr Value(int value):n(value) {}
    constexpr int get() const { return n+offset; }
    int update() { ++updates; return ++total; }
  };
  inline int step(int delta=0) {
    static int calls=0;
    ++calls;
    total+=delta;
    return calls;
  }
  int provider_step(int delta);
  int provider_total();
  int provider_updates();
}
int NativeInline::provider_step(int delta) { return step(delta); }
int NativeInline::provider_total() { return total; }
int NativeInline::provider_updates() { return Value::updates; }

struct NativeComparable {
  int n;
  int values[2];
  bool operator==(const NativeComparable &) const=default;
  bool operator!=(const NativeComparable &) const=default;
};
struct NativeFriendComparable {
  NativeComparable value;
  friend bool operator==(const NativeFriendComparable &, const NativeFriendComparable &)=default;
};
struct NativeDerivedComparable : NativeComparable {
  int extra;
  bool operator==(const NativeDerivedComparable &) const=default;
};
bool native_module_compare(NativeFriendComparable first, NativeFriendComparable second) {
  return first==second;
}

class NativeOutOfLineDefaulted {
  int value=40;
public:
  NativeOutOfLineDefaulted();
  NativeOutOfLineDefaulted(const NativeOutOfLineDefaulted &);
  NativeOutOfLineDefaulted(NativeOutOfLineDefaulted &&);
  NativeOutOfLineDefaulted &operator=(const NativeOutOfLineDefaulted &);
  NativeOutOfLineDefaulted &operator=(NativeOutOfLineDefaulted &&);
  ~NativeOutOfLineDefaulted();
  int get() const { return value+2; }
};
NativeOutOfLineDefaulted::NativeOutOfLineDefaulted()=default;
NativeOutOfLineDefaulted::NativeOutOfLineDefaulted(const NativeOutOfLineDefaulted &)=default;
NativeOutOfLineDefaulted::NativeOutOfLineDefaulted(NativeOutOfLineDefaulted &&)=default;
NativeOutOfLineDefaulted &NativeOutOfLineDefaulted::operator=(const NativeOutOfLineDefaulted &)=default;
NativeOutOfLineDefaulted &NativeOutOfLineDefaulted::operator=(NativeOutOfLineDefaulted &&)=default;
NativeOutOfLineDefaulted::~NativeOutOfLineDefaulted()=default;
NativeOutOfLineDefaulted native_module_defaulted(NativeOutOfLineDefaulted value) __objc3_async_frame {
  __objc3_await native_module_borrow_leaf(); return value;
}

struct NativeImmediate {
  int value=42;
  consteval NativeImmediate()=default;
  consteval bool operator==(const NativeImmediate &) const=default;
  constexpr int get() const { return value; }
};
struct NativeImmediateOut {
  int value=42;
  consteval NativeImmediateOut();
};
consteval NativeImmediateOut::NativeImmediateOut()=default;
inline constexpr NativeImmediate native_immediate{};
inline constexpr NativeImmediateOut native_immediate_out{};

struct NativeConstexprOut {
  int value=42;
  constexpr NativeConstexprOut();
};
constexpr NativeConstexprOut::NativeConstexprOut()=default;
inline constexpr NativeConstexprOut native_constexpr_out{};

namespace __attribute__((visibility("hidden"))) NativeHiddenScope {
  struct Value { int n; };
  int get(Value);
}
namespace NativeHiddenScope { int later(int); }
int NativeHiddenScope::get(Value value) { return value.n+2; }
int NativeHiddenScope::later(int n) { return n+2; }
namespace NativeVersioned {
  inline namespace __attribute__((abi_tag("v2"))) v2 {
    struct Value { int n; };
    int get(Value);
  }
}
int NativeVersioned::v2::get(Value value) { return value.n+2; }

inline int native_closure(int value) {
  int current=value;
  auto bump=[&current](int delta) { current+=delta; return current; };
  auto invoke=[&] { return bump(2); };
  if (invoke()!=value+2) return 0;
  auto add=[base=value](int delta) { return base+delta; };
  auto apply=[copy=add] { return copy(2); };
  return apply();
}
struct NativeClosureDefaults {
  int value=[] { return 40; }();
  int get(int delta=[] { return 2; }()) const { return value+delta; }
};
int native_closure_frame(int value=[base=40] { return base; }()) __objc3_async_frame {
  __objc3_await native_module_borrow_leaf(); return native_closure(value);
}

static int native_thread_constructed=0;
static int native_thread_destroyed=0;
int native_thread_initializer() { return 40; }
struct NativeThreadLifetime {
  int value;
  NativeThreadLifetime();
  ~NativeThreadLifetime();
};
NativeThreadLifetime::NativeThreadLifetime() : value(40) {
  __atomic_add_fetch(&native_thread_constructed,1,__ATOMIC_SEQ_CST);
}
NativeThreadLifetime::~NativeThreadLifetime() {
  __atomic_add_fetch(&native_thread_destroyed,1,__ATOMIC_SEQ_CST);
}
thread_local int native_thread_value=40;
inline thread_local int native_inline_thread_value=native_thread_initializer();
struct NativeThreadState {
  static thread_local int value;
  inline static thread_local int shared=native_thread_initializer();
};
thread_local int NativeThreadState::value=40;
thread_local NativeThreadLifetime native_thread_object;
inline thread_local NativeThreadLifetime native_inline_thread_object;
inline int native_local_thread_value() {
  static thread_local int value=native_thread_initializer();
  return ++value;
}
int native_thread_local_bridge() { return native_local_thread_value(); }
int native_thread_sum() {
  return native_thread_value + native_inline_thread_value + NativeThreadState::value +
      NativeThreadState::shared + native_thread_object.value + native_inline_thread_object.value;
}
int native_thread_lifetimes(int destroyed) {
  return __atomic_load_n(destroyed ? &native_thread_destroyed : &native_thread_constructed,__ATOMIC_SEQ_CST);
}

struct NativeFriendReader;
class NativeFriendValue {
  int value;
public:
  constexpr NativeFriendValue(int n):value(n) {}
  friend struct NativeFriendReader;
  friend constexpr int native_friend_reveal(const NativeFriendValue &v) {return v.value;}
  friend bool operator==(const NativeFriendValue &a,const NativeFriendValue &b) {return a.value==b.value;}
  friend int native_friend_external(NativeFriendValue);
  friend inline int native_friend_delayed(const NativeFriendValue &);
  friend int operator+(NativeFriendValue,int)=delete;
};
struct NativeFriendReader {
  static int read(const NativeFriendValue &v) {return v.value;}
};
int native_friend_external(NativeFriendValue v) {return v.value+2;}
inline int native_friend_delayed(const NativeFriendValue &v) {return v.value+2;}

namespace NativeFriendScope {
  struct Value { int n; friend int probe(Value v) {return v.n+2;} };
}

inline int native_published_block(int value) {
  int original=value;
  auto moved=^[move value]int(int delta) { return value+delta; };
  __block int state=original;
  auto update=^int(int delta) { state+=delta; return state; };
  auto nested=^int(int delta) {
    auto reader=^int(int extra) { return original+extra; };
    return reader(delta);
  };
  if (value!=0 || moved(2)!=original+2 || update(2)!=original+2) return 0;
  return nested(2);
}
inline int native_published_statement(int value) {
  return ({ int first=value; int second=2; first+second; });
}
inline int native_published_local_type(int value) {
  int destroyed=0;
  int answer=0;
  {
    enum class Kind : unsigned {delta=2};
    struct Base { virtual ~Base() {} virtual int get(int delta) const=0; };
    struct Value final : Base {
      int n;
      int *destroyed;
      Value(int n,int *destroyed):n(n),destroyed(destroyed) {}
      Value(const Value &source):n(source.n),destroyed(source.destroyed) {}
      ~Value() {++*destroyed;}
      int get(int delta) const override {return n+delta;}
      struct Nested {int n;};
    };
    Value first(value,&destroyed);
    Value copy(first);
    Base *base=&copy;
    if (dynamic_cast<Value *>(base)!=&copy) return 0;
    Value::Nested nested{base->get((int)Kind::delta)};
    answer=nested.n;
  }
  return destroyed==2 ? answer : 0;
}
inline constexpr int native_local_constant() {
  enum class Kind {delta=2};
  struct Value {int n; constexpr int get() const {return n+(int)Kind::delta;}};
  return Value{40}.get();
}
inline int native_local_lambda(int value) {
  return [value] {
    struct Value {int n; int get() const {return n+2;}};
    return Value{value}.get();
  }();
}
int native_local_type_bridge(int value) {return native_published_local_type(value);}
static int native_statement_live_count=0;
struct NativeStatementLifetime {
  int value;
  NativeStatementLifetime();
  NativeStatementLifetime(const NativeStatementLifetime &);
  NativeStatementLifetime(NativeStatementLifetime &&);
  ~NativeStatementLifetime();
};
NativeStatementLifetime::NativeStatementLifetime():value(42) {++native_statement_live_count;}
NativeStatementLifetime::NativeStatementLifetime(const NativeStatementLifetime &source):value(source.value) {
  ++native_statement_live_count;
}
NativeStatementLifetime::NativeStatementLifetime(NativeStatementLifetime &&source):value(source.value) {
  source.value=0; ++native_statement_live_count;
}
NativeStatementLifetime::~NativeStatementLifetime() {--native_statement_live_count;}
int native_statement_live() {return native_statement_live_count;}
inline NativeStatementLifetime native_statement_value() {
  return ({ NativeStatementLifetime value; value; });
}

struct NativeBlockDefaults {
  int n=^int {return 40;}();
  int get(int delta=^int {return 2;}()) const {return n+delta;}
};

inline int native_published_carrier_capture(int value) {
  Optional(NativeStatementLifetime) source = Some(NativeStatementLifetime, NativeStatementLifetime());
  auto moved=^[move source] {
    return __objc3_match (source) {
      case .Some(let selected) => selected.value;
      case .None => 0;
    };
  };
  int moved_from=__objc3_match (source) {
    case .Some(let selected) => selected.value;
    case .None => -1;
  };
  Optional(int) plain=Some(int,value);
  auto plain_move=^[move plain] {return plain ?? 0;};
  return moved_from==0 ? moved()+plain_move() : -1;
}

inline int native_published_mixed_capture(int value) {
#if __has_feature(objc_arc)
  NativeModuleResult *owner=[NativeModuleResult new];
  [owner setNumber:value];
  __objc3_result(id,NativeStatementLifetime) source=
      __objc3_ok(id,NativeStatementLifetime,owner);
  owner=(id)0;
  auto snapshot=^{return (int)__objc3_is_ok(source);};
  if (!snapshot()) return -1;
  auto moved=^[move source] {
    return __objc3_match (source) {
      case .Ok(let object) => [(NativeModuleResult *)object number];
      case .Err(let object) => object.value;
    };
  };
  return moved();
#else
  return value;
#endif
}

using NativeCaptureFunction = int (*)(int);
inline int native_published_plain_native_capture(int value) {
  Optional(int *) data=Some(int *, &value);
  Optional(NativeCaptureFunction) function=
      Some(NativeCaptureFunction, native_published_statement);
  auto moved=^[move data, move function] {
    guard let pointer=data else {return -1;}
    guard let call=function else {return -2;}
    return call(*pointer);
  };
  guard let null_data=data else {return -3;}
  guard let null_function=function else {return -4;}
  return null_data==(int *)0 && null_function==(NativeCaptureFunction)0
      ? moved() : -5;
}

int native_isolated_entry(int value) noexcept __attribute__((objc_executor(main)));
int native_isolated_entry(int value) noexcept {
  objc3_executor_t *destination=nullptr;
  if (objc3_executor_get_main(&destination)!=OBJC3_SERVICE_OK ||
      !objc3_executor_is_current(destination)) __builtin_trap();
  objc3_executor_release(destination);
  return value+2;
}
inline int native_published_isolation_frame(int value) __objc3_async_frame {
  return __objc3_await native_isolated_entry(value);
}
inline int native_published_isolation_task(int value) __objc3_async {
  return __objc3_await native_isolated_entry(value);
}

inline int native_published_task_await(int value) __objc3_async {
  return __objc3_await native_module_value(value);
}

int native_isolated_task_entry(int value) noexcept __objc3_async
    __attribute__((objc_executor(main)));
int native_isolated_task_entry(int value) noexcept __objc3_async {
  objc3_executor_t *destination=nullptr;
  if (objc3_executor_get_main(&destination)!=OBJC3_SERVICE_OK) __builtin_trap();
  objc3::task_destination owner(destination);
  objc3_executor_release(destination);
  int result=__objc3_await native_module_value(value);
  if (!objc3_executor_is_current(owner.executor())) __builtin_trap();
  return result+1;
}
inline int native_published_task_entry_frame(int value) __objc3_async_frame {
  return __objc3_await native_isolated_task_entry(value);
}
inline int native_published_task_entry(int value) __objc3_async {
  return __objc3_await native_isolated_task_entry(value);
}
inline int native_published_frame_await(int &value) __objc3_async {
  return __objc3_await native_module_reference(value);
}

inline int native_published_runtime(int value,objc3_executor_t *destination)
    __objc3_async {
  __objc3_await objc3::task_checkpoint;
  __objc3_await objc3::task_yield;
  __objc3_await objc3::hop_to(destination);
  if (!objc3_executor_is_current(destination)) return 0;
  auto child=native_module_value(value).start(destination);
  int result=__objc3_await child;
  return result+1;
}
inline int native_published_group(int value) __objc3_async
    __attribute__((objc_task_group)) {
  auto group=objc3::task_group_handle::current_scope();
  auto child=native_module_value(value).start_child(group);
  group.close();
  auto completed=__objc3_await group.next<int>();
  if (!completed) return 0;
  int result=completed.result();
  auto end=__objc3_await group.next<int>();
  if (end) return 0;
  __objc3_await group;
  return result+1;
}
inline int native_published_raw_task(objc3_task_t *task) __objc3_async {
  return __objc3_await objc3::join_task<int>(task);
}
inline int native_published_actor(int value,objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  objc3_current_actor_facts facts{};
  facts.struct_size=sizeof(facts);
  facts.version=1;
  if (objc3_actor_current_facts(&facts)!=OBJC3_SERVICE_OK ||
      facts.actor_identity!=objc3_actor_identity(actor)) return 0;
  __objc3_await objc3::task_yield;
  return objc3_actor_current_facts(&facts)==OBJC3_SERVICE_OK &&
         facts.actor_identity==objc3_actor_identity(actor) ? value : 0;
}

inline constexpr auto native_published_checkpoint=objc3::task_checkpoint;
struct NativePublishedRuntime {
  int bias;
  int run(int value,objc3_executor_t *destination) __objc3_async {
    __objc3_await native_published_checkpoint;
    __objc3_await objc3::hop_to(destination);
    __objc3_await objc3::task_yield;
    return objc3_executor_is_current(destination) ? value+bias : 0;
  }
};

// Source-free ownership witnesses use ordinary synchronous native calls too.
struct NativeForeignCounts {int consumed;int destroyed;};
struct NativeForeignOwner {NativeForeignCounts *counts;int value;};
using NativeForeignRef __attribute__((objc_retainable_family(NativeForeign))) = NativeForeignOwner *;
using NativeForeignConsumer=void (*)(NativeForeignRef __attribute__((cf_consumed)));
NativeForeignRef native_foreign_create(NativeForeignCounts *counts,int value) {
  return new NativeForeignOwner{counts,value};
}
void native_foreign_consume(NativeForeignRef owner __attribute__((cf_consumed)))
    __attribute__((objc_family_release(NativeForeign))) {
  ++owner->counts->consumed;
  ++owner->counts->destroyed;
  delete owner;
}
NativeForeignConsumer native_foreign_callback() {return native_foreign_consume;}
void native_foreign_forward(NativeForeignConsumer callback,
                            NativeForeignRef owner __attribute__((cf_consumed))) {
  callback(owner);
}
borrowed const int *native_foreign_borrow(NativeForeignRef owner)
    __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->value;}
inline bool native_foreign_scrubbed(NativeForeignRef owner) {
  struct Holder {const int *pointer;};Holder holder{native_foreign_borrow(owner)};
  __builtin_memset(&holder,0,sizeof holder);
  return holder.pointer==nullptr;
}
inline void native_foreign_captured_release(NativeForeignRef owner __attribute__((cf_consumed))) {
  auto release=[owner]{native_foreign_consume(owner);};
  struct Holder {decltype(release) callback;};Holder holder{release};
  auto invoke=[holder]{holder.callback();};
  auto counted=[invoke,count=0]()mutable{++count;invoke();};counted();
}
using NativeRetainedFactory=id (__attribute__((ns_returns_retained)) *)(int);
id native_retained_create(int value) __attribute__((ns_returns_retained));
id native_retained_create(int value) __attribute__((ns_returns_retained)) {
  NativeModuleResult *result=[NativeModuleResult new];
  [result setNumber:value];
  return result;
}
NativeRetainedFactory native_retained_callback() {return native_retained_create;}
int native_result_destructions() {return native_result_destruction_count;}

int native_foreign_select(NativeForeignConsumer) {return 42;}
int native_foreign_select(void (*)(NativeForeignRef)) {return 43;}

struct NativeIntegratedOpaque {unsigned char opaque;};
using NativeIntegratedRef __attribute__((NSObject,objc_retainable_family(NativeIntegrated))) = NativeIntegratedOpaque *;
using NativeIntegratedAlias=NativeIntegratedRef;
int native_integrated_value(NativeIntegratedAlias _Nonnull owner) {
#if __has_feature(objc_arc)
  return [(__bridge NativeModuleResult *_Nonnull)owner number];
#else
  return [(NativeModuleResult *_Nonnull)owner number];
#endif
}

inline constexpr int native_unnamed_constant() {
  enum {delta=2};
  struct {int n; constexpr int get() const {return n+delta;}} value{40};
  return value.get();
}
inline int native_unnamed_nested(int n) {
  struct {struct {int n; int get() const {return n+2;}} part;
          int get() const {return part.get();}} value{{n}};
  using Alias=decltype(value);Alias copy=value;
  return copy.get();
}
inline int native_unnamed_copies(int n) {
  NativeClassStats stats={0,0,0,0};int answer=0;
  {
    struct {NativeClassValue member;} first{NativeClassValue(&stats,n)};
    auto second=first;
    auto closure=[second] {return second.member.number()+2;};
    answer=closure();
  }
  return stats.live==0 && stats.copied==2 && stats.destroyed==3 ? answer : 0;
}
inline int native_unnamed_virtual(int n) {
  struct Base {virtual ~Base() {} virtual int get() const=0;};
  struct : Base {int n;int get() const override {return n+2;}} value;
  value.n=n;Base *base=&value;
  if (dynamic_cast<void *>(base)!=static_cast<void *>(&value)) return 0;
  return base->get();
}
int native_unnamed_bridge(int n) {
  return native_unnamed_nested(n)+native_unnamed_copies(n)+native_unnamed_virtual(n);
}

inline int native_generic_local(int n) {
  auto add=[offset=2](auto value){return value+offset;};
  auto pack=[]<class... T>(T... values){return (0+...+values);};
  auto fixed=[]<int N>(int value){return value+N;};
  if (add(2L)!=4 || pack(n,2)!=n+2 || pack()!=0 || fixed.template operator()<2>(n)!=n+2) return 0;
  return add(n);
}
inline constexpr int native_generic_constant() {return [](auto n){return n+2;}(40);}
inline int native_generic_nested(int n) {
  return [](auto outer){return [](auto inner){return inner+2;}(outer);}(n);
}
int native_generic_bridge(int n) {return native_generic_local(n)+native_generic_nested(n);}

inline int native_generic_conversion(int n) {
  int (*callback)(int)=[](auto value){return value+2;};return callback(n);
}
inline int native_generic_mutable(int n) {
  auto update=[n](auto delta) mutable {n+=delta;return n;};
  if (update(1)!=n+1) return 0;return update(1L);
}
inline int native_generic_record(int n) {
  NativeClassStats stats={0,0,0,0};int answer=0;
  {
    NativeClassValue value(&stats,n);
    auto read=[](const auto &object){return object.number()+2;};answer=read(value);
  }
  return stats.live==0 && stats.destroyed==1 ? answer : 0;
}
inline constexpr int native_generic_immediate() {return [](auto n) consteval {return n+2;}(40);}
inline constexpr int native_generic_default() {
  auto value=[]<class T=int>(T n=42){return n;};return value.template operator()<>();
}
inline int native_generic_static(int n) {
#if __cplusplus>202002L
  return [](auto value) static {return value+2;}(n);
#else
  return [](auto value){return value+2;}(n);
#endif
}

struct NativeGenericConfigBase {int bias;};
struct NativeGenericConfig : NativeGenericConfigBase {int values[2];double scale;};
struct NativeGenericNullableConfig {const int *pointer;int value;};
int native_generic_probe(int value) {return value;}
long native_generic_probe(long value) {return value;}
inline constexpr int native_generic_contracts() {
  auto checked=[]<class T>(T value) noexcept(noexcept(value+2))
      requires requires(T item) {native_generic_probe(item);item+2;} {return value+2;};
  auto head=[]<class T> requires (sizeof(T)>0) (T value) {return value+2;};
  auto structural=[]<NativeGenericConfig C>() {
    return C.bias+C.values[0]+C.values[1]+int(C.scale);
  };
  auto floating=[]<double D>() {return int(D);};
  auto nullable=[]<NativeGenericNullableConfig C>() {return C.pointer ? 0 : C.value;};
  static_assert(noexcept(checked(40)));
  return checked(40)+head(40)+
      structural.template operator()<NativeGenericConfig{{2},{20,18},2.0}>()+
      floating.template operator()<42.0>()+
      nullable.template operator()<NativeGenericNullableConfig{nullptr,42}>();
}
int native_generic_contracts_bridge() {return native_generic_contracts();}

inline constexpr int native_generic_float_bits() {
  auto zero=[]<double D>() {return __builtin_signbit(D) ? 2 : 1;};
  auto nan=[]<double D>() {return int(__builtin_bit_cast(unsigned long long,D)&3ull);};
  return zero.template operator()<0.0>()+zero.template operator()<-0.0>()+
      nan.template operator()<__builtin_nan("1")>()+
      nan.template operator()<__builtin_nan("2")>();
}

inline int native_generic_captured_local(int n) {
  struct Local {int pick(int) const {return 42;} int pick(long) const {return 42;}};
  Local receiver;auto run=[receiver](auto value){return receiver.pick(value);};return run(n);
}

union NativePlainUnion {long integer;double floating;};
NativePlainUnion native_union_make(long value) {return NativePlainUnion{.integer=value};}
long native_union_read(NativePlainUnion value) {return value.integer;}
union NativeUnionChoice {int left;int right;};
inline constexpr int native_union_choices() {
  auto use=[]<NativeUnionChoice C>() {return 42;};
  return use.template operator()<NativeUnionChoice{.left=42}>()+
         use.template operator()<NativeUnionChoice{.right=42}>();
}
union NativeUnionValue {
  NativeClassValue value;long other;
  NativeUnionValue(NativeClassStats *stats,int n):value(stats,n){}
  NativeUnionValue(const NativeUnionValue &source):value(source.value){}
  ~NativeUnionValue() {value.~NativeClassValue();}
  int number() const {return value.number();}
};
inline int native_union_lifecycle(int n) {
  NativeClassStats stats={0,0,0,0};int answer=0;
  {
    NativeUnionValue first(&stats,n);NativeUnionValue second=first;
    auto reader=[second]{return second.number()+2;};answer=reader();
  }
  return stats.live==0 && stats.copied==2 && stats.destroyed==3 ? answer : 0;
}
inline int native_union_local(int n) {
  union Local {int n;double d;};Local first{.n=n};
  union {int n;double d;} second{.n=first.n};
  auto reader=[second]{return second.n+2;};return reader();
}
int native_union_bridge(int n) {return native_union_lifecycle(n)+native_union_local(n);}

struct NativeUnionThrowStats {int live;int copied;int destroyed;bool fail;};
struct NativeUnionThrowMember {
  NativeUnionThrowStats *stats;int n;
  NativeUnionThrowMember(NativeUnionThrowStats *s,int value):stats(s),n(value) {++stats->live;}
  NativeUnionThrowMember(const NativeUnionThrowMember &source):stats(source.stats),n(source.n) {
    if (stats->fail) throw 7;
    ++stats->live;++stats->copied;
  }
  ~NativeUnionThrowMember() {--stats->live;++stats->destroyed;}
};
union NativeUnionThrowValue {
  NativeUnionThrowMember member;long other;
  NativeUnionThrowValue(NativeUnionThrowStats *stats,int n):member(stats,n){}
  NativeUnionThrowValue(const NativeUnionThrowValue &source):member(source.member){}
  ~NativeUnionThrowValue() {member.~NativeUnionThrowMember();}
};
inline int native_union_throwing(int n) {
  NativeUnionThrowStats stats={0,0,0,false};NativeClassStats pins={0,0,0,0};int answer=0;
  {
    NativeUnionThrowValue first(&stats,n);stats.fail=true;
    try {
      auto reader=[pin=NativeClassValue(&pins,1),first]{return first.member.n+pin.number();};
      (void)reader;return 0;
    } catch (int code) {
      if (code!=7 || stats.live!=1 || stats.copied!=0 || stats.destroyed!=0 ||
          pins.live!=0 || pins.destroyed!=1) return 0;
    }
    stats.fail=false;NativeUnionThrowValue second=first;answer=second.member.n+2;
  }
  return stats.live==0 && stats.copied==1 && stats.destroyed==2 ? answer : 0;
}

template<class T> concept NativeNumber=requires(T n){n+2;};
template<class T> concept NativeWide=NativeNumber<T> && (sizeof(T)>=sizeof(int));
template<int N> concept NativePositive=N>0;
template<class... T> concept NativeNumbers=(NativeNumber<T> && ...);
inline constexpr int native_concept_value() {return []<NativeWide T>(T n){return n+2;}(40);}
int native_concept_bridge() {return native_concept_value();}

template<class T,int Bias=2> constexpr T native_template_add(T n){return n+Bias;}
template<class... T> constexpr int native_template_sum(T... n){return (0+...+n);}
template<class T> requires NativeNumber<T> constexpr int native_template_rank(T){return 40;}
template<class T> requires NativeWide<T> constexpr int native_template_rank(T){return 42;}
template<int N> int native_template_count(){static int visits=0;return N+visits++;}
int native_template_producer(){return native_template_count<40>();}
int native_template_bridge(){return native_template_add(40)+native_template_sum(20,22)+native_template_rank(1);}
template<class T> T native_template_copy(T n){return n;}
template<class T> T native_template_fail_copy(T n){if(n.number()==40)throw 7;return n;}

struct NativeAliasBox{using element=int;};
template<class T=int> using NativeTemplatePointer=T*;
template<class... T> using NativeTemplateCallback=int(*)(T...);
template<class T> using NativeTemplateElement=typename T::element;
template<NativeNumber T> using NativeNumericPointer=T*;

template<class T=int,int Bias=2> constexpr T native_forwarded(T n=T{40});
template<class U,int Delta> constexpr U native_forwarded(U value){return value+Delta;}
template<class T,int N> constexpr T native_late_default(T n);
template<class U,int M=2> constexpr U native_late_default(U value){return value+M;}
template<class T=int,int N=2> constexpr T native_tail_decl(T n=T{40}){return n+N;}
template<class U,int M> constexpr U native_tail_decl(U value);
template<int N=40> requires(N>0) constexpr int native_constrained_default(int n=N);
template<int M> requires(M>0) constexpr int native_constrained_default(int value){return value+2;}
template<class T=int> constexpr T native_template_argument_default(T n=T{40}){return n+2;}
int native_template_defaults_bridge(){return native_forwarded()+native_late_default(40)+
    native_tail_decl()+native_constrained_default()+native_template_argument_default();}


template<class T=int,int N=40> inline constexpr T native_template_answer=T{N+2};
template<class T=int> inline T native_template_shared=40;
template<class T=int> inline thread_local T native_template_tls=40;
template<class... T> inline constexpr unsigned native_template_arity=sizeof...(T);
template<NativeNumber T> inline constexpr unsigned native_template_width=sizeof(T);
template<class T> T native_template_zero;
int native_variable_template_producer(){return ++native_template_shared<> + ++native_template_tls<>;}


template<NativeNumber T=int,int Bias=2> class NativeTemplateBox {
  T value;
public:
  using element=T;
  inline static int visits=0;
  constexpr NativeTemplateBox(T n=T{40}):value(n){}
  constexpr NativeTemplateBox(const NativeTemplateBox&)=default;
  constexpr T number()const noexcept{return value+Bias;}
  static int next(){return ++visits;}
};
template<class... T> struct NativeTemplateArity {static constexpr unsigned count=sizeof...(T);};
template<class T=int> class NativeTemplateValue {
  T value;
public:
  inline static int live=0;
  explicit NativeTemplateValue(T n=T{40}):value(n){++live;}
  NativeTemplateValue(const NativeTemplateValue &other):value(other.value){++live;}
  NativeTemplateValue(NativeTemplateValue &&other)noexcept:value(other.value){++live;}
  ~NativeTemplateValue(){--live;}
  T number()const{return value;}
};
int native_class_template_producer(){return NativeTemplateBox<>::next();}
inline NativeTemplateValue<int> native_class_template_task(NativeTemplateValue<int> value) __objc3_async {
  __objc3_await objc3::task_yield;
  return value;
}

NativeTemplateValue<int> native_class_template_frame(NativeTemplateValue<int> value) __objc3_async_frame {
  __objc3_await native_module_borrow_leaf();
  return value;
}


template<class T=int,int N=sizeof(T)> extern T native_template_forward_value;
template<class U,int M> U native_template_forward_value=M;
template<class T=int,int Bias=2> class NativeTemplateRoot;
template<class U,int Delta> class NativeTemplateRoot {
  U value;
public:
  inline static int live=0;
  explicit NativeTemplateRoot(U n=U{40}):value(n){++live;}
  virtual ~NativeTemplateRoot(){--live;}
  virtual int number()const{return int(value)+Delta;}
};
template<class T=int> struct NativeTemplateLeft:virtual NativeTemplateRoot<T>{
  NativeTemplateLeft():NativeTemplateRoot<T>(T{40}){}
};
template<class T=int> struct NativeTemplateRight:virtual NativeTemplateRoot<T>{
  NativeTemplateRight():NativeTemplateRoot<T>(T{40}){}
};
template<class T=int> struct NativeTemplateDiamond:NativeTemplateLeft<T>,NativeTemplateRight<T>{
  NativeTemplateDiamond():NativeTemplateRoot<T>(T{40}){}
  int number()const override{return NativeTemplateRoot<T>::number();}
};
NativeTemplateRoot<int> *native_template_hierarchy_new(){return new NativeTemplateDiamond<>;}


template<class T> constexpr auto native_template_deduced_add(T n){return n+2;}
template<class T> constexpr decltype(auto) native_template_deduced_borrow(T &n){return (n);}
template<class T> constexpr auto native_template_make_reader(T n){return [n](int d){return n+d;};}
template<class T> auto native_template_object_reader(T object){
  return [object=static_cast<T&&>(object)](){return object.number();};
}
template<class T> class NativeTemplateHolder {
  T value;
public:
  constexpr NativeTemplateHolder(T n):value(n){}
  constexpr auto number()const{return value+2;}
  constexpr auto reader()const{return [n=value](int d){return n+d;};}
  constexpr decltype(auto) reference(){return (value);}
};
template<class T=int> inline constexpr auto native_template_deduced_answer=T{42};
template<class T> inline auto native_template_deduced_reader=[n=T{40}](int d){return n+d;};
int native_template_deduction_bridge(){return native_template_make_reader(40)(2)+native_template_deduced_reader<int>(2);}

    template<class T> class NativeMemberBox {
      T value;
    public:
      constexpr NativeMemberBox(T n):value(n){}
      template<class U> requires requires(U n){static_cast<T>(n);}
      constexpr explicit(sizeof(U)>0) NativeMemberBox(U n):value(static_cast<T>(n)){}
      template<class U=T,int Delta=2>
      constexpr auto add(U n=U{2})const noexcept(noexcept(value+n)){return value+n+Delta;}
      template<class U=T> constexpr decltype(auto) reference(){return (value);}
      template<class F> auto map(F body)const {
        return [n=value,body](){return body(n);};
      }
      template<class U> constexpr auto reader(U offset)const {
        return [n=value,offset](int extra){return n+offset+extra;};
      }
      template<class... U> constexpr auto sum(U... n)const{return (value+...+n);}
      template<class U> constexpr auto operator()(U n)const{return value+n;}
      template<class U> constexpr explicit operator U()const{return static_cast<U>(value);}
      template<class U=int> static int next(){static int calls=0;return ++calls;}
    };
    constexpr int native_member_probe(){NativeMemberBox<int> box(40L);return box.add(0);}
    int native_member_producer(){return NativeMemberBox<int>::next<>();}


template<class T> struct NativeMemberAliases {
  template<class U=T> using Pointer=U*;
  template<class... U> using Callback=T(*)(U...);
  template<int N> using Array=T[N];
  template<class U=T> using Rebind=NativeTemplateValue<U>;
  template<class U> requires requires{typename U::element;}
  using Element=typename U::element;
};
int native_member_alias_probe(){
  NativeMemberAliases<int>::Rebind<> value(42);
  return value.number();
}

    template<class T> class NativeNestedFamily {
    public:
      using element=T;
      struct Detail {using element=T;T value;explicit Detail(T n):value(n){} T number()const{return value;}};
      template<class U=T,int Bias=2> class Box {
        U value;
      public:
        using element=U;
        template<class V=U> using Pointer=V*;
        inline static int live=0;
        explicit Box(U n=U{40}):value(n){++live;}
        Box(const Box &other):value(other.value){++live;}
        virtual ~Box(){--live;}
        virtual U number()const{return value+Bias;}
        template<class V> auto add(V n)const{return number()+n;}
      };
      template<class U=int> inline static U shared=U{40};
      template<class U=int> inline static thread_local U local=U{40};
      template<int N> inline static constexpr int answer=N+2;
    };
    NativeNestedFamily<int>::Box<int> *native_make_nested(){return new NativeNestedFamily<int>::Box<int>;}
    int native_nested_state(){return ++NativeNestedFamily<int>::shared<> + ++NativeNestedFamily<int>::local<>;}
    template<double D> struct NativeFloatingFamily {
      template<class T> struct Inner {static constexpr int answer(){return 42;}};
    };
    inline constexpr double native_nested_argument=__builtin_bit_cast(double,0x0000000000000000ULL);
    inline constexpr int native_nested_argument_probe(){return NativeFloatingFamily<native_nested_argument>::Inner<int>::answer();}

// C-style ellipses retain native promotions and va_list lifetimes in consumers.
template<class T=int> T native_variadic_sum(unsigned count, ...) {
  __builtin_va_list args, copy;
  __builtin_va_start(args,count);__builtin_va_copy(copy,args);
  int result=0;for(unsigned i=0;i<count;++i)result+=__builtin_va_arg(copy,int);
  __builtin_va_end(copy);__builtin_va_end(args);return T(result);
}
template<class T> struct NativeVariadic {
  T sum(unsigned count, ...) const {
    __builtin_va_list args;__builtin_va_start(args,count);
    int result=0;for(unsigned i=0;i<count;++i)result+=__builtin_va_arg(args,int);
    __builtin_va_end(args);return T(result);
  }
  template<class U=T> U add(unsigned count, ...) const {
    __builtin_va_list args;__builtin_va_start(args,count);
    double result=0;for(unsigned i=0;i<count;++i)result+=__builtin_va_arg(args,double);
    __builtin_va_end(args);return U(result);
  }
};
int native_variadic_provider(){return native_variadic_sum<>(2,40,2);}

class NativeConcreteTemplates {
  template<class U> using Storage=U;
  Storage<int> value;
public:
  NativeConcreteTemplates():value(40){}
  template<class U> explicit(sizeof(U)>1) NativeConcreteTemplates(U n):value(int(n)){}
  virtual ~NativeConcreteTemplates()=default;
  virtual int number()const{return value;}
  template<class U=int> auto add(U n=U{2})const noexcept(noexcept(value+n)){return value+n;}
  template<class U> requires requires(U n){n+1;}
  U operator()(U n)const{return U(value)+n;}
  template<class U> static auto reader(U n){return [n]{return n;};}
  template<class U=int> static int next(){static int n=40;return ++n;}
  template<class U=int> using Pointer=U*;
  template<int N> using Array=int[N];
  template<class U=int> inline static U shared=U{40};
  template<class U=int> inline static thread_local U local=U{40};
  template<int N> inline static constexpr int answer=N+2;
  template<class U=int,int Bias=2> class Box {
    template<class V=U> using Storage=V;
    Storage<U> value;
  public:
    using element=U;
    template<class V=U> using Pointer=V*;
    inline static int live=0;
    explicit Box(U n=U{40}):value(n){++live;}
    Box(const Box &other):value(other.value){++live;}
    virtual ~Box(){--live;}
    virtual U number()const{return value+Bias;}
    template<class V> auto add(V n)const{return number()+n;}
  };
  template<double D> struct Floating {
    template<class U> struct Inner {static constexpr int answer(){return 42;}};
  };

};
NativeConcreteTemplates *native_concrete_make(){return new NativeConcreteTemplates(40L);}
int native_concrete_state(){return NativeConcreteTemplates::next<>()+
    ++NativeConcreteTemplates::shared<> + ++NativeConcreteTemplates::local<>;}

NativeConcreteTemplates::Box<int> *native_concrete_nested_make(){return new NativeConcreteTemplates::Box<int>;}
inline constexpr double native_concrete_nested_argument=__builtin_bit_cast(double,0x0000000000000000ULL);
inline constexpr int native_concrete_nested_probe(){return NativeConcreteTemplates::Floating<native_concrete_nested_argument>::Inner<int>::answer();}

template<class T> struct NativeTTBox{T value;};
template<class T> using NativeTTAlias=NativeTTBox<T>;
template<template<class>class C=NativeTTAlias,class T=int> constexpr T native_tt_read(C<T> value){return value.value;}
template<template<class>class C=NativeTTBox> struct NativeTTWrapper{C<int> value;};
template<template<template<class>class>class W=NativeTTWrapper,template<class>class C=NativeTTAlias>
using NativeTTHigher=W<C>;
template<template<class>class C=NativeTTAlias,class T=int> using NativeTTRebind=C<T>;
template<template<class>class C=NativeTTAlias> inline constexpr unsigned native_tt_width=sizeof(C<int>);
template<template<class>class... C> constexpr unsigned native_tt_arity(){return sizeof...(C);}
template<template<class>class C=NativeTTAlias> requires(sizeof(C<int>)>0)
constexpr int native_tt_constrained(C<int> value){return value.value;}
template<template<class>class C> int native_tt_next(){static int n=40;return ++n;}
template<template<class>class C=NativeTTAlias> inline int native_tt_shared=40;
template<template<class>class C=NativeTTAlias> inline thread_local int native_tt_local=40;
template<class T> struct NativeTTFamily {
  template<class U> using Member=NativeTTBox<U>;
  template<template<class>class C=NativeTTAlias> using Type=C<T>;
  template<template<class>class C=NativeTTAlias> static constexpr int read(C<T> n){return n.value;}
  template<template<class>class C=NativeTTAlias> inline static C<T> saved{40};
};
struct NativeTTConcrete {
  template<template<class>class C=NativeTTAlias> using Type=C<int>;
  template<template<class>class C=NativeTTAlias> static constexpr int read(C<int> n){return n.value;}
  template<template<class>class C=NativeTTAlias> inline static C<int> saved{40};
  template<template<class>class C=NativeTTAlias> struct Nested{C<int> value;};
};
template<template<__SIZE_TYPE__,class...>class C=__type_pack_element>
using NativeTTFirst=C<0,int,long>;
int native_tt_state(){return native_tt_next<NativeTTAlias>()+
    ++native_tt_shared<NativeTTAlias> + ++native_tt_local<NativeTTAlias>;}
int native_tt_member_state(){return native_tt_next<NativeTTFamily<int>::Member>();}

template<template<class>class C=NativeTTAlias> int native_tt_forwarded(int n);
template<template<class>class C> int native_tt_forwarded(int n){return C<int>{n}.value;}
inline int native_tt_lambda(){auto call=[]<template<class>class C>(C<int> n){return n.value;};return call.template operator()<NativeTTAlias>({42});}

template<class T> T native_spec_add(T n){return n+1;}
template<> inline int native_spec_add<int>(int n){return n+2;}
template<> long native_spec_add<long>(long n){return n+3;}
template<auto Value> constexpr int native_spec_value(){return 0;}
template<> constexpr int native_spec_value<0.0>(){return 1;}
template<> constexpr int native_spec_value<-0.0>(){return 2;}
template<class T> int native_spec_next(){static int n=40;return ++n;}
template<> inline int native_spec_next<int>(){static int n=100;return ++n;}
int native_spec_producer(){return native_spec_next<int>();}

template<class T> inline int native_spec_shared=1;
template<> inline int native_spec_shared<int> =40;
template<class T> inline thread_local int native_spec_tls=1;
template<> inline thread_local int native_spec_tls<int> =40;
template<auto Value> inline constexpr int native_spec_number=0;
template<> inline constexpr int native_spec_number<0.0> =1;
template<> inline constexpr int native_spec_number<-0.0> =2;
int native_variable_spec_producer(){return ++native_spec_shared<int> + ++native_spec_tls<int>;}

template<class T> struct NativeExplicitFamily{T value;};
template<> struct NativeExplicitFamily<int>{
  inline static int live=0;
private:
  int value;
public:
  explicit NativeExplicitFamily(int n):value(n){++live;}
  NativeExplicitFamily(const NativeExplicitFamily &other):value(other.value){++live;}
  virtual ~NativeExplicitFamily(){--live;}
  virtual int get()const{return value;}
  template<class U>int add(U n)const{return [](auto value){return value;}(get()+n);}
  template<template<class>class C>int size()const{return sizeof(C<int>);}
};
template<> struct NativeExplicitFamily<short>{
  inline static int live=0;
private:
  short value;
public:
  explicit NativeExplicitFamily(short n):value(n){++live;}
  NativeExplicitFamily(const NativeExplicitFamily &other):value(other.value){++live;}
  virtual ~NativeExplicitFamily(){--live;}
  virtual int get()const{return value;}
};
NativeExplicitFamily<short> *native_explicit_plain_factory(){return new NativeExplicitFamily<short>(42);}
int native_explicit_plain_live(){return NativeExplicitFamily<short>::live;}
NativeExplicitFamily<int> *native_explicit_class_factory(){return new NativeExplicitFamily<int>(40);}
int native_explicit_class_live(){return NativeExplicitFamily<int>::live;}
template<auto Value>struct NativeExplicitNumber{static constexpr int value=0;};
template<>struct NativeExplicitNumber<0.0>{static constexpr int value=1;};
template<>struct NativeExplicitNumber<-0.0>{static constexpr int value=2;};

template<class T>concept NativePartialValue=requires(T n){n+1;};
template<class T>struct NativePartialFamily{T value;};
template<NativePartialValue T>struct NativePartialFamily<T*>{
  inline static int live=0;
private:
  T* value;
public:
  explicit NativePartialFamily(T* n):value(n){++live;}
  NativePartialFamily(const NativePartialFamily &other):value(other.value){++live;}
  virtual ~NativePartialFamily(){--live;}
  virtual int get()const{return *value;}
  template<class U>int add(U n)const{return get()+n;}
};
NativePartialFamily<int*> *native_partial_factory(int* n){return new NativePartialFamily<int*>(n);}
int native_partial_live(){return NativePartialFamily<int*>::live;}
template<int N,bool Positive=true>struct NativePartialCount{static constexpr int value=0;};
template<int N>requires(N>0)struct NativePartialCount<N,true>{static constexpr int value=N;};
template<class...Ts>struct NativePartialPack{static constexpr int value=0;};
template<class T,class...Ts>struct NativePartialPack<T,Ts...>{static constexpr int value=1+sizeof...(Ts);};
template<class T>struct NativePartialTemplate{static constexpr int size=0;};
template<template<class>class C,class T>struct NativePartialTemplate<C<T>>{static constexpr int size=sizeof(C<T>);};

template<class T>inline int native_partial_shared=0;
template<NativePartialValue T>inline int native_partial_shared<T*> =40;
template<class T>inline thread_local int native_partial_tls=0;
template<NativePartialValue T>inline thread_local int native_partial_tls<T*> =40;
template<int N,bool Positive=true>inline constexpr int native_partial_count=0;
template<int N>requires(N>0)inline constexpr int native_partial_count<N,true> =N;
template<class...Ts>inline constexpr unsigned native_partial_width=0;
template<class T,class...Ts>inline constexpr unsigned native_partial_width<T,Ts...> =1+sizeof...(Ts);
template<class T>inline auto native_partial_auto=0;
template<class T>inline auto native_partial_auto<T*> =T{40};
template<class T>inline constexpr int native_partial_size=0;
template<template<class>class C,class T>inline constexpr int native_partial_size<C<T>> =sizeof(C<T>);
template<class T>int native_partial_ordinary=0;
template<class T>int native_partial_ordinary<T*> =40;
int native_partial_variable_producer(){return ++native_partial_shared<int*> + ++native_partial_tls<int*> + ++native_partial_ordinary<int*>;}

struct NativeExternalVariableValue{
  inline static int alive=0;
  int value;
  explicit NativeExternalVariableValue(int n):value(n){++alive;}
  ~NativeExternalVariableValue(){--alive;}
};
template<class T>NativeExternalVariableValue native_external_object{40};
template<>NativeExternalVariableValue native_external_object<int>{42};
template<class T>int native_external_special=0;
static int native_external_secret_init(){return 40;}
template<>int native_external_special<int> =native_external_secret_init();
template<class T>thread_local int native_external_tls=0;
template<>thread_local int native_external_tls<int> =native_external_secret_init();
template<class T>inline int native_zero_special=1;
template<>inline int native_zero_special<int>;
int native_external_producer(){return ++native_external_special<int> + ++native_external_tls<int>;}
int native_external_object_read(){return native_external_object<int>.value;}
int native_external_zero(){return ++native_zero_special<int>;}
