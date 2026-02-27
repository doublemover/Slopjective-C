// Baseline parity fixture: minimal Objective-C message dispatch shape.
#import <objc/NSObject.h>

@interface Probe : NSObject
- (int)value;
@end

@implementation Probe
- (int)value {
  return 7;
}
@end
