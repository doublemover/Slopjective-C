typedef struct AvailabilityValue { int value; int old_field __attribute__((deprecated("old field"))); } AvailabilityValue;
struct HiddenAvailability { int private_value; };
#ifdef __cplusplus
class ImplementationOnly {
  int value;
public:
  ImplementationOnly() : value(7) {}
};
template<class T> T private_template(T value) { return value; }
#endif
int availability_hidden(void) { return 1; }
AvailabilityValue availability_value(void) {
  struct BodyOnly { int value; } local = {42};
  AvailabilityValue result = {local.value};
  return result;
}

int availability_old(void) __attribute__((deprecated("use availability_value", "availability_value")));
int availability_removed(void) __attribute__((unavailable("removed API")));
extern int availability_old_global __attribute__((deprecated("old global")));
typedef int AvailabilityOldAlias __attribute__((deprecated("old alias")));
enum AvailabilityChoice { availability_current_choice=0, availability_old_choice __attribute__((deprecated("old choice")))=1 };
int availability_platform(void) __attribute__((availability(macos,introduced=10.0,deprecated=11.0,obsoleted=12.0,replacement="availability_value",message="platform API")));
#ifdef __OBJC__
__attribute__((objc_root_class)) @interface AvailabilityRoot
- (int)oldMethod __attribute__((deprecated("old method")));
@property(readonly) int oldProperty __attribute__((deprecated("old property")));
@end
#endif
