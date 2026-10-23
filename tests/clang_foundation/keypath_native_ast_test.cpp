#include "clang/AST/ASTImporter.h"
#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/Decl.h"
#include "clang/AST/ObjC3KeyPath.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace {
std::unique_ptr<clang::ASTUnit> parse(llvm::StringRef code,
                                     llvm::StringRef file) {
  const std::vector<std::string> args{
      "-x", "objective-c++", "-std=c++20", "-fobjc-version=3",
      "-fobjc-runtime=gnustep-2.2", "-fobjc-arc",
      "-I" OBJC3_KEYPATH_TEST_REPO_ROOT "/native/objc3c/src"};
  return clang::tooling::buildASTFromCodeWithArgs(code, args, file);
}
clang::FunctionDecl *function(clang::ASTUnit &unit, llvm::StringRef name) {
  for (auto *decl : unit.getASTContext().getTranslationUnitDecl()->decls())
    if (auto *fn = llvm::dyn_cast<clang::FunctionDecl>(decl))
      if (fn->getName() == name) return fn;
  assert(false && "fixture function not found");
  return nullptr;
}
}

int main() {
  const char *source = R"objc(
#include "runtime/public/objc3_keypath.h"
typedef struct ASTPathPair { long first, second; } ASTPathPair;
__attribute__((objc_root_class))
@interface ASTPathRoot
@property(nonatomic) int count;
@property(nonatomic, readonly) int identity;
@property(nonatomic) ASTPathPair pair;
@end
WritableKeyPath<ASTPathRoot *, int> writable_path(void) {
  return @keypath(ASTPathRoot, count);
}
KeyPath<ASTPathRoot *, int> readonly_path(void) {
  return @keypath(ASTPathRoot, identity);
}
WritableKeyPath<ASTPathRoot *, long> field_path(void) {
  return @keypath(ASTPathRoot, pair.first);
}
)objc";
  auto provider = parse(source, "keypath-provider.mm");
  auto consumer = parse("", "keypath-consumer.mm");
  assert(provider && consumer);
  assert(!provider->getDiagnostics().hasErrorOccurred());
  auto *writable = function(*provider, "writable_path");
  auto *readonly = function(*provider, "readonly_path");
  clang::ObjC3KeyPathInfo before, read;
  assert(clang::getObjC3KeyPathInfo(writable->getReturnType(), before));
  assert(before.Read && before.Set && before.Value->isIntegerType());
  assert(clang::getObjC3KeyPathInfo(readonly->getReturnType(), read));
  assert(!read.Read && !read.Set);
  clang::PrintingPolicy policy(provider->getLangOpts());
  const auto signature = writable->getReturnType().getAsString(policy);
  assert(signature.find("WritableKeyPath<ASTPathRoot * _Nonnull, int>") !=
         std::string::npos);

  // Native AST import includes the typed accessors, immutable metadata, and
  // record attributes. Recognition must not depend on a producer Sema table.
  clang::ASTImporter importer(consumer->getASTContext(), consumer->getFileManager(),
      provider->getASTContext(), provider->getFileManager(), false);
  auto imported = importer.Import(writable);
  assert(imported);
  auto *target = llvm::cast<clang::FunctionDecl>(*imported);
  clang::ObjC3KeyPathInfo after;
  assert(clang::getObjC3KeyPathInfo(target->getReturnType(), after));
  assert(after.Read && after.Set && after.Value->isIntegerType());
  assert(target->getReturnType().getAsString(policy) == signature);
  std::string printed;
  llvm::raw_string_ostream out(printed);
  target->print(out, policy);
  out.flush();
  assert(printed.find("@keypath(ASTPathRoot * _Nonnull, count)") !=
         std::string::npos);
  assert(printed.find("__objc3_keypath_get_") == std::string::npos);
  auto imported_field = importer.Import(function(*provider, "field_path"));
  assert(imported_field);
  auto *field_target = llvm::cast<clang::FunctionDecl>(*imported_field);
  clang::ObjC3KeyPathInfo field_info;
  assert(clang::getObjC3KeyPathInfo(field_target->getReturnType(), field_info));
  assert(field_info.Set && field_info.Value->isIntegerType());
  std::string field_printed;
  llvm::raw_string_ostream field_out(field_printed);
  field_target->print(field_out, policy);
  field_out.flush();
  assert(field_printed.find("@keypath(ASTPathRoot * _Nonnull, pair.first)") !=
         std::string::npos);
}
