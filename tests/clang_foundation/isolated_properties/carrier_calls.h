#ifndef OBJC3_NATIVE_CPP_CARRIER_CALLS_H
#define OBJC3_NATIVE_CPP_CARRIER_CALLS_H
#ifdef __cplusplus
static int carrier_call_live,carrier_call_created,carrier_call_destroyed;
static int carrier_call_fault,carrier_call_entries,carrier_call_stage;
struct CarrierCallOwner {
  int value;
  explicit CarrierCallOwner(int value):value(value) {
    if(carrier_call_fault==1)throw 721;
    ++carrier_call_live;++carrier_call_created;
  }
  CarrierCallOwner(const CarrierCallOwner &other):value(other.value) {
    if(carrier_call_fault==2)throw 722;
    ++carrier_call_live;++carrier_call_created;
  }
  CarrierCallOwner(CarrierCallOwner &&other):value(other.value) {
    if(carrier_call_fault==3)throw 723;
    other.value=-1;++carrier_call_live;++carrier_call_created;
  }
  ~CarrierCallOwner() noexcept {--carrier_call_live;++carrier_call_destroyed;assert(carrier_call_live>=0);}
};
using CarrierCallOptional=__objc3_optional(CarrierCallOwner);
using CarrierCallResult=__objc3_result(CarrierCallOwner,CarrierCallOwner);
using CarrierCallNested=__objc3_optional(CarrierCallResult);
static CarrierCallOptional carrier_call_optional(bool some) {
  if(some)return __objc3_some(CarrierCallOwner,CarrierCallOwner(71));
  return __objc3_none(CarrierCallOwner);
}
static CarrierCallResult carrier_call_result(bool ok) {
  if(ok)return __objc3_ok(CarrierCallOwner,CarrierCallOwner,CarrierCallOwner(72));
  return __objc3_err(CarrierCallOwner,CarrierCallOwner,CarrierCallOwner(73));
}
static CarrierCallNested carrier_call_nested(bool some,bool ok) {
  if(some)return __objc3_some(CarrierCallResult,carrier_call_result(ok));
  return __objc3_none(CarrierCallResult);
}
static int carrier_call_read(const CarrierCallOptional &value) {
  return __objc3_match(value) {
    case __objc3_case_some(let payload) => payload.value;
    case __objc3_case_none => -1;
  };
}
static int carrier_call_many(CarrierCallOptional a,CarrierCallOptional b,CarrierCallOptional c,
    CarrierCallOptional d,CarrierCallOptional e,CarrierCallOptional f,
    CarrierCallOptional g,CarrierCallOptional h,CarrierCallOptional i) {
  ++carrier_call_entries;
  return __objc3_has_value(a)+__objc3_has_value(b)+__objc3_has_value(c)+
      __objc3_has_value(d)+__objc3_has_value(e)+__objc3_has_value(f)+
      __objc3_has_value(g)+__objc3_has_value(h)+__objc3_has_value(i);
}
static int carrier_call_take(CarrierCallOptional value,int fault) {
  ++carrier_call_entries;
  if(fault)throw 724;
  assert(carrier_call_read(value)==(__objc3_has_value(value)?71:-1));
  return __objc3_has_value(value);
}
static int carrier_call_checked_take(CarrierCallOptional value,bool fail) __objc3_throws(id<Error>) {
  ++carrier_call_entries;
  if(fail)__objc3_throw(checked_property_error);
  return __objc3_has_value(value);
}
static CarrierCallOptional carrier_call_maybe_fault(bool fault) {
  if(fault)throw 726;
  return carrier_call_optional(true);
}
struct CarrierCallFields {CarrierCallOptional first,second;};
static CarrierCallOptional carrier_call_checked_result(int mode) __objc3_throws(id<Error>) {
  if(mode==2)__objc3_throw(checked_property_error);
  auto value=carrier_call_optional(mode!=1);
  if(mode==3)__objc3_throw(checked_property_error);
#if __has_feature(objc_exceptions)
  @try {return __objc3_move(value);}
  @finally {if(mode==4)__objc3_throw(checked_property_error);if(mode==5)throw 727;}
#else
  if(mode==4)__objc3_throw(checked_property_error);
  __objc3_defer {if(mode==5)throw 727;}
  return __objc3_move(value);
#endif
}
static CarrierCallResult carrier_call_checked_union(int mode) __objc3_throws(id<Error>) {
  if(mode==2)__objc3_throw(checked_property_error);
  auto value=carrier_call_result(mode!=1);
#if __has_feature(objc_exceptions)
  @try {return __objc3_move(value);}
  @finally {if(mode==3)__objc3_throw(checked_property_error);}
#else
  if(mode==3)__objc3_throw(checked_property_error);
  return __objc3_move(value);
#endif
}
static CarrierCallOptional carrier_call_checked_forward(int mode) __objc3_throws(id<Error>) {
  return __objc3_try(carrier_call_checked_result(mode));
}
static int carrier_call_argument_fault() {throw 725;}
static CarrierCallOptional carrier_call_copy(const CarrierCallOptional &value) {return __objc3_copy(value);}
static CarrierCallOptional carrier_call_move(CarrierCallOptional &value) {return __objc3_move(value);}
static CarrierCallOptional carrier_call_return_cleanup(bool fault) {
  CheckedResultParameter parameter(fault);
  return carrier_call_optional(true);
}
struct CarrierCallVirtual {
  virtual CarrierCallOptional make(bool some) {return carrier_call_optional(some);}
  virtual CarrierCallOptional checked(int mode) __objc3_throws(id<Error>) {return __objc3_try(carrier_call_checked_result(mode));}
  virtual ~CarrierCallVirtual() noexcept=default;
};
struct CarrierCallDerived : CarrierCallVirtual {
  CarrierCallOptional make(bool some) override {return carrier_call_optional(some);}
  CarrierCallOptional checked(int mode) __objc3_throws(id<Error>) override {return __objc3_try(carrier_call_checked_result(mode));}
};
template<class T> static T carrier_call_forward(T (*factory)(bool),bool some) {return factory(some);}
using CarrierCallMove=__objc3_optional(CheckedMoveResult);
static CarrierCallMove carrier_call_move_only() {
  return __objc3_some(CheckedMoveResult,CheckedMoveResult(79));
}
static int carrier_call_take_move(CarrierCallMove value) {return __objc3_has_value(value);}
static void carrier_call_assert_clean() {
  if(carrier_call_live || carrier_call_created!=carrier_call_destroyed)
    fprintf(stderr,"carrier stage=%d live=%d created=%d destroyed=%d\n",carrier_call_stage,carrier_call_live,carrier_call_created,carrier_call_destroyed);
  assert(!carrier_call_live && carrier_call_created==carrier_call_destroyed);
}
static void carrier_call_suite(PropertyMain *object) {
 @autoreleasepool {
  checked_property_error=object;
  for(int some=0;some!=2;++some) {
    {
      auto value=carrier_call_optional(some);assert(__objc3_has_value(value)==some);
      auto copied=carrier_call_copy(value);assert(__objc3_has_value(copied)==some);
      auto moved=carrier_call_move(copied);assert(__objc3_has_value(moved)==some);
      assert(carrier_call_take(__objc3_copy(value),0)==some);
      auto forward=carrier_call_forward(carrier_call_optional,some);assert(__objc3_has_value(forward)==some);
      CarrierCallDerived derived;CarrierCallVirtual &base=derived;
      auto dispatched=base.make(some);assert(__objc3_has_value(dispatched)==some);
      auto conditional=some?carrier_call_optional(true):carrier_call_optional(false);
      assert(__objc3_has_value(conditional)==some);
      carrier_call_optional(some);
    }
    carrier_call_assert_clean();
    assert(carrier_call_many(carrier_call_optional(some),carrier_call_optional(some),
        carrier_call_optional(some),carrier_call_optional(some),carrier_call_optional(some),
        carrier_call_optional(some),carrier_call_optional(some),carrier_call_optional(some),
        carrier_call_optional(some))==9*some);
    carrier_call_assert_clean();
    for(int ok=0;ok!=2;++ok) {
      {auto result=carrier_call_result(ok);assert(__objc3_is_ok(result)==ok);
       auto nested=carrier_call_nested(some,ok);assert(__objc3_has_value(nested)==some);}
      carrier_call_assert_clean();
    }
    for(int fault=0;fault!=2;++fault) {
      bool caught=false;
      try{assert(carrier_call_take(carrier_call_optional(some),fault)==some);}
      catch(int error){assert(error==724 && fault);caught=true;}
      assert(caught==bool(fault));carrier_call_assert_clean();
      __objc3_do {assert(__objc3_try(carrier_call_checked_take(carrier_call_optional(some),fault))==some);}
      __objc3_catch(id<Error> error){assert(fault && error==checked_property_error);}
      carrier_call_assert_clean();
    }
  }
  for(int fault=0;fault!=2;++fault) {
    bool caught=false;
    try{auto value=carrier_call_return_cleanup(fault);assert(__objc3_has_value(value));}
    catch(int error){assert(error==709 && fault);caught=true;}
    assert(caught==bool(fault));carrier_call_assert_clean();
  }
  for(int mode=0;mode!=6;++mode)for(int route=0;route!=7;++route) {
    carrier_call_stage=6000+mode*10+route;
    bool typed=false,foreign=false;
    try {
      __objc3_do {
        if(route==0) {
          auto value=__objc3_try(carrier_call_checked_forward(mode));
          assert(__objc3_has_value(value)==(mode!=1));
        }else if(route==1) {
          __objc3_try(carrier_call_checked_result(mode));
        }else if(route==2) {
          auto value=__objc3_try_optional(carrier_call_checked_result(mode));
          assert(__objc3_has_value(value)==(mode<2));
        }else if(route==3) {
          __objc3_try_optional(carrier_call_checked_result(mode));
        }else if(route==4) {
          const CarrierCallOptional &value=__objc3_try(carrier_call_checked_result(mode));
          assert(__objc3_has_value(value)==(mode!=1));
        }else if(route==5) {
          typedef CarrierCallOptional (*Factory)(int) __objc3_throws(id<Error>);
          Factory factory=carrier_call_checked_result;
          auto value=__objc3_try(factory(mode));assert(__objc3_has_value(value)==(mode!=1));
        }else {
          CarrierCallDerived derived;CarrierCallVirtual &base=derived;
          auto value=__objc3_try(base.checked(mode));assert(__objc3_has_value(value)==(mode!=1));
        }
      }__objc3_catch(id<Error> error){assert(error==checked_property_error);typed=true;}
    }catch(int error){assert(mode==5 && error==727);foreign=true;}
    assert(foreign==(mode==5));
    assert(typed==(mode>=2 && mode<=4 && route!=2 && route!=3));
    carrier_call_assert_clean();
  }
  for(int mode=0;mode!=4;++mode) {
    carrier_call_stage=7000+mode;
    __objc3_do {
      auto value=__objc3_try(carrier_call_checked_union(mode));assert(__objc3_is_ok(value)==(mode!=1));
    }__objc3_catch(id<Error> error){assert(mode>=2 && error==checked_property_error);}
    auto optional=__objc3_try_optional(carrier_call_checked_union(mode));
    assert(__objc3_has_value(optional)==(mode<2));
  }
  carrier_call_assert_clean();
  for(int route=0;route!=5;++route)for(int fault=0;fault!=2;++fault) {
    bool caught=false;
    try {
      if(route==0) {
        CarrierCallOptional values[2]={carrier_call_optional(true),carrier_call_maybe_fault(fault)};
        assert(__objc3_has_value(values[0]) && __objc3_has_value(values[1]));
      }else if(route==1) {
        CarrierCallFields values={carrier_call_optional(true),carrier_call_maybe_fault(fault)};
        assert(__objc3_has_value(values.first) && __objc3_has_value(values.second));
      }else if(route==2) {
        auto *values=new CarrierCallFields{carrier_call_optional(true),carrier_call_maybe_fault(fault)};
        assert(__objc3_has_value(values->second));delete values;
      }else if(route==3) {
        auto *values=new CarrierCallOptional[2]{carrier_call_optional(true),carrier_call_maybe_fault(fault)};
        assert(__objc3_has_value(values[1]));delete[] values;
      }else {
        alignas(CarrierCallOptional) unsigned char storage[sizeof(CarrierCallOptional)];
        auto *placed=new (storage) CarrierCallOptional(carrier_call_maybe_fault(fault));
        assert(__objc3_has_value(*placed));placed->~CarrierCallOptional();
        placed=new (storage) CarrierCallOptional(carrier_call_optional(false));
        assert(!__objc3_has_value(*placed));placed->~CarrierCallOptional();
      }
    }catch(int error){assert(error==726 && fault);caught=true;}
    assert(caught==bool(fault));carrier_call_assert_clean();
  }
  int before=carrier_call_entries;bool caught=false;
  try{(void)carrier_call_take(carrier_call_optional(true),carrier_call_argument_fault());}
  catch(int error){assert(error==725);caught=true;}
  assert(caught && carrier_call_entries==before);carrier_call_assert_clean();
  for(int fault=1;fault!=4;++fault) {
    bool caught=false;
    try {
      if(fault==1){carrier_call_fault=fault;carrier_call_optional(true);}
      else {
        auto value=carrier_call_optional(true);carrier_call_fault=fault;
        if(fault==2)carrier_call_copy(value);else carrier_call_move(value);
      }
    }catch(int error){assert(error==720+fault);caught=true;}
    carrier_call_fault=0;assert(caught);carrier_call_assert_clean();
  }
  {
    auto moved=carrier_call_move_only();
    assert(carrier_call_take_move(__objc3_move(moved))==1);
  }
  assert(!checked_move_resources);carrier_call_assert_clean();
  assert(atexit(carrier_call_assert_clean)==0);
  static const CarrierCallOptional &global_optional=carrier_call_optional(true);
  static const CarrierCallResult &global_result=carrier_call_result(false);
  assert(__objc3_has_value(global_optional) && !__objc3_is_ok(global_result));
  assert(carrier_call_live==2);
  checked_property_error=(id)0;
 }
}
#else
static void carrier_call_suite(PropertyMain *object){(void)object;}
#endif
#endif
