#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/SourceMgr.h"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;
namespace fs=std::filesystem;
namespace {
void Success(const CompileResult &result) {
  if(!result.success) {
    std::cerr<<result.failure<<'\n';
    for(const auto &diagnostic:result.diagnostics)std::cerr<<diagnostic.message<<'\n';
    for(const auto &diagnostic:result.interface_import_diagnostics)std::cerr<<diagnostic.message<<'\n';
  }
  assert(result.success);
}
void Write(const fs::path &path,const std::string &text) {
  std::ofstream output(path);output<<text;assert(output.good());
}
CompileRequest Request(const fs::path &input,BaseLanguage language,bool arc,const char *optimization) {
  CompileRequest request;
  request.input_path=input.string();request.language=language;
  request.objc3=language==BaseLanguage::ObjectiveC;
  request.arc=arc;
  if(request.objc3)request.objc_runtime="gnustep-2.2";
  request.target_triple="x86_64-unknown-linux-gnu";
  request.arguments={"-std=c17",optimization};
  request.emit=EmitKind::Object;
  return request;
}
void CheckIR(const fs::path &path,const std::vector<std::string> &symbols,bool provider) {
  llvm::LLVMContext context;llvm::SMDiagnostic error;
  auto module=llvm::parseIRFile(path.string(),error,context);assert(module);
  for(const auto &symbol:symbols) {
    const auto *function=module->getFunction(symbol);
    if(provider)assert(function && !function->isDeclaration() && function->hasExternalLinkage());
    else assert(!function || function->isDeclaration() || function->hasAvailableExternallyLinkage());
  }
}
void Run(const fs::path &root,BaseLanguage language,bool arc,const char *optimization) {
  fs::create_directories(root/"provider");
  const auto input=root/"provider/module.m";
  Write(input,R"(
    inline int inline_owned(int value){return value+1;}
    extern inline int external_owned(int value){return value+2;}
    inline int later_external(int value){return value+3;}
    extern int later_external(int);
    inline int declared_inline(int);
    int declared_inline(int value){return value+4;}
  )");
  auto provider=Request(input,language,arc,optimization);
  provider.module_name="fixture.CInline";provider.package_identity="c-inline-v1";
  provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
  provider.emit_semantic_interface=true;provider.output_path=(root/"provider.o").string();
  auto built=Compile(provider);Success(built);
  assert(built.interface_artifact && built.interface_artifact->success);
  const auto &artifact=*built.interface_artifact;
  std::vector<std::string> symbols;
  for(const auto &declaration:built.declarations)
    if(HasCInlineDefinitionIdentity(declaration))symbols.push_back(declaration.native_symbol_name);
  assert(symbols.size()==4);
  Write(root/"interface.h",artifact.source_bytes);
  auto ir=provider;ir.emit=EmitKind::LLVM;ir.output_path=(root/"provider.ll").string();
  Success(Compile(ir));CheckIR(root/"provider.ll",symbols,true);
  // Consumers have only the canonical public interface and the provider object.
  assert(fs::remove(input));
  for(const char *name:{"a","b"}) {
    auto source=root/(std::string("consumer-")+name+".m");
    Write(source,"#include \"interface.h\"\nint call_"+std::string(name)+"(void){return inline_owned(41)+external_owned(40)+later_external(39)+declared_inline(38);}\n"
        "typedef int (*CInlinePointer)(int);CInlinePointer address_"+name+"(void){return external_owned;}\n");
    auto consumer=Request(source,language,arc,optimization);
    consumer.output_path=(root/(std::string(name)+".o")).string();Success(Compile(consumer));
    consumer.emit=EmitKind::LLVM;consumer.output_path=(root/(std::string(name)+".ll")).string();
    Success(Compile(consumer));CheckIR(root/(std::string(name)+".ll"),symbols,false);
  }
  auto main=root/"main.c";
  Write(main,"int call_a(void);int call_b(void);typedef int (*Pointer)(int);Pointer address_a(void);Pointer address_b(void);"
      "int main(void){return call_a()==168 && call_b()==168 && address_a()==address_b()?0:1;}\n");
  auto entry=Request(main,BaseLanguage::C,false,optimization);
  entry.output_path=(root/"main.o").string();Success(Compile(entry));
  const std::string linker=OBJC3_C_INLINE_TEST_LINKER;
  std::vector<std::string> owned={linker,"-fuse-ld=lld",(root/"provider.o").string(),
      (root/"a.o").string(),(root/"b.o").string(),(root/"main.o").string(),"-o",(root/"witness").string()};
  if(language==BaseLanguage::ObjectiveC) {
    const fs::path runtime=OBJC3_C_INLINE_TEST_OBJC_LIBRARY;
    owned.push_back(runtime.string());
    owned.push_back("-Wl,-rpath,"+runtime.parent_path().string());
  }
  std::vector<llvm::StringRef> argv(owned.begin(),owned.end());
  assert(llvm::sys::ExecuteAndWait(linker,argv)==0);
  owned={(root/"witness").string()};argv.assign(owned.begin(),owned.end());
  assert(llvm::sys::ExecuteAndWait(owned.front(),argv)==0);
  auto plain=root/"ordinary.c";Write(plain,"inline int ordinary_inline(int value){return value+1;}\n");
  auto unchanged=Request(plain,BaseLanguage::C,false,optimization);unchanged.emit=EmitKind::LLVM;
  unchanged.output_path=(root/"ordinary.ll").string();Success(Compile(unchanged));
  CheckIR(root/"ordinary.ll",{"ordinary_inline"},false);
  auto gnu_input=root/"provider/gnu.m";
  Write(gnu_input,"inline int gnu_inline_value(int value){return value+1;}\n");
  auto gnu=provider;gnu.input_path=gnu_input.string();gnu.arguments.push_back("-fgnu89-inline");
  gnu.output_path=(root/"gnu-rejected.o").string();
  auto rejected=Compile(gnu);
  assert(!rejected.success && !fs::exists(gnu.output_path));
  assert(std::any_of(rejected.diagnostics.begin(),rejected.diagnostics.end(),
      [](const auto &diagnostic){return diagnostic.message.find("ISO inline")!=std::string::npos;}));
}
}
int main() {
  llvm::SmallString<128> temporary;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-c-inline-provider",temporary));
  const fs::path root(temporary.c_str());
  for(const char *optimization:{"-O0","-O2"}) {
    Run(root/(std::string("c")+optimization),BaseLanguage::C,false,optimization);
    Run(root/(std::string("objc-mrc")+optimization),BaseLanguage::ObjectiveC,false,optimization);
    Run(root/(std::string("objc-arc")+optimization),BaseLanguage::ObjectiveC,true,optimization);
  }
  fs::remove_all(root);
}
