#ifndef OBJC3_CHECKED_CPP_OPTIONAL_RESULTS_H
#define OBJC3_CHECKED_CPP_OPTIONAL_RESULTS_H
#ifdef __cplusplus
typedef __objc3_optional(CheckedPropertyOwner) CheckedOptionalOwner;
typedef __objc3_optional(CheckedMoveResult) CheckedOptionalMove;
static int checked_optional_route(int mode,int route) {
  if(route==0) {
    CheckedOptionalOwner value=__objc3_try_optional(checked_result_produce(mode));
    assert(__objc3_has_value(value)==!mode);
    assert(checked_owner_live==(!mode?1:0));
    auto copy=__objc3_copy(value);assert(__objc3_has_value(copy)==!mode);
    assert(checked_owner_live==(!mode?2:0));
    return !mode?40:-1;
  }
  if(route==1) {
    CheckedResultFactory factory=checked_result_produce;
    CheckedOptionalOwner value=__objc3_try_optional(factory(mode));
    return __objc3_has_value(value)?40:-1;
  }
  if(route==2) {
    CheckedResultDerived factory;CheckedResultBase &base=factory;
    CheckedOptionalOwner value=__objc3_try_optional(base.make(mode));
    return __objc3_has_value(value)?40:-1;
  }
  if(route==3) {
    CheckedResultBlock factory=^CheckedPropertyOwner(int input) __objc3_throws(id<Error>) {return __objc3_try(checked_result_produce(input));};
    CheckedOptionalOwner value=__objc3_try_optional(factory(mode));
    return __objc3_has_value(value)?40:-1;
  }
  if(route==4) {
    __objc3_try_optional(checked_result_produce(mode));
    return !mode?40:-1;
  }
  CheckedOptionalOwner value=__objc3_try_optional(checked_generic_forward<CheckedPropertyOwner>(mode));
  return __objc3_has_value(value)?40:-1;
}
static CheckedPropertyOwner checked_optional_producer(int mode) __objc3_async_frame __objc3_throws(id<Error>) {
  return __objc3_try(checked_result_produce(mode));
}
static int checked_optional_await(int mode,int style) __objc3_async_frame {
  if(style==1) {
    const CheckedOptionalOwner &value=__objc3_try_optional(__objc3_await checked_optional_producer(mode));
    return __objc3_has_value(value)?40:-1;
  }
  if(style==2) {
    CheckedOptionalOwner value=(mode&1)
        ?__objc3_try_optional(__objc3_await checked_optional_producer(mode))
        :__objc3_try_optional(__objc3_await checked_optional_producer(mode));
    return __objc3_has_value(value)?40:-1;
  }
  CheckedOptionalOwner value=__objc3_try_optional(__objc3_await checked_optional_producer(mode));
  return __objc3_has_value(value)?40:-1;
}
struct CheckedOptionalFields {CheckedOptionalOwner first,second;};
struct CheckedOptionalReference {const CheckedOptionalOwner &first;int tail;};
static void checked_optional_inspect(const CheckedOptionalOwner &value) {assert(__objc3_has_value(value));}
static void checked_optional_regions(int route,int fault,bool selected) {
  if(route==0) {
    CheckedOptionalOwner value=selected
        ?__objc3_try_optional(checked_result_produce((CheckedResultParameter(fault),0)))
        :__objc3_try_optional(checked_result_produce((CheckedResultParameter(fault),0)));
    assert(__objc3_has_value(value));return;
  }
  if(route==1) {
    CheckedOptionalOwner value=(checked_optional_inspect(__objc3_try_optional(checked_result_produce(0))),
        __objc3_try_optional(checked_result_produce((CheckedResultParameter(fault),0))));
    assert(__objc3_has_value(value));return;
  }
  if(route==2) {
    const CheckedOptionalOwner &value=__objc3_try_optional(checked_result_produce((CheckedResultParameter(fault),0)));
    assert(__objc3_has_value(value));return;
  }
  if(route==3) {
    CheckedOptionalOwner values[2]={__objc3_try_optional(checked_result_produce(0)),
        __objc3_try_optional(checked_result_produce((CheckedResultParameter(fault),0)))};
    assert(__objc3_has_value(values[0]) && __objc3_has_value(values[1]));return;
  }
  if(route==4) {
    CheckedOptionalFields values={__objc3_try_optional(checked_result_produce(0)),
        __objc3_try_optional(checked_result_produce((CheckedResultParameter(fault),0)))};
    assert(__objc3_has_value(values.first) && __objc3_has_value(values.second));return;
  }
  if(route==6) {
    CheckedOptionalOwner value=selected
        ?__objc3_some(CheckedPropertyOwner,CheckedPropertyOwner((CheckedResultParameter(fault),40)))
        :__objc3_none(CheckedPropertyOwner);
    assert(__objc3_has_value(value)==selected);return;
  }
  if(route==7) {
    const CheckedOptionalOwner &value=(CheckedResultParameter(fault),__objc3_try_optional(checked_result_produce(0)));
    assert(__objc3_has_value(value));return;
  }
  if(route==8) {
    const CheckedPropertyOwner &value=(CheckedResultParameter(fault),__objc3_try_force(checked_result_produce(0)));
    assert(value.value==40);return;
  }
  __objc3_do {
    CheckedOptionalReference value={__objc3_try_optional(checked_result_produce(0)),
        __objc3_try(checked_region_integer(fault))};
    assert(__objc3_has_value(value.first) && value.tail==19);
  } __objc3_catch(id<Error> error){assert(error==checked_property_error);}
}
static void checked_optional_suite(PropertyMain *object) {
  @autoreleasepool {
    checked_property_error=object;
    for(int route=0;route!=6;++route)for(int mode=0;mode!=5;++mode) {
      checked_owner_stage=3000+route*10+mode;bool foreign=false;
      try {assert(checked_optional_route(mode,route)==(!mode?40:-1));}
      catch(int error){assert(error==704 && mode==4);foreign=true;}
      assert(foreign==(mode==4));
      if(checked_owner_live)fprintf(stderr,"optional route=%d mode=%d live=%d\n",route,mode,checked_owner_live);
      assert(!checked_owner_live && checked_owner_created==checked_owner_destroyed);
    }
    for(int mode=0;mode!=5;++mode) {
      bool foreign=false;
      try {
        CheckedOptionalMove value=__objc3_try_optional(checked_move_relay(mode));
        assert(__objc3_has_value(value)==!mode && checked_move_resources==(!mode?1:0));
        auto moved=__objc3_move(value);assert(__objc3_has_value(moved)==!mode);
      }catch(int error){assert(error==704 && mode==4);foreign=true;}
      assert(foreign==(mode==4) && !checked_move_resources);
    }
    for(int argument=0;argument!=2;++argument)for(int fail=0;fail!=2;++fail) {
      bool foreign=false;
      try {
        CheckedOptionalOwner value=__objc3_try_optional(checked_parameter_result(CheckedResultParameter(argument),fail));
        assert(__objc3_has_value(value)==!fail);
      }catch(int error){assert(error==709);foreign=true;}
      assert(foreign==bool(argument) && !checked_parameter_live && !checked_owner_live && checked_owner_created==checked_owner_destroyed);
    }
    for(int route=0;route!=9;++route)for(int fault=0;fault!=2;++fault)for(int selected=0;selected!=2;++selected) {
      checked_owner_stage=4000+route*100+fault*10+selected;bool foreign=false;
      try {checked_optional_regions(route,fault,selected);}
      catch(int error){assert(error==709);foreign=true;}
      assert(foreign==bool(fault && route!=5 && (route!=6 || selected)));
      if(checked_owner_live)fprintf(stderr,"optional region route=%d fault=%d selected=%d owners=%d\n",route,fault,selected,checked_owner_live);
      assert(!checked_owner_live && !checked_parameter_live && checked_owner_created==checked_owner_destroyed);
    }
    bool foreign=false;
    try {
      CheckedOptionalOwner value=__objc3_try_optional(checked_result_produce((CheckedResultParameter(true),0)));
      (void)value;
    }catch(int error){assert(error==709);foreign=true;}
    assert(foreign && !checked_parameter_live && !checked_owner_live && checked_owner_created==checked_owner_destroyed);
    for(int style=0;style!=3;++style)for(int mode=0;mode!=5;++mode) {
      objc3_async_frame frame=checked_optional_await(mode,style);objc3_task_t *task=property_start(&frame);
      objc3_completion done=property_finish(task,0);
      if(mode==4) {
        bool caught=false;
        try{objc3::task_detail::rethrow_completion(done);}catch(int error){assert(error==704);caught=true;}
        assert(done.kind==OBJC3_COMPLETION_ERROR && caught);
      }else assert(done.kind==OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload==(!mode?40:-1));
      objc3_task_release(task);
      assert(!checked_owner_live && checked_owner_created==checked_owner_destroyed);
    }
    checked_property_error=(id)0;
  }
}
#else
static void checked_optional_suite(PropertyMain *object){(void)object;}
#endif
#endif
