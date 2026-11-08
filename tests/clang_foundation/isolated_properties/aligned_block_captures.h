#ifndef OBJC3_NATIVE_ALIGNED_BLOCK_CAPTURES_H
#define OBJC3_NATIVE_ALIGNED_BLOCK_CAPTURES_H
#ifdef __cplusplus
static int aligned_block_live,aligned_block_created,aligned_block_destroyed;
static int aligned_block_copy_attempt,aligned_block_fail_copy;
struct alignas(128) AlignedBlockOwner {
 int value;
 explicit AlignedBlockOwner(int value):value(value){
  assert((__UINTPTR_TYPE__)this%alignof(AlignedBlockOwner)==0);
  ++aligned_block_live;++aligned_block_created;
 }
 AlignedBlockOwner(const AlignedBlockOwner &other):value(other.value){
  assert((__UINTPTR_TYPE__)this%alignof(AlignedBlockOwner)==0);
  if(++aligned_block_copy_attempt==aligned_block_fail_copy)throw 750;
  ++aligned_block_live;++aligned_block_created;
 }
 ~AlignedBlockOwner() noexcept{--aligned_block_live;++aligned_block_destroyed;assert(aligned_block_live>=0);}
};
struct alignas(64) AlignedBlockPlain {int value;};
using AlignedBlockMaybe=__objc3_optional(AlignedBlockOwner);
typedef int (^AlignedBlockProbe)(void);
#if __has_feature(objc_arc)
#define ALIGNED_BLOCK_COPY(value) ((__bridge_transfer AlignedBlockProbe)_Block_copy((__bridge const void *)(value)))
#define ALIGNED_BLOCK_POINTER(value) ((__bridge const void *)(value))
#define ALIGNED_BLOCK_RELEASE(value) ((value)=(AlignedBlockProbe)0)
#else
#define ALIGNED_BLOCK_COPY(value) ((AlignedBlockProbe)_Block_copy((const void *)(value)))
#define ALIGNED_BLOCK_POINTER(value) ((const void *)(value))
#define ALIGNED_BLOCK_RELEASE(value) (_Block_release((const void *)(value)))
#endif
static void aligned_block_assert_clean() {
 assert(!aligned_block_live && aligned_block_created==aligned_block_destroyed);
}
static void aligned_block_capture_suite() {
 for(int route=0;route!=3;++route)for(int fault=0;fault!=3;++fault) {
  bool caught=false;
  try {
   if(route==0) {
    AlignedBlockOwner value(81);
    if(fault==1)aligned_block_fail_copy=aligned_block_copy_attempt+1;
    __unsafe_unretained AlignedBlockProbe literal=^int{return value.value;};
    if(fault==2)aligned_block_fail_copy=aligned_block_copy_attempt+1;
    AlignedBlockProbe copied=ALIGNED_BLOCK_COPY(literal);
    assert(copied && (__UINTPTR_TYPE__)ALIGNED_BLOCK_POINTER(copied)%128==0 && copied()==81);
    int owners=aligned_block_live;
    AlignedBlockProbe retained=ALIGNED_BLOCK_COPY(copied);
    assert(ALIGNED_BLOCK_POINTER(retained)==ALIGNED_BLOCK_POINTER(copied) && aligned_block_live==owners);
    ALIGNED_BLOCK_RELEASE(retained);ALIGNED_BLOCK_RELEASE(copied);
   }else if(route==1) {
    auto value=__objc3_some(AlignedBlockOwner,AlignedBlockOwner(83));
    if(fault==1)aligned_block_fail_copy=aligned_block_copy_attempt+1;
    __unsafe_unretained AlignedBlockProbe literal=^int{return __objc3_has_value(value);};
    if(fault==2)aligned_block_fail_copy=aligned_block_copy_attempt+1;
    AlignedBlockProbe copied=ALIGNED_BLOCK_COPY(literal);
    assert(copied && (__UINTPTR_TYPE__)ALIGNED_BLOCK_POINTER(copied)%128==0 && copied()==1);
    ALIGNED_BLOCK_RELEASE(copied);
   }else {
    AlignedBlockPlain value{87};
    __unsafe_unretained AlignedBlockProbe literal=^int{return value.value;};
    AlignedBlockProbe copied=ALIGNED_BLOCK_COPY(literal);
    assert(copied && (__UINTPTR_TYPE__)ALIGNED_BLOCK_POINTER(copied)%64==0 && copied()==87);
    ALIGNED_BLOCK_RELEASE(copied);
   }
  }catch(int error){assert(error==750 && fault && route<2);caught=true;}
  aligned_block_fail_copy=0;assert(caught==(fault && route<2));aligned_block_assert_clean();
 }
 {
  auto empty=__objc3_none(AlignedBlockOwner);
  __unsafe_unretained AlignedBlockProbe literal=^int{return __objc3_has_value(empty);};
  AlignedBlockProbe copied=ALIGNED_BLOCK_COPY(literal);
  assert(copied && (__UINTPTR_TYPE__)ALIGNED_BLOCK_POINTER(copied)%128==0 && copied()==0);
  ALIGNED_BLOCK_RELEASE(copied);
 }
 aligned_block_assert_clean();
#if __has_feature(objc_arc)
 for(int some=0;some!=2;++some)for(int fault=0;fault!=3;++fault) {
  bool caught=false;
  @autoreleasepool {
   try {
    auto value=some?__objc3_ok(AlignedBlockOwner,MixedCallStrong,AlignedBlockOwner(89)):
        __objc3_err(AlignedBlockOwner,MixedCallStrong,[MixedCallObject new]);
    if(fault==1)aligned_block_fail_copy=aligned_block_copy_attempt+1;
    __unsafe_unretained AlignedBlockProbe literal=^int{return __objc3_is_ok(value);};
    if(fault==2)aligned_block_fail_copy=aligned_block_copy_attempt+1;
    AlignedBlockProbe copied=ALIGNED_BLOCK_COPY(literal);
    assert(copied && (__UINTPTR_TYPE__)ALIGNED_BLOCK_POINTER(copied)%128==0 && copied()==some);
    ALIGNED_BLOCK_RELEASE(copied);
   }catch(int error){assert(error==750 && fault && some);caught=true;}
  }
  aligned_block_fail_copy=0;assert(caught==bool(some && fault));aligned_block_assert_clean();
  assert(mixed_call_objects_created==mixed_call_objects_destroyed);
 }
#endif
}
#undef ALIGNED_BLOCK_COPY
#undef ALIGNED_BLOCK_POINTER
#undef ALIGNED_BLOCK_RELEASE
#else
static void aligned_block_capture_suite(void){}
#endif
#endif
