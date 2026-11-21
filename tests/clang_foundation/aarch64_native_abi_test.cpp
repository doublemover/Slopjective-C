#include "compile_test_diagnostics.h"
#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_codec.h"
#include <set>
#include <algorithm>
#include "llvm/ADT/ScopeExit.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/FileSystem.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>

using namespace objc3::clang_foundation;

int main() {
  llvm::SmallString<128> temporary;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-aarch64-native-abi", temporary));
  const std::filesystem::path root(temporary.c_str());
  auto cleanup=llvm::scope_exit([&]{std::filesystem::remove_all(root);});
  for(const char *standard:{"c17","c++20","c++23"})for(bool arc:{false,true}) {
    const bool cpp=std::string(standard)!="c17";
    CompileRequest request;
    request.input_path=cpp?"aarch64-native.mm":"aarch64-native.m";
    request.output_path=(root/(std::string(standard)+(arc?"-arc.ll":"-mrc.ll"))).string();
    request.language=cpp?BaseLanguage::ObjectiveCxx:BaseLanguage::ObjectiveC;
    request.objc3=true;request.arc=arc;request.target_triple="aarch64-unknown-linux-gnu";
    request.objc_runtime="gnustep-2.2";request.emit=EmitKind::LLVM;
    request.arguments={std::string("-std=")+standard,"-fblocks","-ffreestanding"};
    request.source_text=R"abi(
#if !defined(__aarch64__) || !__OBJC3_FEATURE_OPTIONALS__ || !__OBJC3_FEATURE_RESULT__ || !__OBJC3_FEATURE_DEFER__
#error Native ARM64 target and implemented carrier/control-flow predicates are required
#endif
#ifdef __cplusplus
extern "C" {
#endif
Optional(int) plain(Optional(int) value){return value;}
Result(int,int) result(Result(int,int) value){return value;}
#if __has_feature(objc_arc)
Optional(id) managed(Optional(id) value){return value;}
typedef Optional(id __weak) Weak;
Weak weak_value(Weak value){return value;}
extern void ninth(Weak,Weak,Weak,Weak,Weak,Weak,Weak,Weak,Weak);
void argument_spill(Weak value){ninth(value,value,value,value,value,value,value,value,value);}
#endif
extern void observe(int);
typedef void (^Block)(void);
extern Block escape(Block);
struct __attribute__((aligned(64))) Aligned {int value;};
Block aligned_value(void){
  struct Aligned value={42};
  return escape(^{observe(value.value);});
}
Block aligned_reference(void){
  __block struct Aligned value={42};
  return escape(^{observe(++value.value);});
}
#ifdef __cplusplus
}
struct Owner {int value;Owner(int input):value(input){} Owner(const Owner &other):value(other.value){} ~Owner(){observe(value);}};
Optional(Owner) cxx_owner(Optional(Owner) value){return value;}
#endif
)abi";
    auto result=Compile(request);test::ReportFailure(result);assert(result.success);
    assert(result.target_triple==request.target_triple);
    std::ifstream file(request.output_path);
    const std::string ir{std::istreambuf_iterator<char>(file),{}};
    assert(ir.find("target triple = \"aarch64-unknown-linux-gnu\"")!=std::string::npos);
    assert(ir.find("i64 @plain(i64")!=std::string::npos);
    assert(ir.find("objc3alignedv1_64_")!=std::string::npos);
    assert(ir.find("inttoptr (i64 64 to ptr)")!=std::string::npos);
    if(arc) {
      assert(ir.find("objc.retain")!=std::string::npos && ir.find("objc.storeStrong")!=std::string::npos);
      assert(ir.find("byval(")==std::string::npos);
      assert(ir.find("objc.copyWeak")!=std::string::npos && ir.find("objc.destroyWeak")!=std::string::npos);
    }
    if(cpp)assert(ir.find("sret(")!=std::string::npos);
  }
  // This producer is removed before either consumer reparses the canonical
  // interface, so native target-version bodies cannot be recovered from source.
  for(const char *standard:{"c++20","c++23"}) {
    CompileRequest provider;
    provider.input_path="aarch64-dispatch.mm";provider.language=BaseLanguage::ObjectiveCxx;
    provider.objc3=true;provider.objc_runtime="gnustep-2.2";provider.target_triple="aarch64-unknown-linux-gnu";
    provider.arguments={std::string("-std=")+standard,"--rtlib=compiler-rt"};
    provider.module_name="fixture.AArch64Dispatch";provider.package_identity="aarch64-dispatch-v1";
    provider.module_owned_names=true;provider.emit_semantic_interface=true;provider.interface_exports={"Dispatch"};
    provider.source_text=R"cpu(
namespace Dispatch {
__attribute__((target_version("default"))) inline int versioned(int value){return value+6;}
__attribute__((target_version("sve"))) inline int versioned(int value){return value+7;}
__attribute__((target_clones("default","sve"))) inline int cloned(int value){return value+2;}
__attribute__((aarch64_vector_pcs,noinline)) inline int vector(int value){return value;}
inline int invoke(int value){auto call=[](int input) __attribute__((aarch64_vector_pcs,noinline)){return input;};return call(value);}
}
)cpu";
    auto emitted=Compile(provider);test::ReportFailure(emitted);
    assert(emitted.success && emitted.interface_artifact && emitted.interface_artifact->success);
    const auto artifact=*emitted.interface_artifact;
    assert(artifact.source_bytes.find("target_version")!=std::string::npos);
    assert(artifact.sidecar_json.find(kNativeEmissionPolicySemantic)!=std::string::npos);
    std::vector<llvm::StringRef> supported;
    for(const auto capability:SupportedSemanticInterfaceCapabilities())supported.push_back(capability);
    auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,supported);assert(decoded);
    std::set<std::string> variants,families;
    for(const auto &declaration:decoded->declarations)if(declaration.qualified_name=="Dispatch::versioned") {
      variants.insert(declaration.stable_id);families.insert(declaration.native_symbol_name);
    }
    assert(variants.size()==2 && families.size()==1);
    for(int mutation=0;mutation!=4;++mutation) {
      auto invalid=*decoded;
      auto declaration=std::find_if(invalid.declarations.begin(),invalid.declarations.end(),
          [](const auto &d){return d.qualified_name=="Dispatch::versioned";});
      assert(declaration!=invalid.declarations.end());
      if(mutation==0)declaration->stable_id+="0";
      if(mutation==1)declaration->stable_id+="@cpu-target@"+std::string(64,'a');
      if(mutation==2)declaration->kind="Namespace";
      if(mutation==3) {
        auto &identity=declaration->canonical_type_identity;
        const std::string marker=";native-cpu-variant:v1;";
        identity.erase(identity.find(marker),marker.size());
      }
      auto encoded=EncodeSemanticInterfaceSidecar(invalid,supported);
      assert(!encoded);llvm::consumeError(encoded.takeError());
    }
    const auto interface=root/(std::string(standard)+".objc3i");
    {std::ofstream file(interface);file<<artifact.source_bytes;}
    {std::ofstream file(interface.string()+".json");file<<artifact.sidecar_json;}
    provider.source_text.reset();
    for(const char *name:{"a","b"}) {
      CompileRequest consumer=provider;
      consumer.input_path=std::string("aarch64-consumer-")+name+".mm";
      consumer.module_owned_names=false;consumer.emit_semantic_interface=false;consumer.module_name.clear();
      consumer.interface_exports.clear();consumer.emit=EmitKind::LLVM;
      consumer.output_path=(root/(std::string(standard)+name+".ll")).string();
      consumer.source_text="int consume(int value){return Dispatch::versioned(value)+Dispatch::cloned(value)+Dispatch::vector(value)+Dispatch::invoke(value);}";
      InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
      consumer.imported_interfaces={imported};
      auto result=Compile(consumer);test::ReportFailure(result);assert(result.success);
      std::ifstream file(consumer.output_path);const std::string ir{std::istreambuf_iterator<char>(file),{}};
      assert(ir.find("ifunc i32")!=std::string::npos && ir.find(".resolver")!=std::string::npos);
      assert(ir.find("call aarch64_vector_pcs")!=std::string::npos);
    }
  }

}
