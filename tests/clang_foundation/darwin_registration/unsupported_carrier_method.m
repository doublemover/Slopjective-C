typedef __objc3_optional(int) Maybe;

__attribute__((objc_root_class))
@interface DarwinUnsupportedCarrierMethod
- (int)answer:(Maybe)input;
@end

#if DARWIN_UNSUPPORTED_SEND
int unsupported_carrier_method_send(DarwinUnsupportedCarrierMethod *receiver) {
  return [receiver answer:__objc3_some(int, 1)];
}
#else
@implementation DarwinUnsupportedCarrierMethod
- (int)answer:(Maybe)input {
  return __objc3_has_value(input);
}
@end
#endif
