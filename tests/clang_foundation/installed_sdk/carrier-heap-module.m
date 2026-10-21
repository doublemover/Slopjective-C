typedef __objc3_optional(id) RecordStrong;
typedef __objc3_optional(id __weak) RecordWeak;
typedef __objc3_result(id, id __weak) RecordMixed;
typedef void (^RecordAction)(void);
typedef __objc3_optional(RecordAction) RecordBlock;
@protocol Error @end
struct RecordCall { int marker; RecordStrong value; RecordWeak weak; };
struct RecordPair { int marker; RecordStrong first; RecordWeak weak; RecordStrong values[2]; };
struct RecordNested { long before; struct RecordPair child; RecordMixed arm; };
#ifdef __cplusplus
extern "C" {
#endif
id record_object(int key) __attribute__((ns_returns_retained));
int record_destroyed(int key);
int record_destroyed_order(int index);
struct RecordCall record_make(int key);
struct RecordCall record_echo(struct RecordCall value);
void record_replace(struct RecordCall value, int key);
void record_native(struct RecordCall value);
#ifdef __cplusplus
}
#endif

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
