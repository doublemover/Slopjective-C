#include "clang/AST/ASTImporter.h"
#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/Decl.h"
#include "clang/AST/ObjC3KeyPath.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Lex/Lexer.h"
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

struct ApplicationRanges
    : clang::RecursiveASTVisitor<ApplicationRanges> {
  clang::ASTContext &context;
  unsigned seen[4] = {};
  explicit ApplicationRanges(clang::ASTContext &context) : context(context) {}

  bool VisitPseudoObjectExpr(clang::PseudoObjectExpr *expression) {
    clang::ObjC3KeyPathApplicationInfo info;
    if (!clang::getObjC3KeyPathApplication(expression, info)) return true;
    ++seen[info.Operation];
    const auto &source = context.getSourceManager();
    const auto range = info.Syntax->getSourceRange();
    assert(range.getBegin() == expression->getBeginLoc());
    assert(range.getEnd() == expression->getEndLoc());
    const auto text = clang::Lexer::getSourceText(
        clang::CharSourceRange::getTokenRange(range), source,
        context.getLangOpts());
    assert(text.starts_with(clang::getObjC3KeyPathOperationName(info.Operation)));
    assert(text.ends_with(")"));
    unsigned previous = source.getFileOffset(range.getBegin());
    for (const clang::Expr *operand : info.Syntax->arguments()) {
      const auto begin = source.getFileOffset(operand->getBeginLoc());
      const auto end = source.getFileOffset(operand->getEndLoc());
      assert(previous <= begin && begin <= end);
      assert(end <= source.getFileOffset(range.getEnd()));
      previous = end;
    }
    // Separate native captures own the semantic evaluation. Each operand has
    // exactly one binding, in the same order as the public syntactic call.
    assert(expression->getNumSemanticExprs() >= info.Syntax->getNumArgs());
    for (unsigned i = 0; i < info.Syntax->getNumArgs(); ++i) {
      auto *capture = llvm::dyn_cast<clang::OpaqueValueExpr>(
          expression->getSemanticExpr(i));
      assert(capture && capture->getSourceExpr());
    }
    return true;
  }

  bool VisitMemberExpr(clang::MemberExpr *member) {
    const clang::Expr *path = nullptr;
    if (clang::getObjC3KeyPathMember(member, path) ==
        clang::ObjC3KeyPathMemberKind::None) return true;
    assert(path);
    const auto &source = context.getSourceManager();
    assert(!source.isBeforeInTranslationUnit(member->getEndLoc(),
                                             member->getBeginLoc()));
    return true;
  }
};
}

int main() {
  const char *source = R"objc(
#include "runtime/public/objc3_keypath.h"
typedef struct ASTPathPair { long first, second; } ASTPathPair;
struct ASTLeft { int left; };
struct ASTRight { long right; };
struct ASTInherited : ASTLeft, ASTRight { unsigned bits:3; };
__attribute__((objc_root_class))
@interface ASTPathRoot
@property(nonatomic) int count;
@property(nonatomic, readonly) int identity;
@property(nonatomic) ASTPathPair pair;
@property(nonatomic) ASTInherited inherited;
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
WritableKeyPath<ASTPathRoot *, long> inherited_path(void) {
  return @keypath(ASTPathRoot, inherited.right);
}
WritableKeyPath<ASTPathRoot *, unsigned> bit_path(void) {
  return @keypath(ASTPathRoot, inherited.bits);
}
int apply_path(ASTPathRoot * _Nonnull root) {
  auto path = writable_path();
  KeyPathSet(path, root, 9);
  auto read = KeyPathAsReadOnly(path);
  (void)KeyPathMetadata(read);
  return KeyPathGet(read, root);
}
template<int Unused> int template_application(ASTPathRoot * _Nonnull root) {
  return KeyPathGet(writable_path(), root);
}
template int template_application<1>(ASTPathRoot * _Nonnull);
)objc";
  auto provider = parse(source, "keypath-provider.mm");
  auto consumer = parse("", "keypath-consumer.mm");
  assert(provider && consumer);
  assert(!provider->getDiagnostics().hasErrorOccurred());
  ApplicationRanges provider_ranges(provider->getASTContext());
  provider_ranges.TraverseDecl(function(*provider, "apply_path"));
  for (unsigned count : provider_ranges.seen) assert(count == 1);
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

  for (const char *name : {"inherited_path", "bit_path"}) {
    auto imported_record = importer.Import(function(*provider, name));
    assert(imported_record);
    auto *record_target = llvm::cast<clang::FunctionDecl>(*imported_record);
    clang::ObjC3KeyPathInfo record_info;
    assert(clang::getObjC3KeyPathInfo(record_target->getReturnType(), record_info));
    assert(record_info.Set && record_info.Value->isIntegerType());
    std::string record_printed;
    llvm::raw_string_ostream record_out(record_printed);
    record_target->print(record_out, policy);
    record_out.flush();
    assert(record_printed.find("@keypath(ASTPathRoot * _Nonnull, inherited.") !=
           std::string::npos);
    assert(record_printed.find("__objc3_keypath_get_") == std::string::npos);
  }

  auto imported_application = importer.Import(function(*provider, "apply_path"));
  assert(imported_application);
  auto *application = llvm::cast<clang::FunctionDecl>(*imported_application);
  ApplicationRanges imported_ranges(consumer->getASTContext());
  imported_ranges.TraverseDecl(application);
  for (unsigned count : imported_ranges.seen) assert(count == 1);
  std::string application_printed;
  llvm::raw_string_ostream application_out(application_printed);
  application->print(application_out, policy);
  application_out.flush();
  assert(application_printed.find("KeyPathSet(path, root, 9)") !=
         std::string::npos);
  assert(application_printed.find("KeyPathGet(read, root)") !=
         std::string::npos);
  assert(application_printed.find("__objc3_") == std::string::npos);
}
