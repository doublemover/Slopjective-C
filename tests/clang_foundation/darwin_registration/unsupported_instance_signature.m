__attribute__((objc_root_class))
@interface DarwinUnsupportedSignature
- (int)answer:(int)input, ...;
@end

#if DARWIN_UNSUPPORTED_SEND
int unsupported_send(DarwinUnsupportedSignature *receiver) {
  return [receiver answer:1, 2];
}
#else
@implementation DarwinUnsupportedSignature
- (int)answer:(int)input, ... {
  return input;
}
@end
#endif
