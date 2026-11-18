#include "native_target_test_support.h"
#include "clang_foundation/compile_session.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <tuple>
using namespace objc3::clang_foundation;
namespace fs=std::filesystem;
int main() {
 llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-linux-backends",temporary));
 fs::path root(temporary.c_str());struct Cleanup{fs::path path;~Cleanup(){fs::remove_all(path);}}cleanup{root};
 const std::string configured="," OBJC3_TEST_CODEGEN_BACKENDS ",";
 unsigned tested=0;bool native=false;
 for(const auto &[backend,triple,machine]:{std::tuple{"X86","x86_64-unknown-linux-gnu",62},std::tuple{"AArch64","aarch64-unknown-linux-gnu",183}}) {
  if(configured.find(","+std::string(backend)+",")==std::string::npos)continue;
  ++tested;
  CompileRequest request;request.input_path=(root/(std::string(backend)+".c")).string();
  request.source_text="struct Pair{long first,second;};long sum(struct Pair value){return value.first+value.second;}int answer(void){return 42;}";
  request.language=BaseLanguage::C;request.target_triple=triple;request.arguments={"-std=c17","-O2"};
  request.emit=EmitKind::LLVM;request.output_path=(root/(std::string(backend)+".ll")).string();
  auto ir=Compile(request);if(!ir.success)std::cerr<<ir.failure<<'\n';assert(ir.success);
  {std::ifstream file(request.output_path);std::string bytes((std::istreambuf_iterator<char>(file)),{});assert(bytes.find("target triple = \""+std::string(triple)+"\"")!=std::string::npos);}
  request.emit=EmitKind::Object;request.output_path=(root/(std::string(backend)+".o")).string();
  auto object=Compile(request);if(!object.success)std::cerr<<object.failure<<'\n';assert(object.success);
  {std::ifstream file(request.output_path,std::ios::binary);std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(file)),{});
   assert(bytes.size()>20 && bytes[0]==0x7f && bytes[1]=='E' && bytes[2]=='L' && bytes[3]=='F');
   assert(bytes[4]==2 && bytes[5]==1 && (unsigned(bytes[18])|(unsigned(bytes[19])<<8))==unsigned(machine));}
  if(std::string(triple)==OBJC3_TEST_NATIVE_LINUX_TARGET) {
   native=true;const auto main=root/"main.c",program=root/"native-program";
   {std::ofstream file(main);file<<"int answer(void);int main(void){return answer()==42?0:1;}";assert(file.good());}
   std::vector<std::string> command={object.compiler_driver_path,"--no-default-config","--target="+std::string(triple),"-fuse-ld=lld",request.output_path,main.string(),"-o",program.string()};
   std::vector<llvm::StringRef> args(command.begin(),command.end());assert(llvm::sys::ExecuteAndWait(command.front(),args)==0);
   const std::string executable=program.string();llvm::StringRef run[]={executable};assert(llvm::sys::ExecuteAndWait(executable,run)==0);
  }
  std::cout<<"Validated configured backend "<<backend<<" / "<<triple<<'\n';
 }
 assert(tested && native);
 // The SDK's configured backend set is an admission boundary, not a promise
 // that target-info parsing alone enables an unlinked generator.
 CompileRequest absent;absent.input_path=(root/"absent.c").string();absent.source_text="int answer(void){return 42;}";
 absent.language=BaseLanguage::C;absent.target_triple="riscv64-unknown-linux-gnu";absent.emit=EmitKind::Object;absent.output_path=(root/"absent.o").string();
 auto rejected=Compile(absent);assert(!rejected.success && !fs::exists(absent.output_path));
 bool unavailable=false;for(const auto &diagnostic:rejected.diagnostics)
  unavailable|=diagnostic.message.find("No available targets")!=std::string::npos;
 assert(unavailable);
}
