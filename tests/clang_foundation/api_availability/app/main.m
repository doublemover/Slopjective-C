#ifdef __OBJC__
@import fixture.AvailabilityTop;
#endif
int main(void) {
#ifdef __OBJC__
  @fixture.AvailabilityLeaf.AvailabilityValue value = @fixture.AvailabilityLeaf.availability_value();
  return @fixture.AvailabilityTop.availability_answer() == 42 && value.value == 42 ? 0 : 1;
#else
  AvailabilityValue value = availability_value();
  return availability_answer() == 42 && value.value == 42 ? 0 : 1;
#endif
}
