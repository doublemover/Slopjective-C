#ifndef OBJC3_CHECKED_CPP_REGIONS_RUNTIME_H
#define OBJC3_CHECKED_CPP_REGIONS_RUNTIME_H
#ifdef __cplusplus
#include <new>
static int checked_region_integer(bool fail) __objc3_throws(id<Error>) {
  if(fail)__objc3_throw(checked_property_error);
  return 19;
}
struct CheckedRegionAggregate {CheckedPropertyOwner first,second;int tail;};
struct CheckedRegionReferences {const CheckedPropertyOwner &first;int tail;};
struct CheckedRegionTwoReferences {const CheckedPropertyOwner &first,&second;int tail;};
struct CheckedRegionConstructed {
  CheckedPropertyOwner value;
  explicit CheckedRegionConstructed(CheckedPropertyOwner input):value(static_cast<CheckedPropertyOwner &&>(input)) {}
};
struct CheckedRegionBase {CheckedPropertyOwner first;};
struct CheckedRegionDerived : CheckedRegionBase {CheckedPropertyOwner second;int tail;};
static int checked_region_allocations;
struct CheckedRegionHeap {
  CheckedPropertyOwner first;int tail;
  static void *operator new(__SIZE_TYPE__ size) {void *p=::operator new(size);++checked_region_allocations;return p;}
  static void operator delete(void *p) noexcept {--checked_region_allocations;::operator delete(p);}
  static void *operator new[](__SIZE_TYPE__ size) {void *p=::operator new[](size);++checked_region_allocations;return p;}
  static void operator delete[](void *p) noexcept {--checked_region_allocations;::operator delete[](p);}
};
static void checked_region_static_failure(bool local_thread) {
  if(local_thread) {
    static thread_local CheckedPropertyOwner value=__objc3_try_force(checked_parameter_result(CheckedResultParameter(true),false));
    (void)value;
  } else {
    static CheckedPropertyOwner value=__objc3_try_force(checked_parameter_result(CheckedResultParameter(true),false));
    (void)value;
  }
}
static int checked_region_async(int route,int fault) __objc3_async_frame {
  __objc3_do {
    if(route==0) {
      CheckedRegionAggregate value={__objc3_await checked_async_completed(40),
          __objc3_await checked_async_completed((CheckedResultParameter(fault==2),40)),
          __objc3_try(checked_region_integer(fault==1))};
      return value.first.value+value.second.value+value.tail;
    }
    CheckedRegionTwoReferences value={__objc3_await checked_async_completed(40),
        __objc3_await checked_async_completed((CheckedResultParameter(fault==2),40)),
        __objc3_try(checked_region_integer(fault==1))};
    return value.first.value+value.second.value+value.tail;
  } __objc3_catch(id<Error> error) {assert(error==checked_property_error);return -1;}
}
static void checked_region_suite(PropertyMain *object) {
  @autoreleasepool {
    checked_property_error=object;
    for(int route=0;route!=9;++route)for(int selected=0;selected!=2;++selected)
      for(int fault=0;fault!=3;++fault) {
        checked_owner_stage=1000+route*100+selected*10+fault;
        bool checked=false,foreign=false;
        try {
          __objc3_do {
            if(route==0) {
              CheckedPropertyOwner value=selected
                  ?__objc3_try(checked_parameter_result(CheckedResultParameter(fault==2),fault==1))
                  :__objc3_try(checked_parameter_result(CheckedResultParameter(fault==2),fault==1));
              assert(value.value==40);
            } else if(route==1) {
              CheckedPropertyOwner value=(__objc3_try(checked_region_integer(false)),
                  __objc3_try(checked_parameter_result(CheckedResultParameter(fault==2),fault==1)));
              assert(value.value==40);
            } else if(route==2) {
              CheckedRegionAggregate value={__objc3_try(checked_result_produce(0)),
                  __objc3_try(checked_parameter_result(CheckedResultParameter(fault==2),false)),
                  __objc3_try(checked_region_integer(fault==1))};
              assert(value.first.value==40 && value.second.value==40 && value.tail==19);
            } else if(route==3) {
              CheckedRegionReferences value={__objc3_try(checked_parameter_result(CheckedResultParameter(fault==2),false)),
                  __objc3_try(checked_region_integer(fault==1))};
              assert(value.first.value==40 && value.tail==19);
            } else if(route==4) {
              const CheckedPropertyOwner &value=selected
                  ?__objc3_try(checked_parameter_result(CheckedResultParameter(fault==2),fault==1))
                  :__objc3_try(checked_parameter_result(CheckedResultParameter(fault==2),fault==1));
              assert(value.value==40);
            } else if(route==5) {
              CheckedRegionAggregate value[2]={
                  {__objc3_try(checked_result_produce(0)),__objc3_try(checked_result_produce(0)),19},
                  {__objc3_try(checked_result_produce(0)),
                   __objc3_try(checked_parameter_result(CheckedResultParameter(fault==2),false)),
                   __objc3_try(checked_region_integer(fault==1))}};
              assert(value[0].tail==19 && value[1].tail==19);
            } else if(route==6) {
              CheckedRegionConstructed value(__objc3_try(checked_parameter_result(CheckedResultParameter(fault==2),fault==1)));
              assert(value.value.value==40);
            } else if(route==7) {
              CheckedRegionDerived value={{__objc3_try(checked_result_produce(0))},
                  __objc3_try(checked_parameter_result(CheckedResultParameter(fault==2),false)),
                  __objc3_try(checked_region_integer(fault==1))};
              assert(value.first.value==40 && value.second.value==40 && value.tail==19);
            } else {
              CheckedRegionTwoReferences value={__objc3_try(checked_result_produce(0)),
                  __objc3_try(checked_parameter_result(CheckedResultParameter(fault==2),false)),
                  __objc3_try(checked_region_integer(fault==1))};
              assert(value.first.value==40 && value.second.value==40 && value.tail==19);
            }
          } __objc3_catch(id<Error> error) {assert(error==checked_property_error);checked=true;}
        } catch(int error) {assert(error==709);foreign=true;}
        assert(checked==(fault==1) && foreign==(fault==2));
        if(checked_owner_live || checked_parameter_live)
          fprintf(stderr,"checked region route=%d selected=%d fault=%d owners=%d params=%d\n",route,selected,fault,checked_owner_live,checked_parameter_live);
        assert(checked_owner_live==0 && checked_parameter_live==0 && checked_owner_created==checked_owner_destroyed);
      }
    for(int array=0;array!=2;++array)for(int selected=0;selected!=2;++selected)
      for(int fault=0;fault!=2;++fault) {
        bool caught=false;
        __objc3_do {
          CheckedRegionHeap *value;
          if(array) value=new CheckedRegionHeap[2]{
              {__objc3_try(checked_result_produce(0)),19},
              {__objc3_try(checked_result_produce(0)),__objc3_try(checked_region_integer(fault))}};
          else value=selected
              ?new CheckedRegionHeap{__objc3_try(checked_result_produce(0)),__objc3_try(checked_region_integer(fault))}
              :new CheckedRegionHeap{__objc3_try(checked_result_produce(0)),__objc3_try(checked_region_integer(fault))};
          assert(value[0].first.value==40);
          if(array)delete[] value;else delete value;
        } __objc3_catch(id<Error> error) {assert(error==checked_property_error);caught=true;}
        if(checked_region_allocations || checked_owner_live)
          fprintf(stderr,"checked heap array=%d selected=%d fault=%d allocations=%d owners=%d\n",array,selected,fault,checked_region_allocations,checked_owner_live);
        assert(caught==bool(fault) && checked_region_allocations==0 && checked_owner_live==0 && checked_owner_created==checked_owner_destroyed);
      }
    for(int local_thread=0;local_thread!=2;++local_thread)for(int attempt=0;attempt!=2;++attempt) {
      bool caught=false;
      try {checked_region_static_failure(local_thread);}catch(int error) {assert(error==709);caught=true;}
      if(checked_owner_live)fprintf(stderr,"checked static thread=%d attempt=%d owners=%d\n",local_thread,attempt,checked_owner_live);
      assert(caught && checked_owner_live==0 && checked_parameter_live==0 && checked_owner_created==checked_owner_destroyed);
    }
    for(int route=0;route!=2;++route)for(int fault=0;fault!=3;++fault) {
      checked_owner_stage=2000+route*10+fault;
      objc3_async_frame frame=checked_region_async(route,fault);objc3_task_t *task=property_start(&frame);
      objc3_completion done=property_finish(task,0);
      if(fault==2) {
        bool caught=false;
        try {objc3::task_detail::rethrow_completion(done);}catch(int error) {assert(error==709);caught=true;}
        assert(done.kind==OBJC3_COMPLETION_ERROR && caught);
      } else assert(done.kind==OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload==(fault?-1:99));
      objc3_task_release(task);
      if(checked_owner_live || checked_parameter_live)
        fprintf(stderr,"checked async region route=%d fault=%d owners=%d params=%d\n",route,fault,checked_owner_live,checked_parameter_live);
      assert(checked_owner_live==0 && checked_parameter_live==0 && checked_owner_created==checked_owner_destroyed);
    }
    checked_property_error=(id)0;
  }
}
#else
static void checked_region_suite(PropertyMain *object) {(void)object;}
#endif
#endif
