#ifdef __OBJC__
@import fixture.Leaf;
#endif
int left(void) {
#ifdef __OBJC__
  return @fixture.Leaf.leaf() + 1;
#else
  return leaf() + 1;
#endif
}

#if defined(__OBJC__) && defined(__cplusplus) && __has_feature(objc_arc)
@fixture.Leaf.PackageValue left_value(void) {
  return @fixture.Leaf.package_value();
}
#endif
