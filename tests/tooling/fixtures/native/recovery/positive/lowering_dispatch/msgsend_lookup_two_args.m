__attribute__((objc_root_class))
@interface DispatchMath
- (int)sum:(int)left with:(int)right;
@end

@implementation DispatchMath
- (int)sum:(int)left with:(int)right {
  return left + right;
}
@end

int main(void) {
  DispatchMath *math = ((DispatchMath *)0);
  return [math sum:7 with:5];
}
