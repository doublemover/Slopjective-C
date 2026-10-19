typedef Optional(id) CallStrong;
typedef Optional(id __weak) CallWeak;
typedef Optional(CallStrong) NestedStrong;
typedef Optional(CallWeak) NestedWeak;
typedef Result(CallStrong, CallWeak) NestedMixed;
@protocol Error @end
id call_object(int key) __attribute__((ns_returns_retained));
int call_destroyed(int key);
int throws_created(void);
int throws_destroyed(void);
NestedStrong nested_make(int key);
NestedStrong nested_identity(NestedStrong value);
int nested_state(NestedStrong value);
int nested_weak_state(NestedWeak value);
NestedStrong nested_forward(NestedStrong value, int key, int fail) throws(id<Error>);
__attribute__((objc_root_class))
@interface NestedCalls
+ (id)new;
- (NestedStrong)echo:(NestedStrong)value;
- (NestedStrong)direct:(NestedStrong)value __attribute__((objc_direct));
- (NestedStrong)throwing:(NestedStrong)value key:(int)key fail:(int)fail throws(id<Error>);
@end
