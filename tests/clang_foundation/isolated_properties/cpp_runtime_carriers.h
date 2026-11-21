#ifndef OBJC3_CPP_RUNTIME_CARRIER_CALLS_H
#define OBJC3_CPP_RUNTIME_CARRIER_CALLS_H
#ifdef __cplusplus
#include <cstring>
#include <cstdlib>
extern "C" {
  struct objc_method;
  Class object_getClass(id);
  objc_method *class_getInstanceMethod(Class,SEL);
  char *method_copyReturnType(objc_method *);
  __SIZE_TYPE__ objc_sizeof_type(const char *);
  __SIZE_TYPE__ objc_alignof_type(const char *);
  void *_Block_copy(const void *);
  void _Block_release(const void *);
}
static int cpp_runtime_layout_live,cpp_runtime_layout_created,cpp_runtime_layout_destroyed;
struct alignas(16) CppRuntimeAlignedOwner {
  int value;
  explicit CppRuntimeAlignedOwner(int v):value(v){++cpp_runtime_layout_live;++cpp_runtime_layout_created;}
  CppRuntimeAlignedOwner(const CppRuntimeAlignedOwner &v):value(v.value){++cpp_runtime_layout_live;++cpp_runtime_layout_created;}
  ~CppRuntimeAlignedOwner() noexcept{--cpp_runtime_layout_live;++cpp_runtime_layout_destroyed;}
};
struct __attribute__((packed)) CppRuntimePackedOwner {
  char value;
  explicit CppRuntimePackedOwner(int v):value(v){++cpp_runtime_layout_live;++cpp_runtime_layout_created;}
  CppRuntimePackedOwner(const CppRuntimePackedOwner &v):value(v.value){++cpp_runtime_layout_live;++cpp_runtime_layout_created;}
  ~CppRuntimePackedOwner() noexcept{--cpp_runtime_layout_live;++cpp_runtime_layout_destroyed;}
};
using CppRuntimeAligned=__objc3_optional(CppRuntimeAlignedOwner);
using CppRuntimePacked=__objc3_optional(CppRuntimePackedOwner);
static int cpp_runtime_carrier_entries;
@protocol CppRuntimeCarrierProtocol
- (CarrierCallOptional)cppEcho:(CarrierCallOptional)value fail:(int)mode __objc3_throws(id<Error>);
@end
@interface PropertyMain (CppRuntimeCarriers) <CppRuntimeCarrierProtocol>
- (void)cppCopyParameter:(CarrierRecordMaybe)value;
- (CppRuntimeAligned)cppAligned:(CppRuntimeAligned)value;
- (CppRuntimePacked)cppPacked:(CppRuntimePacked)value;
- (CarrierCallOptional)cppRaw:(CarrierCallOptional)value;
- (CarrierCallOptional)cppEcho:(CarrierCallOptional)value fail:(int)mode __objc3_throws(id<Error>);
+ (CarrierCallResult)cppResult:(CarrierCallResult)value fail:(int)mode __objc3_throws(id<Error>);
- (int)cppSum:(CarrierCallOptional)a b:(CarrierCallOptional)b c:(CarrierCallOptional)c
 d:(CarrierCallOptional)d e:(CarrierCallOptional)e f:(CarrierCallOptional)f
 g:(CarrierCallOptional)g h:(CarrierCallOptional)h i:(CarrierCallOptional)i;
@end
@implementation PropertyMain (CppRuntimeCarriers)
- (void)cppCopyParameter:(CarrierRecordMaybe)value{assert(__objc3_has_value(value));++cpp_runtime_carrier_entries;}
- (CppRuntimeAligned)cppAligned:(CppRuntimeAligned)value{return __objc3_copy(value);}
- (CppRuntimePacked)cppPacked:(CppRuntimePacked)value{return __objc3_copy(value);}
- (CarrierCallOptional)cppRaw:(CarrierCallOptional)value {++cpp_runtime_carrier_entries;return __objc3_copy(value);}
- (CarrierCallOptional)cppEcho:(CarrierCallOptional)value fail:(int)mode __objc3_throws(id<Error>) {
  ++cpp_runtime_carrier_entries;if(mode==1)__objc3_throw(checked_property_error);if(mode==2)throw 745;
  return __objc3_copy(value);
}
+ (CarrierCallResult)cppResult:(CarrierCallResult)value fail:(int)mode __objc3_throws(id<Error>) {
  ++cpp_runtime_carrier_entries;if(mode==1)__objc3_throw(checked_property_error);if(mode==2)throw 745;
  return __objc3_copy(value);
}
- (int)cppSum:(CarrierCallOptional)a b:(CarrierCallOptional)b c:(CarrierCallOptional)c
 d:(CarrierCallOptional)d e:(CarrierCallOptional)e f:(CarrierCallOptional)f
 g:(CarrierCallOptional)g h:(CarrierCallOptional)h i:(CarrierCallOptional)i {
  ++cpp_runtime_carrier_entries;
  return __objc3_has_value(a)+__objc3_has_value(b)+__objc3_has_value(c)+__objc3_has_value(d)+
      __objc3_has_value(e)+__objc3_has_value(f)+__objc3_has_value(g)+__objc3_has_value(h)+__objc3_has_value(i);
}
@end
@interface PropertyChild (CppRuntimeCarriers)
- (CarrierCallOptional)cppSuper:(CarrierCallOptional)value;
@end
@implementation PropertyChild (CppRuntimeCarriers)
- (CarrierCallOptional)cppSuper:(CarrierCallOptional)value {return [super cppRaw:__objc3_copy(value)];}
@end
typedef CarrierCallOptional (^CppRuntimeCarrierBlock)(CarrierCallOptional);
static void cpp_runtime_copy_sink(CarrierRecordMaybe value){assert(__objc3_has_value(value));++cpp_runtime_carrier_entries;}
static void cpp_runtime_carrier_suite(PropertyMain *object,PropertyChild *child) {
 @autoreleasepool {
  checked_property_error=object;
  for(int some=0;some!=2;++some)for(int route=0;route!=3;++route) {
    {
      auto original=carrier_call_optional(some);
      auto value=route==0?[object cppRaw:__objc3_copy(original)]:
          route==1?[child cppSuper:original]:[object cppRaw:carrier_call_optional(some)];
      assert(__objc3_has_value(value)==bool(some));
      [object cppRaw:__objc3_copy(original)];
    }
    carrier_call_assert_clean();
  }
  for(int mode=0;mode!=3;++mode)for(int route=0;route!=4;++route) {
    bool typed=false,foreign=false;
    try {
      __objc3_do {
        if(route==0){auto value=__objc3_try([object cppEcho:carrier_call_optional(true) fail:mode]);assert(__objc3_has_value(value));}
        else if(route==1){auto value=__objc3_try_optional([object cppEcho:carrier_call_optional(true) fail:mode]);assert(__objc3_has_value(value)==!mode);}
        else if(route==2){auto value=__objc3_try([PropertyMain cppResult:carrier_call_result(false) fail:mode]);assert(!__objc3_is_ok(value));}
        else {id<CppRuntimeCarrierProtocol> view=object;__objc3_try([view cppEcho:carrier_call_optional(false) fail:mode]);}
      }__objc3_catch(id<Error> error){assert(mode==1 && error==checked_property_error);typed=true;}
    }catch(int error){assert(mode==2 && error==745);foreign=true;}
    assert(typed==(mode==1 && route!=1) && foreign==(mode==2));carrier_call_assert_clean();
  }
  {
    auto value=carrier_call_optional(true);auto empty=carrier_call_optional(false);
    int count=[object cppSum:value b:empty c:__objc3_copy(value)
        d:__objc3_copy(empty) e:__objc3_copy(value) f:__objc3_copy(empty)
        g:__objc3_copy(value) h:__objc3_copy(empty) i:__objc3_copy(value)];
    assert(count==5);
  }
  carrier_call_assert_clean();
  for(int route=0;route!=2;++route) {
    bool failed=false;int entries=cpp_runtime_carrier_entries;
    try {
      PropertyMain *nil=(PropertyMain *)0;
      if(route==0){auto value=[nil cppRaw:carrier_call_optional(true)];(void)value;}
      else {auto value=__objc3_try_optional([nil cppEcho:carrier_call_optional(true) fail:0]);(void)value;}
      assert(!"nil C++ carrier message produced a value");
    }catch(const objc3::service_failure &error){assert(error.status()==OBJC3_SERVICE_INVALID_ARGUMENT);failed=true;}
    assert(failed && cpp_runtime_carrier_entries==entries);carrier_call_assert_clean();
  }
  for(int fail=0;fail!=2;++fail) {
    bool caught=false;
    try {
      auto captured=carrier_call_optional(true);
      __unsafe_unretained CppRuntimeCarrierBlock literal=^CarrierCallOptional(CarrierCallOptional argument) {
        return __objc3_has_value(argument)?__objc3_copy(captured):__objc3_none(CarrierCallOwner);
      };
      carrier_call_fault=fail?2:0;
#if __has_feature(objc_arc)
      CppRuntimeCarrierBlock copied=(__bridge_transfer CppRuntimeCarrierBlock)_Block_copy((__bridge const void *)literal);
#else
      CppRuntimeCarrierBlock copied=(CppRuntimeCarrierBlock)_Block_copy((const void *)literal);
#endif
      carrier_call_fault=0;
      {
        auto value=copied(carrier_call_optional(true));assert(__objc3_has_value(value));
        auto empty=copied(carrier_call_optional(false));assert(!__objc3_has_value(empty));
        copied(carrier_call_optional(true));
      }
#if __has_feature(objc_arc)
      copied=(CppRuntimeCarrierBlock)0;
#else
      _Block_release((const void *)copied);
#endif
    }catch(int error){assert(fail && error==722);caught=true;}
    carrier_call_fault=0;assert(caught==bool(fail));carrier_call_assert_clean();
  }
  {
    auto aligned=[object cppAligned:__objc3_some(CppRuntimeAlignedOwner,CppRuntimeAlignedOwner(17))];
    auto packed=[object cppPacked:__objc3_some(CppRuntimePackedOwner,CppRuntimePackedOwner(19))];
    assert(__objc3_has_value(aligned) && __objc3_has_value(packed));
  }
  assert(!cpp_runtime_layout_live && cpp_runtime_layout_created==cpp_runtime_layout_destroyed);
  auto reflect=[&](SEL selector,__SIZE_TYPE__ size,__SIZE_TYPE__ alignment){
    auto method=class_getInstanceMethod(object_getClass(object),selector);assert(method);
    char *type=method_copyReturnType(method);assert(type && std::strstr(type,"__objc3_cxx_storage_v1_"));
    assert(objc_sizeof_type(type)==size && objc_alignof_type(type)==alignment);std::free(type);
  };
  reflect(@selector(cppAligned:),sizeof(CppRuntimeAligned),alignof(CppRuntimeAligned));
  reflect(@selector(cppPacked:),sizeof(CppRuntimePacked),alignof(CppRuntimePacked));
  for(int route=0;route!=3;++route)for(int fault=0;fault!=3;++fault) {
    {
      auto original=carrier_record_element(7,0);
      carrier_record_copy_attempt=carrier_record_temp_attempt=0;
      carrier_record_fail_copy=fault==1?1:0;carrier_record_fail_temp=fault==2?1:0;
      bool failed=false;
      try {
        if(route==0)cpp_runtime_copy_sink(original);
        else if(route==1){auto block=^(CarrierRecordMaybe value){cpp_runtime_copy_sink(value);};block(original);}
        else [object cppCopyParameter:original];
      }catch(int error){assert((fault==1 && error==732)||(fault==2 && error==734));failed=true;}
      carrier_record_fail_copy=carrier_record_fail_temp=0;
      assert(failed==bool(fault) && __objc3_has_value(original) && carrier_record_live==1);
    }
    carrier_record_assert_clean();
  }
  auto method=class_getInstanceMethod(object_getClass(object),@selector(cppRaw:));assert(method);
  char *encoding=method_copyReturnType(method);assert(encoding && std::strstr(encoding,"__objc3_v13_optional_C"));
  assert(objc_sizeof_type(encoding)==sizeof(CarrierCallOptional) && objc_alignof_type(encoding)==alignof(CarrierCallOptional));
  std::free(encoding);checked_property_error=(id)0;
 }
}
#else
static void cpp_runtime_carrier_suite(PropertyMain *object,PropertyChild *child){(void)object;(void)child;}
#endif
#endif
