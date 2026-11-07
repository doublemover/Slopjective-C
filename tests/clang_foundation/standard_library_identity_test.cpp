#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_import.h"
#include "clang_foundation/module_names.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Tooling/Tooling.h"
#include "clang/UnifiedSymbolResolution/USRGeneration.h"
#include "llvm/ADT/ScopeExit.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/FileSystem.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;
int main() {
  llvm::SmallString<256> temporary;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-native-template-identity",temporary));
  const std::filesystem::path root(temporary.c_str());
  auto cleanup=llvm::scope_exit([&]{std::filesystem::remove_all(root);});
  {
    std::ofstream header(root/"functional");
    header<<R"(
namespace std {inline namespace __fixture {
template<class T>struct class_probe {};
template<class T>struct native_sink {
  struct result {T value;};
  virtual ~native_sink()=default;
  result reserve(T value){return {value};}
};
template<class T>struct native_deduced {
  native_sink<T> *sink;
  auto read()const{return sink->reserve(42);}
};
inline native_sink<int> existing_sink;
inline const native_deduced<int> existing_deduced{&existing_sink};
template<class T>struct native_storage {
  struct {T value;};
  constexpr T read()const{return value;}
};
template<class T>using alias_probe=class_probe<T>;
template<class T>inline constexpr bool variable_probe=false;
template<class T>T function_probe(T value);
template<class T>struct native_iterator {
  template<class U,unsigned long N>friend class span;
  template<class U>friend U *copy_probe(native_iterator<U>,U *);
  T *ptr;
};
template<class T,unsigned long N=0>class span {
  using iterator=native_iterator<T>;
  constexpr iterator begin() const {return {nullptr};}
};
}}
)";
    assert(header.good());
  }
  auto ast=clang::tooling::buildASTFromCodeWithArgs(
      "#include <functional>\nnamespace Public {template<class T>struct Box {};"
      "inline int read(std::native_iterator<int> value){return *value.ptr;}"
      "inline int read(int value){return value;}"
      "inline std::native_storage<int> storage(){return {{42}};}"
      "inline int deduced(){return std::existing_deduced.read().value;}}",
      {"-xc++","-std=c++23","-isystem",root.string()},"native-template-identity.cpp");
  assert(ast && !ast->getDiagnostics().hasErrorOccurred());
  std::vector<clang::TemplateDecl *> native;
  clang::ClassTemplateDecl *user=nullptr;
  auto collect=[&](auto &&self,clang::DeclContext *scope)->void {
    for(auto *declaration:scope->decls()) {
      if(auto *space=llvm::dyn_cast<clang::NamespaceDecl>(declaration))self(self,space);
      if(auto *pattern=llvm::dyn_cast<clang::TemplateDecl>(declaration)) {
        if(pattern->getName()=="Box")user=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern);
        else native.push_back(pattern);
      }
    }
  };
  auto &context=ast->getASTContext();collect(collect,context.getTranslationUnitDecl());
  assert(native.size()==9 && user);
  const auto original_policy=context.getPrintingPolicy();
  for(auto *pattern:native) {
    llvm::SmallString<128> expected;
    assert(!clang::index::generateUSRForDecl(pattern->getCanonicalDecl(),expected));
    std::set<std::string> display_names;
    for(auto mode:{clang::PrintingPolicy::SuppressInlineNamespaceMode::None,
                   clang::PrintingPolicy::SuppressInlineNamespaceMode::Redundant,
                   clang::PrintingPolicy::SuppressInlineNamespaceMode::All}) {
      auto policy=context.getPrintingPolicy();
      policy.SuppressInlineNamespace=static_cast<unsigned>(mode);context.setPrintingPolicy(policy);
      display_names.insert(pattern->getQualifiedNameAsString());
      assert(ReadInterfaceDeclarationStableId(pattern)==expected.str());
      assert(ReadInterfaceDeclarationStableId(user)=="c:@ClassTemplate@Public::Box");
    }
    assert(display_names.size()>1);
  }
  context.setPrintingPolicy(original_policy);
  assert(ApplyModuleOwnedNames(context,context.getTranslationUnitDecl(),
      "fixture.NativeTemplateIdentities","test",{}));
  auto artifact=EmitSemanticInterface(context,"fixture.NativeTemplateIdentities","test",
      {},{},{},{"Public"},{},&ast->getPreprocessor());
  if(!artifact.success)for(const auto &diagnostic:artifact.diagnostics)
    std::cerr<<diagnostic.code<<": "<<diagnostic.message<<" "<<diagnostic.subject<<'\n';
  assert(artifact.success);
  const auto interface=root/"native.objc3i";
  {std::ofstream out(interface);out<<artifact.source_bytes;assert(out.good());}
  {std::ofstream out(interface.string()+".json");out<<artifact.sidecar_json;assert(out.good());}
  CompileRequest consumer;consumer.input_path=(root/"consumer.cpp").string();
  consumer.source_text="int main(){int value=42;return Public::read(std::native_iterator<int>{&value})+Public::read(7)+Public::storage().read()+Public::deduced()-133;}";
  consumer.language=BaseLanguage::Cxx;consumer.objc3=false;consumer.arc=false;consumer.target_triple="x86_64-unknown-linux-gnu";
  consumer.arguments={"-std=c++23","-isystem",root.string()};
  consumer.emit=EmitKind::Object;consumer.output_path=(root/"consumer.o").string();
  InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
  consumer.imported_interfaces={imported};
  auto target_ast=clang::tooling::buildASTFromCodeWithArgs(
      "#include <functional>\n",
      {"-xc++","-std=c++23","-isystem",root.string()},"native-target.cpp");
  assert(target_ast && !target_ast->getDiagnostics().hasErrorOccurred());
  auto span_redeclarations=[&] {
    unsigned count=0;
    auto walk=[&](auto &&self,clang::DeclContext *scope)->void {
      for(auto *declaration:scope->decls()) {
        if(auto *space=llvm::dyn_cast<clang::NamespaceDecl>(declaration))self(self,space);
        if(auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration);
            pattern && pattern->getName()=="span") {
          count=0;for(auto *redecl:pattern->redecls()){(void)redecl;++count;}
        }
      }
    };
    walk(walk,target_ast->getASTContext().getTranslationUnitDecl());return count;
  };
  const auto existing_spans=span_redeclarations();assert(existing_spans>0);
  auto storage_fields=[&] {
    unsigned count=0;
    auto walk=[&](auto &&self,clang::DeclContext *scope)->void {
      for(auto *declaration:scope->decls()) {
        if(auto *space=llvm::dyn_cast<clang::NamespaceDecl>(declaration))self(self,space);
        if(auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration);
            pattern && pattern->getName()=="native_storage")
          for(auto *field:pattern->getTemplatedDecl()->fields()){(void)field;++count;}
      }
    };
    walk(walk,target_ast->getASTContext().getTranslationUnitDecl());return count;
  };
  const auto existing_fields=storage_fields();assert(existing_fields==1);
  InterfaceImportTarget target;
  target.target_triple=artifact.identity.target_triple;
  target.runtime_abi=artifact.identity.runtime_abi;
  target.extension_abi=artifact.identity.extension_abi;
  target.language_revision=artifact.identity.language_revision;
  target.language_profile=artifact.identity.language_profile;
  target.allow_function_overloads=true;
  target.ast_arguments={"-xc++","-std=c++23","-isystem",root.string()};
  std::vector<InterfaceContractDiagnostic> diagnostics;
  auto session=InterfaceImportSession::Prepare({imported},target,diagnostics);
  if(!session)for(const auto &diagnostic:diagnostics)
    std::cerr<<diagnostic.code<<": "<<diagnostic.message<<'\n';
  assert(session);
  auto report=session->ImportInto(target_ast->getASTContext(),target_ast->getFileManager(),
      false,{},nullptr,&target_ast->getPreprocessor());
  if(!report.success)for(const auto &diagnostic:report.diagnostics)
    std::cerr<<diagnostic.code<<": "<<diagnostic.message<<'\n';
  assert(report.success && span_redeclarations()==existing_spans);
  assert(storage_fields()==existing_fields);
  // Each native function keeps its source redeclaration chain. Both overloads
  // remain distinct, without splitting a definition from its forward declaration.
  std::set<const clang::FunctionDecl *> reads;
  unsigned read_declarations=0;
  for(auto *top:target_ast->getASTContext().getTranslationUnitDecl()->decls())
    if(auto *space=llvm::dyn_cast<clang::NamespaceDecl>(top);space && space->getName()=="Public")
      for(auto *member:space->decls())
        if(auto *function=llvm::dyn_cast<clang::FunctionDecl>(member);
            function && function->getName()=="read") {
          reads.insert(function->getCanonicalDecl());
          ++read_declarations;
        }
  assert(reads.size()==2 && read_declarations==4);
  auto result=Compile(consumer);
  if(!result.success) {
    std::cerr<<result.failure<<'\n';
    for(const auto &diagnostic:result.diagnostics)
      std::cerr<<diagnostic.spelling.file<<':'<<diagnostic.spelling.line<<": "<<diagnostic.message<<'\n';
    for(const auto &diagnostic:result.interface_import_diagnostics)
      std::cerr<<diagnostic.code<<": "<<diagnostic.message<<'\n';
  }
  assert(result.success && std::filesystem::exists(consumer.output_path));
}
