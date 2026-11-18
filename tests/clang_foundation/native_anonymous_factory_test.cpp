#include "native_target_test_support.h"
#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Error.h"
#include <set>
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace objc3::clang_foundation;
namespace fs=std::filesystem;
namespace {
void Success(const CompileResult &R){if(!R.success){std::cerr<<R.failure<<'\n';for(const auto &D:R.diagnostics)std::cerr<<D.message<<'\n';if(R.interface_artifact)for(const auto &D:R.interface_artifact->diagnostics)std::cerr<<D.code<<" ["<<D.subject<<"] "<<D.message<<'\n';}assert(R.success);}
void Write(const fs::path &P,const std::string &S){std::ofstream F(P);F<<S;assert(F.good());}
void Execute(const std::vector<std::string> &Command){std::vector<llvm::StringRef> args(Command.begin(),Command.end());assert(llvm::sys::ExecuteAndWait(Command.front(),args)==0);}
}
int main(){
 llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-anonymous-factory",temporary));
 fs::path root(temporary.c_str());struct Cleanup{fs::path path;~Cleanup(){fs::remove_all(path);}}cleanup{root};
 for(const char *standard:{"c++20","c++23"}) {
  CompileRequest provider;provider.input_path=(root/"provider.cpp").string();provider.language=BaseLanguage::Cxx;
  provider.target_triple=OBJC3_TEST_NATIVE_LINUX_TARGET;provider.arguments={std::string("-std=")+standard};
  provider.emit_semantic_interface=true;provider.module_name="test.AnonymousFactories";provider.package_identity="anonymous-factories-v1";
  provider.interface_exports={"make_record","make_nested","make_generic","global_record","other_record","nested_record","read_global"};
  provider.source_text=R"source(
constexpr auto make_record(){struct {int value;constexpr int read()const{return value;}} result{42};return result;}
constexpr auto make_nested(){struct {struct {int value;} nested;} result{{42}};return result;}
template<class T>constexpr auto make_generic(T value){struct {T value;constexpr T read()const{return value;}} result{value};return result;}
inline constexpr struct {int value;constexpr int read()const{return value;}} global_record{42};
inline constexpr struct {int value;constexpr int read()const{return value;}} other_record{7};
inline constexpr struct {struct Left{int value;constexpr int read()const{return value;}} left;struct Right{int value;constexpr int read()const{return value;}} right;template<class T>constexpr T sum(T n)const{return left.value+right.value+n;}} nested_record{{20},{22}};
template<decltype(global_record) Value>constexpr int read_global(){return Value.value;}
)source";
  auto produced=Compile(provider);Success(produced);assert(produced.interface_artifact && produced.interface_artifact->success);
  const auto &artifact=*produced.interface_artifact;
  auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,SupportedSemanticInterfaceCapabilities());assert(decoded);
  std::set<std::string> ids;bool first=false,second=false;
  for(const auto &D:decoded->declarations){assert(ids.insert(D.stable_id).second);first|=D.export_root=="global_record";second|=D.export_root=="other_record";}
  assert(first && second);
  auto relocated=provider;relocated.input_path=(root/"relocated/provider.cpp").string();
  auto moved=Compile(relocated);Success(moved);assert(moved.interface_artifact->identity.semantic_fingerprint==artifact.identity.semantic_fingerprint);
  auto supported=SupportedSemanticInterfaceCapabilities();
  std::vector<llvm::StringRef> old_reader(supported.begin(),supported.end());
  old_reader.erase(std::remove(old_reader.begin(),old_reader.end(),llvm::StringRef(kCxxInlineClosureObjectSemantic)),old_reader.end());
  auto rejected=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,old_reader);assert(!rejected);llvm::consumeError(rejected.takeError());
  auto missing=provider;missing.interface_exports={"read_global"};assert(!Compile(missing).success);
  for(const char *private_member:{"static constexpr int secret(){return 42;}\ninline constexpr struct{int value;constexpr int read()const{return secret();}} global_record{42};",
      "static constexpr int secret=42;\ninline constexpr struct{int value;constexpr int read()const{return secret;}} global_record{42};"}){
    auto hidden=provider;hidden.interface_exports={"global_record"};hidden.source_text=private_member;assert(!Compile(hidden).success);
  }
  for(const char *unsupported:{"inline constexpr struct{mutable int value;} global_record{42};",
      "struct State{mutable int value;};inline constexpr struct{State state;} global_record{{42}};",
      "struct State{int value;constexpr ~State(){}};inline constexpr struct{State state;} global_record{{42}};"}) {
    auto bad=provider;bad.interface_exports={"global_record"};bad.source_text=unsupported;assert(!Compile(bad).success);
  }
  auto spi=provider;spi.interface_exports={"global_record"};
  spi.source_text="inline constexpr struct{int value;constexpr int read()const{return value;}} global_record __attribute__((annotate(\"objc3.spi:Testing\"))){42};";
  auto restricted=Compile(spi);Success(restricted);
  auto spi_rows=DecodeSemanticInterfaceSidecar(restricted.interface_artifact->sidecar_json,restricted.interface_artifact->source_bytes,SupportedSemanticInterfaceCapabilities());assert(spi_rows);
  for(const auto &D:spi_rows->declarations)if(D.export_root=="global_record")assert(D.spi_group=="Testing");
  auto spi_path=root/"Restricted.objc3i";
  Write(spi_path,restricted.interface_artifact->source_bytes);Write(spi_path.string()+".json",restricted.interface_artifact->sidecar_json);
  auto restricted_use=provider;restricted_use.emit_semantic_interface=false;restricted_use.interface_exports.clear();
  restricted_use.source_text="static_assert(global_record.read()==42);";
  InterfaceImportInput restricted_import;restricted_import.source_path=spi_path.string();restricted_import.sidecar_path=spi_path.string()+".json";restricted_import.locked_identity=restricted.interface_artifact->identity;
  restricted_use.imported_interfaces={restricted_import};assert(!Compile(restricted_use).success);
  restricted_use.imported_interfaces[0].spi_groups={"Testing"};Success(Compile(restricted_use));
  auto interface=root/"Anonymous.objc3i";
  Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  CompileRequest consumer;consumer.input_path=(root/"consumer.cpp").string();consumer.language=BaseLanguage::Cxx;
  consumer.target_triple=OBJC3_TEST_NATIVE_LINUX_TARGET;consumer.arguments={std::string("-std=")+standard};
  InterfaceImportInput imported;imported.source_path=interface.string();imported.sidecar_path=interface.string()+".json";imported.locked_identity=artifact.identity;consumer.imported_interfaces={imported};
  consumer.source_text="static_assert(make_record().read()==42);static_assert(make_nested().nested.value==42);static_assert(make_generic(42).read()==42);static_assert(global_record.read()==42);static_assert(other_record.read()==7);static_assert(nested_record.left.read()==20);static_assert(nested_record.right.read()==22);static_assert(nested_record.sum(0)==42);static_assert(read_global<global_record>()==42);extern \"C\" int anonymous_result(){return make_record().read()+make_nested().nested.value+make_generic(42).read();}";
  consumer.emit=EmitKind::Object;consumer.output_path=(root/"consumer.o").string();auto consumed=Compile(consumer);Success(consumed);
  Write(root/"main.c","int anonymous_result(void);int main(void){return anonymous_result()==126?0:1;}");
  Execute({consumed.compiler_driver_path,"--no-default-config","-fuse-ld=lld",consumer.output_path,(root/"main.c").string(),"-o",(root/"witness").string()});Execute({(root/"witness").string()});
 }
}
