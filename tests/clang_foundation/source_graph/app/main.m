#ifdef __OBJC__
@import fixture.Top;
#endif
int main(void) {
#if defined(__OBJC__) && defined(__cplusplus) && __has_feature(objc_arc)
  {
    auto owned = @fixture.Top.top_value();
    if (owned.marker != 42 || @fixture.Leaf.package_destroyed())
      return 10;
    auto copy = owned;
    owned.value = None(id);
    if (@fixture.Leaf.package_destroyed())
      return 11;
    copy.value = None(id);
    if (@fixture.Leaf.package_destroyed() != 1)
      return 12;
    match(owned.weak) {
    case .Some(let object): {
      if (object != (id)0)
        return 13;
    }
    case .None: {
      return 14;
    }
    }
  }
#endif
#ifdef __OBJC__
  @fixture.Leaf.LeafValue value = @fixture.Leaf.leaf();
  return @fixture.Top.answer() == 42 && value == 40 ? 0 : 1;
#else
  LeafValue value = leaf();
  return answer() == 42 && value == 40 ? 0 : 1;
#endif
}
