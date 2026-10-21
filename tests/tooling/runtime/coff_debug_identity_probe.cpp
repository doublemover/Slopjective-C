#include "io/objc3_object_format.h"

#include <filesystem>

int main(int argc, char **argv) {
  if (argc < 2) {
    return 2;
  }
  for (int index = 1; index < argc; ++index) {
    NormalizeObjectDeterminism(std::filesystem::path(argv[index]));
  }
  return 0;
}
