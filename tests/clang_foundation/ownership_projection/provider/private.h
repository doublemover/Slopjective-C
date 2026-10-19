#ifdef __cplusplus
extern "C" {
#endif
id class_createInstance(Class type, unsigned long extra)
    __attribute__((ns_returns_retained));
id object_dispose(id object);
id objc_autorelease(id object);
void objc_release(id object);
#ifdef __cplusplus
}
#endif
__attribute__((objc_root_class))
@interface ProjectionObject {
  Class isa;
}
+ (id)new;
@end
