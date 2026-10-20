#ifdef __OBJC__
@import fixture.NamesLeft;
@import fixture.NamesRight;
#endif
int main(void) { return left_value() == 42 && right_value() == 84 ? 0 : 1; }
