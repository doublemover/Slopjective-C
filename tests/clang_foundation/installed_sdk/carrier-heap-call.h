#pragma once
#include "carrier-record-call.h"
typedef int (^StorageState)(int);
typedef struct RecordPair (^HeapRecordCall)(struct RecordPair);
__attribute__((objc_root_class))
@interface StorageBox { Class isa;
@public RecordStrong _strong; RecordWeak _weak; struct RecordPair _pair; RecordStrong _array[2];
}
+ (id)new;
@property(nonatomic) RecordStrong strong;
@property(nonatomic) RecordWeak weak;
@property(nonatomic) struct RecordPair pair;
- (struct RecordPair)echo:(struct RecordPair)value;
@end
__attribute__((objc_root_class))
@interface PropertyFailure { Class isa; RecordStrong _value; struct RecordPair _pair; }
+ (id)new;
@property(nonatomic) RecordStrong value;
@property(nonatomic) struct RecordPair pair;
@end
#ifdef __cplusplus
extern "C" {
#endif
StorageState heap_capture_record(struct RecordPair value);
StorageState heap_capture_shared(int key);
StorageState heap_capture_shared_record(int key);
int heap_weak_reads(void);
HeapRecordCall heap_record_block(void);
struct RecordPair heap_invoke_block(HeapRecordCall block, struct RecordPair value);
#ifdef __cplusplus
}
#endif
