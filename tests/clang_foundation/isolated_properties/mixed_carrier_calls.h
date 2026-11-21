#ifndef OBJC3_NATIVE_MIXED_CARRIER_CALLS_H
#define OBJC3_NATIVE_MIXED_CARRIER_CALLS_H
#if defined(__cplusplus) && __has_feature(objc_arc)
static int mixed_call_objects_created,mixed_call_objects_destroyed;
__attribute__((objc_root_class)) @interface MixedCallObject {Class isa;}
+ (instancetype)new;
@end
@implementation MixedCallObject
+ (instancetype)new {++mixed_call_objects_created;return (MixedCallObject *)class_createInstance(self,0);}
- (void)dealloc {++mixed_call_objects_destroyed;object_dispose(self);}
@end
using MixedCallStrong=MixedCallObject *__strong;
using MixedCallWeak=MixedCallObject *__weak;
using MixedCallValue=__objc3_result(CarrierCallOwner,MixedCallStrong);
using MixedCallWeakValue=__objc3_result(CarrierCallOwner,MixedCallWeak);
using MixedCallOptional=__objc3_optional(MixedCallValue);
static MixedCallValue mixed_call_raw(bool success) {
  if(success)return __objc3_ok(CarrierCallOwner,MixedCallStrong,CarrierCallOwner(91));
  return __objc3_err(CarrierCallOwner,MixedCallStrong,[MixedCallObject new]);
}
static MixedCallValue mixed_call_copy(MixedCallValue value){return __objc3_copy(value);}
using MixedCallFaultValue=__objc3_result(CarrierRecordLeaf,MixedCallStrong);
static void mixed_call_fault_argument(MixedCallFaultValue value){(void)value;}
static void mixed_call_fault_pair(MixedCallValue first,MixedCallFaultValue second){(void)first;(void)second;}

static MixedCallWeakValue mixed_call_weak_copy(MixedCallWeakValue value){return __objc3_copy(value);}
static MixedCallOptional mixed_call_outer(bool present,bool success) {
  if(present)return __objc3_some(MixedCallValue,mixed_call_raw(success));
  return __objc3_none(MixedCallValue);
}
static MixedCallValue mixed_call_checked(bool success,int mode) __objc3_throws(id<Error>) {
  if(mode==2)__objc3_throw(checked_property_error);
  auto value=mixed_call_raw(success);
  if(mode==3)__objc3_throw(checked_property_error);
#if __has_feature(objc_exceptions)
  @try{return __objc3_move(value);}
  @finally {if(mode==4)__objc3_throw(checked_property_error);if(mode==5)throw 746;}
#else
  if(mode==4)__objc3_throw(checked_property_error);
  __objc3_defer {if(mode==5)throw 746;}
  return __objc3_move(value);
#endif
}
@interface PropertyMain (MixedCarrierCalls)
- (MixedCallValue)mixedEcho:(MixedCallValue)value;
- (MixedCallValue)mixedChecked:(int)mode success:(bool)success __objc3_throws(id<Error>);
@end
@implementation PropertyMain (MixedCarrierCalls)
- (MixedCallValue)mixedEcho:(MixedCallValue)value{return __objc3_copy(value);}
- (MixedCallValue)mixedChecked:(int)mode success:(bool)success __objc3_throws(id<Error>){return __objc3_try(mixed_call_checked(success,mode));}
@end
static void mixed_carrier_call_suite(PropertyMain *object) {
 @autoreleasepool {
  checked_property_error=object;
  for(int success=0;success!=2;++success)for(int route=0;route!=5;++route) {
   @autoreleasepool {
    {
      auto original=mixed_call_raw(success);
      auto value=route==0?mixed_call_copy(original):route==1?mixed_call_copy(__objc3_copy(original)):
          route==2?[object mixedEcho:original]:route==3?[object mixedEcho:mixed_call_raw(success)]:mixed_call_copy(__objc3_move(original));
      assert(__objc3_is_ok(value)==bool(success));
      mixed_call_raw(success);[object mixedEcho:__objc3_copy(value)];
      auto nested=mixed_call_outer(true,success);assert(__objc3_has_value(nested));
      auto empty=mixed_call_outer(false,success);assert(!__objc3_has_value(empty));
      assert(__objc3_is_ok(mixed_call_raw(success))==bool(success));
    }
   }
   carrier_call_assert_clean();assert(mixed_call_objects_created==mixed_call_objects_destroyed);
  }
  for(int success=0;success!=2;++success)for(int mode=0;mode!=6;++mode)for(int route=0;route!=5;++route) {
   @autoreleasepool {
    bool typed=false,foreign=false;
    try {
      __objc3_do {
        if(route==0){auto value=__objc3_try(mixed_call_checked(success,mode));assert(__objc3_is_ok(value)==bool(success));}
        else if(route==1){auto value=__objc3_try_optional(mixed_call_checked(success,mode));assert(__objc3_has_value(value)==(mode<2));}
        else if(route==2){__objc3_try(mixed_call_checked(success,mode));}
        else if(route==3){auto value=__objc3_try([object mixedChecked:mode success:success]);assert(__objc3_is_ok(value)==bool(success));}
        else {auto value=__objc3_try_optional([object mixedChecked:mode success:success]);assert(__objc3_has_value(value)==(mode<2));}
      }__objc3_catch(id<Error> error){assert(error==checked_property_error);typed=true;}
    }catch(int error){assert(mode==5 && error==746);foreign=true;}
    assert(typed==(mode>=2 && mode<5 && route!=1 && route!=4) && foreign==(mode==5));
   }
   carrier_call_assert_clean();assert(mixed_call_objects_created==mixed_call_objects_destroyed);
  }
  {
    __weak MixedCallObject *observer;
    {
      MixedCallObject *owner=[MixedCallObject new];observer=owner;
      auto original=__objc3_err(CarrierCallOwner,MixedCallWeak,owner);
      auto copied=mixed_call_weak_copy(original);assert(!__objc3_is_ok(copied));
      owner=(MixedCallObject *)0;assert(!observer);
    }
    assert(!observer);
  }
  assert(mixed_call_objects_created==mixed_call_objects_destroyed);
  for(int success=0;success!=2;++success) {
   @autoreleasepool {
    auto block=^MixedCallValue(MixedCallValue value){return __objc3_copy(value);};
    auto original=mixed_call_raw(success);auto value=block(original);assert(__objc3_is_ok(value)==bool(success));
    block(__objc3_copy(original));
   }
   carrier_call_assert_clean();assert(mixed_call_objects_created==mixed_call_objects_destroyed);
  }
  for(int fault=0;fault!=3;++fault) {
    {
      auto original=__objc3_ok(CarrierRecordLeaf,MixedCallStrong,CarrierRecordLeaf(21,0));
      carrier_record_fail_copy=carrier_record_copy_attempt+1;
      carrier_record_fail_temp=0;
      if(fault==1){carrier_record_fail_copy=0;carrier_record_fail_temp=carrier_record_temp_attempt+1;}
      if(fault==2){carrier_record_fail_copy=0;carrier_record_fail_move=carrier_record_move_attempt+1;}
      bool caught=false;
      try {
        if(fault==2)mixed_call_fault_argument(__objc3_move(original));
        else {
          auto strong=mixed_call_raw(false);
          mixed_call_fault_pair(strong,original);
        }
      }catch(int error){assert(error==(fault==0?732:fault==1?734:733));caught=true;}
      assert(caught && carrier_record_live==1);
      carrier_record_fail_copy=carrier_record_fail_move=carrier_record_fail_temp=0;
    }
    carrier_record_assert_clean();
  }
  bool failed=false;
  try {PropertyMain *nil=(PropertyMain *)0;auto value=[nil mixedEcho:mixed_call_raw(true)];(void)value;}
  catch(const objc3::service_failure &error){assert(error.status()==OBJC3_SERVICE_INVALID_ARGUMENT);failed=true;}
  assert(failed);carrier_call_assert_clean();assert(mixed_call_objects_created==mixed_call_objects_destroyed);
  checked_property_error=(id)0;
 }
}
#else
static void mixed_carrier_call_suite(PropertyMain *object){(void)object;}
#endif
#endif
