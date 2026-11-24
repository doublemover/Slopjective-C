__attribute__((objc_root_class))
@interface DarwinUnsupportedSignature
- (int)answer:(double)input;
@end

#if DARWIN_UNSUPPORTED_SEND
int unsupported_send(DarwinUnsupportedSignature *receiver) {
  return [receiver answer:1.0];
}
#else
@implementation DarwinUnsupportedSignature
- (int)answer:(double)input {
  return (int)input;
}
@end
#endif
