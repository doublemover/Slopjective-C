#include "llvm/Support/LLVMDriver.h"
int clang_scan_deps_main(int, char **, const llvm::ToolContext &);
int main(int argc, char **argv) {
  return clang_scan_deps_main(argc, argv, {argv[0], nullptr, false});
}
