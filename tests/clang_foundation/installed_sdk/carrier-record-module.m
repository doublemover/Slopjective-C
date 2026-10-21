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
