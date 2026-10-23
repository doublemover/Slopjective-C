#include "clang_foundation/compile_session.h"
#include "clang/AST/ASTImporter.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ODRHash.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Error.h"
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
const char *Declarations = R"objc(
void use(int);
void invoke(void (^work)(void) __attribute__((noescape)));
void launch(void (^work)(void));
void close_handle(int) __attribute__((nothrow));
void close_throwing(int);
void close_arbitrary(int *);
)objc";
CompileRequest request(const fs::path &Root, const std::string &Source,
                       BaseLanguage Language, const char *Standard, bool ARC) {
  CompileRequest R;
  R.input_path = (Root / (Language == BaseLanguage::ObjectiveC
                             ? "captures.m" : "captures.mm")).string();
  R.source_text = std::string(Declarations) + Source;
  R.language = Language;
  R.objc3 = true;
  R.arc = ARC;
  R.profile = SemanticProfile::StrictSystem;
  R.objc_runtime = "gnustep-2.2";
  R.target_triple = "x86_64-unknown-linux-gnu";
  R.arguments = {std::string("-std=") + Standard, "-fblocks"};
  return R;
}
void expect(const CompileRequest &Request, bool Success,
            const std::string &Message = {}) {
  const auto Result = Compile(Request);
  if (Result.success != Success) {
    std::cerr << "unexpected capture result for:\n" << *Request.source_text << '\n';
    for (const auto &D : Result.diagnostics) std::cerr << D.message << '\n';
  }
  assert(Result.success == Success);
  if (!Message.empty()) {
    bool Found = false;
    for (const auto &D : Result.diagnostics)
      Found |= D.message.find(Message) != std::string::npos;
    assert(Found && "capture diagnostic must identify the violated obligation");
  }
}
void semantics(const fs::path &Root, BaseLanguage Language,
                 const char *Standard, bool ARC) {
  const std::vector<std::string> Accepted = {
      "void f(id source) { invoke(^[strong source] { (void)source; }); }",
      "void f(id source) { invoke(^[unowned source] { (void)source; }); }",
      "void f(id source) { invoke(^[move source] { (void)source; }); }",
      "int f(void) { int x=42; int (^b)(void)=^[move x]{return x;}; return b()+x; }",
      "int f(void) { __block int x=42; int (^b)(void)=^[x]{return x;}; x=9; return b(); }",
      "void f(void) { int strong=1,weak=2,unowned=3,move=4; invoke(^[strong,weak,unowned,move]{}); }",
      "void f(void) { int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{use(h);}); }",
      "void f(void) { int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{use(h);}); if(h!=-1)__builtin_trap(); reset(h,4); invoke(^[move h]{use(h);}); }",
  };
  for (const auto &Source : Accepted)
    expect(request(Root, Source, Language, Standard, ARC), true);
  expect(request(Root,
      "void f(id x){ invoke(^[weak x]{(void)x;}); }", Language, Standard, ARC),
      ARC, ARC ? "" : "requires automatic reference counting");
  const std::vector<std::pair<std::string, std::string>> Rejected = {
      {"void f(int x){invoke(^[weak x]{});}", "require an Objective-C object"},
      {"void f(int x){invoke(^[unowned x]{});}", "require an Objective-C object"},
      {"void f(id x){invoke(^[x,strong x]{});}", "appear only once"},
      {"void f(void){invoke(^[missing]{});}", "must name a local variable"},
      {"void f(void){const int x=3;invoke(^[move x]{});}", "requires a mutable"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[h]{});}", "must use the move modifier"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^{use(h);});}", "requires an explicit move"},
      {"void f(void){int h __attribute__((objc_resource(close=close_throwing,invalid=-1)))=3; invoke(^[move h]{});}", "statically nonthrowing cleanup"},
      {"void f(void){int h __attribute__((cleanup(close_arbitrary)))=3; invoke(^[move h]{});}", "arbitrary cleanup storage"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{use(h);});use(h);}", "may have been moved"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{use(h);});close_handle(h);}", "already"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{close_handle(h);});}", "cleanup"},
  };
  for (const auto &[Source, Message] : Rejected)
    expect(request(Root, Source, Language, Standard, ARC), false, Message);
  if (Language == BaseLanguage::ObjectiveCxx) {
    expect(request(Root,
        "struct R { int x; }; void f(void){R x{};invoke(^[move x]{});}",
        Language, Standard, ARC), false, "currently requires a scalar");
    expect(request(Root,
        "template<class T> int f(T x){auto b=^[move x]{return x;};return b()+x;} int g(){return f(42);}",
        Language, Standard, ARC), true);
  }
}
class Blocks : public clang::RecursiveASTVisitor<Blocks> {
public:
  std::vector<clang::BlockExpr *> Values;
  bool VisitBlockExpr(clang::BlockExpr *E) { Values.push_back(E); return true; }
};
clang::FunctionDecl *function(clang::ASTUnit &Unit, const char *Name) {
  for (auto *D : Unit.getASTContext().getTranslationUnitDecl()->decls())
    if (auto *F = llvm::dyn_cast<clang::FunctionDecl>(D))
      if (F->getName() == Name) return F;
  assert(false && "fixture function not found");
  return nullptr;
}
std::unique_ptr<clang::ASTUnit> parse(const std::string &Source,
                                    const fs::path &Path,
                                    const std::vector<std::string> &Extra = {}) {
  std::vector<std::string> Arguments{
      "-x", "objective-c++", "-std=c++20", "-fobjc-version=3", "-fblocks",
      "-fobjc-arc", "-fobjc-runtime=gnustep-2.2",
      "--target=x86_64-unknown-linux-gnu"};
  Arguments.insert(Arguments.end(), Extra.begin(), Extra.end());
  auto Unit = clang::tooling::buildASTFromCodeWithArgs(Source, Arguments, Path.string());
  assert(Unit && !Unit->getDiagnostics().hasErrorOccurred());
  return Unit;
}
void check_body(clang::FunctionDecl *Function) {
  Blocks Visitor;
  Visitor.TraverseStmt(Function->getBody());
  assert(Visitor.Values.size() == 2);
  auto *First = Visitor.Values[0]->getBlockDecl();
  assert(First->hasObjC3CaptureList() && First->captures().size() == 2);
  assert(First->captures()[0].getObjC3CaptureKind() ==
         clang::BlockDecl::ObjC3CaptureKind::Weak);
  assert(First->captures()[0].getCaptureType().getObjCLifetime() ==
         clang::Qualifiers::OCL_Weak);
  assert(First->captures()[1].getObjC3CaptureKind() ==
         clang::BlockDecl::ObjC3CaptureKind::Move);
  auto *Second = Visitor.Values[1]->getBlockDecl();
  assert(Second->captures().size() == 1);
  const auto &Resource = Second->captures()[0];
  assert(Resource.isObjC3ResourceCapture());
  assert(Resource.getVariable()->hasAttr<clang::ObjCResourceAttr>());
  assert(Resource.getCaptureType()->isIntegerType());
  assert(Resource.getObjC3CaptureRange().isValid());
  std::string Printed;
  llvm::raw_string_ostream Stream(Printed);
  Visitor.Values[0]->printPretty(Stream, nullptr,
      clang::PrintingPolicy(Function->getASTContext().getLangOpts()));
  Stream.flush();
  assert(Printed.find("^[weak object, move number]") != std::string::npos);
}
void persistence(const fs::path &Root) {
  const std::string Source = std::string(Declarations) + R"objc(
int capture_provider(id object) {
  int number=42;
  int (^first)(void)=^[weak object, move number]{return object?number:0;};
  int handle __attribute__((objc_resource(close=close_handle,invalid=-1)))=7;
  int (^second)(void)=^[move handle]{return handle;};
  return first()+second()+number;
}
)objc";
  const auto Header = Root / "capture-provider.h";
  { std::ofstream File(Header); File << Source; }
  auto From = parse(Source, Header);
  auto To = parse("", Root / "capture-consumer.mm");
  auto *Provider = function(*From, "capture_provider");
  check_body(Provider);
  clang::ASTImporter Importer(To->getASTContext(), To->getFileManager(),
      From->getASTContext(), From->getFileManager(), false);
  auto Imported = Importer.Import(Provider);
  if (!Imported) llvm::logAllUnhandledErrors(Imported.takeError(), llvm::errs());
  assert(Imported);
  check_body(llvm::cast<clang::FunctionDecl>(*Imported));
  auto ChangedSource = Source;
  const auto Position = ChangedSource.find("^[weak object, move number]");
  assert(Position != std::string::npos);
  ChangedSource.replace(Position, std::string("^[weak").size(), "^[strong");
  auto Changed = parse(ChangedSource, Root / "different-ownership.mm");
  clang::ODRHash Before, After;
  Before.AddFunctionDecl(Provider);
  After.AddFunctionDecl(function(*Changed, "capture_provider"));
  assert(Before.CalculateHash() != After.CalculateHash());
  const auto PCH = Root / "captures.pch";
  assert(!From->Save(PCH.string()));
  auto Reloaded = parse("int capture_client(id x){return capture_provider(x);}",
      Root / "capture-pch-client.mm", {"-include-pch", PCH.string()});
  check_body(function(*Reloaded, "capture_provider"));
}
} // namespace
int main() {
  const auto Root = fs::temp_directory_path() /
      ("objc3-explicit-captures-" + std::to_string(
          std::chrono::steady_clock::now().time_since_epoch().count()));
  fs::create_directories(Root);
  semantics(Root, BaseLanguage::ObjectiveC, "c17", false);
  semantics(Root, BaseLanguage::ObjectiveC, "c17", true);
  semantics(Root, BaseLanguage::ObjectiveCxx, "c++20", false);
  semantics(Root, BaseLanguage::ObjectiveCxx, "c++20", true);
  semantics(Root, BaseLanguage::ObjectiveCxx, "c++23", false);
  semantics(Root, BaseLanguage::ObjectiveCxx, "c++23", true);
  persistence(Root);
  fs::remove_all(Root);
}
