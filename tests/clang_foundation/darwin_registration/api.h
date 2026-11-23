__attribute__((objc_root_class))
@interface DarwinRegistrationRoot
+ (int)answer;
@end

struct class_record;
const struct class_record *objc3_darwin_class_v1(const char *name);
