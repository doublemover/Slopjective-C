#ifndef OBJC3_CHECKED_CPP_RESULTS_RUNTIME_H
#define OBJC3_CHECKED_CPP_RESULTS_RUNTIME_H
#ifdef __cplusplus
#include <memory>
static int checked_result_checkpoints,checked_move_resources;
static CheckedPropertyOwner checked_result_produce(int mode) __objc3_throws(id<Error>) {
  if(mode==1)__objc3_throw(checked_property_error);
  CheckedPropertyOwner result(40);
  if(mode==2)__objc3_throw(checked_property_error);
#if __has_feature(objc_exceptions)
  @try {return result;}
  @finally {
    ++checked_result_checkpoints;
    if(mode==3)__objc3_throw(checked_property_error);
    if(mode==4)throw 704;
  }
#else
  if(mode==3)__objc3_throw(checked_property_error);
  __objc3_defer {++checked_result_checkpoints;if(mode==4)throw 704;}
  return result;
#endif
}
static CheckedPropertyOwner checked_result_relay(int mode) __objc3_throws(id<Error>) {
  CheckedPropertyOwner result=__objc3_try(checked_result_produce(mode));
  return result;
}
typedef CheckedPropertyOwner (*CheckedResultFactory)(int) __objc3_throws(id<Error>);
typedef CheckedPropertyOwner (^CheckedResultBlock)(int) __objc3_throws(id<Error>);
struct CheckedResultBase {
  virtual CheckedPropertyOwner make(int mode) __objc3_throws(id<Error>)=0;
  virtual ~CheckedResultBase() noexcept=default;
};
struct CheckedResultPadding {virtual ~CheckedResultPadding() noexcept=default;int padding=17;};
struct CheckedResultDerived : CheckedResultPadding,CheckedResultBase {
  CheckedPropertyOwner make(int mode) __objc3_throws(id<Error>) override {
    return __objc3_try(checked_result_produce(mode));
  }
};
static int checked_result_take(CheckedPropertyOwner value,int fail) __objc3_throws(id<Error>) {
  assert(value.value==40);
  if(fail)__objc3_throw(checked_property_error);
  return value.value;
}
template<class T> T checked_generic_result(int mode) __objc3_throws(id<Error>);
template<> inline CheckedPropertyOwner checked_generic_result<CheckedPropertyOwner>(int mode) __objc3_throws(id<Error>) {
  return __objc3_try(checked_result_produce(mode));
}
template<class T> T checked_generic_forward(int mode) __objc3_throws(id<Error>) {
  return __objc3_try(checked_generic_result<T>(mode));
}
static int checked_result_use(int mode,int route) {
  __objc3_do {
    if(route==0) {CheckedPropertyOwner value=__objc3_try(checked_result_produce(mode));return value.value;}
    if(route==1) {CheckedPropertyOwner value=__objc3_try(checked_result_relay(mode));return value.value;}
    if(route==2) {CheckedResultFactory factory=&checked_result_produce;CheckedPropertyOwner value=__objc3_try(factory(mode));return value.value;}
    if(route==3) {
      CheckedResultDerived factory;CheckedResultBase &base=factory;
      CheckedPropertyOwner value=__objc3_try(base.make(mode));return value.value;
    }
    if(route==4) {
      CheckedResultBlock factory=^CheckedPropertyOwner(int input) __objc3_throws(id<Error>) {
        return __objc3_try(checked_result_produce(input));
      };
      CheckedPropertyOwner value=__objc3_try(factory(mode));return value.value;
    }
    if(route==5) {__objc3_try(checked_result_produce(mode));return 40;}
    if(route==6)return __objc3_try(checked_result_produce(mode)).value;
    if(route==8) {CheckedPropertyOwner value=__objc3_try(checked_generic_forward<CheckedPropertyOwner>(mode));return value.value;}
    if(route==9) {const CheckedPropertyOwner &value=__objc3_try(checked_result_produce(mode));return value.value;}
    return __objc3_try(checked_result_take(__objc3_try(checked_result_produce(0)),mode));
  } __objc3_catch(id<Error> error) {assert(error==checked_property_error);return -1000-mode;}
}
struct CheckedMoveResult {
  std::unique_ptr<int> value;
  explicit CheckedMoveResult(int number):value(new int(number)) {++checked_move_resources;}
  CheckedMoveResult(const CheckedMoveResult &)=delete;
  CheckedMoveResult(CheckedMoveResult &&) noexcept=default;
  ~CheckedMoveResult() noexcept {if(value)--checked_move_resources;assert(checked_move_resources>=0);}
};
static CheckedMoveResult checked_move_result(int mode) __objc3_throws(id<Error>) {
  if(mode==1)__objc3_throw(checked_property_error);
  CheckedMoveResult result(47);
  if(mode==2)__objc3_throw(checked_property_error);
#if __has_feature(objc_exceptions)
  @try {return result;}
  @finally {if(mode==3)__objc3_throw(checked_property_error);if(mode==4)throw 704;}
#else
  if(mode==3)__objc3_throw(checked_property_error);
  __objc3_defer {if(mode==4)throw 704;}
  return result;
#endif
}
static CheckedMoveResult checked_move_relay(int mode) __objc3_throws(id<Error>) {
  return __objc3_try(checked_move_result(mode));
}
static int checked_parameter_live;
struct CheckedResultParameter {
  bool fail;
  explicit CheckedResultParameter(bool failure) noexcept:fail(failure) {++checked_parameter_live;}
  CheckedResultParameter(const CheckedResultParameter &other) noexcept:fail(other.fail) {++checked_parameter_live;}
  ~CheckedResultParameter() noexcept(false) {--checked_parameter_live;if(fail)throw 709;}
};
static CheckedPropertyOwner checked_parameter_result(CheckedResultParameter parameter,bool fail) __objc3_throws(id<Error>) {
  (void)parameter;
  CheckedPropertyOwner result(40);
  if(fail)__objc3_throw(checked_property_error);
  return result;
}
static bool checked_return_move_fault;
struct CheckedThrowMoveResult {
  CheckedPropertyOwner item;
  explicit CheckedThrowMoveResult(int value) noexcept:item(value) {}
  CheckedThrowMoveResult(const CheckedThrowMoveResult &)=delete;
  CheckedThrowMoveResult(CheckedThrowMoveResult &&other):item(static_cast<CheckedPropertyOwner &&>(other.item)) {
    if(checked_return_move_fault)throw 708;
  }
  ~CheckedThrowMoveResult() noexcept=default;
};
static CheckedThrowMoveResult checked_throw_move_result(void) __objc3_throws(id<Error>) {
  CheckedThrowMoveResult result(40);return static_cast<CheckedThrowMoveResult &&>(result);
}
static CheckedPropertyOwner checked_async_completed(int value) __objc3_async_frame {return CheckedPropertyOwner(value);}
static int checked_async_initialization(int mode) __objc3_async_frame {
  if(mode==0) {
    CheckedPropertyOwner value=__objc3_await checked_async_completed((CheckedResultParameter(true),40));
    return value.value;
  }
  if(mode==1) {
    const CheckedPropertyOwner &value=__objc3_await checked_async_completed((CheckedResultParameter(true),40));
    return value.value;
  }
  CheckedPropertyOwner values[2]={__objc3_await checked_async_completed(40),
      __objc3_await checked_async_completed((CheckedResultParameter(true),40))};
  return values[0].value+values[1].value;
}
static void checked_result_suite(PropertyMain *object) {
  @autoreleasepool {
  checked_property_error=object;
  for(int route=0;route!=10;++route)for(int mode=0;mode!=5;++mode) {
    checked_owner_stage=route*10+mode;
    bool foreign=false;
    try {
      int result=checked_result_use(mode,route);
      assert(result==(mode?-1000-mode:40));
    } catch(int error) {assert(error==704 && mode==4 && route!=7);foreign=true;}
    assert(foreign==(mode==4 && route!=7));
    assert(checked_owner_live==0 && checked_owner_created==checked_owner_destroyed);
  }
  {CheckedPropertyOwner value=__objc3_try_force(checked_result_produce(0));assert(value.value==40);}
  for(int mode=0;mode!=5;++mode) {
    bool checked=false,foreign=false;
    try {
      __objc3_do {CheckedMoveResult result=__objc3_try(checked_move_relay(mode));assert(*result.value==47);}
      __objc3_catch(id<Error> error) {assert(error==checked_property_error);checked=true;}
    } catch(int error) {assert(error==704);foreign=true;}
    assert(checked==(mode>=1 && mode<=3) && foreign==(mode==4));
    assert(checked_move_resources==0);
  }
  for(int argument=0;argument!=2;++argument)for(int fail=0;fail!=2;++fail) {
    bool checked=false,foreign=false;
    try {
      __objc3_do {
        CheckedPropertyOwner result=__objc3_try(checked_parameter_result(CheckedResultParameter(argument),fail));
        assert(result.value==40);
      } __objc3_catch(id<Error> error) {assert(error==checked_property_error);checked=true;}
    } catch(int error) {assert(error==709);foreign=true;}
    assert(foreign==bool(argument) && checked==bool(fail && !argument));
    if(checked_parameter_live || checked_owner_live || checked_owner_created!=checked_owner_destroyed)
      fprintf(stderr,"checked param argument=%d fail=%d params=%d owners=%d created=%d destroyed=%d\n",argument,fail,checked_parameter_live,checked_owner_live,checked_owner_created,checked_owner_destroyed);
    assert(checked_parameter_live==0 && checked_owner_live==0 && checked_owner_created==checked_owner_destroyed);
  }
  for(int fault=0;fault!=2;++fault) {
    checked_return_move_fault=fault;bool caught=false;
    try {
      CheckedThrowMoveResult result=__objc3_try_force(checked_throw_move_result());assert(result.item.value==40);
    } catch(int error) {assert(error==708);caught=true;}
    assert(caught==bool(fault) && checked_owner_live==0 && checked_owner_created==checked_owner_destroyed);
  }
  for(int mode=0;mode!=5;++mode) {
    bool checked=false,foreign=false;
    try {
      __objc3_do {
        CheckedMoveResult values[2]={__objc3_try(checked_move_relay(0)),__objc3_try(checked_move_relay(mode))};
        assert(*values[0].value==47 && *values[1].value==47);
      } __objc3_catch(id<Error> error) {assert(error==checked_property_error);checked=true;}
    } catch(int error) {assert(error==704);foreign=true;}
    assert(checked==(mode>=1 && mode<=3) && foreign==(mode==4));
    if(checked_move_resources)fprintf(stderr,"checked array mode=%d resources=%d\n",mode,checked_move_resources);
    assert(checked_move_resources==0);
  }
  for(int reference=0;reference!=2;++reference) {
    bool foreign=false;
    try {
      __objc3_do {
        if(reference) {
          const CheckedPropertyOwner &value=__objc3_try(checked_parameter_result(CheckedResultParameter(true),false));
          (void)value;
        } else {
          CheckedPropertyOwner values[2]={__objc3_try(checked_result_produce(0)),__objc3_try(checked_parameter_result(CheckedResultParameter(true),false))};
          (void)values;
        }
      } __objc3_catch(id<Error>) {assert(!"unexpected array/reference error");}
    } catch(int error) {assert(error==709);foreign=true;}
    if(checked_owner_live)fprintf(stderr,"checked completion reference=%d owners=%d\n",reference,checked_owner_live);
    assert(foreign && checked_owner_live==0 && checked_parameter_live==0 && checked_owner_created==checked_owner_destroyed);
  }
  for(int mode=0;mode!=3;++mode) {
    objc3_async_frame frame=checked_async_initialization(mode);objc3_task_t *task=property_start(&frame);
    objc3_completion done=property_finish(task,0);bool caught=false;
    try {objc3::task_detail::rethrow_completion(done);}catch(int error) {assert(error==709);caught=true;}
    assert(done.kind==OBJC3_COMPLETION_ERROR && caught);objc3_task_release(task);
    if(checked_owner_live || checked_parameter_live)fprintf(stderr,"checked async completion mode=%d owners=%d params=%d\n",mode,checked_owner_live,checked_parameter_live);
    assert(checked_owner_live==0 && checked_parameter_live==0 && checked_owner_created==checked_owner_destroyed);
  }
  assert(checked_owner_live==0 && checked_owner_created==checked_owner_destroyed);
  checked_property_error=(id)0;
  }
}
#else
static void checked_result_suite(PropertyMain *object) {(void)object;}
#endif
#endif
