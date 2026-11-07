#ifndef OBJC3_CHECKED_STATIC_REFERENCE_REGIONS_H
#define OBJC3_CHECKED_STATIC_REFERENCE_REGIONS_H
#ifdef __cplusplus
static const CheckedOptionalOwner &checked_static_optional_reference(bool thread,bool fault,bool empty) {
  if(thread) {
    static thread_local const CheckedOptionalOwner &value=
        (CheckedResultParameter(fault),__objc3_try_optional(checked_result_produce(empty?1:0)));
    return value;
  }
  static const CheckedOptionalOwner &value=
      (CheckedResultParameter(fault),__objc3_try_optional(checked_result_produce(empty?1:0)));
  return value;
}
static const CheckedPropertyOwner &checked_static_native_reference(bool thread,bool fault) {
  if(thread) {
    static thread_local const CheckedPropertyOwner &value=
        (CheckedResultParameter(fault),__objc3_try_force(checked_result_produce(0)));
    return value;
  }
  static const CheckedPropertyOwner &value=
      (CheckedResultParameter(fault),__objc3_try_force(checked_result_produce(0)));
  return value;
}
static int checked_static_propagating_guard(bool fault) __objc3_throws(id<Error>) {
  static const CheckedOptionalOwner value=__objc3_try_optional(
      checked_result_produce((__objc3_try(checked_region_integer(fault)),1)));
  return __objc3_has_value(value)?1:0;
}
static int checked_static_cancel_guard() __objc3_async_frame {
  static const CheckedOptionalOwner value=__objc3_try_optional(
      __objc3_await checked_optional_producer(1));
  return __objc3_has_value(value)?1:0;
}
static void checked_static_reference_suite(PropertyMain *object) {
  @autoreleasepool {
    checked_property_error=object;
    for(int thread=0;thread!=2;++thread)for(int native=0;native!=2;++native)for(int attempt=0;attempt!=2;++attempt) {
      bool caught=false;
      try {
        if(native)(void)checked_static_native_reference(thread,true);
        else (void)checked_static_optional_reference(thread,true,false);
      }catch(int error){assert(error==709);caught=true;}
      assert(caught && !checked_owner_live && !checked_parameter_live && checked_owner_created==checked_owner_destroyed);
    }
    const CheckedOptionalOwner &empty=checked_static_optional_reference(false,false,true);
    assert(!__objc3_has_value(empty));
    const CheckedOptionalOwner &same=checked_static_optional_reference(false,false,false);
    assert(&same==&empty && !__objc3_has_value(same) && !checked_owner_live);
    std::thread worker([] {
      const auto &first=checked_static_optional_reference(true,false,false);
      const auto &same=checked_static_optional_reference(true,false,false);
      const auto &native=checked_static_native_reference(true,false);
      assert(&first==&same && __objc3_has_value(first) && native.value==40);
      assert(checked_owner_live==2);
    });
    worker.join();assert(!checked_owner_live && checked_owner_created==checked_owner_destroyed);
    bool caught=false;
    __objc3_do {(void)__objc3_try(checked_static_propagating_guard(true));}
    __objc3_catch(id<Error> error){assert(error==checked_property_error);caught=true;}
    assert(caught);
    __objc3_do {assert(__objc3_try(checked_static_propagating_guard(false))==0);}
    __objc3_catch(id<Error>){assert(!"unexpected static retry failure");}
    assert(!checked_owner_live && checked_owner_created==checked_owner_destroyed);
    for(int attempt=0;attempt!=2;++attempt) {
      objc3_async_frame frame=checked_static_cancel_guard();
      objc3_task_t *task=property_start(&frame);
      uint32_t executed=0;
      assert(objc3_executor_pump(property_caller,1,&executed)==OBJC3_SERVICE_OK && executed==1);
      objc3_task_request_cancel(task);
      objc3_completion done=property_finish(task,0);
      assert(done.kind==OBJC3_COMPLETION_CANCELLED);
      objc3_task_release(task);
      assert(!checked_owner_live && checked_owner_created==checked_owner_destroyed);
    }
    objc3_async_frame frame=checked_static_cancel_guard();
    objc3_task_t *task=property_start(&frame);
    objc3_completion done=property_finish(task,0);
    assert(done.kind==OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload==0);
    objc3_task_release(task);
    checked_property_error=(id)0;
  }
}
#else
static void checked_static_reference_suite(PropertyMain *object){(void)object;}
#endif
#endif
