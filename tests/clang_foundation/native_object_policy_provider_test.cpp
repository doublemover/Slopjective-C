#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>
#include <tuple>
using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
template<class T>bool Fails(llvm::Expected<T> value){if(value)return false;llvm::consumeError(value.takeError());return true;}
constexpr const char *Source=R"policies(
__attribute__((objc_protocol_requires_explicit_implementation)) @protocol PolicyExplicit
- (int)required;
@end
__attribute__((objc_non_runtime_protocol)) @protocol PolicyStatic
- (int)typed;
@end
__attribute__((objc_root_class,objc_requires_property_definitions)) @interface PolicyRoot<PolicyExplicit> {
 Class isa;
@public int stored;
}
+ (id)new;
+ (id)alloc;
+ (id)make __attribute__((objc_method_family(new)));
- (id)init __attribute__((objc_designated_initializer));
- (void)refresh __attribute__((objc_requires_super));
- (int)required;
@property(nonatomic) int value;
@end
__attribute__((objc_root_class)) @interface PolicyUnconformed - (int)required; @end
@interface PolicyChild:PolicyRoot @end
__attribute__((objc_subclassing_restricted)) @interface PolicyLeaf:PolicyRoot @end
__attribute__((objc_nonlazy_class)) @interface PolicyEager:PolicyRoot @end
__attribute__((objc_runtime_visible)) @interface PolicyVisible:PolicyRoot @end
struct __attribute__((objc_boxable)) PolicyBox {int value;};
__attribute__((objc_root_class)) @interface NSValue {Class isa;@public int boxed;}
- (int)intValue;
+ (NSValue *)valueWithBytes:(const void *)value objCType:(const char *)type;
@end
inline NSValue *policy_box(struct PolicyBox value){return @(value);}
inline int policy_read(PolicyRoot *object){
 id precise __attribute__((objc_precise_lifetime))=object;
 [object refresh];return precise!=0?object.value+[object required]:-1;
}
#ifdef __cplusplus
extern "C" {
#endif
extern unsigned policy_destroyed,policy_loaded,policy_refreshes,policy_boxed_destroyed;
extern id policy_owner;
void policy_drop_owner(void);
void policy_release(void *);
#ifdef __cplusplus
}
#endif
inline unsigned policy_precise_hold(__unsafe_unretained PolicyRoot *object,void (*drop)(void)){
 id precise __attribute__((objc_precise_lifetime))=object;drop();return policy_destroyed;
}
)policies";
constexpr const char *Consumer=R"policies(
#include <assert.h>
static int execute(void){
 assert(policy_loaded==1);
 unsigned before=policy_destroyed,refreshes=policy_refreshes,boxes=policy_boxed_destroyed;
 @autoreleasepool {
  NSValue *boxed=policy_box((struct PolicyBox){42});assert([boxed intValue]==42);
  PolicyChild *value=[PolicyChild make];assert(value->stored==40);
  value.value=41;assert(policy_read(value)==84);assert(policy_refreshes==refreshes+2);
#if !__has_feature(objc_arc)
  policy_release((__bridge void *)value);
#endif
 }
 assert(policy_destroyed==before+1 && policy_boxed_destroyed==boxes+1);
#if __has_feature(objc_arc)
 policy_owner=[PolicyRoot new];
 assert(policy_precise_hold(policy_owner,policy_drop_owner)==before+1);
 assert(policy_destroyed==before+2);
#endif
 return 0;
}
)policies";
constexpr const char *Main=R"policies(
#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif
id class_createInstance(Class,__SIZE_TYPE__) __attribute__((ns_returns_retained));
id object_dispose(__unsafe_unretained id);
void objc_release(void *);
id objc_autorelease(id);
unsigned policy_destroyed=0,policy_loaded=0,policy_refreshes=0,policy_boxed_destroyed=0;
id policy_owner=0;
void policy_drop_owner(void){policy_owner=0;}
void policy_release(void *value){objc_release(value);}
int check_a(void);int check_b(void);
#ifdef __cplusplus
}
#endif
@implementation NSValue
+ (NSValue *)valueWithBytes:(const void *)bytes objCType:(const char *)type{
 assert(bytes && type);NSValue *value=class_createInstance(self,0);value->boxed=((const struct PolicyBox *)bytes)->value;
#if __has_feature(objc_arc)
 return value;
#else
 return objc_autorelease(value);
#endif
}
- (int)intValue{return boxed;}
- (void)dealloc{++policy_boxed_destroyed;object_dispose(self);}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease{}
#endif
@end
@implementation PolicyRoot
+ (id)new{return [[self alloc] init];}
+ (id)make{return [[self alloc] init];}
+ (id)alloc{return class_createInstance(self,0);}
- (id)init{stored=40;return self;}
- (int)required{return 42;}
- (int)value{return stored;}
- (void)setValue:(int)value{stored=value;}
- (void)refresh{++policy_refreshes;}
- (void)dealloc{++policy_destroyed;object_dispose(self);}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease{}
#endif
@end
@implementation PolicyChild
- (void)refresh{[super refresh];++policy_refreshes;++stored;}
@end
@implementation PolicyEager
+ (void)load{++policy_loaded;}
@end
int main(void){return check_a() || check_b();}
)policies";
void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");
 auto request=[&](const fs::path &input,const fs::path &output){auto value=Request(input,output,standard,ownership,"-O2",{});
  if(std::string(standard)=="c17")value.language=BaseLanguage::ObjectiveC;return value;};
 const auto source=root/"provider/policies.mm";
 auto publish=[&](const std::string &text,bool owned=true){Write(source,text);auto value=request(source,root/"provider.o");
  value.module_name="fixture.ObjectPolicies";value.package_identity="object-policies-v1";value.module_owned_names=owned;
  if(owned)value.module_source_root=(root/"provider").string();value.emit_semantic_interface=true;
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;};
 const auto artifact=publish(Source),replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 const auto &reader=SupportedSemanticInterfaceCapabilities();auto metadata=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(metadata);
 assert(std::find(metadata->required_semantics.begin(),metadata->required_semantics.end(),kNativeObjectPolicySemantic)!=metadata->required_semantics.end());
 std::vector<llvm::StringRef> old(reader.begin(),reader.end());std::erase(old,kNativeObjectPolicySemantic);assert(Fails(DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,old)));
 auto missing=*metadata;std::erase(missing.required_semantics,kNativeObjectPolicySemantic);assert(Fails(EncodeSemanticInterfaceSidecar(missing,reader)));
 for(const char *kind:{"Namespace","Enum","EnumConstant"}){auto forged=*metadata;auto row=std::find_if(forged.declarations.begin(),forged.declarations.end(),HasNativeObjectPolicyIdentity);assert(row!=forged.declarations.end());row->kind=kind;assert(Fails(EncodeSemanticInterfaceSidecar(forged,reader)));}
 for(const char *name:{"objc_designated_initializer","objc_requires_super","objc_subclassing_restricted","objc_requires_property_definitions",
     "objc_protocol_requires_explicit_implementation","objc_non_runtime_protocol","objc_boxable","objc_precise_lifetime","objc_nonlazy_class","objc_runtime_visible","objc_method_family"})
  assert(artifact.source_bytes.find(name)!=std::string::npos);
 const auto hidden=root/"hidden.mm";
 Write(hidden,"__attribute__((objc_root_class)) @interface HiddenPolicyRoot @end\nint hidden_body(HiddenPolicyRoot *object){id held __attribute__((objc_precise_lifetime))=object;return held!=0;}\n");
 auto hidden_request=request(hidden,root/"hidden.o");hidden_request.module_name="fixture.HiddenPolicies";hidden_request.package_identity="hidden-policies-v1";hidden_request.emit_semantic_interface=true;
 auto hidden_result=Compile(hidden_request);Success(hidden_result);assert(hidden_result.interface_artifact && hidden_result.interface_artifact->success);
 auto hidden_metadata=DecodeSemanticInterfaceSidecar(hidden_result.interface_artifact->sidecar_json,hidden_result.interface_artifact->source_bytes,reader);assert(hidden_metadata);
 assert(std::find(hidden_metadata->required_semantics.begin(),hidden_metadata->required_semantics.end(),kNativeObjectPolicySemantic)==hidden_metadata->required_semantics.end());
 publish(Source);assert(fs::remove(source));const auto interface=root/"ObjectPolicies.objc3i";
 Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
 auto imported=[&](const fs::path &input,const fs::path &output){auto value=request(input,output);value.imported_interfaces={dependency};return value;};
 for(const char *name:{"a","b"}){const auto input=root/(std::string(name)+".mm");auto text=std::string(Consumer);
  if(std::string(standard)!="c17")text+="extern \"C\" ";text+="int check_"+std::string(name)+"(void){return execute();}\n";
  Write(input,text);Success(Compile(imported(input,root/(std::string(name)+".o"))));}
 const auto boxed=root/"boxed.mm";Write(boxed,"NSValue *boxed_value(void){return policy_box((struct PolicyBox){42});}\n");
 auto box_request=imported(boxed,root/"boxed.o");box_request.emit=EmitKind::Syntax;Success(Compile(box_request));
 unsigned ordinal=0;
 for(const auto &[text,option,message]:std::vector<std::tuple<std::string,std::string,std::string>>{
     {"@interface BadLeaf:PolicyLeaf @end","","cannot subclass"},
     {"@interface BadRefresh:PolicyRoot @end\n@implementation BadRefresh\n- (void)refresh{}\n@end","-Werror=objc-missing-super-calls","missing a [super"},
     {"@interface BadInit:PolicyRoot\n- (id)initWithNumber:(int)value __attribute__((objc_designated_initializer));\n@end\n@implementation BadInit\n- (id)initWithNumber:(int)value{return self;}\n@end","-Werror=objc-designated-initializers","designated initializer"},
     {"@interface BadProperties:PolicyRoot @property(nonatomic) int extra;@end\n@implementation BadProperties @end","-Werror=objc-property-implementation","property"},
     {"@interface BadExplicit:PolicyUnconformed<PolicyExplicit> @end\n@implementation BadExplicit @end","-Werror=protocol","not implemented"},
     {"Protocol *bad_static_protocol(void){return @protocol(PolicyStatic);}","","objc_non_runtime_protocol"}}){
  const auto input=root/("bad-"+std::to_string(ordinal++)+".mm");Write(input,text);auto value=imported(input,root/"bad.o");if(!option.empty())value.arguments.push_back(option);
  auto result=Compile(value);if(result.success)llvm::errs()<<"unexpected policy admission: "<<text<<'\n';
  assert(!result.success);assert(std::any_of(result.diagnostics.begin(),result.diagnostics.end(),[&](const auto &d){return d.message.find(message)!=std::string::npos;}));}
 for(const char *policy:{"objc_requires_super","objc_designated_initializer","objc_subclassing_restricted","objc_requires_property_definitions","objc_protocol_requires_explicit_implementation","objc_non_runtime_protocol","objc_boxable","objc_precise_lifetime","objc_nonlazy_class","objc_runtime_visible","objc_method_family(new)"}){
  auto changed=artifact.source_bytes;auto at=changed.find(policy);assert(at!=std::string::npos);
  // Keep valid GNU attribute grammar while removing exactly one semantic policy.
  changed.replace(at,std::string(policy).size(),std::string(policy)=="objc_method_family(new)"?"objc_method_family(none)":"unused");
  if(std::string(policy)=="objc_boxable"){
   const auto value=changed.find("return @(value)");assert(value!=std::string::npos);
   changed.replace(value,std::string("return @(value)").size(),"return 0");
  }
  const auto mutation_source=root/"canonical-mutation.mm";Write(mutation_source,changed);
  auto canonical_request=request(mutation_source,root/"canonical-mutation.o");canonical_request.emit=EmitKind::Syntax;canonical_request.emit_semantic_interface=true;
  canonical_request.module_name="fixture.ObjectPolicies";canonical_request.package_identity="object-policies-v1";
  auto canonical=Compile(canonical_request);Success(canonical);assert(canonical.interface_artifact && canonical.interface_artifact->success);
  changed=canonical.interface_artifact->source_bytes;assert(canonical.interface_artifact->identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint);
  auto mutation=*metadata;mutation.canonical_source_sha256=ComputeInterfaceSourceSha256(changed);
  auto encoded=EncodeSemanticInterfaceSidecar(mutation,reader);assert(encoded);const auto altered=root/"Altered.objc3i";
  Write(altered,changed);Write(altered.string()+".json",*encoded);const auto input=root/"mutation.mm";Write(input,"int mutation_control(void){return 42;}\n");
  auto bad=imported(input,root/"mutation.o");bad.imported_interfaces[0].source_path=altered.string();auto result=Compile(bad);assert(!result.success);
  const bool semantic_mismatch=std::any_of(result.interface_import_diagnostics.begin(),result.interface_import_diagnostics.end(),[](const auto &d){return d.code=="O3IFC139";});
  if(!semantic_mismatch){llvm::errs()<<"policy mutation: "<<policy<<'\n';test::ReportFailure(result);}
  assert(semantic_mismatch);}
 const auto main=root/"main.mm";Write(main,Main);auto entry=imported(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,true,{});
 assert(!fs::exists(source));
 assert(fs::remove(root/"Altered.objc3i"));assert(fs::remove(root/"Altered.objc3i.json"));
 auto directed=[&](const fs::path &input,const fs::path &output){auto value=request(input,output);value.source_directed_imports=true;value.interface_search_paths={root.string()};
  InterfaceModuleRequest module;module.canonical_name=artifact.identity.canonical_name;module.locked_identity=artifact.identity;module.initially_visible=false;value.module_imports={module};return value;};
 for(const char *name:{"a","b"}){const auto input=root/(std::string("directed-")+name+".mm");auto text=std::string("@import fixture.ObjectPolicies;\n")+Consumer;
  if(std::string(standard)!="c17")text+="extern \"C\" ";text+="int check_"+std::string(name)+"(void){return execute();}\n";
  Write(input,text);Success(Compile(directed(input,root/(std::string(name)+".o"))));}
 LinkAndExecute(root,entry.output_path,true,{});
}
}
int main(){test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-object-policies",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{if(std::getenv("OBJC3_KEEP_TEST_ARTIFACTS"))llvm::errs()<<"policy artifacts: "<<root.string()<<'\n';else fs::remove_all(root);});
 for(const char *standard:{"c17","c++20","c++23"})for(int ownership:{1,2}){const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}return cases.Finish();}
