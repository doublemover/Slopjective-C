#include <dlfcn.h>
#include <stdio.h>

#if !defined(__OBJC3__) || __OBJC3__ != 1
#error Objective-C 3 language mode is required
#endif

#if defined(OBJC3_ASAN_WITNESS)
#if !__has_feature(address_sanitizer) || __has_feature(undefined_behavior_sanitizer)
#error the address witness requires standalone AddressSanitizer
#endif
#define RUNTIME_SYMBOL "__asan_init"
#elif defined(OBJC3_UBSAN_WITNESS)
#if !__has_feature(undefined_behavior_sanitizer) || __has_feature(address_sanitizer)
#error the undefined behavior witness requires standalone UndefinedBehaviorSanitizer
#endif
#define RUNTIME_SYMBOL "__ubsan_handle_add_overflow_abort"
#elif __has_feature(address_sanitizer) || __has_feature(undefined_behavior_sanitizer)
#error the plain witness requires uninstrumented compilation
#endif

struct class_record;
const struct class_record *objc3_darwin_class_v1(const char *name);
void *objc3_darwin_allocate_v1(const struct class_record *record);
void objc3_darwin_deallocate_v1(void *instance);

__attribute__((noinline))
int objc3_darwin_witness_sum(int left, int right) { return left + right; }

__attribute__((noinline))
int objc3_darwin_witness_load(const int *values, unsigned index) { return values[index]; }

__attribute__((objc_root_class))
@interface SanitizerWitness
+ (int)answer;
- (int)answer;
@end

@implementation SanitizerWitness
+ (int)answer {
  int values[] = {17, 25};
  return objc3_darwin_witness_sum(objc3_darwin_witness_load(values, 0),
                                 objc3_darwin_witness_load(values, 1));
}
- (int)answer { return 17; }
@end

int main(void) {
  const struct class_record *record = objc3_darwin_class_v1("SanitizerWitness");
  if (!record || [SanitizerWitness answer] != 42)
    return 1;
  SanitizerWitness *first = objc3_darwin_allocate_v1(record);
  if (!first)
    return 2;
  {
    defer { objc3_darwin_deallocate_v1(first); }
    SanitizerWitness *second = objc3_darwin_allocate_v1(record);
    if (!second)
      return 3;
    defer { objc3_darwin_deallocate_v1(second); }
    SanitizerWitness *empty = (SanitizerWitness *)0;
    if (first == second || [first answer] != 17 || [second answer] != 17 || [empty answer] != 0)
      return 4;
  }
  puts("owned Objective-C 3 Darwin sanitizer witness: PASS");
#ifdef RUNTIME_SYMBOL
  Dl_info library;
  void *symbol = dlsym(RTLD_DEFAULT, RUNTIME_SYMBOL);
  if (!symbol || !dladdr(symbol, &library) || !library.dli_fname)
    return 5;
  puts(library.dli_fname);
#endif
  return 0;
}
