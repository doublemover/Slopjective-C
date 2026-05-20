#import <Foundation/Foundation.h>
#import "LegacyWidget-Swift.h"
#include "CppWidget.hpp"

@interface LegacyWidget : NSObject
@property(nonatomic, strong) NSString *name;
- (void)renderWithCppWidget:(CppWidget *)widget swiftActor:(SwiftActor *)actor;
@end

@implementation LegacyWidget
- (void)renderWithCppWidget:(CppWidget *)widget swiftActor:(SwiftActor *)actor {
  BOOL enabled = YES;
  id fallback = NULL;
  if (enabled && fallback == nil) {
    [actor renderWithWidget:widget];
  }
}
@end
