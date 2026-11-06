#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/runtime_interface_types.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/StringSaver.h"
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
  const bool frames=language==BaseLanguage::ObjectiveC;
  if(frames) {
    fs::create_directories(root/"runtime/objc3c/runtime");
    Write(root/"runtime/objc3c/runtime/objc3_runtime_services.h",RuntimeInterfaceHeaderSource().str());
  }
  std::string source=R"(
    inline int inline_owned(int value){return value+1;}
    extern inline int external_owned(int value){return value+2;}
    inline int later_external(int value){return value+3;}
    extern int later_external(int);
    inline int declared_inline(int);
    int declared_inline(int value){return value+4;}
  )";
  if(frames)source="#include <objc3c/runtime/objc3_runtime_services.h>\n"+source+R"(
    inline int inline_frame(int value) __objc3_async_frame {return value+5;}
    extern inline int external_frame(int value) __objc3_async_frame {return value+6;}
    inline int later_frame(int value) __objc3_async_frame {return value+7;}
    extern int later_frame(int) __objc3_async_frame;
    inline int chained_frame(int value) __objc3_async_frame {return __objc3_await inline_frame(value)+1;}
  )";
  Write(input,source);
  auto provider=Request(input,language,arc,optimization);
  if(frames)provider.arguments.push_back("-I"+(root/"runtime").string());
  provider.module_name="fixture.CInline";provider.package_identity="c-inline-v1";
  provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
  provider.emit_semantic_interface=true;provider.output_path=(root/"provider.o").string();
  auto built=Compile(provider);Success(built);
  assert(built.interface_artifact && built.interface_artifact->success);
  const auto &artifact=*built.interface_artifact;
  std::vector<std::string> symbols;
  for(const auto &declaration:built.declarations)
    if(HasCInlineDefinitionIdentity(declaration))symbols.push_back(declaration.native_symbol_name);
  assert(symbols.size()==(frames?8:4));
  Write(root/"interface.h",artifact.source_bytes);
  auto ir=provider;ir.emit=EmitKind::LLVM;ir.output_path=(root/"provider.ll").string();
  Success(Compile(ir));CheckIR(root/"provider.ll",symbols,true);
  // Consumers have only the canonical public interface and the provider object.
  assert(fs::remove(input));
  for(const char *name:{"a","b"}) {
    auto source=root/(std::string("consumer-")+name+".m");
    std::string body="#include \"interface.h\"\nint call_"+std::string(name)+"(void){return inline_owned(41)+external_owned(40)+later_external(39)+declared_inline(38); }\n"
        "typedef int (*CInlinePointer)(int);CInlinePointer address_"+name+"(void){return external_owned;}\n";
    if(frames)body+="objc3_async_frame frame_"+std::string(name)+R"((int number){
      switch(number){
      case 0:return inline_frame(37);
      case 1:return external_frame(36);
      case 2:return later_frame(35);
      default:return chained_frame(36);
      }
    }
    )"+"void *frame_address_"+name+"(void){return (void *)external_frame;}\n";
    Write(source,body);
    auto consumer=Request(source,language,arc,optimization);
    if(frames)consumer.arguments.push_back("-I"+(root/"runtime").string());
    consumer.output_path=(root/(std::string(name)+".o")).string();Success(Compile(consumer));
    consumer.emit=EmitKind::LLVM;consumer.output_path=(root/(std::string(name)+".ll")).string();
    Success(Compile(consumer));CheckIR(root/(std::string(name)+".ll"),symbols,false);
  }
  auto main=root/"main.c";
  std::string main_source="int call_a(void);int call_b(void);typedef int (*Pointer)(int);Pointer address_a(void);Pointer address_b(void);\n";
  if(frames)main_source="#include <objc3c/runtime/objc3_runtime_services.h>\n"+main_source+R"(
    objc3_async_frame frame_a(int);objc3_async_frame frame_b(int);
    void *frame_address_a(void);void *frame_address_b(void);
    static int check_frame(objc3_async_frame frame,objc3_executor_t *executor) {
      objc3_task_t *task=0;
      if(objc3_async_frame_validate(&frame)!=OBJC3_SERVICE_OK ||
          objc3_async_frame_start(&frame,executor,0,&task)!=OBJC3_SERVICE_OK || frame.context)return 1;
      uint32_t count=0;
      if(objc3_executor_pump(executor,100,&count)!=OBJC3_SERVICE_OK || !count)return 2;
      objc3_completion completion={0};
      if(objc3_task_get_completion(task,&completion)!=OBJC3_SERVICE_OK ||
          completion.kind!=OBJC3_COMPLETION_SUCCESS || !completion.has_payload ||
          !completion.payload || *(const int *)completion.payload!=42)return 3;
      objc3_task_release(task);return 0;
    }
    static int check_frames(void) {
      objc3_executor_t *executor=0;
      if(objc3_executor_create(1,&executor)!=OBJC3_SERVICE_OK)return 1;
      if(frame_address_a()!=frame_address_b())return 2;
      for(int number=0;number!=4;++number) {
        if(check_frame(frame_a(number),executor) || check_frame(frame_b(number),executor))return 3;
        objc3_async_frame unused=frame_a(number);
        if(objc3_async_frame_dispose(&unused)!=OBJC3_SERVICE_OK || unused.context)return 4;
      }
      objc3_executor_release(executor);return 0;
    }
  )";
  main_source+="int main(void){return call_a()==168 && call_b()==168 && address_a()==address_b()"+
      std::string(frames?" && check_frames()==0":"")+"?0:1;}\n";
  Write(main,main_source);
  auto entry=Request(main,BaseLanguage::C,false,optimization);
  if(frames)entry.arguments.push_back("-I"+(root/"runtime").string());
  entry.output_path=(root/"main.o").string();Success(Compile(entry));
  const std::string linker=OBJC3_C_INLINE_TEST_LINKER;
  std::vector<std::string> owned={linker,"-fuse-ld=lld",(root/"provider.o").string(),
      (root/"a.o").string(),(root/"b.o").string(),(root/"main.o").string(),"-o",(root/"witness").string()};
  if(frames) {
    owned.push_back(OBJC3_C_INLINE_TEST_RUNTIME_LIBRARY);
    owned.push_back("-pthread");
    llvm::BumpPtrAllocator allocator;llvm::StringSaver saver(allocator);
    llvm::SmallVector<const char *,16> flags;
    llvm::cl::TokenizeGNUCommandLine(OBJC3_C_INLINE_TEST_LINK_FLAGS,saver,flags);
    for(const char *flag:flags)owned.emplace_back(flag);
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
