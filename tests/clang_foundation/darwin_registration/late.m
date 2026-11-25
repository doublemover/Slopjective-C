#include "late_api.h"

@interface DarwinRegistrationLate : DarwinRegistrationLateParent
+ (int)answer;
- (int)answer;
@end

@implementation DarwinRegistrationLate
+ (int)answer {
  return 7;
}

- (int)answer {
  if ([self store:19] != 19)
    return -1;
  return [super storedValue] + 4;
}
@end

int darwin_registration_late_answer(void) {
  return [DarwinRegistrationLate answer];
}

int darwin_registration_late_instance_answer(void) {
  DarwinRegistrationLate *instance =
      objc3_darwin_allocate_v1(objc3_darwin_class_v1("DarwinRegistrationLate"));
  if (!instance)
    return -1;
  int answer = [instance answer];
  objc3_darwin_deallocate_v1(instance);
  return answer;
}
