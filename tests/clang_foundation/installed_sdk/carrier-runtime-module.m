typedef __objc3_optional(id) CallStrong;
typedef __objc3_optional(id __weak) CallWeak;
typedef CallStrong (^RuntimeStrongBlock)(CallStrong);
id call_object(int key) __attribute__((ns_returns_retained));
int call_destroyed(int key);
CallStrong call_make(int key);
RuntimeStrongBlock runtime_make_block(int key);
@protocol RuntimeCarrierProtocol
- (CallStrong)echo:(CallStrong)value;
- (CallWeak)echoWeak:(CallWeak)value;
@end
__attribute__((objc_root_class))
@interface RuntimeCalls <RuntimeCarrierProtocol> { Class isa; }
+ (instancetype)new;
- (CallStrong)echo:(CallStrong)value;
- (CallWeak)echoWeak:(CallWeak)value;
@end
