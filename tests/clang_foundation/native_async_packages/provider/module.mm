#include <objc3c/runtime/objc3_task.hpp>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
id class_createInstance(Class, size_t) __attribute__((ns_returns_retained));

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
  int score() const override final;
  NativeVirtualDiamond *self() noexcept override;
  int frame(int &) const & __objc3_async_frame override;
};
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
