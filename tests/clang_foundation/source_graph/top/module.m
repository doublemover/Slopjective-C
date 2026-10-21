#ifdef __OBJC__
@import fixture.Left;
@import fixture.Right;
#endif
int answer(void) {
#ifdef __OBJC__
  return @fixture.Left.left() + @fixture.Right.right() - 41;
#else
  return left() + right() - 41;
#endif
}

#if defined(__OBJC__) && defined(__cplusplus) && __has_feature(objc_arc)
@fixture.Leaf.PackageValue top_value(void) {
  return @fixture.Left.left_value();
}
#endif
