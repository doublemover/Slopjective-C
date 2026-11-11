#ifndef OBJC3_NATIVE_ARC_RECORD_PAYLOADS_H
#define OBJC3_NATIVE_ARC_RECORD_PAYLOADS_H
#if defined(__cplusplus) && __has_feature(objc_arc)
struct NativeARCRecord {MixedCallStrong object;MixedCallWeak observer;AlignedBlockProbe callback;};
struct NativeARCWeakRecord {MixedCallWeak observer;};
using NativeARCMaybe=__objc3_optional(NativeARCRecord);
using NativeARCWeakMaybe=__objc3_optional(NativeARCWeakRecord);
using NativeARCResult=__objc3_result(NativeARCRecord,MixedCallStrong);
static NativeARCMaybe native_arc_record(bool present,int seed) {
 if(!present)return __objc3_none(NativeARCRecord);
 MixedCallObject *object=[MixedCallObject new];
 return __objc3_some(NativeARCRecord,NativeARCRecord{object,object,^int{return seed;}});
}
static NativeARCMaybe native_arc_record_echo(NativeARCMaybe value){return __objc3_copy(value);}
static int native_arc_record_payload(const NativeARCMaybe &value) {
 return __objc3_match(value){case __objc3_case_some(let payload)=>payload.callback();case __objc3_case_none=>-1;};
}
struct NativeARCFailRecord {
 MixedCallStrong first;CarrierRecordLeaf last;
 explicit NativeARCFailRecord(int fail):first([MixedCallObject new]),last(151,fail){}
 NativeARCFailRecord(const NativeARCFailRecord &)=default;
 NativeARCFailRecord(NativeARCFailRecord &&)=default;
 ~NativeARCFailRecord() noexcept=default;
};
using NativeARCFailMaybe=__objc3_optional(NativeARCFailRecord);
@interface PropertyMain (NativeARCRecordPayload)
- (NativeARCMaybe)nativeRecordEcho:(NativeARCMaybe)value;
@end
@implementation PropertyMain (NativeARCRecordPayload)
- (NativeARCMaybe)nativeRecordEcho:(NativeARCMaybe)value{return __objc3_copy(value);}
@end
static void native_arc_record_payload_suite(PropertyMain *object) {
 for(int present=0;present!=2;++present) {
  @autoreleasepool {
   {
    auto value=native_arc_record(present,149);auto copied=__objc3_copy(value);auto moved=__objc3_move(copied);
    auto returned=native_arc_record_echo(moved);auto message=[object nativeRecordEcho:returned];
    NativeARCMaybe (^callback)(NativeARCMaybe)=^NativeARCMaybe(NativeARCMaybe input){return __objc3_copy(input);};
    auto blockValue=callback(message);assert(native_arc_record_payload(blockValue)==(present?149:-1));
    auto lambda=[blockValue]{return native_arc_record_payload(blockValue);};auto lambdaCopy=lambda;
    assert(lambdaCopy()==(present?149:-1));
    __unsafe_unretained AlignedBlockProbe literal=^int{return native_arc_record_payload(blockValue);};
    AlignedBlockProbe heap=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)literal);
    assert(heap()==(present?149:-1));heap=(AlignedBlockProbe)0;
    __block NativeARCMaybe byref=native_arc_record(present,150);
    __unsafe_unretained AlignedBlockProbe byrefLiteral=^int{return native_arc_record_payload(byref);};
    AlignedBlockProbe byrefHeap=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)byrefLiteral);
    assert(byrefHeap()==(present?150:-1));byrefHeap=(AlignedBlockProbe)0;
    NativeARCMaybe values[2][2]={{native_arc_record(present,1),native_arc_record(present,2)},
                                {native_arc_record(present,3),native_arc_record(present,4)}};
    assert(native_arc_record_payload(values[1][1])==(present?4:-1));
    auto allocated=new NativeARCMaybe[2]{native_arc_record(present,5),native_arc_record(present,6)};
    assert(native_arc_record_payload(allocated[1])==(present?6:-1));delete[] allocated;
   }
  }
  mixed_storage_assert_clean(7);
 }
 for(int route=0;route!=4;++route) {
  @autoreleasepool {
   bool caught=false;
   try {
    if(route==0){auto value=__objc3_some(NativeARCFailRecord,NativeARCFailRecord(151));}
    else {
     auto original=__objc3_some(NativeARCFailRecord,NativeARCFailRecord(0));
     carrier_record_copy_attempt=carrier_record_move_attempt=carrier_record_temp_attempt=0;
     if(route==1){carrier_record_fail_copy=1;auto value=__objc3_copy(original);}
     else if(route==2){carrier_record_fail_move=1;auto value=__objc3_move(original);}
     else {carrier_record_fail_temp=1;auto value=__objc3_copy(original);}
    }
   }catch(int error){assert(error==731+route);caught=true;}
   carrier_record_fail_copy=carrier_record_fail_move=carrier_record_fail_temp=0;assert(caught);
  }
  mixed_storage_assert_clean(8,0,route);
 }
 {
  MixedCallObject *owner=[MixedCallObject new];__weak MixedCallObject *observer=owner;
  auto value=__objc3_some(NativeARCWeakRecord,NativeARCWeakRecord{owner});
  auto copied=__objc3_copy(value);auto moved=__objc3_move(copied);owner=(MixedCallObject *)0;assert(!observer);
  bool zero=__objc3_match(moved){case __objc3_case_some(let payload)=>payload.observer==(id)0;case __objc3_case_none=>false;};
  assert(zero);
 }
 mixed_storage_assert_clean(9);
}
#else
static void native_arc_record_payload_suite(PropertyMain *object){(void)object;}
#endif
#endif
