#include "native_target_test_support.h"
#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace objc3::clang_foundation;
namespace fs=std::filesystem;
namespace {
void Write(const fs::path &P,const std::string &S){std::ofstream F(P);F<<S;assert(F.good());}
void Success(const CompileResult &R){if(!R.success){std::cerr<<R.failure<<'\n';for(const auto &D:R.diagnostics)std::cerr<<D.message<<'\n';if(R.interface_artifact)for(const auto &D:R.interface_artifact->diagnostics)std::cerr<<D.message<<'\n';}assert(R.success);}
void Execute(const std::vector<std::string> &Command){std::vector<llvm::StringRef> args(Command.begin(),Command.end());auto status=llvm::sys::ExecuteAndWait(Command.front(),args);assert(status==0);}
}
int main(){
 llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-transparent-union",temporary));
 fs::path root(temporary.c_str());struct Cleanup{fs::path path;~Cleanup(){fs::remove_all(path);}}cleanup{root};
 CompileRequest provider;provider.input_path=(root/"provider.c").string();provider.language=BaseLanguage::C;
 provider.target_triple=OBJC3_TEST_NATIVE_LINUX_TARGET;provider.arguments={"-std=c17"};
 provider.module_name="test.Transparent";provider.package_identity="transparent-union-v1";
 provider.interface_exports={"NativePointerStorage","NativePointerArgument","pointer_value"};
 provider.emit_semantic_interface=true;provider.emit=EmitKind::Object;provider.output_path=(root/"provider.o").string();
 provider.source_text=R"source(
union __attribute__((transparent_union)) NativePointerStorage {void *opaque;int *integer;const char *bytes;};
typedef union NativePointerStorage NativePointerArgument;
int pointer_value(NativePointerArgument value){return *(int *)value.opaque;}
)source";
 for(const char *standard:{"c17","c23"}) {
 provider.arguments={std::string("-std=")+standard};
 auto produced=Compile(provider);Success(produced);assert(produced.interface_artifact && produced.interface_artifact->success);
 const auto &artifact=*produced.interface_artifact;assert(artifact.source_bytes.find("transparent_union")!=std::string::npos);
 auto interface=root/"Transparent.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);

  CompileRequest consumer;consumer.input_path=(root/"consumer.c").string();consumer.language=BaseLanguage::C;
  consumer.target_triple=OBJC3_TEST_NATIVE_LINUX_TARGET;consumer.arguments={std::string("-std=")+standard};
  InterfaceImportInput input;input.source_path=interface.string();input.sidecar_path=interface.string()+".json";input.locked_identity=artifact.identity;consumer.imported_interfaces={input};
  consumer.source_text="int transparent_consume(int *value){return pointer_value(value);}";
  consumer.emit=EmitKind::Object;consumer.output_path=(root/"consumer.o").string();auto consumed=Compile(consumer);Success(consumed);
  Write(root/"main.c","int transparent_consume(int *);int main(void){int value=42;return transparent_consume(&value)==42?0:1;}");
  Execute({produced.compiler_driver_path,"--no-default-config","-fuse-ld=lld",provider.output_path,consumer.output_path,(root/"main.c").string(),"-o",(root/"witness").string()});Execute({(root/"witness").string()});
  auto invalid=consumer;invalid.emit=EmitKind::Syntax;invalid.output_path.clear();invalid.source_text="int rejected(double value){return pointer_value(value);}";assert(!Compile(invalid).success);
  auto wrong_revision=consumer;wrong_revision.emit=EmitKind::Syntax;wrong_revision.output_path.clear();wrong_revision.arguments={std::string(standard)=="c17"?"-std=c23":"-std=c17"};
  auto rejected=Compile(wrong_revision);assert(!rejected.success && rejected.failure.find("O3IFC135")!=std::string::npos);
  auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,SupportedSemanticInterfaceCapabilities());assert(decoded);
  std::string original_type;for(const auto &D:decoded->declarations)if(D.qualified_name=="NativePointerStorage")original_type=D.canonical_type_identity;
  assert(original_type.find(";native-declaration-attributes:v6;")!=std::string::npos);
  auto changed=provider;changed.emit=EmitKind::Syntax;changed.output_path.clear();const std::string attribute="__attribute__((transparent_union)) ";
  auto offset=changed.source_text->find(attribute);assert(offset!=std::string::npos);changed.source_text->erase(offset,attribute.size());
  auto ordinary=Compile(changed);Success(ordinary);assert(ordinary.interface_artifact);
  auto ordinary_api=DecodeSemanticInterfaceSidecar(ordinary.interface_artifact->sidecar_json,ordinary.interface_artifact->source_bytes,SupportedSemanticInterfaceCapabilities());assert(ordinary_api);
  bool changed_type=false;for(const auto &D:ordinary_api->declarations)if(D.qualified_name=="NativePointerStorage")changed_type=D.canonical_type_identity!=original_type;
  assert(changed_type);
 }
}
