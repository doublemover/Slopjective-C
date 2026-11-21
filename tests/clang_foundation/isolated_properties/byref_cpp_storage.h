#ifndef OBJC3_NATIVE_BYREF_CPP_STORAGE_H
#define OBJC3_NATIVE_BYREF_CPP_STORAGE_H
#ifdef __cplusplus
static int byref_throw_live,byref_throw_created,byref_throw_destroyed;
static bool byref_throw_once;
struct alignas(64) ByrefThrowOwner {
 int value;explicit ByrefThrowOwner(int v):value(v){++byref_throw_live;++byref_throw_created;}
 ByrefThrowOwner(const ByrefThrowOwner &other) noexcept:value(other.value){++byref_throw_live;++byref_throw_created;}
 ~ByrefThrowOwner() noexcept(false){
  --byref_throw_live;++byref_throw_destroyed;
  if(byref_throw_once){byref_throw_once=false;throw 751;}
 }
};
static void *byref_throwing_factory() {
 __block ByrefThrowOwner value(131);
 __unsafe_unretained AlignedBlockProbe literal=^int{return value.value;};
#if __has_feature(objc_arc)
 return _Block_copy((__bridge const void *)literal);
#else
 return _Block_copy((const void *)literal);
#endif
}
#if __has_feature(objc_arc)
static __weak id byref_mixed_observed;
static int byref_mixed_created,byref_mixed_destroyed;
__attribute__((objc_root_class)) @interface ByrefMixedObject {Class isa;}
+ (instancetype)new;
@end
@implementation ByrefMixedObject
+ (instancetype)new {
 ByrefMixedObject *value=(ByrefMixedObject *)class_createInstance(self,0);
 ++byref_mixed_created;byref_mixed_observed=value;return value;
}
- (void)dealloc {++byref_mixed_destroyed;object_dispose(self);}
@end
using ByrefMixedStrong=ByrefMixedObject *__strong;
using ByrefMixedValue=__objc3_result(AlignedBlockOwner,ByrefMixedStrong);
static ByrefMixedValue byref_mixed_value(bool managed) {
 if(managed)return __objc3_err(AlignedBlockOwner,ByrefMixedStrong,[ByrefMixedObject new]);
 return __objc3_ok(AlignedBlockOwner,ByrefMixedStrong,AlignedBlockOwner(127));
}
struct ByrefMixedObservation {
 bool managed;
 ~ByrefMixedObservation() noexcept {
  if(managed)assert(byref_mixed_observed!=(id)0);
  else assert(!aligned_block_live);
 }
};
#endif
static void byref_cpp_storage_suite() {
 for(int route=0;route!=2;++route)for(int fault=0;fault!=2;++fault) {
  bool caught=false;
  {
   if(route==0) {
    __block AlignedBlockOwner value(111);
    __unsafe_unretained AlignedBlockProbe literal=^int {
     assert((__UINTPTR_TYPE__)&value%128==0);return value.value;
    };
    if(fault)aligned_block_fail_copy=aligned_block_copy_attempt+1;
    try {
#if __has_feature(objc_arc)
     AlignedBlockProbe copied=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)literal);
#else
     AlignedBlockProbe copied=(AlignedBlockProbe)_Block_copy((const void *)literal);
#endif
     assert(!fault && copied()==111);
#if __has_feature(objc_arc)
     copied=(AlignedBlockProbe)0;
#else
     _Block_release((const void *)copied);
#endif
    }catch(int error){assert(fault && error==750);caught=true;}
    aligned_block_fail_copy=0;assert(literal()==111);
   }else {
    __block AlignedBlockMaybe value=__objc3_some(AlignedBlockOwner,AlignedBlockOwner(113));
    __unsafe_unretained AlignedBlockProbe literal=^int {
     assert((__UINTPTR_TYPE__)&value%128==0);return __objc3_has_value(value);
    };
    if(fault)aligned_block_fail_copy=aligned_block_copy_attempt+1;
    try {
#if __has_feature(objc_arc)
     AlignedBlockProbe copied=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)literal);
#else
     AlignedBlockProbe copied=(AlignedBlockProbe)_Block_copy((const void *)literal);
#endif
     assert(!fault && copied()==1);
#if __has_feature(objc_arc)
     copied=(AlignedBlockProbe)0;
#else
     _Block_release((const void *)copied);
#endif
    }catch(int error){assert(fault && error==750);caught=true;}
    aligned_block_fail_copy=0;assert(literal()==1);
   }
  }
  assert(caught==bool(fault));aligned_block_assert_clean();
 }
 {
  __block CppValueMaybe value=cpp_value_source(true);
  int moves=cpp_value_moves;
  __unsafe_unretained AlignedBlockProbe literal=^int{return __objc3_has_value(value);};
#if __has_feature(objc_arc)
  AlignedBlockProbe copied=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)literal);
#else
  AlignedBlockProbe copied=(AlignedBlockProbe)_Block_copy((const void *)literal);
#endif
  assert(copied()==1 && cpp_value_moves==moves+1 && cpp_value_payload(value)==31);
#if __has_feature(objc_arc)
  copied=(AlignedBlockProbe)0;
#else
  _Block_release((const void *)copied);
#endif
 }
 {
  void *raw=byref_throwing_factory();assert(raw && byref_throw_live==1);
#if __has_feature(objc_arc)
  __weak AlignedBlockProbe weak=(__bridge AlignedBlockProbe)raw;
#endif
  byref_throw_once=true;bool caught=false;
  try{_Block_release(raw);}catch(int error){assert(error==751);caught=true;}
  assert(caught && !byref_throw_once && !byref_throw_live && byref_throw_created==byref_throw_destroyed);
#if __has_feature(objc_arc)
  assert(!weak);
#endif
 }
#if __has_feature(objc_arc)
 for(int managed=0;managed!=2;++managed) {
  @autoreleasepool {
   {
    __objc3_defer {(void)managed;}
    ByrefMixedObservation observation{bool(managed)};
    __block ByrefMixedValue value=byref_mixed_value(managed);
    __unsafe_unretained AlignedBlockProbe literal=^int {
     assert((__UINTPTR_TYPE__)&value%128==0);return __objc3_is_ok(value);
    };
    AlignedBlockProbe copied=(__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)literal);
    assert(copied()==!managed);copied=(AlignedBlockProbe)0;
   }
  }
  assert(!byref_mixed_observed && byref_mixed_created==byref_mixed_destroyed);aligned_block_assert_clean();
 }
#endif
}
#else
static void byref_cpp_storage_suite(void){}
#endif
#endif
