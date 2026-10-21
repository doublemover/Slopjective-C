#ifdef __OBJC__
@import fixture.SurfaceTop;
#endif
int main(void) {
#ifdef __OBJC__
  @fixture.SurfaceLeaf.SurfaceValue value = @fixture.SurfaceLeaf.surface_value();
  return @fixture.SurfaceTop.surface_answer() == 42 && value.value == 42 ? 0 : 1;
#else
  SurfaceValue value = surface_value();
  return surface_answer() == 42 && value.value == 42 ? 0 : 1;
#endif
}
