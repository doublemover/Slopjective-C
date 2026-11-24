#if DARWIN_UNSUPPORTED_ARITY
#define PARAMETERS (int)first offset:(int)second extra:(int)third
#define ARGUMENTS 1 offset:2 extra:3
#define RESULT first + second + third
#else
#define PARAMETERS (int)first offset:(double)second
#define ARGUMENTS 1 offset:2.0
#define RESULT first + (int)second
#endif

__attribute__((objc_root_class))
@interface DarwinUnsupportedIntMethod
- (int)answer:PARAMETERS;
@end

#if DARWIN_UNSUPPORTED_SEND
int unsupported_int_method_send(DarwinUnsupportedIntMethod *receiver) {
  return [receiver answer:ARGUMENTS];
}
#else
@implementation DarwinUnsupportedIntMethod
- (int)answer:PARAMETERS {
  return RESULT;
}
@end
#endif
