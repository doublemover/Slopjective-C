#if defined(__OBJC__) || defined(__cplusplus) || __has_feature(objc_arc)
#error expected plain C
#endif
#if __has_extension(blocks) != defined(EXPECT_C_BLOCKS)
#error unexpected Blocks support in C
#endif

int c_value(void) {
#ifdef EXPECT_C_BLOCKS
  int (^value)(void) = ^{ return 36; };
  return value();
#else
  return 36;
#endif
}
