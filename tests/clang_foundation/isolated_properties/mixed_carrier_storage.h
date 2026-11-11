#ifndef OBJC3_NATIVE_MIXED_CARRIER_STORAGE_H
#define OBJC3_NATIVE_MIXED_CARRIER_STORAGE_H
#include <stdio.h>
#if defined(__cplusplus) && __has_feature(objc_arc)
using MixedStorageValue=__objc3_result(CarrierRecordLeaf,MixedCallStrong);
using MixedStorageWeak=__objc3_result(CarrierRecordLeaf,MixedCallWeak);
using MixedStorageMaybe=__objc3_optional(MixedStorageValue);
static MixedStorageValue mixed_storage_value(int index,unsigned managed,int fail=0) {
 if(managed&(1u<<index))return __objc3_err(CarrierRecordLeaf,MixedCallStrong,[MixedCallObject new]);
 return __objc3_ok(CarrierRecordLeaf,MixedCallStrong,CarrierRecordLeaf(index,fail));
}
struct MixedStorageRecord {
 MixedStorageValue first,values[2][2];
 explicit MixedStorageRecord(unsigned managed,int fail=0):first(mixed_storage_value(1,managed,fail)),
  values{{mixed_storage_value(2,managed,fail),mixed_storage_value(3,managed,fail)},
         {mixed_storage_value(4,managed,fail),mixed_storage_value(5,managed,fail)}} {}
 MixedStorageRecord(const MixedStorageRecord &)=default;
 MixedStorageRecord(MixedStorageRecord &&)=default;
 ~MixedStorageRecord() noexcept=default;
};
struct MixedStorageAggregate {MixedStorageValue values[2];};
struct MixedStorageDerived:MixedStorageRecord {
 MixedStorageValue last;
 explicit MixedStorageDerived(unsigned managed):MixedStorageRecord(managed),last(mixed_storage_value(6,managed)){}
};
static MixedStorageRecord mixed_storage_echo(MixedStorageRecord value){return value;}
using MixedStorageOuter=__objc3_optional(MixedStorageRecord);
using MixedStorageOuterResult=__objc3_result(MixedStorageRecord,MixedCallStrong);
static MixedStorageOuter mixed_storage_outer(bool present,unsigned managed) {
 if(present)return __objc3_some(MixedStorageRecord,MixedStorageRecord(managed));
 return __objc3_none(MixedStorageRecord);
}
static MixedStorageOuter mixed_storage_outer_echo(MixedStorageOuter value){return __objc3_copy(value);}
static MixedStorageOuterResult mixed_storage_outer_result(bool native,unsigned managed) {
 if(native)return __objc3_ok(MixedStorageRecord,MixedCallStrong,MixedStorageRecord(managed));
 return __objc3_err(MixedStorageRecord,MixedCallStrong,[MixedCallObject new]);
}

static void mixed_storage_assert_clean(int stage=0,unsigned pattern=0,int route=0,int fault=0) {
 if(carrier_record_live || carrier_record_created!=carrier_record_destroyed)fprintf(stderr,"mixed storage stage=%d pattern=%u route=%d fault=%d live=%d created=%d destroyed=%d\n",stage,pattern,route,fault,carrier_record_live,carrier_record_created,carrier_record_destroyed);
 carrier_record_assert_clean();assert(mixed_call_objects_created==mixed_call_objects_destroyed);
}
@interface PropertyMain (MixedStoragePayload)
- (MixedStorageOuter)mixedStorageEcho:(MixedStorageOuter)value;
@end
@implementation PropertyMain (MixedStoragePayload)
- (MixedStorageOuter)mixedStorageEcho:(MixedStorageOuter)value{return __objc3_copy(value);}
@end
static void mixed_carrier_storage_suite(PropertyMain *object) {
 for(unsigned managed:{0u,0x2au,0x54u,0x7eu}) {
  @autoreleasepool {
   {
    MixedStorageRecord original(managed);MixedStorageRecord copied(original);
    MixedStorageRecord moved(static_cast<MixedStorageRecord &&>(copied));
    auto returned=mixed_storage_echo(original);
    assert(__objc3_is_ok(returned.first)==!(managed&2));
    assert(__objc3_is_ok(moved.values[1][1])==!(managed&32));
    MixedStorageAggregate aggregate{{mixed_storage_value(1,managed),mixed_storage_value(2,managed)}};
    MixedStorageAggregate aggregateCopy(aggregate),aggregateMove(static_cast<MixedStorageAggregate &&>(aggregateCopy));
    assert(__objc3_is_ok(aggregateMove.values[1])==!(managed&4));
    MixedStorageDerived derived(managed),derivedCopy(derived),derivedMove(static_cast<MixedStorageDerived &&>(derivedCopy));
    assert(__objc3_is_ok(derivedMove.last)==!(managed&64));
    __unsafe_unretained AlignedBlockProbe literal=^int{return __objc3_is_ok(original.values[1][0]);};
    AlignedBlockProbe copiedBlock=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)literal);
    assert(copiedBlock()==!(managed&16));copiedBlock=(AlignedBlockProbe)0;
    for(int present=0;present!=2;++present) {
     auto outer=mixed_storage_outer(present,managed);
     auto outerCopy=__objc3_copy(outer);auto outerMove=__objc3_move(outerCopy);
     auto returnedOuter=mixed_storage_outer_echo(outerMove);
     assert(__objc3_has_value(returnedOuter)==bool(present));
     auto messageOuter=[object mixedStorageEcho:outerMove];
     assert(__objc3_has_value(messageOuter)==bool(present));
     MixedStorageOuter (^callback)(MixedStorageOuter)=^MixedStorageOuter(MixedStorageOuter input){return __objc3_copy(input);};
     auto blockOuter=callback(outerMove);assert(__objc3_has_value(blockOuter)==bool(present));
     __unsafe_unretained AlignedBlockProbe outerLiteral=^int{return __objc3_has_value(outerMove);};
     AlignedBlockProbe heapOuter=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)outerLiteral);
     assert(heapOuter()==present);heapOuter=(AlignedBlockProbe)0;

     auto outerResult=mixed_storage_outer_result(present,managed);
     auto outerResultCopy=__objc3_copy(outerResult);auto outerResultMove=__objc3_move(outerResultCopy);
     assert(__objc3_is_ok(outerResultMove)==bool(present));
    }
    MixedStorageMaybe empty[2][2]={};
    assert(!__objc3_has_value(empty[0][0]) && !__objc3_has_value(empty[1][1]));
   }
  }
  mixed_storage_assert_clean(1,managed);
  int cppCount=0;for(int i=1;i<=5;++i)cppCount+=!(managed&(1u<<i));
  for(int route=0;route!=6;++route)for(int fail=1;fail<=cppCount;++fail) {
   @autoreleasepool {
    bool caught=false;
    try {
     if(route<3) {
      MixedStorageRecord original(managed);
      carrier_record_copy_attempt=carrier_record_move_attempt=carrier_record_temp_attempt=0;
      if(route==0){carrier_record_fail_copy=fail;MixedStorageRecord value(original);}
      else if(route==1){carrier_record_fail_move=fail;MixedStorageRecord value(static_cast<MixedStorageRecord &&>(original));}
      else {carrier_record_fail_temp=fail;MixedStorageRecord value(original);}
     }else {
      auto original=mixed_storage_outer(true,managed);
      carrier_record_copy_attempt=carrier_record_move_attempt=carrier_record_temp_attempt=0;
      if(route==3){carrier_record_fail_copy=fail;auto value=__objc3_copy(original);}
      else if(route==4){carrier_record_fail_move=fail;auto value=__objc3_move(original);}
      else {carrier_record_fail_temp=fail;auto value=__objc3_copy(original);}
     }
    }catch(int error){assert(error==(route%3==0?732:route%3==1?733:734));caught=true;}
    carrier_record_fail_copy=carrier_record_fail_move=carrier_record_fail_temp=0;
    assert(caught);
   }
   mixed_storage_assert_clean(2,managed,route,fail);
  }
 }
 for(int route=0;route!=3;++route)for(int fail=0;fail<=6;++fail) {
  @autoreleasepool {
   bool caught=false;
   try {
    if(route==0) {
     MixedStorageValue values[2][3]={{mixed_storage_value(1,0,fail),mixed_storage_value(2,0,fail),mixed_storage_value(3,0,fail)},
       {mixed_storage_value(4,0,fail),mixed_storage_value(5,0,fail),mixed_storage_value(6,0,fail)}};
     assert(__objc3_is_ok(values[1][2]));
    }else if(route==1) {
     MixedStorageRecord *value=new MixedStorageRecord(0,fail);delete value;
    }else {
     MixedStorageRecord *values=new MixedStorageRecord[2]{MixedStorageRecord(0),MixedStorageRecord(0,fail)};delete[] values;
    }
   }catch(int error){assert(error==731);caught=true;}
   assert(caught==bool(fail && (route==0 || fail<=5)));
  }
  mixed_storage_assert_clean(3,0,route,fail);
 }
 {
  MixedCallObject *owner=[MixedCallObject new];__weak MixedCallObject *observer=owner;
  struct WeakRecord {MixedStorageWeak values[2];};
  WeakRecord original{{__objc3_err(CarrierRecordLeaf,MixedCallWeak,owner),__objc3_err(CarrierRecordLeaf,MixedCallWeak,owner)}};
  WeakRecord copied(original),moved(static_cast<WeakRecord &&>(copied));
  owner=(MixedCallObject *)0;assert(!observer && !__objc3_is_ok(moved.values[1]));
 }
 mixed_storage_assert_clean();
}
#else
static void mixed_carrier_storage_suite(PropertyMain *object){(void)object;}
#endif
#endif
