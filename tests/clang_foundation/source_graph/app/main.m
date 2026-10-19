@import fixture.Top;
int main(void) {
  @fixture.Leaf.LeafValue value = @fixture.Leaf.leaf();
  return @fixture.Top.answer() == 42 && value == 40 ? 0 : 1;
}
