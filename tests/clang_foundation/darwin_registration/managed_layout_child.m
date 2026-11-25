#include "managed_layout_api.h"

// This translation unit cannot see the parent's private fields.
@implementation DarwinManagedChild {
  int _canary;
  id _added;
}
- (void)setAdded:(id)value {
  _canary = 25;
  _added = value;
}
- (id)added {
  return _canary == 25 ? _added : (id)0;
}
- (void)dealloc {
  compound_managed_event(_canary == 25 && _added ? 2 : -2);
}
@end
