#ifndef OBJC3_NATIVE_UNION_CARRIER_PAYLOADS_H
#define OBJC3_NATIVE_UNION_CARRIER_PAYLOADS_H
#if defined(__cplusplus) && __has_feature(objc_arc)
#include <new>
union NativeManagedUnion;
union alignas(64) NativeManagedUnion {
  struct Owned {
    int tag;MixedCallStrong object;MixedCallWeak observer;
    AlignedBlockProbe callback;CarrierRecordLeaf payload;
  } owned;
  struct Plain {int tag,value;} plain;
  NativeManagedUnion():plain{0,0} {}
  NativeManagedUnion(int value,bool managed,int fail=0) {
    if(managed) {
      MixedCallObject *object=[MixedCallObject new];
      ::new((void *)&owned) Owned{1,object,object,^int{return value;},CarrierRecordLeaf(value,fail)};
    }else ::new((void *)&plain) Plain{0,value};
  }
  NativeManagedUnion(const NativeManagedUnion &other) {
    if(other.plain.tag)::new((void *)&owned) Owned(other.owned);
    else ::new((void *)&plain) Plain(other.plain);
  }
  NativeManagedUnion(NativeManagedUnion &&other) {
    if(other.plain.tag)::new((void *)&owned) Owned(static_cast<Owned &&>(other.owned));
    else ::new((void *)&plain) Plain(other.plain);
  }
  ~NativeManagedUnion() noexcept {if(plain.tag)owned.~Owned();}
  int value() const {return plain.tag?owned.payload.value:plain.value;}
};
using NativeManagedUnionMaybe=__objc3_optional(NativeManagedUnion);
using NativeManagedUnionResult=__objc3_result(NativeManagedUnion,MixedCallStrong);
using NativeManagedUnionOuter=__objc3_optional(NativeManagedUnionMaybe);
static NativeManagedUnionMaybe native_managed_union(bool present,bool managed,int value) {
  if(!present)return __objc3_none(NativeManagedUnion);
  return __objc3_some(NativeManagedUnion,NativeManagedUnion(value,managed));
}
static NativeManagedUnionMaybe native_managed_union_echo(NativeManagedUnionMaybe value){return __objc3_copy(value);}
static int native_managed_union_value(const NativeManagedUnionMaybe &value) {
  return __objc3_match(value){case __objc3_case_some(let payload)=>payload.value();case __objc3_case_none=>-1;};
}
@interface NativeManagedUnionError : MixedCallObject <Error>
@end
@implementation NativeManagedUnionError
@end
static NativeManagedUnionMaybe native_managed_union_checked(int mode) __objc3_throws(id<Error>) {
  if(mode==1)__objc3_throw(checked_property_error);
  auto value=native_managed_union(true,true,167);
  if(mode==2)__objc3_throw(checked_property_error);
  if(mode==3)throw 758;
  return value;
}
@interface PropertyMain (NativeManagedUnionPayload)
- (NativeManagedUnionMaybe)nativeUnionEcho:(NativeManagedUnionMaybe)value;
- (NativeManagedUnionMaybe)nativeUnionChecked:(int)mode __objc3_throws(id<Error>);
@end
@implementation PropertyMain (NativeManagedUnionPayload)
- (NativeManagedUnionMaybe)nativeUnionEcho:(NativeManagedUnionMaybe)value{return __objc3_copy(value);}
- (NativeManagedUnionMaybe)nativeUnionChecked:(int)mode __objc3_throws(id<Error>){return __objc3_try(native_managed_union_checked(mode));}
@end
static void native_union_carrier_payload_suite(PropertyMain *object) {
  for(int present=0;present!=2;++present)for(int managed=0;managed!=2;++managed) {
    @autoreleasepool {
      auto value=native_managed_union(present,managed,163);
      auto copied=__objc3_copy(value);auto moved=__objc3_move(copied);
      auto returned=native_managed_union_echo(moved);auto message=[object nativeUnionEcho:returned];
      NativeManagedUnionMaybe (^callback)(NativeManagedUnionMaybe)=^NativeManagedUnionMaybe(NativeManagedUnionMaybe input){return __objc3_copy(input);};
      auto blockValue=callback(message);assert(native_managed_union_value(blockValue)==(present?163:-1));
      auto lambda=[blockValue]{return native_managed_union_value(blockValue);};auto lambdaCopy=lambda;
      assert(lambdaCopy()==(present?163:-1));
      __unsafe_unretained AlignedBlockProbe literal=^int{return native_managed_union_value(blockValue);};
      AlignedBlockProbe heap=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)literal);
      assert(heap()==(present?163:-1));heap=(AlignedBlockProbe)0;
      __block NativeManagedUnionMaybe byref=native_managed_union(present,managed,164);
      __unsafe_unretained AlignedBlockProbe byrefLiteral=^int{return native_managed_union_value(byref);};
      AlignedBlockProbe byrefHeap=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)byrefLiteral);
      assert(byrefHeap()==(present?164:-1));byrefHeap=(AlignedBlockProbe)0;
      NativeManagedUnionMaybe values[2][2]={{native_managed_union(present,managed,1),native_managed_union(present,managed,2)},
                                          {native_managed_union(present,managed,3),native_managed_union(present,managed,4)}};
      assert(native_managed_union_value(values[1][1])==(present?4:-1));
      auto allocated=new NativeManagedUnionMaybe[2]{native_managed_union(present,managed,5),native_managed_union(present,managed,6)};
      assert(native_managed_union_value(allocated[1])==(present?6:-1));delete[] allocated;
      NativeManagedUnionOuter outer=__objc3_some(NativeManagedUnionMaybe,__objc3_copy(value));
      auto outerCopy=__objc3_copy(outer);assert(__objc3_has_value(outerCopy));
      NativeManagedUnionResult result=__objc3_ok(NativeManagedUnion,MixedCallStrong,NativeManagedUnion(165,managed));
      auto resultCopy=__objc3_copy(result);assert(__objc3_is_ok(resultCopy));
      NativeManagedUnionResult error=__objc3_err(NativeManagedUnion,MixedCallStrong,[MixedCallObject new]);
      auto errorCopy=__objc3_copy(error);assert(!__objc3_is_ok(errorCopy));
    }
    mixed_storage_assert_clean(12,present,managed);
  }
  for(int route=0;route!=3;++route) {
    @autoreleasepool {
      bool failed=false;
      try {
        if(route==0){auto value=__objc3_some(NativeManagedUnion,NativeManagedUnion(169,true,169));}
        else if(route==1){auto value=__objc3_ok(NativeManagedUnion,MixedCallStrong,NativeManagedUnion(169,true,169));}
        else {auto values=new NativeManagedUnionMaybe[2]{native_managed_union(true,true,168),__objc3_some(NativeManagedUnion,NativeManagedUnion(169,true,169))};delete[] values;}
      }catch(int error){assert(error==731);failed=true;}
      assert(failed);
    }
    mixed_storage_assert_clean(15,route);
  }
  for(int kind=0;kind!=3;++kind)for(int route=0;route!=7;++route)for(int fault=1;fault!=5;++fault) {
    @autoreleasepool {
      auto original=native_managed_union(true,true,166);
      carrier_record_copy_attempt=carrier_record_move_attempt=carrier_record_temp_attempt=0;
      carrier_record_fail_copy=kind==0?fault:0;carrier_record_fail_move=kind==1?fault:0;carrier_record_fail_temp=kind==2?fault:0;
      bool failed=false;
      try {
        if(route==0){auto value=__objc3_copy(original);}
        else if(route==1){auto value=__objc3_move(original);}
        else if(route==2){auto value=[object nativeUnionEcho:original];}
        else if(route==3){__unsafe_unretained AlignedBlockProbe literal=^int{return native_managed_union_value(original);};AlignedBlockProbe heap=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)literal);}
        else if(route==4){auto callback=[original]{return native_managed_union_value(original);};auto copy=callback;}
        else if(route==5){auto values=new NativeManagedUnionMaybe[2]{__objc3_copy(original),__objc3_copy(original)};delete[] values;}
        else {__block NativeManagedUnionMaybe cell=__objc3_copy(original);__unsafe_unretained AlignedBlockProbe literal=^int{return native_managed_union_value(cell);};AlignedBlockProbe heap=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)literal);}
      }catch(int error){assert(error==732 || error==733 || error==734);failed=true;}
      int attempts=kind==0?carrier_record_copy_attempt:kind==1?carrier_record_move_attempt:carrier_record_temp_attempt;
      assert(failed==(attempts>=fault));
      carrier_record_fail_copy=carrier_record_fail_move=carrier_record_fail_temp=0;
    }
    mixed_storage_assert_clean(13,kind,route,fault);
  }
  for(int mode=0;mode!=4;++mode)for(int route=0;route!=4;++route) {
    @autoreleasepool {
      checked_property_error=[NativeManagedUnionError new];bool typed=false,native=false;
      try {
        __objc3_do {
          if(route==0){auto value=__objc3_try(native_managed_union_checked(mode));assert(native_managed_union_value(value)==167);}
          else if(route==1){auto value=__objc3_try([object nativeUnionChecked:mode]);assert(native_managed_union_value(value)==167);}
          else if(route==2){auto value=__objc3_try_optional(native_managed_union_checked(mode));assert(__objc3_has_value(value)==(mode==0));}
          else __objc3_try(native_managed_union_checked(mode));
        }__objc3_catch(id<Error> error){assert(error==checked_property_error);typed=true;}
      }catch(int error){assert(error==758);native=true;}
      assert(native==(mode==3));assert(typed==((mode==1 || mode==2) && route!=2));
      checked_property_error=(id<Error>)0;
    }
    mixed_storage_assert_clean(14,mode,route);
  }
}
#else
static void native_union_carrier_payload_suite(PropertyMain *object){(void)object;}
#endif
#endif
