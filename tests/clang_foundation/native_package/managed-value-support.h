// Private implementation included by provider.m; the installed interface
// exports an opaque id and its ownership contract, rather than this class.
extern id class_createInstance(Class cls, __SIZE_TYPE__ extra);
extern Class objc_getClass(const char *name);
extern id object_dispose(id value);
extern id objc_autorelease(id value);
extern void *objc_autoreleasePoolPush(void);
extern void objc_autoreleasePoolPop(void *token);
static int ManagedDeallocations;
__attribute__((objc_root_class))
@interface PackageManagedValue <Error> {
  Class isa;
  int references;
}
- (void)initializeManaged;
- (id)retain;
- (oneway void)release;
- (id)autorelease;
@end
@implementation PackageManagedValue
- (void)initializeManaged { references = 1; }
- (id)retain { ++references; return self; }
- (oneway void)release {
  if (--references == 0) { ++ManagedDeallocations; object_dispose(self); }
}
- (id)autorelease {
  void *token = objc_autoreleasePoolPush();
  objc_autoreleasePoolPop(token);
  return objc_autorelease(self);
}
@end
static id create_managed_value(void) {
  PackageManagedValue *value = class_createInstance(objc_getClass("PackageManagedValue"), 0);
  [value initializeManaged];
  return value;
}
