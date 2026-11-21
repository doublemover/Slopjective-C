#ifndef OBJC3_NATIVE_ALIGNED_C_BLOCK_CAPTURES_H
#define OBJC3_NATIVE_ALIGNED_C_BLOCK_CAPTURES_H
#ifdef __cplusplus
extern "C" {
#endif
void *_Block_copy(const void *);
void _Block_release(const void *);
#ifdef __cplusplus
}
#endif
struct __attribute__((aligned(64))) AlignedCBlockValue {int value;};
typedef int (^AlignedCBlockProbe)(void);
static void aligned_c_block_capture_suite(int seed) {
 struct AlignedCBlockValue value={seed};
 __unsafe_unretained AlignedCBlockProbe literal=^int{return value.value;};
#if __has_feature(objc_arc)
 AlignedCBlockProbe copied=(__bridge_transfer AlignedCBlockProbe)_Block_copy((__bridge const void *)literal);
 const void *pointer=(__bridge const void *)copied;
#else
 AlignedCBlockProbe copied=(AlignedCBlockProbe)_Block_copy((const void *)literal);
 const void *pointer=(const void *)copied;
#endif
 assert(copied && (__UINTPTR_TYPE__)pointer%64==0 && copied()==seed);
#if __has_feature(objc_arc)
 copied=(AlignedCBlockProbe)0;
#else
 _Block_release((const void *)copied);
#endif
}
#endif
