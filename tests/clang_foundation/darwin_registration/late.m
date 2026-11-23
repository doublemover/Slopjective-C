#include "api.h"

__attribute__((objc_root_class))
@interface DarwinRegistrationLate
+ (int)answer;
@end

@implementation DarwinRegistrationLate
+ (int)answer {
  return 7;
}
@end

int darwin_registration_late_answer(void) {
  return [DarwinRegistrationLate answer];
}
