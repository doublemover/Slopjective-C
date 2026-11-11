#ifndef OBJC3_NATIVE_CHECKED_REGISTER_RESULTS_H
#define OBJC3_NATIVE_CHECKED_REGISTER_RESULTS_H
#ifdef __cplusplus
static int checked_register_live,checked_register_created,checked_register_destroyed,checked_register_mode;
struct CheckedRegisterSmall {int value;};
union CheckedRegisterUnion {int value;double other;};
struct CheckedRegisterEmpty {};
static CheckedRegisterEmpty checked_register_empty(bool fail) __objc3_throws(id<Error>) {if(fail)__objc3_throw(checked_property_error);return {};}
struct CheckedRegisterPair {long value,other;};
struct CheckedRegisterMixed {double value;int other;};
struct CheckedRegisterLarge {long value,other[4];};
struct [[clang::trivial_abi]] CheckedRegisterOwner {
  int value;int *resource;
  explicit CheckedRegisterOwner(int v):value(v),resource(new int(v)){++checked_register_live;++checked_register_created;}
  CheckedRegisterOwner(const CheckedRegisterOwner &v):value(v.value),resource(new int(v.value)){++checked_register_live;++checked_register_created;}
  CheckedRegisterOwner(CheckedRegisterOwner &&v) noexcept:value(v.value),resource(v.resource){v.resource=nullptr;}
  ~CheckedRegisterOwner() noexcept {if(resource){delete resource;--checked_register_live;++checked_register_destroyed;}}
};
template<class T> static T checked_register_source(int mode) __objc3_throws(id<Error>) {
  if(mode==2)__objc3_throw(checked_property_error);
  T value{47};
  if(mode==3)__objc3_throw(checked_property_error);
#if __has_feature(objc_exceptions)
  @try {return value;}
  @finally {if(mode==4)__objc3_throw(checked_property_error);if(mode==5)throw 744;}
#else
  if(mode==4)__objc3_throw(checked_property_error);
  __objc3_defer {if(mode==5)throw 744;}
  return value;
#endif
}
template<class T> static T checked_register_forward(int mode) __objc3_throws(id<Error>) {
  return __objc3_try(checked_register_source<T>(mode));
}
template<class T> static void checked_register_calls() {
  for(int mode=0;mode!=6;++mode)for(int route=0;route!=7;++route) {
    bool typed=false,foreign=false;
    try {
      __objc3_do {
        if(route==0){auto value=__objc3_try(checked_register_source<T>(mode));assert(value.value==47);}
        else if(route==1){auto value=__objc3_try(checked_register_forward<T>(mode));assert(value.value==47);}
        else if(route==2){auto value=__objc3_try_optional(checked_register_source<T>(mode));assert(__objc3_has_value(value)==(mode<2));}
        else if(route==3){__objc3_try(checked_register_source<T>(mode));}
        else if(route==4){const auto &value=__objc3_try(checked_register_source<T>(mode));assert(value.value==47);}
        else if(route==5){auto function=&checked_register_source<T>;auto value=__objc3_try(function(mode));assert(value.value==47);}
        else {auto block=^T(int input) __objc3_throws(id<Error>){return __objc3_try(checked_register_source<T>(input));};
          auto value=__objc3_try(block(mode));assert(value.value==47);}
      }__objc3_catch(id<Error> error){assert(mode>=2 && mode<5 && error==checked_property_error);typed=true;}
    }catch(int error){assert(mode==5 && error==744);foreign=true;}
    assert(typed==(mode>=2 && mode<5 && route!=2) && foreign==(mode==5));
    assert(!checked_register_live && checked_register_created==checked_register_destroyed);
  }
}
@interface PropertyMain (CheckedRegisterResults)
@property(nonatomic,readonly)CheckedRegisterSmall nativeSmall;
- (CheckedRegisterSmall)nativeSmall __objc3_throws(id<Error>);
@property(nonatomic,readonly)CheckedRegisterOwner nativeRegisterOwner;
- (CheckedRegisterOwner)nativeRegisterOwner __objc3_throws(id<Error>);
@end
@implementation PropertyMain (CheckedRegisterResults)
- (CheckedRegisterSmall)nativeSmall __objc3_throws(id<Error>){return __objc3_try(checked_register_source<CheckedRegisterSmall>(checked_register_mode));}
- (CheckedRegisterOwner)nativeRegisterOwner __objc3_throws(id<Error>){return __objc3_try(checked_register_source<CheckedRegisterOwner>(checked_register_mode));}
@end
static void checked_register_result_suite(PropertyMain *object) {
 @autoreleasepool {
  checked_property_error=object;
  checked_register_calls<CheckedRegisterSmall>();checked_register_calls<CheckedRegisterPair>();
  checked_register_calls<CheckedRegisterMixed>();checked_register_calls<CheckedRegisterLarge>();
  checked_register_calls<CheckedRegisterOwner>();checked_register_calls<CheckedRegisterUnion>();
  for(int fail=0;fail!=2;++fail) {
    auto optional=__objc3_try_optional(checked_register_empty(fail));assert(__objc3_has_value(optional)==!fail);
    __objc3_try_optional(checked_register_empty(fail));
    bool caught=false,completed=false;
    __objc3_do {auto value=__objc3_try(checked_register_empty(fail));(void)value;completed=true;}
    __objc3_catch(id<Error> error){assert(fail && error==checked_property_error);caught=true;}
    assert(caught==bool(fail) && completed==!fail);
  }
  for(int mode=0;mode!=6;++mode)for(int route=0;route!=4;++route) {
    checked_register_mode=mode;bool typed=false,foreign=false;
    try {
      __objc3_do {
        if(route==0){auto value=__objc3_try(object.nativeSmall);assert(value.value==47);}
        else if(route==1){auto value=__objc3_try_optional(object.nativeSmall);assert(__objc3_has_value(value)==(mode<2));}
        else if(route==2){auto value=__objc3_try(object.nativeRegisterOwner);assert(value.value==47);}
        else {auto value=__objc3_try_optional(object.nativeRegisterOwner);assert(__objc3_has_value(value)==(mode<2));}
      }__objc3_catch(id<Error> error){assert(error==checked_property_error);typed=true;}
    }catch(int error){assert(mode==5 && error==744);foreign=true;}
    assert(typed==(mode>=2 && mode<5 && !(route&1)) && foreign==(mode==5));
    assert(!checked_register_live && checked_register_created==checked_register_destroyed);
  }
  for(int route=0;route!=2;++route) {
    bool failed=false;
    try {
      __objc3_do {
        PropertyMain *nil=(PropertyMain *)0;
        if(route==0){auto value=__objc3_try(nil.nativeSmall);(void)value;}
        else {auto value=__objc3_try_optional(nil.nativeRegisterOwner);(void)value;}
        assert(!"nil checked register result produced a value");
      }__objc3_catch(id<Error>){assert(!"nil checked register result produced a typed error");}
    }catch(const objc3::service_failure &error){assert(error.status()==OBJC3_SERVICE_INVALID_ARGUMENT);failed=true;}
    assert(failed && !checked_register_live && checked_register_created==checked_register_destroyed);
  }
  checked_property_error=(id)0;
 }
}
#else
static void checked_register_result_suite(PropertyMain *object){(void)object;}
#endif
#endif
