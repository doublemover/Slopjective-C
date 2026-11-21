#ifndef OBJC3_NATIVE_CARRIER_LAMBDA_STORAGE_H
#define OBJC3_NATIVE_CARRIER_LAMBDA_STORAGE_H
#if defined(__cplusplus) && __has_feature(objc_arc)
template<class T>static auto carrier_lambda_factory(const T &value) {
 return [value]<class U>(U count){return __objc3_is_ok(value)+count;};
}
static void carrier_lambda_storage_suite() {
 for(unsigned pattern:{0u,2u}) {
  @autoreleasepool {
   {
    auto value=mixed_storage_value(1,pattern);
    auto original=[value]{return __objc3_is_ok(value);};
    auto copied=original;auto moved=static_cast<decltype(copied) &&>(copied);
    auto generic=carrier_lambda_factory(value);
    auto mutableCapture=[value,count=0]() mutable {return __objc3_is_ok(value)+ ++count;};
    auto explicitCopy=[value=__objc3_copy(value)]{return __objc3_is_ok(value);};
    auto explicitMove=[value=__objc3_move(value)]{return __objc3_is_ok(value);};
    assert(moved()==!(pattern&2) && generic(3)==3+!(pattern&2));
    assert(mutableCapture()==1+!(pattern&2) && mutableCapture()==2+!(pattern&2));
    assert(explicitCopy()==!(pattern&2) && explicitMove()==!(pattern&2));
    auto heap=new decltype(original)(original);assert((*heap)()==!(pattern&2));delete heap;
    __unsafe_unretained AlignedBlockProbe literal=^int{return original();};
    AlignedBlockProbe heapBlock=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)literal);
    assert(heapBlock()==!(pattern&2));heapBlock=(AlignedBlockProbe)0;
   }
  }
  mixed_storage_assert_clean(4,pattern);
 }
 for(unsigned pattern:{0u,2u,10u})for(int route=0;route!=3;++route)for(int fault=0;fault<=4-!!(pattern&2)-!!(pattern&8);++fault) {
  @autoreleasepool {
   bool caught=false;
   try {
    MixedStorageValue values[2][2]={{mixed_storage_value(1,pattern),mixed_storage_value(2,pattern)},
                                   {mixed_storage_value(3,pattern),mixed_storage_value(4,pattern)}};
    if(route==0) {
     carrier_record_copy_attempt=0;carrier_record_fail_copy=fault;
     auto callback=[values]{return __objc3_is_ok(values[1][1]);};assert(callback());
    }else if(route==1) {
     auto callback=[values]{return __objc3_is_ok(values[1][1]);};
     carrier_record_copy_attempt=0;carrier_record_fail_copy=fault;
     auto copied=callback;assert(copied());
    }else {
     carrier_record_temp_attempt=0;carrier_record_fail_temp=fault;
     auto callback=[values]{return __objc3_is_ok(values[1][1]);};assert(callback());
    }
   }catch(int error){assert(error==(route==2?734:732));caught=true;}
   carrier_record_fail_copy=carrier_record_fail_temp=0;assert(caught==bool(fault));
  }
  mixed_storage_assert_clean(5,pattern,route,fault);
 }
 {
  MixedCallObject *owner=[MixedCallObject new];__weak MixedCallObject *observer=owner;
  auto weakValue=__objc3_err(CarrierRecordLeaf,MixedCallWeak,owner);
  auto callback=[weakValue]{return __objc3_is_ok(weakValue);};auto copied=callback;
  auto moved=static_cast<decltype(copied) &&>(copied);
  owner=(MixedCallObject *)0;assert(!observer && !moved());
 }
 mixed_storage_assert_clean(6);
}
#else
static void carrier_lambda_storage_suite(void){}
#endif
#endif
