// Deliberate compiler-process fault. This is not a CTest expected-success case
// and never emits or executes client code; the qualifier owns both outcomes.
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>
#include <string>

extern "C" __attribute__((noinline)) void objc3_asan_detector(int index) {
  volatile int *values = new int[4];
  values[index] = 42;
  delete[] values;
}

namespace {
class CompilerConsumer final : public clang::ASTConsumer {
  bool Bad;
  bool &Parsed;
public:
  CompilerConsumer(bool bad, bool &parsed) : Bad(bad), Parsed(parsed) {}
  __attribute__((noinline)) void HandleTranslationUnit(clang::ASTContext &context) override {
    for (const auto *declaration : context.getTranslationUnitDecl()->decls()) {
      const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
      if (!function || function->getName() != "compiler_fixture" || !function->hasBody()) continue;
      if (!context.getLangOpts().ObjC3) return;
      Parsed = true;
      llvm::errs() << "objc3:parsed-ast-detector\n";
      llvm::errs().flush();
      objc3_asan_detector(Bad ? 16 : 0);
    }
  }
};
class CompilerAction final : public clang::ASTFrontendAction {
  bool Bad;
  bool &Parsed;
public:
  CompilerAction(bool bad, bool &parsed) : Bad(bad), Parsed(parsed) {}
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &, llvm::StringRef) override {
    return std::make_unique<CompilerConsumer>(Bad, Parsed);
  }
};
}

int main(int argc, char **argv) {
  if (argc != 2 || (std::string(argv[1]) != "good" && std::string(argv[1]) != "bad")) return 2;
  bool parsed = false;
  const bool compiled = clang::tooling::runToolOnCodeWithArgs(
      std::make_unique<CompilerAction>(std::string(argv[1]) == "bad", parsed),
      "int compiler_fixture(int value) { defer { ++value; } return value; }",
      {"-x", "objective-c++", "-std=c++20", "-fobjc-version=3", "-fobjc-runtime=gnustep-2.2"},
      "compiler-process-input.mm");
  return compiled && parsed ? 0 : 3;
}
