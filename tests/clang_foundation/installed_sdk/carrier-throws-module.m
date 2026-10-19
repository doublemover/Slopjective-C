typedef Optional(id) CallStrong;
typedef Optional(id __weak) CallWeak;
@protocol Error @end
typedef CallStrong (^ThrowsBlock)(CallStrong, int, int) __objc3_throws(id<Error>);
id call_object(int key) __attribute__((ns_returns_retained));
int call_destroyed(int key);
CallStrong call_make(int key);
int throws_created(void);
int throws_destroyed(void);
CallStrong throws_forward(CallStrong value, int key, int fail) __objc3_throws(id<Error>);
ThrowsBlock throws_make_block(id capture);
@protocol ThrowsProtocol
- (CallStrong)value:(CallStrong)value key:(int)key fail:(int)fail __objc3_throws(id<Error>);
- (CallWeak)weak:(CallWeak)value fail:(int)fail __objc3_throws(id<Error>);
@end
__attribute__((objc_root_class))
@interface ThrowsCalls <ThrowsProtocol>
+ (id)new;
- (CallStrong)value:(CallStrong)value key:(int)key fail:(int)fail __objc3_throws(id<Error>);
- (CallWeak)weak:(CallWeak)value fail:(int)fail __objc3_throws(id<Error>);
@end
