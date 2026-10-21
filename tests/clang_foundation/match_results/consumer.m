@import fixture.MatchResults;
#include <assert.h>
#include <objc/runtime.h>
static int destroyed;
__attribute__((objc_root_class))
@interface InterfaceObject { Class isa; }
+ (instancetype)new;
@end
@implementation InterfaceObject
+ (instancetype)new { return class_createInstance(self, 0); }
- (void)dealloc { ++destroyed; }
@end
int main(void) {
  @autoreleasepool {
    id object = [InterfaceObject new];
    Optional(id) source = Some(id, object);
    Optional(id) result = @fixture.MatchResults.match_optional(source);
    Result(id, id) outcome = @fixture.MatchResults.match_result(Err(id, id, object));
    object = (id)0;
    source = None(id);
    assert(hasValue(result) && !isOk(outcome) && destroyed == 0);
    result = None(id);
    assert(destroyed == 0);
    outcome = Ok(id, id, (id)0);
  }
  assert(destroyed == 1);
  assert(!hasValue(@fixture.MatchResults.match_optional(None(id))));
  assert(isOk(@fixture.MatchResults.match_result(Ok(id, id, (id)0))));
  return 0;
}
