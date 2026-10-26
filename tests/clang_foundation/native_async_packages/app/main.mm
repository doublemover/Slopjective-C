#include <objc3c/runtime/objc3_task.hpp>
#include <objc/runtime.h>
@import fixture.NativeAsync;
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
  if (objc3_actor_close(actor) != OBJC3_SERVICE_OK ||
      objc3_actor_join(actor) != OBJC3_SERVICE_OK)
    return 5;
  objc3_actor_release(actor);
  objc3_executor_release(executor);
}
