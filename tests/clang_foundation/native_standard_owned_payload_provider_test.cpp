#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>
using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
void Run(const fs::path &root,const char *standard,int ownership,const char *optimization) {
  fs::create_directories(root/"provider");fs::create_directories(root/"include/objc3c");
  fs::create_directory_symlink(OBJC3_STANDARD_OWNED_RUNTIME_HEADERS,root/"include/objc3c/runtime");
  auto request=[&](const fs::path &input,const fs::path &output) {
    auto value=Request(input,output,standard,ownership,optimization,
        {"-fblocks","-fexceptions","-fcxx-exceptions","-fobjc-exceptions","-fobjc3-profile=strict-system"});
    value.arguments.push_back("-I"+(root/"include").string());return value;
  };
  const auto source=root/"provider/module.mm";
  Write(root/"foreign-runtime.h",R"(
extern "C" id _Nonnull class_createInstance(Class,__SIZE_TYPE__) __attribute__((ns_returns_retained));
extern "C" id _Nullable object_dispose(id _Nullable);
extern "C" const void * _Nullable CFRetain(const void * _Nullable) noexcept __attribute__((cf_returns_retained));
extern "C" void CFRelease(const void * _Nullable) noexcept;
)");
  Write(source,R"owned(
#include <objc3c/runtime/objc3_task.hpp>
#include <optional>
#include <variant>
#if __cplusplus > 202002L
#include <expected>
#endif
#include "../foreign-runtime.h"
__attribute__((objc_root_class)) @interface StandardNativeReceiver
-(int)number __objc3_async;
@end
namespace StandardOwned {
extern int objects,leaves,destroyed,entered,foreign_objects,selected_destructions;
struct Foreign {int references;};using ForeignRef=const Foreign *;
ForeignRef create_foreign() __attribute__((cf_returns_retained));
inline ForeignRef foreign_result() noexcept __objc3_async __attribute__((nothrow,cf_returns_retained));
inline ForeignRef foreign_result() noexcept __objc3_async {return create_foreign();}
id create() __attribute__((ns_returns_retained));id hold(id);void drop(id);
struct __attribute__((objc_unsafe_sendable)) Leaf {
 id object;int code;
#if __has_feature(objc_arc)
 Leaf(id object,int code):object(object),code(code){++leaves;}
 Leaf(const Leaf &other):object(other.object),code(other.code){++leaves;}
#else
 Leaf(id object,int code):object(hold(object)),code(code){++leaves;}
 Leaf(const Leaf &other):object(hold(other.object)),code(other.code){++leaves;}
#endif
 Leaf(Leaf &&other)noexcept:object(other.object),code(other.code){other.object=0;++leaves;}
 ~Leaf()noexcept{drop(object);--leaves;}
};
template<bool Active>struct Selected {
 Leaf value;
 ~Selected()=default;
 ~Selected()requires Active {++selected_destructions;}
};
inline int sync_selection(id object) {
 const int previous=selected_destructions;
 {Selected<true> active{Leaf(object,50)};Selected<false> plain{Leaf(object,51)};}
 return selected_destructions==previous+1?0:1;
}
using Optional=std::optional<Leaf>;
using Variant=std::variant<Leaf,int>;
inline Optional optional_result(Leaf value,int mode) __objc3_async_frame {
 ++entered;if(mode==1)throw 907;if(mode==2)return std::nullopt;
 return Optional(std::in_place,static_cast<Leaf &&>(value));
}
inline Variant variant_result(Leaf value,int mode) __objc3_async_frame {
 ++entered;if(mode==1)throw 908;if(mode==2)return Variant(std::in_place_index<1>,43);
 return Variant(std::in_place_index<0>,static_cast<Leaf &&>(value));
}
#if __cplusplus > 202002L
using Expected=std::expected<Leaf,int>;
inline Expected expected_result(Leaf value,int mode) __objc3_async_frame {
 ++entered;if(mode==1)throw 909;if(mode==2)return std::unexpected(44);
 return Expected(std::in_place,static_cast<Leaf &&>(value));
}
#endif
inline auto nil_task(){return [(StandardNativeReceiver *)0 number];}
inline int local_result(Leaf input) __objc3_async_frame {
 struct Private {Leaf value;};
 auto inner=[](Leaf value)->Private __objc3_async_frame {
  return Private{static_cast<Leaf &&>(value)};
 };
 auto value=__objc3_await inner(static_cast<Leaf &&>(input));return value.value.code;
}
inline int exercise() {
 if(objects || leaves)return 1;
 const int before=destroyed;
 @autoreleasepool {
  id object=create();objc3_executor_t *executor=nullptr;uint32_t ran=0;
  if(objc3_executor_create(1,&executor)!=OBJC3_SERVICE_OK)return 2;
  {
   auto frame=optional_result(Leaf(object,41),0);
   auto task=objc3::task<Optional>::from_native(&frame).start(executor);
   if(frame.context || objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 3;
   auto result=task.result();task={};if(!result || result->object!=object || result->code!=41 || objects!=1)return 4;
   Optional copy(result);if(!copy || copy->object!=object)return 5;
   result.reset();if(!copy || objects!=1)return 6;copy.reset();
  }
  if(leaves || objects!=1)return 7;
  {
   id transient=create();auto frame=optional_result(Leaf(transient,48),0);
#if !__has_feature(objc_arc)
   drop(transient);
#endif
   transient=0;
   auto task=objc3::task<Optional>::from_native(&frame).start(executor);
   if(objects!=2 || objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 27;
   auto result=task.result();task={};if(!result || !result->object || result->code!=48 || objects!=2)return 28;
   result.reset();
  }
  if(leaves || objects!=1)return 29;
  {
   auto task=nil_task().start(executor);
   if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK || task.result()!=0)return 30;
  }
  {
   auto frame=local_result(Leaf(object,49));
   auto task=objc3::task<int>::from_native(&frame).start(executor);
   if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK || task.result()!=49)return 31;
  }
  if(leaves || objects!=1)return 32;
  if(sync_selection(object) || leaves || objects!=1)return 38;
  {
   auto task=foreign_result().start(executor);
   if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 33;
   auto result=task.result();if(!result || foreign_objects!=1 || result->references!=1)return 34;
   task={};if(foreign_objects)return 35;
  }
  {
   auto task=foreign_result().start(executor);task.cancel();
   if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 36;
   bool cancelled=false;try{(void)task.result();}catch(const objc3::task_cancelled &){cancelled=true;}
   if(!cancelled || foreign_objects)return 37;
  }
  {
   auto frame=variant_result(Leaf(object,42),0);
   auto task=objc3::task<Variant>::from_native(&frame).start(executor);
   if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 8;
   auto result=task.result();task={};if(result.index()!=0 || std::get<0>(result).object!=object || std::get<0>(result).code!=42)return 9;
   Variant copy(result);result.emplace<1>(43);if(std::get<0>(copy).object!=object || objects!=1)return 10;
   copy.emplace<1>(44);
  }
  if(leaves || objects!=1)return 11;
  for(int kind=0;kind!=2;++kind)for(int mode=1;mode!=4;++mode) {
   const int previous=entered;
   if(kind==0) {
    auto frame=optional_result(Leaf(object,45),mode==3?0:mode);
    auto task=objc3::task<Optional>::from_native(&frame).start(executor);
    if(mode==3)task.cancel();
    if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 12;
    bool caught=false;
    try{auto value=task.result();if(mode==2 && value)return 13;}
    catch(int value){caught=mode==1 && value==907;}
    catch(const objc3::task_cancelled &){caught=mode==3;}
    if((mode!=2 && !caught) || (mode==3 && entered!=previous))return 14;
   } else {
    auto frame=variant_result(Leaf(object,46),mode==3?0:mode);
    auto task=objc3::task<Variant>::from_native(&frame).start(executor);
    if(mode==3)task.cancel();
    if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 15;
    bool caught=false;
    try{auto value=task.result();if(mode==2 && (value.index()!=1 || std::get<1>(value)!=43))return 16;}
    catch(int value){caught=mode==1 && value==908;}
    catch(const objc3::task_cancelled &){caught=mode==3;}
    if((mode!=2 && !caught) || (mode==3 && entered!=previous))return 17;
   }
   if(leaves || objects!=1)return 18;
  }
#if __cplusplus > 202002L
  for(int mode=0;mode!=4;++mode) {
   const int previous=entered;
   auto frame=expected_result(Leaf(object,47),mode==3?0:mode);
   auto task=objc3::task<Expected>::from_native(&frame).start(executor);
   if(mode==3)task.cancel();
   if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 19;
   bool caught=false;
   try {
    auto result=task.result();task={};
    if(mode==0) {
     if(!result || result->object!=object || result->code!=47)return 20;
     Expected copy(result);result=std::unexpected(44);
     if(!copy || copy->object!=object || result.error()!=44)return 21;
    } else if(mode==2 && (result || result.error()!=44))return 22;
   } catch(int value){caught=mode==1 && value==909;}
     catch(const objc3::task_cancelled &){caught=mode==3;}
   if((mode==1 || mode==3) && !caught)return 23;
   if(mode==3 && entered!=previous)return 24;
  }
  if(leaves || objects!=1)return 25;
#endif
  objc3_executor_release(executor);
#if !__has_feature(objc_arc)
  drop(object);
#endif
 }
 return objects || leaves || destroyed!=before+2 ? 26 : 0;
}
}
__attribute__((objc_root_class)) @interface StandardOwnedTracked {Class isa;@public int count;}
+(id _Nonnull)alloc;
#if !__has_feature(objc_arc)
-(id _Nonnull)retain;-(oneway void)release;
#endif
@end
@implementation StandardOwnedTracked
+(id _Nonnull)alloc {StandardOwnedTracked *value=(StandardOwnedTracked *)class_createInstance(self,0);value->count=1;++StandardOwned::objects;return value;}
#if __has_feature(objc_arc)
-(void)dealloc{--StandardOwned::objects;++StandardOwned::destroyed;object_dispose(self);}
#else
-(id _Nonnull)retain{++count;return self;}
-(oneway void)release{if(!--count){--StandardOwned::objects;++StandardOwned::destroyed;object_dispose(self);}}
#endif
@end
namespace StandardOwned {
int objects=0,leaves=0,destroyed=0,entered=0,foreign_objects=0,selected_destructions=0;
ForeignRef create_foreign(){++foreign_objects;return new Foreign{1};}
id create(){return [StandardOwnedTracked alloc];}
id hold(id value){
#if !__has_feature(objc_arc)
 if(value)[value retain];
#endif
 return value;
}
void drop(id value){
#if !__has_feature(objc_arc)
 if(value)[value release];
#else
 (void)value;
#endif
}
}
)owned");
  auto provider=request(source,root/"provider.o");provider.module_owned_names=true;
  provider.module_source_root=(root/"provider").string();provider.module_name="fixture.StandardOwned";
  provider.package_identity="standard-owned-v1";provider.interface_exports={"StandardOwned","StandardNativeReceiver"};provider.emit_semantic_interface=true;
  auto built=Compile(provider);Success(built);assert(built.interface_artifact && built.interface_artifact->success);
  const auto artifact=*built.interface_artifact;
  const auto capabilities=SupportedSemanticInterfaceCapabilities();
  auto metadata=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,capabilities);
  assert(metadata);
  for(const char *required:{kNativeCxxAsyncImportSemantic,kCxxSpecialMemberSelectionSemantic}) {
    assert(std::find(metadata->required_semantics.begin(),metadata->required_semantics.end(),required)!=metadata->required_semantics.end());
    std::vector<llvm::StringRef> older;
    for(auto capability:capabilities)if(capability!=required)older.push_back(capability);
    auto unsupported=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,older);
    assert(!unsupported);llvm::consumeError(unsupported.takeError());
    auto missing=*metadata;std::erase(missing.required_semantics,std::string(required));
    auto malformed=EncodeSemanticInterfaceSidecar(missing,capabilities);
    assert(!malformed);llvm::consumeError(malformed.takeError());
  }
  const auto interface=root/"StandardOwned.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  assert(fs::remove(source));
  for(const char *name:{"a","b"}) {
    const auto input=root/(std::string("consumer-")+name+".mm");
    std::string consumer_source="#include \"foreign-runtime.h\"\nextern \"C\" int check_"+std::string(name)+"(){return StandardOwned::exercise();}\n";
    if(std::string(name)=="a")consumer_source+=R"foreign(
extern "C" const void *CFRetain(const void *value) noexcept {
 if(value)++const_cast<StandardOwned::Foreign *>(static_cast<StandardOwned::ForeignRef>(value))->references;
 return value;
}
extern "C" void CFRelease(const void *value) noexcept {
 if(value) {
  auto *object=const_cast<StandardOwned::Foreign *>(static_cast<StandardOwned::ForeignRef>(value));
  if(!--object->references){--StandardOwned::foreign_objects;delete object;}
 }
}
)foreign";
    Write(input,consumer_source);
    auto consumer=request(input,root/(std::string(name)+".o"));InterfaceImportInput imported;
    imported.source_path=interface.string();imported.locked_identity=artifact.identity;consumer.imported_interfaces={imported};Success(Compile(consumer));
  }
  const auto main=root/"main.cpp";Write(main,"#include <cstdio>\nextern \"C\" int check_a();extern \"C\" int check_b();int main(){int a=check_a(),b=check_b();if(a||b)std::fprintf(stderr,\"standard owned payloads: %d %d\\n\",a,b);return a||b;}\n");
  auto entry=request(main,root/"main.o");Success(Compile(entry));
  LinkAndExecute(root,entry.output_path,true,{OBJC3_STANDARD_OWNED_RUNTIME_LIBRARY,"-pthread"});
}
}
int main(int argc,char **argv) {
  if(argc!=4)return 1;
  const std::string standard=argv[1],ownership=argv[2],optimization=argv[3];
  if((standard!="c++20" && standard!="c++23") || (ownership!="1" && ownership!="2") ||
      (optimization!="-O0" && optimization!="-O2"))return 1;
  llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-standard-owned",temporary));
  const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
  Run(root,argv[1],ownership=="2"?2:1,argv[3]);
}
