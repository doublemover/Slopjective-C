typedef Optional(id) CallStrong;
typedef Optional(id __weak) CallWeak;
typedef Optional(CallStrong) NestedStrong;
@protocol Error @end
id call_object(int key) __attribute__((ns_returns_retained));
int call_destroyed(int key);
int throws_created(void);
int throws_destroyed(void);
typedef int (^CapturedState)(void);
typedef NestedStrong (^CapturedValue)(void);
typedef NestedStrong (^CapturedThrowing)(int, int) throws(id<Error>);
CapturedState capture_strong(int key);
CapturedState capture_weak(id object);
CapturedState capture_binding(int key);
CapturedValue capture_value(int key);
CapturedThrowing capture_throwing(int key);
