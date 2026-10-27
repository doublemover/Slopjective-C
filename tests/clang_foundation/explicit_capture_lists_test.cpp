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
borrowed const char *capture_bytes(id owner)
  __attribute__((objc_returns_borrowed(owner_index=0)));
void capture_use_bytes(borrowed const char *);
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
    if (!Found) {
      std::cerr << "expected capture diagnostic containing: " << Message
                << "\nfor:\n" << *Request.source_text << '\n';
      for (const auto &D : Result.diagnostics)
        std::cerr << D.message << '\n';
    }
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
      "void f(void){Optional(int) v=Some(int,42);invoke(^[move v]{(void)v;});}",
      "void f(int *p){Optional(int*) v=Some(int*,p);invoke(^[move v]{(void)v;});}",
      "void f(void (*p)(void)){Optional(void(*)(void)) v=Some(void(*)(void),p);invoke(^[move v]{(void)v;});}",
      "void f(void){Optional(Optional(int)) v=Some(Optional(int),None(int));invoke(^[move v]{(void)v;});}",
      "void f(void){__objc3_result(int,int) v=__objc3_err(int,int,42);invoke(^[move v]{(void)v;});}",
      "int f(void) { __block int x=42; int (^b)(void)=^[x]{return x;}; x=9; return b(); }",
      "void f(void) { int strong=1,weak=2,unowned=3,move=4; invoke(^[strong,weak,unowned,move]{}); }",
      "void f(id o) { const char *p=capture_bytes(o); invoke(^[p]{capture_use_bytes(p);}); }",
      "void f(id o) { const char *p=capture_bytes(o); invoke(^[move p]{capture_use_bytes(p);}); }",
      "void f(id o) { const char *p=capture_bytes(o); invoke(^[move p]{capture_use_bytes(p);}); launch(^{capture_use_bytes(p);}); }",
      "void f(void) { int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{use(h);}); }",
      "void f(void) { int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{use(h);}); if(h!=-1)__builtin_trap(); reset(h,4); invoke(^[move h]{use(h);}); }",
      "void f(void) { int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{reset(h,4);discard(h);}); }",
      "void f(void) { int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{int v=take(h);close_handle(v);}); }",
      "void f(void) { int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{defer {discard(h);} use(h);}); }",
      "void f(void) { int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{invoke(^[move h]{use(h);});if(h!=-1)__builtin_trap();reset(h,4);discard(h);}); }",
  };
  for (const auto &Source : Accepted)
    expect(request(Root, Source, Language, Standard, ARC), true);
  if (ARC) {
    expect(request(Root,
        "void f(id x){Optional(id) v=Some(id,x);invoke(^[move v]{(void)v;});}",
        Language, Standard, ARC), true);
    expect(request(Root,
        "void f(id x){Optional(id __weak) v=Some(id __weak,x);invoke(^[move v]{(void)v;});}",
        Language, Standard, ARC), true);
    expect(request(Root,
        "void f(id x){Optional(id const) v=Some(id const,x);invoke(^[move v]{});}",
        Language, Standard, ARC), false, "mutable, nonresource payloads");
  }
  expect(request(Root,
      "void f(id x){ invoke(^[weak x]{(void)x;}); }", Language, Standard, ARC),
      ARC, ARC ? "" : "requires automatic reference counting");
  const std::vector<std::pair<std::string, std::string>> Rejected = {
      {"void f(int x){invoke(^[weak x]{});}", "require an Objective-C object"},
      {"void f(int x){invoke(^[unowned x]{});}", "require an Objective-C object"},
      {"void f(id x){invoke(^[x,strong x]{});}", "appear only once"},
      {"void f(void){invoke(^[missing]{});}", "must name a local variable"},
      {"void f(void){const int x=3;invoke(^[move x]{});}", "requires a mutable"},
      {"void f(void){Optional(const int) x=None(const int);invoke(^[move x]{});}", "mutable, nonresource payloads"},
      {"void f(void){Optional(volatile int) x=None(volatile int);invoke(^[move x]{});}", "mutable, nonresource payloads"},
      {"void f(void){__block int x=3;invoke(^[x]{invoke(^[move x]{});});}", "immutable enclosing capture"},
      {"void f(id o){const char *p=capture_bytes(o);launch(^[p]{capture_use_bytes(p);});}", "borrowed pointer escapes"},
      {"void f(id o){const char *p=capture_bytes(o);launch(^[move p]{capture_use_bytes(p);});}", "borrowed pointer escapes"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[h]{});}", "must use the move modifier"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^{use(h);});}", "requires an explicit move"},
      {"void f(void){int h __attribute__((objc_resource(close=close_throwing,invalid=-1)))=3; invoke(^[move h]{});}", "statically nonthrowing cleanup"},
      {"void f(void){int h __attribute__((cleanup(close_arbitrary)))=3; invoke(^[move h]{});}", "arbitrary cleanup storage"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{use(h);});use(h);}", "may have been moved"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{use(h);});close_handle(h);}", "manual close of registered resource"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{close_handle(h);});}", "manual close of registered resource"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{discard(h);use(h);});}", "may have been moved"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{int a=take(h);close_handle(a);int b=take(h);close_handle(b);});}", "may have been moved"},
      {"void f(void){int h __attribute__((objc_resource(close=close_handle,invalid=-1)))=3; invoke(^[move h]{invoke(^[move h]{use(h);});use(h);});}", "may have been moved"},
  };
  for (const auto &[Source, Message] : Rejected)
    expect(request(Root, Source, Language, Standard, ARC), false, Message);
  if (Language == BaseLanguage::ObjectiveC) {
    expect(request(Root,
        "struct R{int values[2];int *p;};void f(){struct R r={{1,2},0};Optional(struct R) x=Some(struct R,r);invoke(^[move x]{});}",
        Language, Standard, ARC), true);
    expect(request(Root,
        "struct R{const int x;};void f(){struct R r={1};Optional(struct R) x=Some(struct R,r);invoke(^[move x]{});}",
        Language, Standard, ARC), false, "mutable, nonresource payloads");
  }
  if (Language == BaseLanguage::ObjectiveCxx) {
    expect(request(Root,
        "struct R { int x; R()=default; R(const R&)=default; R(R&&v) noexcept:x(v.x){v.x=-1;} }; void f(void){R x{};invoke(^[move x]{});}",
        Language, Standard, ARC), true);
    expect(request(Root,
        "struct R { R()=default; R(const R&)=delete; R(R&&) noexcept{} }; void f(void){R x{};invoke(^[move x]{});}",
        Language, Standard, ARC), false, "deleted");
    expect(request(Root,
        "struct R{int value;R(int x):value(x){} R(const R&)=default;R(R&&x):value(x.value){x.value=-1;} ~R(){}};void f(){Optional(R) x=Some(R,R(3));invoke(^[move x]{});}",
        Language, Standard, ARC), true);
    expect(request(Root,
        "struct R{R()=default;R(const R&)=delete;R(R&&)=default;};void f(){Optional(R) x=Some(R,R());invoke(^[move x]{});}",
        Language, Standard, ARC), false, "deleted");
    if (ARC) {
      expect(request(Root,
          "struct R{R()=default;R(const R&)=default;R(R&&)=default;~R(){}};void f(id o){__objc3_result(id,R) x=__objc3_ok(id,R,o);invoke(^[move x]{});}",
          Language, Standard, ARC), true);
      expect(request(Root,
          "struct R{R()=default;R(const R&)=delete;R(R&&)=default;};void f(id o){__objc3_result(id,R) x=__objc3_ok(id,R,o);invoke(^[move x]{});}",
          Language, Standard, ARC), false, "deleted");
    }
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
void check_record_body(clang::FunctionDecl *Function) {
  Blocks Visitor;
  Visitor.TraverseStmt(Function->getBody());
  assert(Visitor.Values.size() == 1);
  const auto &Capture = Visitor.Values[0]->getBlockDecl()->captures()[0];
  assert(Capture.getObjC3CaptureKind() == clang::BlockDecl::ObjC3CaptureKind::Move);
  assert(Capture.getObjC3InitExpr() && Capture.getCopyExpr());
  assert(Capture.getObjC3InitExpr() != Capture.getCopyExpr());
}
void check_carrier_body(clang::FunctionDecl *Function) {
  check_record_body(Function);
  Blocks Visitor;
  Visitor.TraverseStmt(Function->getBody());
  const auto &Capture = Visitor.Values[0]->getBlockDecl()->captures()[0];
  const auto *Move = llvm::cast<clang::ObjC3CarrierTransferExpr>(
      Capture.getObjC3InitExpr()->IgnoreImplicit());
  const auto *Copy = llvm::cast<clang::ObjC3CarrierTransferExpr>(
      Capture.getCopyExpr()->IgnoreImplicit());
  assert(Move->isMove() && !Copy->isMove());
  assert(!Move->getSource()->getType().isConstQualified());
  assert(Copy->getSource()->getType().isConstQualified());
  assert(llvm::isa<clang::OpaqueValueExpr>(Move->getSource()));
  assert(llvm::isa<clang::OpaqueValueExpr>(Copy->getSource()));
}
void check_implicit_carrier_body(clang::FunctionDecl *Function) {
  Blocks Visitor;
  Visitor.TraverseStmt(Function->getBody());
  assert(Visitor.Values.size() == 1);
  auto *Block = Visitor.Values[0]->getBlockDecl();
  assert(!Block->hasObjC3CaptureList() && Block->captures().size() == 1);
  const auto &Capture = Block->captures()[0];
  assert(!Capture.getObjC3InitExpr() && Capture.getCopyExpr());
  const auto *Copy = llvm::cast<clang::ObjC3CarrierTransferExpr>(
      Capture.getCopyExpr()->IgnoreImplicit());
  assert(!Copy->isMove() && Copy->getSource()->getType().isConstQualified());
}
void check_nested_resource_body(clang::FunctionDecl *Function) {
  Blocks Visitor;
  Visitor.TraverseStmt(Function->getBody());
  assert(Visitor.Values.size() == 2);
  const auto &Outer = Visitor.Values[0]->getBlockDecl()->captures()[0];
  const auto &Inner = Visitor.Values[1]->getBlockDecl()->captures()[0];
  assert(Outer.isObjC3ResourceCapture() && Inner.isObjC3ResourceCapture());
  assert(Outer.getVariable() == Inner.getVariable());
  assert(!Outer.isNested() && Inner.isNested());
  assert(Inner.getObjC3CaptureKind() == clang::BlockDecl::ObjC3CaptureKind::Move);
  assert(Inner.getCaptureType()->isIntegerType());
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
struct CaptureRecord {
  int value;
  explicit CaptureRecord(int value): value(value) {}
  CaptureRecord(const CaptureRecord &other): value(other.value) {}
  CaptureRecord(CaptureRecord &&other) noexcept: value(other.value) {
    other.value=-1;
  }
};
int capture_record_provider(void) {
  CaptureRecord source(3);
  auto block=^[move source]{return source.value;};
  return block()+source.value;
}
int capture_carrier_provider(void) {
  Optional(CaptureRecord) source=Some(CaptureRecord,CaptureRecord(5));
  auto block=^[move source]{return __objc3_has_value(source);};
  return block();
}
int capture_mixed_provider(id value) {
  __objc3_result(id,CaptureRecord) source=__objc3_ok(id,CaptureRecord,value);
  auto block=^[move source]{return (int)__objc3_is_ok(source);};
  return block();
}
int capture_implicit_mixed_provider(id value) {
  __objc3_result(id,CaptureRecord) source=__objc3_ok(id,CaptureRecord,value);
  auto block=^{return (int)__objc3_is_ok(source);};
  return block();
}
void capture_nested_resource_provider(void) {
  int handle __attribute__((objc_resource(close=close_handle,invalid=-1)))=31;
  void (^outer)(void)=^[move handle]{
    void (^inner)(void)=^[move handle]{use(handle);};
    inner();
    if(handle!=-1)__builtin_trap();
  };
  outer();
}
)objc";
  const auto Header = Root / "capture-provider.h";
  { std::ofstream File(Header); File << Source; }
  auto From = parse(Source, Header);
  auto To = parse("", Root / "capture-consumer.mm");
  auto *Provider = function(*From, "capture_provider");
  check_body(Provider);
  check_record_body(function(*From, "capture_record_provider"));
  check_carrier_body(function(*From, "capture_carrier_provider"));
  check_carrier_body(function(*From, "capture_mixed_provider"));
  check_implicit_carrier_body(function(*From, "capture_implicit_mixed_provider"));
  check_nested_resource_body(function(*From, "capture_nested_resource_provider"));
  clang::ASTImporter Importer(To->getASTContext(), To->getFileManager(),
      From->getASTContext(), From->getFileManager(), false);
  auto Imported = Importer.Import(Provider);
  if (!Imported) llvm::logAllUnhandledErrors(Imported.takeError(), llvm::errs());
  assert(Imported);
  check_body(llvm::cast<clang::FunctionDecl>(*Imported));
  auto ImportedRecord = Importer.Import(function(*From, "capture_record_provider"));
  assert(ImportedRecord);
  check_record_body(llvm::cast<clang::FunctionDecl>(*ImportedRecord));
  auto ImportedCarrier = Importer.Import(function(*From, "capture_carrier_provider"));
  assert(ImportedCarrier);
  check_carrier_body(llvm::cast<clang::FunctionDecl>(*ImportedCarrier));
  auto ImportedMixed = Importer.Import(function(*From, "capture_mixed_provider"));
  assert(ImportedMixed);
  check_carrier_body(llvm::cast<clang::FunctionDecl>(*ImportedMixed));
  auto ImportedImplicit = Importer.Import(function(*From, "capture_implicit_mixed_provider"));
  assert(ImportedImplicit);
  check_implicit_carrier_body(llvm::cast<clang::FunctionDecl>(*ImportedImplicit));
  auto ImportedNested = Importer.Import(function(*From, "capture_nested_resource_provider"));
  assert(ImportedNested);
  check_nested_resource_body(llvm::cast<clang::FunctionDecl>(*ImportedNested));
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
  check_record_body(function(*Reloaded, "capture_record_provider"));
  check_carrier_body(function(*Reloaded, "capture_carrier_provider"));
  check_carrier_body(function(*Reloaded, "capture_mixed_provider"));
  check_implicit_carrier_body(function(*Reloaded, "capture_implicit_mixed_provider"));
  check_nested_resource_body(function(*Reloaded, "capture_nested_resource_provider"));
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
