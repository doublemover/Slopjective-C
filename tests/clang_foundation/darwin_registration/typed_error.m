#include <objc3_darwin.h>

extern int puts(const char *);

__attribute__((objc_non_runtime_protocol))
@protocol Error
@end

static int cleanup_count;
static int destroy_count;

__attribute__((objc_root_class))
@interface DarwinErrorRoot {
  Class isa;
}
+ (instancetype)alloc;
+ (instancetype)new;
- (instancetype)init;
@end

@implementation DarwinErrorRoot
+ (instancetype)alloc {
  return (__bridge_transfer id)objc3_darwin_allocate_v1((__bridge const objc3_darwin_class *)self);
}
+ (instancetype)new {
  return [[self alloc] init];
}
- (instancetype)init {
  return self;
}
@end

@interface DarwinIssue : DarwinErrorRoot <Error>
@property(nonatomic, readonly) int code;
@end

@implementation DarwinIssue
- (int)code {
  return 29;
}
- (void)dealloc {
  ++destroy_count;
}
@end

@interface DarwinDetailedIssue : DarwinIssue
@end
@implementation DarwinDetailedIssue
@end

@interface DarwinCheckedProcessor : DarwinErrorRoot
- (int)parse:(int)valid throws(DarwinIssue *);
@end

@implementation DarwinCheckedProcessor
- (int)parse:(int)valid throws(DarwinIssue *) {
  defer { ++cleanup_count; }
  if (!valid) {
    DarwinIssue *error = [DarwinDetailedIssue new];
    throwError(error);
  }
  return 42;
}
@end

static int checked_call(DarwinCheckedProcessor *processor, int valid) throws(DarwinIssue *) {
  defer { ++cleanup_count; }
  return try [processor parse:valid];
}

int main(void) {
  DarwinCheckedProcessor *processor = [DarwinCheckedProcessor new];
  __weak DarwinIssue *observer = (id)0;
  int result = -1;
  @autoreleasepool {
    handle {
      result = tryValue(checked_call(processor, 1));
    } catch (DarwinIssue *error) {
      return error.code;
    }
    if (result != 42 || cleanup_count != 2 || destroy_count)
      return 1;
    handle {
      result = tryValue(checked_call(processor, 0));
      return 2;
    } catch (DarwinIssue *error) {
      observer = error;
      if (error.code != 29 || cleanup_count != 4 || destroy_count)
        return 3;
    }
    if (!observer || destroy_count)
      return 4;
    handle {
      result = tryValue(checked_call((id)0, 1));
    } catch (DarwinIssue *error) {
      return error.code;
    }
    if (result || cleanup_count != 5 || destroy_count)
      return 5;
  }
  if (observer || destroy_count != 1)
    return 6;
  puts("owned Objective-C 3 Darwin typed errors: PASS");
  return 0;
}
