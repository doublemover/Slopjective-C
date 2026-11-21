#include "native_target_test_support.h"
#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace objc3::clang_foundation;
namespace fs=std::filesystem;
namespace {
void Success(const CompileResult &R) {
 if(!R.success){std::cerr<<R.failure<<'\n';for(const auto &D:R.diagnostics)std::cerr<<D.message<<'\n';
  if(R.interface_artifact)for(const auto &D:R.interface_artifact->diagnostics)std::cerr<<D.message<<'\n';}
 assert(R.success);
}
void Write(const fs::path &P,const std::string &S){std::ofstream F(P);F<<S;assert(F.good());}
}
int main() {
 llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-weak-policy",temporary));
 fs::path root(temporary.c_str());struct Cleanup{fs::path path;~Cleanup(){fs::remove_all(path);}}cleanup{root};
 for(bool arc:{false,true})for(const char *standard:{"c17","c++20","c++23"}) {
  CompileRequest R;R.input_path=(root/"provider.m").string();R.objc3=true;R.arc=arc;
  R.language=std::string(standard)=="c17"?BaseLanguage::ObjectiveC:BaseLanguage::ObjectiveCxx;
  R.target_triple=OBJC3_TEST_NATIVE_LINUX_TARGET;R.objc_runtime="gnustep-2.2";
  R.arguments={std::string("-std=")+standard,"-fobjc-weak"};
  R.emit_semantic_interface=true;R.module_name="test.WeakPolicy";R.package_identity="weak-policy-v1";
  R.interface_exports={"WeakRoot","WeakLeaf","OrdinaryRoot"};
  R.source_text=R"source(
__attribute__((objc_root_class,objc_arc_weak_reference_unavailable)) @interface WeakRoot @end
@interface WeakLeaf : WeakRoot @end
__attribute__((objc_root_class)) @interface OrdinaryRoot @end
)source";
  auto provider=Compile(R);Success(provider);assert(provider.interface_artifact && provider.interface_artifact->success);
  const auto &artifact=*provider.interface_artifact;
  assert(artifact.source_bytes.find("objc_arc_weak_reference_unavailable")!=std::string::npos);
  auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,SupportedSemanticInterfaceCapabilities());assert(decoded);
  bool policy=false;for(const auto &D:decoded->declarations)if(D.qualified_name=="WeakRoot")policy=D.canonical_type_identity.find(";native-object-policies:v1;")!=std::string::npos;
  assert(policy);
  auto interface=root/"WeakPolicy.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  auto consumer=R;consumer.input_path=(root/"consumer.m").string();consumer.emit_semantic_interface=false;consumer.interface_exports.clear();
  InterfaceImportInput input;input.source_path=interface.string();input.sidecar_path=interface.string()+".json";input.locked_identity=artifact.identity;
  consumer.imported_interfaces={input};
  consumer.source_text="void ordinary(OrdinaryRoot *value){__weak OrdinaryRoot *reference=value;(void)reference;}void owned(WeakLeaf *value){__strong WeakLeaf *reference=value;(void)reference;}";
  Success(Compile(consumer));
  for(const char *type:{"WeakRoot","WeakLeaf"}) {
   consumer.source_text=std::string("void rejected(")+type+" *value){__weak "+type+" *reference=value;(void)reference;}";
   auto rejected=Compile(consumer);assert(!rejected.success);
   bool diagnosed=false;for(const auto &D:rejected.diagnostics)diagnosed|=D.message.find("weak")!=std::string::npos;
   assert(diagnosed);
  }
 }
}
