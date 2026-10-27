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
