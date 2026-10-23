#include "clang_foundation/compile_session.h"
#include "clang/AST/ASTImporter.h"
#include "clang/AST/Decl.h"
#include "clang/AST/ObjC3Borrowed.h"
#include "clang/AST/ODRHash.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/Error.h"
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
const std::string Header = R"(
typedef borrowed const char *BorrowedBytes;
BorrowedBytes bytes(id owner) __attribute__((objc_returns_borrowed(owner_index=0)));
void use(borrowed const char *pointer);
void use_noescape(const char *pointer __attribute__((noescape)));
void escape(const char *pointer);
void invoke(void (^work)(void) __attribute__((noescape)));
void launch(void (^work)(void));
id make_owner(void);
const char *global_pointer;
void (^global_block)(void);
struct Holder { const char *pointer; };
)";
CompileRequest Request(const fs::path &Root, const std::string &Source,
                       BaseLanguage Language, const std::string &Standard,
                       bool ARC) {
  CompileRequest R;
  R.input_path = (Root / (Language == BaseLanguage::ObjectiveC ? "input.m" : "input.mm")).string();
  R.source_text = Source;
  R.language = Language;
  R.profile = SemanticProfile::StrictSystem;
  R.objc3 = true;
  R.arc = ARC;
  R.objc_runtime = "gnustep-2.2";
  R.target_triple = "x86_64-unknown-linux-gnu";
  R.arguments = {"-std=" + Standard, "-fblocks"};
  return R;
}
void Expect(const CompileRequest &R, bool Success,
            const std::string &Diagnostic = {}) {
  CompileResult Result = Compile(R);
  if (Result.success != Success) {
    std::cerr << "unexpected borrowed-pointer result for:\n" << *R.source_text << '\n';
    for (const auto &D : Result.diagnostics)
      std::cerr << D.message << '\n';
  }
  assert(Result.success == Success);
  if (!Diagnostic.empty()) {
    bool Found = false;
    for (const auto &D : Result.diagnostics)
      Found |= D.message.find(Diagnostic) != std::string::npos;
    assert(Found && "rejection must identify its borrowed operation/contract");
  }
}
void SemanticCases(const fs::path &Root, BaseLanguage Language,
                    const std::string &Standard, bool ARC) {
  const std::vector<std::string> Accepted = {
      "void f(id o) { borrowed const char *p = bytes(o); use(p); }",
      "void f(id o) { const char *p = bytes(o); void *q = (void*)p; use_noescape((const char*)q); }",
      "void f(id o) { const char *p = bytes(o); p = 0; escape(p); }",
      "void f(id o, int b) { const char *p = bytes(o); if(b) p=0; else p=\"owned\"; escape(p); }",
      "void f(id o, int b) { const char *p = bytes(o); while(b--) { use(p); } }",
      "void f(id o) { const char *p = bytes(o); invoke(^{ use(p); }); }",
      "void f(id o) { const char *p = bytes(o); (^{ use(p); })(); }",
      "void f(id o) { const char *p = bytes(o); void (^b)(void)=^{ use(p); }; b(); }",
      "int f(id o) { const char *p=bytes(o); char out[4]; __builtin_memcpy(out,p,4); return out[0]+p[0]; }",
      "void f(id o) { withLifetime(o) { const char *p=bytes(o); use(p); } }",
      "void f(id o, id replacement) { keepAlive(o); const char *p=bytes(o); o=replacement; use(p); }",
      "void f(id o, id replacement) { const char *p=bytes(o); o=replacement; p=bytes(o); use(p); }",
      "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(owner_index=0))) { return bytes(o); }",
      "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(owner_index=0))) { id alias=o; return bytes(alias); }",
      "void f(id o) { const char *p[2]={bytes(o),0}; escape(p[1]); p[0]=0; escape(p[0]); }",
      "void f(void) { borrowed const char *p=0; escape(p); }",
      "long f(id o) { const char *p=bytes(o); return (p+2)-p; }",
      "BorrowedBytes f(id a,id o) __attribute__((objc_returns_borrowed(owner_index=1))) { return bytes(o); }",
      "@returns_borrowed(owner: 0) borrowed const char *f(id o) { return bytes(o); }",
      "int borrowed(int v) { return v; } int f(void) { return borrowed(3); }",
      "void f(id o) { const char *p; { id q=make_owner(); p=bytes(q); use(p); p=0; } escape(p); }",
      "void f(id o) { struct Holder h={bytes(o)}; use(h.pointer); h.pointer=0; escape(h.pointer); }",
  };
  for (const auto &Code : Accepted)
    Expect(Request(Root, Header + Code, Language, Standard, ARC), true);
  const std::vector<std::string> Escapes = {
      "void f(id o) { global_pointer=bytes(o); }",
      "void f(id o) { static const char *p; p=bytes(o); }",
      "void f(id o) { static __thread const char *p; p=bytes(o); }",
      "void f(id o, struct Holder *h) { h->pointer=bytes(o); }",
      "void f(id o) { escape(bytes(o)); }",
      "void f(id o) { const char *p=bytes(o); launch(^{ use(p); }); }",
      "void f(id o) { const char *p=bytes(o); invoke(^{ escape(p); }); }",
      "void f(id o) { const char *p[2]={bytes(o),0}; escape(p[0]); }",
      "void f(id o, char *owned) { const char *p=bytes(o); global_pointer=(const char*)__builtin_memcpy((void*)p,owned,1); }",
      "void f(id o) { const char *p=bytes(o); global_block=^{ use(p); }; }",
      "const char *f(id o) { return bytes(o); }",
      "BorrowedBytes f(id a,id b) __attribute__((objc_returns_borrowed(owner_index=0))) { return bytes(b); }",
      "unsigned long f(id o) { return (unsigned long)bytes(o); }",
      "void f(id o) { void *p=(void*)bytes(o); global_pointer=(const char*)p; }",
      "void f(id o, int b) { const char *p=0; if(b) p=bytes(o); escape(p); }",
      "void f(id o, id replacement) { const char *p=bytes(o); o=replacement; use(p); }",
      "void f(void) { const char *p; { id o=make_owner(); p=bytes(o); } use(p); }",
      "void f(id o, void *heap) { const char *p=bytes(o); __builtin_memcpy(heap,&p,sizeof p); }",
      "void f(id o) { const char *p=bytes(o); struct Holder h={p}; global_pointer=h.pointer; }",
  };
  for (const auto &Code : Escapes)
    Expect(Request(Root, Header + Code, Language, Standard, ARC), false,
           "borrowed pointer escapes");
  Expect(Request(Root, Header + "void f(void) { use(bytes(make_owner())); }",
                 Language, Standard, ARC), false, "non-stable owner");
  for (const auto &Code : std::vector<std::string>{
           "borrowed int bad;",
           "borrowed int (*bad)(void);",
           "const char *f(id o) __attribute__((objc_returns_borrowed(owner_index=0)));",
           "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(owner_index=1)));",
           "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(owner_index=-1)));",
           "BorrowedBytes f(int o) __attribute__((objc_returns_borrowed(owner_index=0)));",
           "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(0)));"})
    Expect(Request(Root, Header + Code, Language, Standard, ARC), false);
  for (const auto &Code : std::vector<std::string>{
           "void f(borrowed const char*); void f(const char*);",
           "BorrowedBytes f(id); const char *f(id);",
           "BorrowedBytes f(id a,id b) __attribute__((objc_returns_borrowed(owner_index=0))); BorrowedBytes f(id a,id b) __attribute__((objc_returns_borrowed(owner_index=1)));"})
    Expect(Request(Root, Header + Code, Language, Standard, ARC), false,
           "borrowed pointer declaration contract");
  auto Compatibility = Request(Root, Header + "void f(id o) { global_pointer=bytes(o); }",
                                Language, Standard, ARC);
  Compatibility.profile = SemanticProfile::Compatibility;
  Expect(Compatibility, true);
}
std::unique_ptr<clang::ASTUnit> Parse(const std::string &Source,
                                     const std::string &Filename,
                                     const std::vector<std::string> &Extra = {}) {
  std::vector<std::string> Arguments = {"-x", "objective-c++", "-std=c++20",
      "-fobjc-version=3", "-fobjc3-profile=strict-system", "-fblocks",
      "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu"};
  Arguments.insert(Arguments.end(), Extra.begin(), Extra.end());
  auto AST = clang::tooling::buildASTFromCodeWithArgs(Source, Arguments, Filename);
  assert(AST && !AST->getDiagnostics().hasErrorOccurred());
  return AST;
}
clang::FunctionDecl *Function(clang::ASTContext &Context, const std::string &Name) {
  for (clang::Decl *D : Context.getTranslationUnitDecl()->decls())
    if (auto *F = llvm::dyn_cast<clang::FunctionDecl>(D))
      if (F->getNameAsString() == Name)
        return F;
  assert(false && "expected borrowed contract function");
  return nullptr;
}
void PersistenceCases(const fs::path &Root) {
  const std::string Provider =
      "typedef borrowed const char *BorrowedBytes;\n"
      "BorrowedBytes exported(id a,id b) __attribute__((objc_returns_borrowed(owner_index=1)));\n"
      "void noescape_api(const char* p __attribute__((noescape)));\n";
  const auto HeaderPath = Root / "provider.h";
  { std::ofstream Out(HeaderPath); Out << Provider; }
  auto From = Parse(Provider, HeaderPath.string());
  auto To = Parse("", (Root / "consumer.mm").string());
  auto *F = Function(From->getASTContext(), "exported");
  assert(clang::isObjC3BorrowedType(From->getASTContext(), F->getReturnType()));
  assert(clang::getObjC3BorrowedReturnContract(F)->getOwnerIndex() == 1);
  clang::ASTImporter Importer(To->getASTContext(), To->getFileManager(),
      From->getASTContext(), From->getFileManager(), false);
  auto Imported = Importer.Import(F);
  assert(Imported);
  auto *ImportedFunction = llvm::cast<clang::FunctionDecl>(*Imported);
  assert(clang::isObjC3BorrowedType(To->getASTContext(), ImportedFunction->getReturnType()));
  assert(clang::getObjC3BorrowedReturnContract(ImportedFunction)->getOwnerIndex() == 1);
  auto Conflict = Parse("const char *exported(id,id);", (Root / "conflict.mm").string());
  clang::ASTImporter ConflictImporter(Conflict->getASTContext(), Conflict->getFileManager(),
      From->getASTContext(), From->getFileManager(), false);
  auto Failed = ConflictImporter.Import(F);
  assert(!Failed && "AST import must reject erased borrowed contract identity");
  llvm::consumeError(Failed.takeError());
  const auto PCHPath = Root / "borrowed.pch";
  assert(!From->Save(PCHPath.string()));
  auto PCHClient = Parse(
      "BorrowedBytes forward(id a,id b) __attribute__((objc_returns_borrowed(owner_index=1))) { return exported(a,b); }",
      (Root / "pch-client.mm").string(), {"-include-pch", PCHPath.string()});
  auto *Forward = Function(PCHClient->getASTContext(), "forward");
  assert(clang::isObjC3BorrowedType(PCHClient->getASTContext(), Forward->getReturnType()));
  // Exercise the maintained source-free interface path as well as native
  // AST serialization: both the typedef marker and named owner must survive.
  auto InterfaceProvider = Request(Root, Provider, BaseLanguage::ObjectiveCxx,
                                    "c++20", false);
  InterfaceProvider.emit_semantic_interface = true;
  InterfaceProvider.module_name = "Borrowed.Contracts";
  InterfaceProvider.package_identity = "borrowed-contracts-v1";
  InterfaceProvider.interface_output_path = (Root / "Borrowed.objc3i").string();
  auto Emitted = Compile(InterfaceProvider);
  if (!Emitted.success)
    for (const auto &D : Emitted.diagnostics)
      std::cerr << D.message << '\n';
  assert(Emitted.success && Emitted.interface_artifact);
  assert(Emitted.interface_artifact->source_bytes.find("owner_index=1") != std::string::npos);
  auto InterfaceClient = Request(Root,
      "BorrowedBytes imported_forward(id a,id b) __attribute__((objc_returns_borrowed(owner_index=1))) { noescape_api(exported(a,b)); return exported(a,b); }",
      BaseLanguage::ObjectiveCxx, "c++20", false);
  InterfaceImportInput Input;
  Input.source_path = InterfaceProvider.interface_output_path;
  Input.locked_identity = Emitted.interface_artifact->identity;
  InterfaceClient.imported_interfaces = {Input};
  Expect(InterfaceClient, true);
  InterfaceClient.source_text = "const char *imported_escape(id a,id b) { return exported(a,b); }";
  Expect(InterfaceClient, false, "borrowed pointer escapes");
  auto OtherOwner = Parse(
      "typedef borrowed const char *BorrowedBytes; BorrowedBytes exported(id a,id b) __attribute__((objc_returns_borrowed(owner_index=0)));",
      (Root / "other.mm").string());
  clang::ODRHash First, Second;
  First.AddFunctionDecl(F);
  Second.AddFunctionDecl(Function(OtherOwner->getASTContext(), "exported"));
  assert(First.CalculateHash() != Second.CalculateHash());
}
} // namespace
int main() {
  const auto Stamp = std::chrono::steady_clock::now().time_since_epoch().count();
  const auto Root = fs::temp_directory_path() / ("objc3-borrowed-" + std::to_string(Stamp));
  fs::create_directories(Root);
  for (bool ARC : {false, true}) {
    SemanticCases(Root, BaseLanguage::ObjectiveC, "c17", ARC);
    SemanticCases(Root, BaseLanguage::ObjectiveCxx, "c++20", ARC);
    SemanticCases(Root, BaseLanguage::ObjectiveCxx, "c++23", ARC);
  }
  PersistenceCases(Root);
  fs::remove_all(Root);
}
