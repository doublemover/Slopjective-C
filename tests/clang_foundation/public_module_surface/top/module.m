#ifdef __OBJC__
@import fixture.SurfaceLeaf;
#endif
int surface_answer(void) { return surface_value().value; }
int implementation_helper(void) { return -1; }
