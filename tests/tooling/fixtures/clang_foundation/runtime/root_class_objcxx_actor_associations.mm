#import <objc/runtime.h>
#include <assert.h>
#include <memory>

extern void *retain_object(void *) __asm__("objc_retain");
extern void release_object(void *) __asm__("objc_release");
static unsigned destroyed, associations_destroyed;
static char association_key;

__attribute__((objc_root_class))
@interface AssociationOwner { Class isa; }
+ (instancetype)new;
- (void)dealloc;
@end
@implementation AssociationOwner
+ (instancetype)new { return class_createInstance(self, 0); }
- (void)dealloc { ++destroyed; object_dispose(self); }
@end

struct AssociationLifetime {
  ~AssociationLifetime() { ++associations_destroyed; }
};

int main() {
  __weak AssociationOwner *weak = nil;
  @autoreleasepool {
    AssociationOwner *owner = [AssociationOwner new];
    weak = owner;
    void *object = (__bridge_retained void *)owner;
    auto lifetime = std::make_shared<AssociationLifetime>();
    // The association owns its capture but has no strong edge to the receiver.
    void (^association)() = ^{ (void)lifetime.get(); };
    objc_setAssociatedObject(owner, &association_key, association,
                             OBJC_ASSOCIATION_RETAIN);
    // Exercise the first ARC operation on the newly installed hidden class
    // before any ordinary message can initialize its dispatch table.
    assert(retain_object(object) == object);
    release_object(object);
    lifetime.reset();
    association = nil;
    owner = nil;
    assert(weak && !destroyed && !associations_destroyed);
    release_object(object);
  }
  assert(!weak && destroyed == 1 && associations_destroyed == 1);
}
