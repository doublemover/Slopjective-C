#import "UnsafeWidget-Swift.h"

@interface UnsafeLegacyWidget : NSObject
- (void)loadUnsafeBridge;
@end

@implementation UnsafeLegacyWidget
- (void)loadUnsafeBridge {
  int enabled = 1;
  void *image = dlopen("UnsafeSwiftCppBridge.dylib", 0);
  id cls = NSClassFromString(@"UnsafeRuntimeName");
  (void)image;
  (void)cls;
}
@end
