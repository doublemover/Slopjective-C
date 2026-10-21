#include "carrier-record-call.h"
#if CASE == 1
union RecordUnion { RecordStrong value; long other; };
void rejected(void) { union RecordUnion value = {record_value(0)}; (void)value; }
#elif CASE == 2
struct __attribute__((packed)) RecordPacked { char c; RecordStrong value; };
void rejected(void) { struct RecordPacked value = {0,record_value(0)}; (void)value; }
#elif CASE == 3
__attribute__((objc_root_class))
@interface AtomicRecordValue { Class isa; }
@property struct RecordCall value;
@end
@implementation AtomicRecordValue @end
#elif CASE == 4
void takes_variadic(int first,...);
void rejected(void) { takes_variadic(0,record_make(0)); }
#elif CASE == 5
struct RecordCall rejected(void) __objc3_throws(id<Error>) { return record_make(0); }
#endif
