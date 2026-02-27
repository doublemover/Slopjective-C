__attribute__((objc_root_class))
@interface DispatchProbe
- (int)value;
@end

@implementation DispatchProbe
- (int)value {
  return 42;
}
@end

int main(void) {
  DispatchProbe *probe = ((DispatchProbe *)0);
  return [probe value];
}
