#ifndef OBJC3_DARWIN_FIXED_LAYOUT_API_H
#define OBJC3_DARWIN_FIXED_LAYOUT_API_H

Class objc3_darwin_class_v1(const char *name);
id objc3_darwin_allocate_v1(Class cls) __attribute__((ns_returns_retained));
void objc3_darwin_deallocate_v1(id object);

__attribute__((objc_root_class))
@interface DarwinFixedBase {
@protected
  int value;
}
- (int)readValue;
- (int)bump;
@end

@interface DarwinFixedChild : DarwinFixedBase
+ (int)layoutVersion;
- (int)bump;
- (int)readInheritedValue;
@end

#endif
