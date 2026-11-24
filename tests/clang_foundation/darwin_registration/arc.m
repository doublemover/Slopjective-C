__attribute__((objc_root_class))
@interface ARCProbe
- (int)answer;
@end

@implementation ARCProbe
- (int)answer { return 42; }
@end

extern Class objc3_darwin_class_v1(const char *name);
extern id objc3_darwin_allocate_v1(Class cls) __attribute__((ns_returns_retained));
extern int objc3_darwin_check_arc_threads(Class cls);
extern int puts(const char *message);

__attribute__((noinline))
static ARCProbe *autoreleased_probe(Class cls) {
  return objc3_darwin_allocate_v1(cls);
}

int main(void) {
  Class cls = objc3_darwin_class_v1("ARCProbe");
  __weak ARCProbe *survives_inner_pool = (id)0;
  @autoreleasepool {
    ARCProbe *owned = autoreleased_probe(cls);
    @autoreleasepool {
      __weak ARCProbe *observer = owned;
      __weak ARCProbe *copy = observer;
      if ([copy answer] != 42)
        return 1;
      survives_inner_pool = observer;
    }
    owned = (id)0;
    if ([survives_inner_pool answer] != 42)
      return 2;
  }
  if (survives_inner_pool)
    return 3;

  __weak ARCProbe *released;
  @autoreleasepool {
    ARCProbe *one = objc3_darwin_allocate_v1(cls);
    ARCProbe *two = one;
    released = one;
    one = (id)0;
    if ([two answer] != 42)
      return 4;
    two = (id)0;
    if (released)
      return 5;
  }
  if (!objc3_darwin_check_arc_threads(cls))
    return 6;
  puts("owned Objective-C 3 Darwin ARC: PASS");
  return 0;
}
