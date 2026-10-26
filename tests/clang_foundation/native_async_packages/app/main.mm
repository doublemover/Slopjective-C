#include <objc3c/runtime/objc3_task.hpp>
#include <objc/runtime.h>
@import fixture.NativeAsync;
template<class T> concept native_has_number = requires(const T &value) { value.number(); };
static_assert(!native_has_number<NativeDeletedMember>);
static_assert(sizeof(NativeDeletedMember) == sizeof(int));
template<class T> concept native_public_first = requires(T *p) { static_cast<NativeFirstBase *>(p); };
static_assert(native_public_first<NativeDerivedValue>);
static_assert(!native_public_first<NativeAccessValue>);
static_assert(std::is_trivially_copyable_v<NativeDefaultedValue>);
static_assert(std::is_aggregate_v<NativeAggregate>);
static_assert(std::is_abstract_v<NativeVirtualRoot>);
static_assert(std::is_polymorphic_v<NativeVirtualDiamond>);
static_assert(std::is_final_v<NativeVirtualDiamond>);
static_assert(std::has_virtual_destructor_v<NativeVirtualRoot>);

static_assert(std::is_same_v<decltype(native_module_object(1)),
                             objc3::task<NativeModuleResult *>>);
static_assert(std::is_same_v<decltype(native_module_block(1)),
                             objc3::task<native_module_reader>>);

#if !__has_feature(objc_arc)
#include <objc/objc-arc.h>
struct consumer_result_owner {
  id value;
  ~consumer_result_owner() { objc_release(value); }
};
#endif

int checked_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  __objc3_do {
    int success = __objc3_try(__objc3_await native_module_checked(42, false));
    if (success != 42) return 0;
    return __objc3_try(__objc3_await native_module_checked(96, true));
  } __objc3_catch(id<NativeModuleError> error) {
    __objc3_await objc3::task_yield;
    objc3_current_actor_facts facts{};
    facts.struct_size = sizeof(facts);
    facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    if (objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK ||
        facts.actor_identity != objc3_actor_identity(actor)) return 0;
    return [(NativeModuleResult *)error number] == 96 ? 42 : 0;
  }
}

int actor_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  auto expected = objc3_actor_identity(actor);
  int result = __objc3_await native_module_value(41);
  NativeModuleResult *object = __objc3_await native_module_object(21);
#if !__has_feature(objc_arc)
  consumer_result_owner owner{objc_retain(object)};
#endif
  native_module_reader reader = __objc3_await native_module_block(21);
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  if (objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK ||
      facts.actor_identity != expected)
    return 0;
  return result == 42 ? [object number] + reader(0) : 0;
}

int frame_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  auto frame = native_module_frame(41);
  int value = __objc3_await objc3::task<int>::from_native(&frame);
  if (frame.context || value != 42) return 0;
  frame = native_module_object_frame(21);
  NativeModuleResult *object =
      __objc3_await objc3::task<NativeModuleResult *>::from_native(&frame);
#if !__has_feature(objc_arc)
  consumer_result_owner owner{objc_retain(object)};
#endif
  if (frame.context) return 0;
  frame = native_module_block_frame(21);
  native_module_reader reader =
      __objc3_await objc3::task<native_module_reader>::from_native(&frame);
  return !frame.context ? [object number] + reader(0) : 0;
}


int member_application(NativeModuleWorker &worker, NativeModuleWorker &base,
                       objc3_actor_t *actor) __objc3_async {
  static_assert(std::is_same_v<decltype(worker(2)), objc3::task<int>>);
  using Member = int (NativeModuleWorker::*)(int) const __objc3_async;
  using Checked = int (NativeModuleWorker::*)(int, bool) const
      __objc3_async __objc3_throws(id<NativeModuleError>);
  Member member = &NativeModuleWorker::value;
  Checked checked = &NativeModuleWorker::checked;
  __objc3_await objc3::hop_to(actor);
  if (__objc3_await (base.*member)(2) != 42 || __objc3_await worker(3) != 43)
    return 0;
  __objc3_do {
    if (__objc3_try(__objc3_await base.checked(42, false)) != 42) return 0;
    return __objc3_try(__objc3_await (base.*checked)(96, true));
  } __objc3_catch(id<NativeModuleError> error) {
    __objc3_await objc3::task_yield;
    return [(NativeModuleResult *)error number] == 96 ? 42 : 0;
  }
}

int checked_values_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  using Record = NativeModuleValue (*)(int, bool)
      __objc3_async __objc3_throws(id<NativeModuleError>);
  Record function = native_module_record;
  auto value = __objc3_try_force(__objc3_await function(42, false));
  auto number = __objc3_try_force(__objc3_await native_module_complex_value(40, false));
  auto data = __objc3_try_force(__objc3_await native_module_data_value(false));
  auto some = __objc3_try_optional(__objc3_await function(42, false));
  auto none = __objc3_try_optional(__objc3_await function(96, true));
  if (!__objc3_has_value(some) || __objc3_has_value(none) || value.*data != 42 ||
      __real__ number + __imag__ number != 42 || value.padding[4] != 5) return 0;
  __objc3_do {
    (void)__objc3_try(__objc3_await function(96, true));
  } __objc3_catch(id<NativeModuleError> error) {
    __objc3_await objc3::task_yield;
    objc3_current_actor_facts facts{};
    facts.struct_size = sizeof(facts); facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    return objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK &&
        facts.actor_identity == objc3_actor_identity(actor) &&
        [(NativeModuleResult *)error number] == 96 ? 42 : 0;
  }
  return 0;
}

int objc_method_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  NativeModuleMethods *receiver = [NativeModuleMethods new];
#if !__has_feature(objc_arc)
  consumer_result_owner owner{receiver};
#endif
  static_assert(std::is_same_v<decltype([receiver direct:42]), objc3::task<int>>);
  id<NativeModuleMethodProtocol> protocol = receiver;
  __objc3_do {
    int scalar = __objc3_try(__objc3_await [protocol checked:42 fail:false]);
    auto record = __objc3_try(__objc3_await [receiver record:42 fail:false]);
    auto present = __objc3_try_optional(__objc3_await [receiver record:42 fail:false]);
    auto absent = __objc3_try_optional(__objc3_await [receiver record:42 fail:true]);
    NativeModuleResult *object = __objc3_await [receiver object:42];
#if !__has_feature(objc_arc)
    consumer_result_owner result_owner{objc_retain(object)};
#endif
    int direct = __objc3_await [receiver direct:42];
    if (scalar != 42 || record.number != 42 || record.padding[4] != 5 ||
        !__objc3_has_value(present) || __objc3_has_value(absent) ||
        [object number] != 42 || direct != 42) return 0;
    return __objc3_try(__objc3_await [protocol checked:96 fail:true]);
  } __objc3_catch(id<NativeModuleError> error) {
    __objc3_await objc3::task_yield;
    objc3_current_actor_facts facts{};
    facts.struct_size = sizeof(facts); facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    if (objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK ||
        facts.actor_identity != objc3_actor_identity(actor)) return 0;
    return [(NativeModuleResult *)error number] == 96 ? 42 : 0;
  }
}

int block_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  native_module_async_block block = __objc3_await native_module_async_factory(40);
#if !__has_feature(objc_arc)
  consumer_result_owner block_owner{objc_retain((id)block)};
#endif
  native_module_checked_block checked = __objc3_await native_module_checked_factory();
#if !__has_feature(objc_arc)
  consumer_result_owner checked_owner{objc_retain((id)checked)};
#endif
  static_assert(std::is_same_v<decltype(block(2)), objc3::task<int>>);
  if (__objc3_await block(2) != 42) return 0;
  __objc3_do {
    auto value = __objc3_try(__objc3_await checked(42, false));
    auto absent = __objc3_try_optional(__objc3_await checked(96, true));
    if (value.number != 42 || value.padding[4] != 5 || __objc3_has_value(absent)) return 0;
    (void)__objc3_try(__objc3_await checked(96, true));
  } __objc3_catch(id<NativeModuleError> error) {
    __objc3_await objc3::task_yield;
    objc3_current_actor_facts facts{};
    facts.struct_size = sizeof(facts); facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    return objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK &&
        facts.actor_identity == objc3_actor_identity(actor) &&
        [(NativeModuleResult *)error number] == 96 ? 42 : 0;
  }
  return 0;
}

int main() {
  objc3_executor_t *executor = nullptr;
  if (objc3_executor_create(1, &executor) != OBJC3_SERVICE_OK)
    return 1;
  auto pending = native_module_value(41).start(executor);
  uint32_t executed = 0;
  if (objc3_executor_pump(executor, 2, &executed) != OBJC3_SERVICE_OK ||
      executed != 2 || pending.result() != 42)
    return 2;
  pending = {};
  auto scoped = native_module_scoped(41).start(executor);
  if (objc3_executor_pump(executor, UINT32_MAX, &executed) != OBJC3_SERVICE_OK ||
      scoped.result() != 42)
    return 39;
  scoped = {};
  auto object = native_module_object(40).start(executor);
  auto block = native_module_block(40).start(executor);
  if (objc3_executor_pump(executor, 100, &executed) != OBJC3_SERVICE_OK)
    return 6;
  @autoreleasepool {
    NativeModuleResult *value = object.result();
    native_module_reader reader = block.result();
    object = {};
    block = {};
    if ([value number] != 40 || reader(2) != 42)
      return 7;
  }
  objc3_actor_t *actor = nullptr;
  if (objc3_actor_create(executor, &actor) != OBJC3_SERVICE_OK)
    return 3;
  pending = actor_application(actor).start(executor);
  if (objc3_executor_pump(executor, 100, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42)
    return 4;
  pending = {};
  pending = checked_application(actor).start(executor);
  if (objc3_executor_pump(executor, 100, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42)
    return 8;
  pending = {};
  NativeModuleWorker worker{40};
  auto *base = static_cast<NativeModuleWorker *>(&worker);
  {
    int extra = 1;
    auto member = &NativeModuleWorker::frame_borrow;
    auto frame = (worker.*member)(extra);
    static_assert(std::is_same_v<decltype(frame), objc3_async_frame>);
    auto borrowed = __objc3_async_from_frame<int>(frame).start(executor);
    int answer = 1;
    auto referenced = __objc3_async_from_frame<int>(native_module_reference(answer)).start(executor);
    extra = 2; answer = 42;
    if (objc3_executor_pump(executor, UINT32_MAX, &executed) != OBJC3_SERVICE_OK ||
        borrowed.result() != 42 || referenced.result() != 42) return 40;
  }
  pending = member_application(worker, *base, actor).start(executor);
  if (objc3_executor_pump(executor, 100, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42) return 10;
  pending = {};
  pending = checked_values_application(actor).start(executor);
  if (objc3_executor_pump(executor, 200, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42) return 11;
  pending = {};
  pending = objc_method_application(actor).start(executor);
  if (objc3_executor_pump(executor, 300, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42) return 12;
  pending = {};
  pending = block_application(actor).start(executor);
  if (objc3_executor_pump(executor, 300, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42) return 13;
  pending = {};
  pending = frame_application(actor).start(executor);
  if (objc3_executor_pump(executor, 300, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42) return 14;
  pending = {};
  {
    NativeClassStats stats{};
    {
      NativeClassValue value(&stats, 42);
      NativeClassValue copy(value);
      NativeClassValue moved(std::move(copy));
      copy = value;
      moved = std::move(copy);
      if (!static_cast<bool>(moved) || moved.number() != 42) return 41;
      auto member = __objc3_async_from_frame<int>(value.asyncNumber()).start(executor);
      auto task = __objc3_async_from_frame<NativeClassValue>(
          native_module_class(std::move(moved))).start(executor);
      auto unique = __objc3_async_from_frame<int>(
          native_module_move_only(NativeMoveOnly(42))).start(executor);
      if (objc3_executor_pump(executor, UINT32_MAX, &executed) != OBJC3_SERVICE_OK ||
          member.result() != 42 || task.result().number() != 42 ||
          unique.result() != 42) return 42;
      NativeClassValue *heap = new NativeClassValue(value);
      if (heap->number() != 42) return 43;
      delete heap;
    }
    if (stats.live || stats.copied < 2 || stats.moved < 2 || !stats.destroyed) return 44;
  }
  {
    NativeClassStats stats{};
    {
      NativeDerivedValue value(&stats, 20);
      NativeDerivedValue copy(value);
      NativeDerivedValue moved(std::move(copy));
      copy = value; moved = std::move(copy);
      NativeClassValue *base = &value;
      if ((void *)base == (void *)&value || base->number() != 20 || value.sum() != 42) return 45;
      int extra = 0;
      auto frame = __objc3_async_from_frame<int>(value.asyncTotal(extra)).start(executor);
      auto member = &NativeClassValue::asyncNumber;
      auto inherited = __objc3_async_from_frame<int>((value.*member)()).start(executor);
      auto result = __objc3_async_from_frame<NativeDerivedValue>(native_module_derived(std::move(moved))).start(executor);
      NativeAggregate aggregate{{}, {30}, 12};
      auto plain = __objc3_async_from_frame<int>(native_module_aggregate(aggregate)).start(executor);
      if (objc3_executor_pump(executor, UINT32_MAX, &executed) != OBJC3_SERVICE_OK ||
          frame.result() != 42 || inherited.result() != 20 || result.result().sum() != 42 || plain.result() != 42) return 46;
      NativeDerivedValue *heap = new NativeDerivedValue(value);
      if (heap->sum() != 42) return 47;
      delete heap;
      NativeDefaultedValue pod{}; pod.first = 30; pod.value = 12;
      NativeDefaultedValue copied(pod); NativeDefaultedValue movedPod(std::move(copied));
      pod = movedPod; copied = std::move(pod);
      if (copied.first + copied.value != 42 || NativeAccessValue(42).total() != 42) return 48;
    }
    if (stats.live || !stats.copied || !stats.moved || !stats.destroyed) return 49;
  }
  {
    NativeDispatchStats stats{};
    {
      NativeVirtualDiamond value(&stats, 20);
      NativeVirtualRoot *root = &value;
      NativeVirtualRight *right = &value;
      NativeVirtualLeft *left = dynamic_cast<NativeVirtualLeft *>(right);
      if (root->score() != 42 || right->right() != 20 || !left ||
          static_cast<NativeVirtualRoot *>(left) != root ||
          root->self() != root || dynamic_cast<void *>(root) != &value ||
          !native_module_virtual_is(root)) return 50;
      int extra = 0;
      auto frame = __objc3_async_from_frame<int>(root->frame(extra)).start(executor);
      auto member = &NativeVirtualRoot::score;
      if ((value.*member)() != 42) return 51;
      NativeVirtualDiamond copied(value);
      auto result = __objc3_async_from_frame<NativeVirtualDiamond>(
          native_module_virtual(std::move(copied))).start(executor);
      if (objc3_executor_pump(executor, UINT32_MAX, &executed) != OBJC3_SERVICE_OK ||
          frame.result() != 42 || result.result().score() != 42) return 52;
      NativeVirtualRoot *heap = new NativeVirtualDiamond(value);
      if (heap->score() != 42) return 53;
      delete heap;
    }
    if (stats.live || !stats.destroyed) return 54;
  }
  if (objc3_actor_close(actor) != OBJC3_SERVICE_OK ||
      objc3_actor_join(actor) != OBJC3_SERVICE_OK)
    return 5;
  objc3_actor_release(actor);
  objc3_executor_release(executor);
}
