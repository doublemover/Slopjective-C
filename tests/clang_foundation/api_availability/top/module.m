#ifdef __OBJC__
@import fixture.AvailabilityLeaf;
#endif
int availability_answer(void) { return availability_value().value; }
int implementation_helper(void) { return -1; }
