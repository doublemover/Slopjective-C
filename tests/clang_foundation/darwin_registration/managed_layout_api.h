#include <objc3_darwin.h>

__attribute__((objc_root_class))
@interface DarwinManagedRoot {
  Class isa;
}
- (void)dealloc;
@end

@interface DarwinManagedParent : DarwinManagedRoot
- (void)setInherited:(id)value;
- (id)inherited;
@end

@interface DarwinManagedChild : DarwinManagedParent
- (void)setAdded:(id)value;
- (id)added;
@end

@interface DarwinManagedValue : DarwinManagedRoot
- (void)setTag:(int)tag;
@end

void compound_managed_event(int value);
void managed_observe_owner(DarwinManagedChild *owner);
unsigned managed_event_count(void);
int managed_event_at(unsigned index);
