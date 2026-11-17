#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>
using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
template<class T>bool Fails(llvm::Expected<T> value){if(value)return false;llvm::consumeError(value.takeError());return true;}
constexpr const char *Operations=R"objects({
 int old=object_receiver(object).value++;
 object_receiver(object).value+=object_argument(2);
 int assigned=(object_receiver(object).value=object_argument(7));
 object_receiver(object)[object_argument(0)]=object_receiver(object);
 object_receiver(object)[object_receiver(object)]=object_receiver(object);
 int indexed=[object_receiver(object)[object_argument(0)] readValue];
 int keyed=[object_receiver(object)[object_receiver(object)] readValue];
 object_receiver(object).implicitValue=object_argument(9);
 ObjectExpressionRoot.classValue=object_argument(11);
 return old+assigned+indexed+keyed+ObjectExpressionRoot.classValue+object_receiver(object).value+object_receiver(object).implicitValue;
})objects";
std::string Source(){return std::string(R"objects(
@protocol ObjectExpressionProtocol - (int)readValue; @end
__attribute__((objc_root_class)) @interface ObjectExpressionRoot<ObjectExpressionProtocol> {
 Class isa;
@public
 int stored;
}
+ (id)new;
@property(nonatomic,getter=readValue,setter=writeValue:) int value;
@property(class) int classValue;
- (id)objectAtIndexedSubscript:(__SIZE_TYPE__)index;
- (void)setObject:(id)value atIndexedSubscript:(__SIZE_TYPE__)index;
- (id)objectForKeyedSubscript:(id)key;
- (void)setObject:(id)value forKeyedSubscript:(id)key;
- (int)implicitValue;
- (void)setImplicitValue:(int)value;
- (void)fill:(out id __autoreleasing *)value;
@end
struct ObjectExpressionPayload {int number;double real;};
#ifdef __cplusplus
extern "C" {
#endif
extern unsigned object_trace[128],object_trace_count,object_destroyed;
extern id object_out_value;
ObjectExpressionRoot *object_receiver(ObjectExpressionRoot *);
int object_argument(int);
void object_reset(void);
void object_release(void *);
void object_inout(id __autoreleasing *);
#ifdef __cplusplus
}
#endif
__attribute__((always_inline)) inline int object_program(ObjectExpressionRoot *object)
)objects")+Operations+R"objects(
__attribute__((always_inline)) inline SEL object_selector(void){return @selector(writeValue:);}
__attribute__((always_inline)) inline Protocol *object_protocol(void){return @protocol(ObjectExpressionProtocol);}
__attribute__((always_inline)) inline const char *object_encoding(void){return @encode(struct ObjectExpressionPayload);}
__attribute__((always_inline)) inline int object_boolean(void){return __objc_yes?1:0;}
__attribute__((always_inline)) inline void *object_bridge(ObjectExpressionRoot *object){return (__bridge void *)object;}
__attribute__((always_inline)) inline int object_writeback(ObjectExpressionRoot *object){
 id inout=object;object_inout(&inout);int first=[inout readValue];
 id out=object;[object fill:&out];return first+[out readValue];
}
__attribute__((always_inline)) inline int object_bridge_hold(ObjectExpressionRoot *object){
#if __has_feature(objc_arc)
 void *held=(__bridge_retained void *)object;
 ObjectExpressionRoot *restored=(__bridge_transfer ObjectExpressionRoot *)held;
 return restored.value;
#else
 return object.value;
#endif
}
)objects";}
std::string Consumer(){return std::string(R"objects(
#include <assert.h>
#include <string.h>
#ifdef __cplusplus
extern "C"
#endif
Class object_getClass(__unsafe_unretained id);
static int baseline(ObjectExpressionRoot *object)
)objects")+Operations+R"objects(
static int execute(void){
 object_destroyed=0;
 @autoreleasepool {
  ObjectExpressionRoot *object=[ObjectExpressionRoot new],*destination=[ObjectExpressionRoot new];
  object->stored=3;object_reset();int expected=baseline(object);
  unsigned count=object_trace_count,trace[128];assert(count<128);for(unsigned n=0;n!=count;++n)trace[n]=object_trace[n];
  object->stored=3;object_reset();assert(object_program(object)==expected && object_trace_count==count);
  for(unsigned n=0;n!=count;++n)assert(trace[n]==object_trace[n]);
  assert([(id)object readValue]==object->stored);
  assert([(Class)object_getClass(object) classValue]==11);
  assert(object_selector()==@selector(writeValue:));assert(object_protocol()==@protocol(ObjectExpressionProtocol));
  assert(strcmp(object_encoding(),@encode(struct ObjectExpressionPayload))==0 && object_boolean()==1);
  assert(object_bridge(object)==(__bridge void *)object && object_bridge_hold(object)==object->stored);
  destination->stored=21;object_out_value=destination;object_reset();assert(object_writeback(object)==42);
#if __has_feature(objc_arc)
  unsigned copies=0,zeroes=0;for(unsigned n=0;n!=object_trace_count;++n){copies+=object_trace[n]==10;zeroes+=object_trace[n]==11;}
  assert(copies==1 && zeroes==1);
#endif
  object_out_value=0;
#if !__has_feature(objc_arc)
  object_release((__bridge void *)object);object_release((__bridge void *)destination);
#endif
 }
 assert(object_destroyed==2);return 0;
}
)objects";}
constexpr const char *Main=R"objects(
#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif
id class_createInstance(Class,__SIZE_TYPE__) __attribute__((ns_returns_retained));
id object_dispose(__unsafe_unretained id);
void objc_release(void *);
unsigned object_trace[128],object_trace_count=0,object_destroyed=0;
id object_out_value=0;
static int class_value=0;
static void event(unsigned value){assert(object_trace_count<128);object_trace[object_trace_count++]=value;}
ObjectExpressionRoot *object_receiver(ObjectExpressionRoot *object){event(1);return object;}
int object_argument(int value){event(2);return value;}
void object_reset(void){object_trace_count=0;class_value=0;}
void object_release(void *object){objc_release(object);}
void object_inout(id __autoreleasing *value){event(*value?10:12);*value=object_out_value;}
int check_a(void);int check_b(void);
#ifdef __cplusplus
}
#endif
@implementation ObjectExpressionRoot
+ (id)new{return class_createInstance(self,0);}
- (int)readValue{event(3);return stored;}
- (void)writeValue:(int)value{event(4);stored=value;}
- (id)objectAtIndexedSubscript:(__SIZE_TYPE__)index{assert(index==0);event(13);return self;}
- (void)setObject:(id)value atIndexedSubscript:(__SIZE_TYPE__)index{assert(index==0 && value==self);event(14);}
- (id)objectForKeyedSubscript:(id)key{assert(key==self);event(15);return self;}
- (void)setObject:(id)value forKeyedSubscript:(id)key{assert(key==self && value==self);event(16);}
- (int)implicitValue{event(5);return stored+100;}
- (void)setImplicitValue:(int)value{event(6);stored=value;}
+ (int)classValue{event(8);return class_value;}
+ (void)setClassValue:(int)value{event(9);class_value=value;}
- (void)fill:(out id __autoreleasing *)value{event(*value?12:11);*value=object_out_value;}
- (void)dealloc{++object_destroyed;object_dispose(self);}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease{}
#endif
@end
int main(void){return check_a()||check_b();}
)objects";
void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");
 auto request=[&](const fs::path &input,const fs::path &output){auto value=Request(input,output,standard,ownership,"-O2",{"-fblocks"});
  if(std::string(standard)=="c17")value.language=BaseLanguage::ObjectiveC;return value;};
 const auto source=root/"provider/module.mm";
 auto publish=[&](const std::string &text,bool owned=true){Write(source,text);auto value=request(source,root/"provider.o");value.module_name="fixture.ObjectExpressions";value.package_identity="object-expressions-v1";value.module_owned_names=owned;
  if(owned)value.module_source_root=(root/"provider").string();value.emit_semantic_interface=true;
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;};
 const auto artifact=publish(Source()),replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 const auto &reader=SupportedSemanticInterfaceCapabilities();auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 assert(std::find(decoded->required_semantics.begin(),decoded->required_semantics.end(),kNativeObjectExpressionSemantic)!=decoded->required_semantics.end());
 std::vector<llvm::StringRef> old(reader.begin(),reader.end());std::erase(old,kNativeObjectExpressionSemantic);assert(Fails(DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,old)));
 auto missing=*decoded;std::erase(missing.required_semantics,kNativeObjectExpressionSemantic);assert(Fails(EncodeSemanticInterfaceSidecar(missing,reader)));
 for(const char *kind:{"Namespace","Enum","EnumConstant"}){auto forged=*decoded;auto row=std::find_if(forged.declarations.begin(),forged.declarations.end(),HasNativeObjectExpressionIdentity);assert(row!=forged.declarations.end());row->kind=kind;assert(Fails(EncodeSemanticInterfaceSidecar(forged,reader)));}
 // An unpublished implementation body creates no expression-graph promise.
 const auto hidden_body=root/"hidden-body.mm";
 Write(hidden_body,"__attribute__((objc_root_class)) @interface HiddenObject @property(nonatomic) int value; @end\nint hidden_implementation(HiddenObject *object){return object.value;}\n");
 auto hidden_request=request(hidden_body,root/"hidden-body.o");hidden_request.module_name="fixture.HiddenObjectImplementation";hidden_request.package_identity="hidden-object-implementation-v1";hidden_request.emit_semantic_interface=true;
 auto hidden_result=Compile(hidden_request);Success(hidden_result);assert(hidden_result.interface_artifact && hidden_result.interface_artifact->success);
 auto hidden_metadata=DecodeSemanticInterfaceSidecar(hidden_result.interface_artifact->sidecar_json,hidden_result.interface_artifact->source_bytes,reader);assert(hidden_metadata);
 assert(std::find(hidden_metadata->required_semantics.begin(),hidden_metadata->required_semantics.end(),kNativeObjectExpressionSemantic)==hidden_metadata->required_semantics.end());
 if(std::string(standard)!="c17"){
  const auto constrained=root/"constraints.mm";
  Write(constrained,"template<class T>concept NativeObjectRequirement=requires{ @selector(readValue); };\nstruct NativeObjectDefaults{int value=__objc_yes?1:0;};\ninline int native_default_argument(int value=__objc_yes?1:0){return value;}\n");
  auto constrained_request=request(constrained,root/"constraints.o");constrained_request.module_name="fixture.ObjectConstraints";constrained_request.package_identity="object-constraints-v1";constrained_request.emit_semantic_interface=true;
  auto constrained_result=Compile(constrained_request);Success(constrained_result);assert(constrained_result.interface_artifact && constrained_result.interface_artifact->success);
  auto constrained_metadata=DecodeSemanticInterfaceSidecar(constrained_result.interface_artifact->sidecar_json,constrained_result.interface_artifact->source_bytes,reader);assert(constrained_metadata);
  for(const char *name:{"NativeObjectRequirement","NativeObjectDefaults","native_default_argument"}){
   auto declaration=std::find_if(constrained_metadata->declarations.begin(),constrained_metadata->declarations.end(),[&](const auto &entry){return entry.qualified_name==name;});
   assert(declaration!=constrained_metadata->declarations.end() && HasNativeObjectExpressionIdentity(*declaration));
  }
 }
 // Literal factories are typechecked without assuming a bundled Foundation runtime.
 const auto literal_source=root/"literal-provider.mm";
 std::string literals=R"objects(
 typedef __SIZE_TYPE__ ObjectCount;
 __attribute__((objc_root_class)) @interface NSString @end
 __attribute__((objc_root_class)) @interface NSNumber
 + (NSNumber *)numberWithInt:(int)value;
 @end
 __attribute__((objc_root_class)) @interface NSArray
 + (NSArray *)arrayWithObjects:(id const __unsafe_unretained *)objects count:(ObjectCount)count;
 @end
 __attribute__((objc_root_class)) @interface NSDictionary
 + (NSDictionary *)dictionaryWithObjects:(id const __unsafe_unretained *)objects forKeys:(id const __unsafe_unretained *)keys count:(ObjectCount)count;
 @end
 inline NSString *literal_string(void){return @"native\n\u03bc";}
 inline NSNumber *literal_number(void){return @42;}
 inline NSNumber *literal_box(int value){return @(value+1);}
 inline NSArray *literal_array(id first,id second){return @[first,second];}
 inline NSDictionary *literal_dictionary(id first,id second){return @{first:second};}
 #ifdef __cplusplus
 template<class...Values>NSArray *literal_array_pack(Values...values){return @[values...];}
 template<class...Values>NSDictionary *literal_dictionary_pack(Values...values){return @{values:values...};}
 #endif
 )objects";
 auto syntax_publish=[&](const std::string &text){Write(literal_source,text);auto value=request(literal_source,root/"literal-provider.o");value.emit=EmitKind::Syntax;
  value.module_name="fixture.ObjectLiterals";value.package_identity="object-literals-v1";value.emit_semantic_interface=true;
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;};
 const auto literal_artifact=syntax_publish(literals),literal_replay=syntax_publish(literal_artifact.source_bytes);
 if(literal_artifact.source_bytes!=literal_replay.source_bytes)llvm::errs()<<"literal first:\n"<<literal_artifact.source_bytes<<"literal replay:\n"<<literal_replay.source_bytes;
 assert(literal_artifact.source_bytes==literal_replay.source_bytes && literal_artifact.identity.semantic_fingerprint==literal_replay.identity.semantic_fingerprint);
 assert(fs::remove(literal_source));const auto literal_interface=root/"ObjectLiterals.objc3i";
 Write(literal_interface,literal_artifact.source_bytes);Write(literal_interface.string()+".json",literal_artifact.sidecar_json);
 InterfaceImportInput literal_dependency;literal_dependency.source_path=literal_interface.string();literal_dependency.locked_identity=literal_artifact.identity;
 for(const char *name:{"first","second"}){const auto consumer=root/(std::string("literal-")+name+".mm");
  std::string calls="void literal_consumer(id first,id second){literal_string();literal_number();literal_box(41);literal_array(first,second);literal_dictionary(first,second);";
  if(std::string(standard)!="c17")calls+="literal_array_pack(first,second);literal_dictionary_pack(first,second);";
  Write(consumer,calls+"}\n");auto value=request(consumer,root/"literal-consumer.o");value.emit=EmitKind::Syntax;value.imported_interfaces={literal_dependency};Success(Compile(value));
 }
 publish(Source());assert(fs::remove(source));
 const auto interface=root/"ObjectExpressions.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
 for(const char *name:{"a","b"}){const auto input=root/(std::string(name)+".mm");auto text=Consumer();
  if(std::string(standard)!="c17")text+="extern \"C\" ";text+="int check_"+std::string(name)+"(void){return execute();}\n";
  Write(input,text);auto value=request(input,root/(std::string(name)+".o"));value.imported_interfaces={dependency};Success(Compile(value));}
 // Rehashing a different reflection expression cannot retain the old semantic identity.
 for(const auto &change:{std::pair<std::string,std::string>{"@selector(writeValue:)","@selector(readValue)"},
                         {"@encode(struct ObjectExpressionPayload)","@encode(int)"}}){
  auto changed=artifact.source_bytes;const auto at=changed.find(change.first);assert(at!=std::string::npos);changed.replace(at,change.first.size(),change.second);
  auto mutation=*decoded;mutation.canonical_source_sha256=ComputeInterfaceSourceSha256(changed);auto metadata=EncodeSemanticInterfaceSidecar(mutation,reader);assert(metadata);
  const auto altered=root/"Altered.objc3i";Write(altered,changed);Write(altered.string()+".json",*metadata);Write(root/"mutation.mm","int mutation_control(void){return 42;}");
  auto bad=request(root/"mutation.mm",root/"altered.o");auto altered_dependency=dependency;altered_dependency.source_path=altered.string();bad.imported_interfaces={altered_dependency};
  auto result=Compile(bad);assert(!result.success);assert(std::any_of(result.interface_import_diagnostics.begin(),result.interface_import_diagnostics.end(),[](const auto &d){return d.code=="O3IFC139";}));
 }
 const auto main=root/"main.mm";Write(main,Main);auto entry=request(main,root/"main.o");entry.imported_interfaces={dependency};Success(Compile(entry));LinkAndExecute(root,entry.output_path,true,{});
 assert(fs::remove(root/"Altered.objc3i"));assert(fs::remove(root/"Altered.objc3i.json"));
 auto directed=[&](const fs::path &input,const fs::path &output){auto value=request(input,output);value.source_directed_imports=true;value.interface_search_paths={root.string()};
  InterfaceModuleRequest item;item.canonical_name=artifact.identity.canonical_name;item.locked_identity=artifact.identity;item.initially_visible=false;value.module_imports={item};return value;};
 if(ownership==1){const auto cached=root/"cached-selector.mm";
  Write(cached,"void cached_miss(id value){[value readValue];}\n@import fixture.ObjectExpressions;\nint resolved_selector(id value){return [value readValue];}\n");
  Success(Compile(directed(cached,root/"cached-selector.o")));
 }
 for(const char *expression:{"classValue","@fixture.ObjectExpressions.classValue"}) {
  const auto bare=root/"bare-property.mm";Write(bare,std::string("@import fixture.ObjectExpressions;\nint bare_property(void){return ")+expression+";}\n");
  assert(!Compile(directed(bare,root/"bare-property.o")).success);
 }
 const auto before=root/"before-import.mm";Write(before,"int before_import(id value){return [value readValue];}\n@import fixture.ObjectExpressions;\n");
 auto hidden=Compile(directed(before,root/"before-import.o"));assert(!hidden.success);
 assert(std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),[](const auto &d){return d.message.find("no known instance method")!=std::string::npos || d.message.find("instance method '-readValue' not found")!=std::string::npos;}));
 for(const char *name:{"a","b"}){const auto input=root/(std::string("directed-")+name+".mm");auto text=std::string("@import fixture.ObjectExpressions;\n")+Consumer();
  if(std::string(standard)!="c17")text+="extern \"C\" ";text+="int check_"+std::string(name)+"(void){return execute();}\n";
  Write(input,text);Success(Compile(directed(input,root/(std::string(name)+".o"))));}
 LinkAndExecute(root,entry.output_path,true,{});
}
}
int main(){test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-object-expressions",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const char *standard:{"c17","c++20","c++23"})for(int ownership:{1,2}){const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}return cases.Finish();}
