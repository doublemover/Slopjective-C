__attribute__((objc_root_class))
@interface DarwinRegistrationRoot
+ (int)answer;
- (int)answer;
- (int)combined;
@end

struct class_record;
const struct class_record *objc3_darwin_class_v1(const char *name);
void *objc3_darwin_allocate_v1(const struct class_record *cls);
void objc3_darwin_deallocate_v1(void *instance);
