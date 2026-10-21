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
  @autoreleasepool {
    Optional(id) source = Some(id, [InterfaceObject new]);
    assert(!hasValue(@fixture.MatchResults.match_mutable(source)) && hasValue(source));
    id selected = @fixture.MatchResults.match_object(source);
    assert(selected != (id)0);
    int (^block)(void) = @fixture.MatchResults.match_block(source);
    source = None(id);
    selected = (id)0;
    assert(block() == 42);
    block = (int (^)(void))0;
  }
  assert(destroyed == 2);
  assert(@fixture.MatchResults.match_object(None(id)) == (id)0);
  assert(@fixture.MatchResults.match_scalar(13) == 17);
  return 0;
}
