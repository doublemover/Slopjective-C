#include <string>

#if defined(__OBJC__) || !defined(__cplusplus) || __has_feature(objc_arc)
#error expected plain C++
#endif
#if __has_extension(blocks) != defined(EXPECT_C_BLOCKS)
#error unexpected Blocks support in C++
#endif

extern "C" int cxx_value(void) {
  return static_cast<int>(std::string("hello").size());
}
