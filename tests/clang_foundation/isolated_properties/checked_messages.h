#ifndef OBJC3_NATIVE_CHECKED_CPP_MESSAGES_H
#define OBJC3_NATIVE_CHECKED_CPP_MESSAGES_H
#ifdef __cplusplus
static int checked_message_entries,checked_message_receivers,checked_message_arguments,checked_message_mode;
static int checked_message_event,checked_message_receiver_event,checked_message_argument_event;
static PropertyMain *checked_message_receiver(PropertyMain *object) {
  ++checked_message_receivers;checked_message_receiver_event=++checked_message_event;return object;
}
static int checked_message_argument(bool fail) {
  ++checked_message_arguments;checked_message_argument_event=++checked_message_event;if(fail)throw 736;return 47;
}
@interface PropertyMain (NativeCppMessages)
@property(nonatomic,readonly) CheckedPropertyOwner nativeDirectOwner;
- (CheckedPropertyOwner)nativeDirectOwner __objc3_throws(id<Error>);
- (CheckedPropertyOwner)nativeCheckedOwner:(int)mode argument:(CheckedPropertyOwner)value __objc3_throws(id<Error>);
+ (CheckedPropertyOwner)nativeCheckedOwner:(int)mode __objc3_throws(id<Error>);
@end
@implementation PropertyMain (NativeCppMessages)
- (CheckedPropertyOwner)nativeDirectOwner __objc3_throws(id<Error>) {
  ++checked_message_entries;return __objc3_try(checked_result_produce(checked_message_mode));
}
- (CheckedPropertyOwner)nativeCheckedOwner:(int)mode argument:(CheckedPropertyOwner)value __objc3_throws(id<Error>) {
  ++checked_message_entries;assert(value.value==47);
  return __objc3_try(checked_result_produce(mode));
}
+ (CheckedPropertyOwner)nativeCheckedOwner:(int)mode __objc3_throws(id<Error>) {
  ++checked_message_entries;return __objc3_try(checked_result_produce(mode));
}
@end
@interface PropertyChild (NativeCppMessages)
- (CheckedPropertyOwner)nativeSuperOwner:(int)mode __objc3_throws(id<Error>);
@end
@implementation PropertyChild (NativeCppMessages)
- (CheckedPropertyOwner)nativeSuperOwner:(int)mode __objc3_throws(id<Error>) {
  auto value=__objc3_try([super nativeCheckedOwner:mode argument:CheckedPropertyOwner(47)]);
  value.value+=1;return value;
}
@end
static void checked_message_suite(PropertyMain *object,PropertyChild *child) {
 @autoreleasepool {
  checked_property_error=object;
  for(int mode=0;mode!=5;++mode)for(int route=0;route!=7;++route) {
    checked_message_mode=mode;bool typed=false,foreign=false;
    try {
      __objc3_do {
        if(route==0) {
          auto value=__objc3_try([checked_message_receiver(object) nativeCheckedOwner:mode
              argument:CheckedPropertyOwner(checked_message_argument(false))]);
          assert(value.value==40);
        }else if(route==1) {
          auto value=__objc3_try_optional([object nativeCheckedOwner:mode argument:CheckedPropertyOwner(47)]);
          assert(__objc3_has_value(value)==!mode);
        }else if(route==2) {
          __objc3_try([object nativeCheckedOwner:mode argument:CheckedPropertyOwner(47)]);
        }else if(route==3) {
          auto value=__objc3_try([PropertyMain nativeCheckedOwner:mode]);assert(value.value==40);
        }else if(route==4) {
          auto value=__objc3_try([child nativeSuperOwner:mode]);assert(value.value==41);
        }else if(route==5) {
          auto value=__objc3_try(checked_message_receiver(object).nativeDirectOwner);assert(value.value==40);
        }else {
          auto value=__objc3_try_optional(object.nativeDirectOwner);assert(__objc3_has_value(value)==!mode);
        }
      }__objc3_catch(id<Error> error){assert(error==checked_property_error);typed=true;}
    }catch(int error){assert(error==704 && mode==4);foreign=true;}
    assert(foreign==(mode==4) && typed==(mode>0 && mode<4 && route!=1 && route!=6));
    assert(!checked_owner_live && checked_owner_created==checked_owner_destroyed);
  }
  for(int fail=0;fail!=2;++fail)for(int optional=0;optional!=2;++optional) {
    int entries=checked_message_entries,receivers=checked_message_receivers,arguments=checked_message_arguments;
    int event=checked_message_event;
    bool service=false,argument=false;
    try {
      __objc3_do {
        PropertyMain *nil=(PropertyMain *)0;
        if(optional) {
          auto value=__objc3_try_optional([checked_message_receiver(nil) nativeCheckedOwner:0
              argument:CheckedPropertyOwner(checked_message_argument(fail))]);(void)value;
        }else {
          auto value=__objc3_try([checked_message_receiver(nil) nativeCheckedOwner:0
              argument:CheckedPropertyOwner(checked_message_argument(fail))]);(void)value;
        }
        assert(!"nil native C++ result returned an object");
      }__objc3_catch(id<Error>){assert(!"nil service failure became a typed object error");}
    }catch(const objc3::service_failure &error){assert(!fail && error.status()==OBJC3_SERVICE_INVALID_ARGUMENT);service=true;}
    catch(int error){assert(fail && error==736);argument=true;}
    assert(service==!fail && argument==bool(fail));
    assert(checked_message_entries==entries && checked_message_receivers==receivers+1 && checked_message_arguments==arguments+1);
    assert(checked_message_receiver_event==event+1 && checked_message_argument_event==event+2);
    assert(!checked_owner_live && checked_owner_created==checked_owner_destroyed);
  }
  checked_message_mode=0;
  for(int optional=0;optional!=2;++optional) {
    bool caught=false;
    try {
      __objc3_do {
        PropertyMain *nil=(PropertyMain *)0;
        if(optional){auto value=__objc3_try_optional(nil.nativeDirectOwner);(void)value;}
        else {auto value=__objc3_try(nil.nativeDirectOwner);(void)value;}
        assert(!"nil native property produced an object");
      }__objc3_catch(id<Error>){assert(!"nil property service error became a typed object error");}
    }catch(const objc3::service_failure &error){assert(error.status()==OBJC3_SERVICE_INVALID_ARGUMENT);caught=true;}
    assert(caught && !checked_owner_live && checked_owner_created==checked_owner_destroyed);
  }
  struct ReceiverTemporary {
    __unsafe_unretained PropertyMain *object;bool fail;
    PropertyMain *get() const noexcept {return object;}
    ~ReceiverTemporary() noexcept(false) {if(fail)throw 737;}
  };
  for(int fail=0;fail!=2;++fail) {
    bool caught=false;
    try {
      __objc3_do {
        auto value=__objc3_try(ReceiverTemporary{object,bool(fail)}.get().nativeDirectOwner);
        assert(value.value==40);
      }__objc3_catch(id<Error>){assert(!"unexpected temporary-receiver typed failure");}
    }catch(int error){assert(fail && error==737);caught=true;}
    assert(caught==bool(fail) && !checked_owner_live && checked_owner_created==checked_owner_destroyed);
  }
  checked_property_error=(id)0;
 }
}
#else
static void checked_message_suite(PropertyMain *object,PropertyChild *child){(void)object;(void)child;}
#endif
#endif
