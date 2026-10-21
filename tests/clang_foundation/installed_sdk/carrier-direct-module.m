typedef __objc3_optional(id) CallStrong;
typedef __objc3_optional(id __weak) CallWeak;
@protocol Error @end
id call_object(int key) __attribute__((ns_returns_retained));
int call_destroyed(int key);
CallStrong call_make(int key);
int direct_initialized(void);
int throws_created(void);
int throws_destroyed(void);
__attribute__((objc_root_class))
@interface DirectCalls
+ (id)new;
+ (CallStrong)echo:(CallStrong)value fail:(int)fail __objc3_throws(id<Error>) __attribute__((objc_direct));
- (CallStrong)echo:(CallStrong)value key:(int)key fail:(int)fail __objc3_throws(id<Error>) __attribute__((objc_direct));
- (CallWeak)weak:(CallWeak)value fail:(int)fail __objc3_throws(id<Error>) __attribute__((objc_direct));
@end
