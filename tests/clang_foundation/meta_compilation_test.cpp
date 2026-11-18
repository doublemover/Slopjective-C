#include "clang_foundation/compile_session.h"
#include "clang_foundation/meta_package.h"
#include "clang_foundation/frontend_api.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace objc3::clang_foundation;
namespace fs=std::filesystem;
namespace {
std::string JSON(const llvm::json::Value &value) {
  std::string bytes;llvm::raw_string_ostream out(bytes);out<<value;out.flush();return bytes;
}
void Write(const fs::path &path,const std::string &bytes) {std::ofstream out(path);out<<bytes;assert(out.good());}
void Success(const CompileResult &result) {
  if(!result.success){std::cerr<<result.failure<<'\n';for(const auto &D:result.diagnostics)std::cerr<<D.message<<'\n';
    if(result.interface_artifact)for(const auto &D:result.interface_artifact->diagnostics)std::cerr<<D.code<<": "<<D.message<<'\n';
    for(const auto &D:result.interface_import_diagnostics)std::cerr<<D.code<<": "<<D.message<<'\n';}
  assert(result.success);
}
void Execute(const std::vector<std::string> &command) {
  std::vector<llvm::StringRef> args(command.begin(),command.end());
  auto status=llvm::sys::ExecuteAndWait(command.front(),args);
  if(status)std::cerr<<"native metaprogramming witness failed: "<<command.front()<<" ("<<status<<")\n";
  assert(status==0);
}
llvm::json::Object Text(const char *text){return llvm::json::Object{{"text",text}};}
}
int main() {
  llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-meta-expansion",temporary));
  fs::path root(temporary.c_str());struct Cleanup{fs::path path;~Cleanup(){fs::remove_all(path);}}cleanup{root};
  llvm::json::Value document(llvm::json::Object{
    {"contract_id","objc3c.meta.package.v1"},{"id","test.ast"},{"version","1.0.0"},
    {"requires",llvm::json::Object{{"meta_revision",1},{"extension_abi",24}}},
    {"capabilities",llvm::json::Array{}},
    {"entrypoints",llvm::json::Array{
      llvm::json::Object{{"name","Snapshot"},{"kind","derive"},{"requires",llvm::json::Array{}},
        {"protocols",llvm::json::Array{"GeneratedProtocol"}},
        {"declarations",llvm::json::Array{Text("- (int)checksum;")}},
        {"implementations",llvm::json::Array{Text("- (int)checksum { return 0"),
          llvm::json::Object{{"for_each","properties"},{"where",llvm::json::Object{{"kind","integer"}}},
            {"body",llvm::json::Array{Text(" + self.${member.getter}")}}},Text("; }")}}},
      llvm::json::Object{{"name","FortyTwo"},{"kind","macro"},{"requires",llvm::json::Array{}},
        {"declarations",llvm::json::Array{Text("- (int)answer;")}},
        {"implementations",llvm::json::Array{Text("- (int)answer { return META_VALUE; }")}}},
      llvm::json::Object{{"name","Doubled"},{"kind","property-behavior"},{"requires",llvm::json::Array{}},
        {"dispatch","dynamic"},{"executor","none"},{"declarations",llvm::json::Array{}},
        {"implementations",llvm::json::Array{llvm::json::Object{{"for_each","properties"},
          {"body",llvm::json::Array{Text("- (${member.type})${member.getter} { return self.value * 2; }")}}}}}},
      llvm::json::Object{{"name","WrongDispatch"},{"kind","property-behavior"},{"requires",llvm::json::Array{}},
        {"dispatch","direct"},{"executor","none"},{"declarations",llvm::json::Array{}},
        {"implementations",llvm::json::Array{Text("- (int)broken { return 1; }")}}},
      llvm::json::Object{{"name","MissingAccessor"},{"kind","property-behavior"},{"requires",llvm::json::Array{}},
        {"dispatch","dynamic"},{"executor","none"},{"declarations",llvm::json::Array{}},
        {"implementations",llvm::json::Array{Text("- (int)unrelated { return 1; }")}}}
    }}});
  std::string error;auto fingerprint=ComputeMetaPackageFingerprint(JSON(document),error);assert(fingerprint);
  (*document.getAsObject())["fingerprint"]=*fingerprint;
  const auto manifest=root/"objc3-meta.json",lock=root/"lock.json";
  Write(manifest,JSON(document));
  Write(lock,JSON(llvm::json::Object{{"contract_id","objc3c.meta.lock.v1"},
    {"packages",llvm::json::Array{llvm::json::Object{{"id","test.ast"},{"version","1.0.0"},{"fingerprint",*fingerprint}}}}}));
  CompileRequest request;request.input_path=(root/"module.m").string();request.objc3=true;
  request.objc_runtime="gnustep-2.2";request.meta_packages={root.string()};request.meta_lock=lock.string();
  request.source_text=R"source(
@protocol GeneratedProtocol
- (int)checksum;
@end
__attribute__((objc_macro)) void marker(void);
enum { META_VALUE = 42 };
#define META_VALUE __DATE__
__attribute__((objc_root_class,objc_derive("test.ast::Snapshot"),objc_expand("FortyTwo")))
@interface Counter <GeneratedProtocol> { Class isa; }
@property(nonatomic) int value;
@property(nonatomic) int other;
@end
#define checksum compiler_must_not_expand_this_macro
@implementation Counter
@end
#undef checksum
int invoke(Counter *value) { return [value checksum] + [value answer]; }
)source";
  for(bool arc:{false,true}) for(int mode:{0,1,2}) {
    request.language=mode?BaseLanguage::ObjectiveCxx:BaseLanguage::ObjectiveC;
    request.arguments=mode?std::vector<std::string>{mode==1?"-std=c++20":"-std=c++23"}:std::vector<std::string>{"-std=c17"};
    request.arguments.insert(request.arguments.end(),{"-MD","-MF",(root/"module.d").string()});
    request.arc=arc;Success(Compile(request));
    {std::ifstream dep(root/"module.d");std::string bytes((std::istreambuf_iterator<char>(dep)),{});
      assert(bytes.find(manifest.string())!=std::string::npos && bytes.find(lock.string())!=std::string::npos);
      assert(bytes.find(".objc3-meta-")==std::string::npos);}

    request.emit=EmitKind::LLVM;request.output_path=(root/(arc?"arc.ll":"mrc.ll")).string();
    Success(Compile(request));
    std::ifstream input(request.output_path);std::string ir((std::istreambuf_iterator<char>(input)),{});
    assert(ir.find("checksum")!=std::string::npos && ir.find("answer")!=std::string::npos);
    assert(ir.find("ret i32 42")!=std::string::npos);
    assert(!Compile(request).semantic_dependency_record.empty());
    auto object_request=request;object_request.emit=EmitKind::Object;object_request.output_path=(root/"provider.o").string();
    auto object=Compile(object_request);Success(object);
    Write(root/"main.c",R"witness(
extern void *objc_getClass(const char *);
extern void *class_createInstance(void *,__SIZE_TYPE__);
extern void *sel_registerName(const char *);
extern void *objc_msgSend(void *,void *,...);
extern void *object_dispose(void *);
int main(void) {
  void *cls=objc_getClass("Counter");if(!cls)return 1;
  void *value=class_createInstance(cls,0);if(!value)return 2;
  const char *names[]={"setValue:","setOther:"};
  for(int i=0;i<2;++i) {
    void *selector=sel_registerName(names[i]);
    ((void (*)(void *,void *,int))objc_msgSend)(value,selector,21);
  }
  const char *getters[]={"checksum","answer"};
  for(int i=0;i<2;++i) {
    void *selector=sel_registerName(getters[i]);
    if(((int (*)(void *,void *))objc_msgSend)(value,selector)!=42)return 3+i;
  }
  object_dispose(value);return 0;
}
)witness");
    const fs::path runtime=OBJC3_META_TEST_OBJC_LIBRARY;
    Execute({object.compiler_driver_path,"--no-default-config","-fuse-ld=lld",object_request.output_path,
      (root/"main.c").string(),runtime.string(),"-Wl,-rpath,"+runtime.parent_path().string(),"-o",(root/"witness").string()});
    Execute({(root/"witness").string()});
    request.emit=EmitKind::Syntax;request.output_path.clear();
    auto exported=request;exported.emit_semantic_interface=true;
    exported.module_name="test.Generated";exported.package_identity="test-meta-v1";
    exported.interface_exports={"Counter","GeneratedProtocol","marker"};
    auto provider=Compile(exported);Success(provider);
    assert(provider.interface_artifact && provider.interface_artifact->success);
    const auto &artifact=*provider.interface_artifact;
    assert(artifact.source_bytes.find("objc3.meta.expanded.v1:")!=std::string::npos);
    assert(artifact.source_bytes.find("objc_derive(")==std::string::npos);
    assert(artifact.source_bytes.find("objc_macro")!=std::string::npos);
    assert(artifact.sidecar_json.find(kMetaprogrammingSemantic)!=std::string::npos);
    auto capabilities=SupportedSemanticInterfaceCapabilities();
    std::vector<llvm::StringRef> restricted;
    for(auto capability:capabilities)if(capability!=kMetaprogrammingSemantic)restricted.push_back(capability);
    auto unsupported=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,restricted);
    assert(!unsupported);llvm::consumeError(unsupported.takeError());
    auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,capabilities);assert(decoded);
    auto forged=*decoded;std::erase(forged.required_semantics,std::string(kMetaprogrammingSemantic));
    forged.module.semantic_fingerprint=ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(forged));
    std::vector<std::string> supported;for(auto C:capabilities)supported.push_back(C.str());
    assert(!ValidateSemanticInterface(forged,forged.module,forged.canonical_source_sha256,forged.declarations,supported).ok());

    auto interface=root/"Generated.objc3i";
    Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
    fs::rename(manifest,root/"hidden-manifest.json");
    CompileRequest consumer;consumer.input_path=(root/"consumer.m").string();consumer.objc3=true;
    consumer.language=request.language;consumer.arguments=request.arguments;
    consumer.arc=arc;consumer.objc_runtime="gnustep-2.2";
    consumer.source_text="int consume(Counter *value){return [value checksum]+[value answer];}";
    InterfaceImportInput imported_input;imported_input.source_path=interface.string();
    imported_input.sidecar_path=interface.string()+".json";imported_input.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported_input};
    auto imported=Compile(consumer);
    fs::rename(root/"hidden-manifest.json",manifest);
    Success(imported);
    assert(!imported.imported_declarations.empty());
  }
  objc3c_frontend_v3_options options{};options.struct_size=sizeof(options);options.abi_version=OBJC3C_FRONTEND_V3_ABI;
  options.base.struct_size=sizeof(options.base);options.base.abi_version=OBJC3C_FRONTEND_V2_ABI;
  options.base.input_path=request.input_path.c_str();options.base.source=request.source_text->c_str();
  options.base.source_size=request.source_text->size();options.base.objc3=1;options.base.language=2;
  options.base.objc_runtime="gnustep-2.2";
  const auto package_path=root.string(),lock_path=lock.string();const char *paths[]={package_path.c_str()};
  options.meta_packages=paths;options.meta_package_count=1;options.meta_lock=lock_path.c_str();
  objc3c_frontend_v2_result *handle=nullptr;
  assert(objc3c_frontend_v3_compile(&options,&handle)==0 && handle);
  auto record=objc3c_frontend_v3_result_dependency_record(handle);
  assert(record && std::string(record).find(manifest.string())!=std::string::npos);
  objc3c_frontend_v2_result_destroy(handle);handle=nullptr;
  auto invalid=options;invalid.meta_packages=nullptr;
  assert(objc3c_frontend_v3_compile(&invalid,&handle)==2 && !handle);
  invalid=options;invalid.meta_lock=nullptr;
  assert(objc3c_frontend_v3_compile(&invalid,&handle)==2 && !handle);
  invalid=options;invalid.struct_size=offsetof(objc3c_frontend_v3_options,meta_lock);
  assert(objc3c_frontend_v3_compile(&invalid,&handle)==2 && !handle);
  for(const char *standard:{"c++20","c++23"}) {
    CompileRequest entries;entries.input_path=(root/"entrypoints.cpp").string();
    entries.language=BaseLanguage::Cxx;entries.arguments={std::string("-std=")+standard};
    entries.emit_semantic_interface=true;entries.module_name="test.MetaEntries";entries.package_identity="meta-entries-v1";
    entries.interface_exports={"meta_template","MetaEntries","MetaGenericEntries","meta_redeclared"};
    entries.source_text=R"entries(
template<class T> __attribute__((objc_macro)) inline constexpr T meta_template(T value){return value;}
template<class T> __attribute__((objc_macro)) inline constexpr T meta_redeclared(T value);
template<class T> inline constexpr T meta_redeclared(T value){return value;}
struct MetaEntries {
 __attribute__((objc_macro)) static constexpr int concrete(int value){return value+1;}
 template<class T> __attribute__((objc_macro)) static constexpr T generic(T value){return value;}
 __attribute__((objc_macro)) constexpr int member(int value)const{return value;}
 static constexpr int outlined(int value);
 struct Nested {__attribute__((objc_macro)) static constexpr int nested(int value){return value;}};
};
__attribute__((objc_macro)) constexpr int MetaEntries::outlined(int value){return value;}
template<class T>struct MetaGenericEntries {
 __attribute__((objc_macro)) static constexpr T invoke(T value){return value;}
};
)entries";
    auto built=Compile(entries);Success(built);assert(built.interface_artifact);
    const auto &artifact=*built.interface_artifact;
    auto sidecar=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,SupportedSemanticInterfaceCapabilities());assert(sidecar);
    bool template_marked=false,record_marked=false,class_template_marked=false,redeclared_marked=false;
    std::string original_record_identity;
    for(const auto &D:sidecar->declarations) {
      if(D.qualified_name=="meta_redeclared")redeclared_marked=HasMetaprogrammingIdentity(D);
      if(D.qualified_name=="meta_template")template_marked=HasMetaprogrammingIdentity(D);
      if(D.qualified_name=="MetaGenericEntries")class_template_marked=HasMetaprogrammingIdentity(D);
      if(D.qualified_name=="MetaEntries") {
        original_record_identity=D.canonical_type_identity;
        record_marked=HasMetaprogrammingIdentity(D);
        auto section=D.canonical_type_identity.substr(D.canonical_type_identity.find(";expanded-metaprogramming:v1;"));
        for(const char *name:{"concrete","generic","member","nested","outlined"})assert(section.find(name)!=std::string::npos);
      }
    }
    assert(template_marked && record_marked && class_template_marked && redeclared_marked);
    auto altered=entries;
    const std::string marker="__attribute__((objc_macro)) constexpr int member";
    auto position=altered.source_text->find(marker);assert(position!=std::string::npos);
    altered.source_text->replace(position,marker.size(),"constexpr int member");
    auto changed=Compile(altered);Success(changed);assert(changed.interface_artifact);
    auto changed_sidecar=DecodeSemanticInterfaceSidecar(changed.interface_artifact->sidecar_json,
        changed.interface_artifact->source_bytes,SupportedSemanticInterfaceCapabilities());assert(changed_sidecar);
    auto changed_record=std::find_if(changed_sidecar->declarations.begin(),changed_sidecar->declarations.end(),[](const auto &D){return D.qualified_name=="MetaEntries";});
    assert(changed_record!=changed_sidecar->declarations.end() && changed_record->canonical_type_identity!=original_record_identity);
    auto forged=*sidecar;
    auto forged_owner=std::find_if(forged.declarations.begin(),forged.declarations.end(),[](const auto &D){return D.qualified_name=="MetaEntries";});
    assert(forged_owner!=forged.declarations.end());forged_owner->kind="Var";
    forged.module.semantic_fingerprint=ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(forged));
    std::vector<std::string> supported;for(auto C:SupportedSemanticInterfaceCapabilities())supported.push_back(C.str());
    auto rejected_owner=ValidateSemanticInterface(forged,forged.module,forged.canonical_source_sha256,forged.declarations,supported);
    assert(std::any_of(rejected_owner.diagnostics.begin(),rejected_owner.diagnostics.end(),[](const auto &D){return D.code=="O3IFC145" && D.message.find("metaprogramming")!=std::string::npos;}));
    auto interface=root/"MetaEntries.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
    auto consumer=entries;consumer.emit_semantic_interface=false;consumer.interface_exports.clear();
    consumer.input_path=(root/"entry-consumer.cpp").string();
    consumer.source_text="static_assert(meta_template(42)==42);static_assert(MetaEntries::concrete(41)==42);static_assert(MetaEntries::generic(42)==42);constexpr MetaEntries entry;static_assert(entry.member(42)==42);static_assert(MetaEntries::Nested::nested(42)==42);static_assert(MetaGenericEntries<int>::invoke(42)==42);static_assert(meta_redeclared(42)==42);static_assert(MetaEntries::outlined(42)==42);";
    InterfaceImportInput input;input.source_path=interface.string();input.sidecar_path=interface.string()+".json";input.locked_identity=artifact.identity;
    consumer.imported_interfaces={input};Success(Compile(consumer));
  }
  auto behavior=request;behavior.source_text=R"source(
__attribute__((objc_root_class)) @interface Behavior
@property(nonatomic) int value;
@property(nonatomic,readonly) int twice __attribute__((objc_property_behavior("Doubled")));
@end
@implementation Behavior @end
int use(Behavior *value){return value.twice;}
)source";
  Success(Compile(behavior));
  auto directBehavior=request;directBehavior.source_text=R"source(
__attribute__((objc_root_class,objc_direct_members)) @interface DirectBehavior
@property(nonatomic,readonly) int broken __attribute__((objc_property_behavior("WrongDispatch")));
@end
@implementation DirectBehavior @end
)source";
  Success(Compile(directBehavior));
  auto badBehavior=behavior;badBehavior.source_text=R"source(
__attribute__((objc_root_class)) @interface Broken
@property(nonatomic,readonly) int broken __attribute__((objc_property_behavior("WrongDispatch")));
@end
@implementation Broken @end
)source";
  assert(!Compile(badBehavior).success);
  badBehavior.source_text=R"source(
__attribute__((objc_root_class)) @interface Broken
@property(nonatomic,readonly) int broken __attribute__((objc_property_behavior("MissingAccessor")));
@end
@implementation Broken @end
)source";
  assert(!Compile(badBehavior).success);
  auto conflict=request;conflict.source_text=R"source(
__attribute__((objc_root_class,objc_expand("FortyTwo"))) @interface Conflict
- (int)answer;
@end
)source";
  assert(!Compile(conflict).success);
  auto unknown=request;unknown.source_text="__attribute__((objc_derive(\"Missing\"),objc_root_class)) @interface Missing @end";
  Write(root/"module.d","keep-old-dependencies");
  auto rejected=Compile(unknown);assert(!rejected.success && rejected.failure.find("unresolved")!=std::string::npos);
  {std::ifstream input(root/"module.d");std::string text;std::getline(input,text);assert(text=="keep-old-dependencies");}
  auto dependencyCollision=request;dependencyCollision.arguments={"-MD","-MF",manifest.string()};
  assert(!Compile(dependencyCollision).success);
  auto collision=request;collision.emit=EmitKind::Object;collision.output_path=manifest.string();
  rejected=Compile(collision);assert(!rejected.success && rejected.failure.find("aliases")!=std::string::npos);
  auto duplicate=request;duplicate.source_text="__attribute__((objc_root_class,objc_expand(\"FortyTwo\"),objc_expand(\"FortyTwo\"))) @interface Duplicate @end";
  rejected=Compile(duplicate);assert(!rejected.success && rejected.failure.find("duplicate")!=std::string::npos);
  auto noPackages=request;noPackages.meta_packages.clear();noPackages.meta_lock.clear();
  assert(!Compile(noPackages).success);
}
