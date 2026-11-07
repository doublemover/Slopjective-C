#ifndef OBJC3_CHECKED_CPP_OPTIONAL_STORAGE_H
#define OBJC3_CHECKED_CPP_OPTIONAL_STORAGE_H
#ifdef __cplusplus
#include <thread>
static const CheckedOptionalOwner &checked_optional_static_value(bool local_thread,bool fault) {
  if(local_thread) {
    static thread_local const CheckedOptionalOwner value=
        __objc3_try_optional(checked_result_produce((CheckedResultParameter(fault),0)));
    return value;
  }
  static const CheckedOptionalOwner value=
      __objc3_try_optional(checked_result_produce((CheckedResultParameter(fault),0)));
  return value;
}
static void checked_optional_storage_suite(PropertyMain *object) {
  @autoreleasepool {
    checked_property_error=object;
    for(int local_thread=0;local_thread!=2;++local_thread)for(int attempt=0;attempt!=2;++attempt) {
      bool caught=false;
      try {(void)checked_optional_static_value(local_thread,true);}
      catch(int error){assert(error==709);caught=true;}
      if(checked_owner_live)fprintf(stderr,"optional static thread=%d attempt=%d owners=%d\n",local_thread,attempt,checked_owner_live);
      assert(caught && !checked_owner_live && !checked_parameter_live && checked_owner_created==checked_owner_destroyed);
    }
    std::thread worker([] {
      const CheckedOptionalOwner &first=checked_optional_static_value(true,false);
      const CheckedOptionalOwner &second=checked_optional_static_value(true,false);
      assert(&first==&second && __objc3_has_value(first));
      assert(checked_owner_live==1 && !checked_parameter_live);
    });
    worker.join();
    assert(!checked_owner_live && !checked_parameter_live && checked_owner_created==checked_owner_destroyed);
    checked_property_error=(id)0;
  }
}
#else
static void checked_optional_storage_suite(PropertyMain *object){(void)object;}
#endif
#endif
