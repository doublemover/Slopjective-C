#include "carrier-heap-call.h"
#if CASE == 1
__attribute__((objc_root_class))
@interface AtomicCarrier { Class isa; }
@property RecordStrong value;
@end
@implementation AtomicCarrier @end
#elif CASE == 2
__attribute__((objc_root_class))
@interface AtomicRecord { Class isa; }
@property struct RecordPair value;
@end
@implementation AtomicRecord @end
#elif CASE == 3
void rejected(void) { __block RecordStrong values[2]={record_value(0)}; (void)values; }
#elif CASE == 4
struct __attribute__((packed)) PackedRecord { char c; RecordStrong value; };
void rejected(void) { struct PackedRecord value={0,record_value(0)}; void (^block)(void)=^{(void)value.c;}; block(); }
#elif CASE == 5
typedef struct RecordPair (^ThrowingRecord)(void) __objc3_throws(id<Error>);
void rejected(ThrowingRecord block) { (void)__objc3_try_force(block()); }
#elif CASE == 6
__attribute__((objc_root_class))
@interface VolatileCarrier { Class isa; volatile RecordStrong value; }
@end
@implementation VolatileCarrier @end
#endif
