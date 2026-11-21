#include "standard_library_test_support.h"
#include "llvm/ADT/ScopeExit.h"

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
void Run(const fs::path &root,const char *standard,const char *optimization) {
  fs::create_directories(root/"provider");
  fs::create_directories(root/"include/objc3c");
  fs::create_directory_symlink(OBJC3_MRC_TEST_RUNTIME_HEADERS,root/"include/objc3c/runtime");
  const std::vector<std::string> flags={"-fblocks","-fexceptions","-fobjc-exceptions"};
  auto request=[&](const fs::path &input,const fs::path &output) {
    auto value=Request(input,output,standard,1,optimization,flags);
    value.arguments.push_back("-fobjc3-profile=strict-system");
    value.arguments.push_back("-I"+(root/"include").string());return value;
  };
  Write(root/"foreign-runtime.h",R"(
extern "C" void *_Block_copy(const void *);
extern "C" void _Block_release(const void *);
extern "C" id _Nonnull class_createInstance(Class,__SIZE_TYPE__);
extern "C" id object_dispose(id);
)");
  const auto source=root/"provider/module.mm";
  Write(source,R"mrc(
#include "../foreign-runtime.h"
#include <objc3c/runtime/objc3_task.hpp>
#include <vector>
namespace MRCFixture {
void *block_copy(const void *);
void block_release(const void *);
extern int live_objects,leases,copies,moves,copy_fault,move_fault;
id _Nonnull create();id hold(id);void drop(id);int references(id);
struct Lease {
  id object;
  explicit Lease(id value):object(hold(value)){++leases;}
  Lease(const Lease &other):object(hold(other.object)){++leases;}
  Lease(Lease &&other)noexcept:object(other.object){other.object=0;++leases;}
  ~Lease()noexcept{drop(object);--leases;}
};
struct Owner {
  Lease lease;int code;
  Owner(id object,int code):lease(object),code(code){}
  Owner(const Owner &other):lease(other.lease),code(other.code){if(copy_fault && ++copies==copy_fault)throw 901;}
  Owner(Owner &&other):lease(static_cast<Lease &&>(other.lease)),code(other.code){if(move_fault && ++moves==move_fault)throw 902;}
  ~Owner()noexcept=default;
};
struct BlockOwner {
  int (^callback)(void);
  explicit BlockOwner(int (^value)(void)):callback((int(^)(void))block_copy(value)){}
  BlockOwner(const BlockOwner &other):callback((int(^)(void))block_copy(other.callback)){}
  BlockOwner(BlockOwner &&other)noexcept:callback(other.callback){other.callback=0;}
  ~BlockOwner()noexcept{if(callback)block_release(callback);}
};
using Maybe=__objc3_optional(Owner);
using Result=__objc3_result(Owner,BlockOwner);
using Nested=__objc3_optional(Maybe);
static_assert(std::is_copy_constructible_v<Maybe>);
static_assert(!std::is_nothrow_copy_constructible_v<Maybe>);
static_assert(!std::is_nothrow_move_constructible_v<Maybe>);
static_assert(std::is_nothrow_destructible_v<Maybe>);
struct Record {Maybe values[2];};
struct View {id object;int code;};using ViewMaybe=__objc3_optional(View);
inline ViewMaybe view(id object){return __objc3_some(View,View{object,41});}
inline ViewMaybe view_echo(ViewMaybe value){return value;}
union ActiveUnion {
  struct Owned {Maybe value;} owned;int scalar;
  ActiveUnion(id object):owned{__objc3_some(Owner,Owner(object,54))}{}
  ActiveUnion(const ActiveUnion &other):owned(other.owned){}
  ~ActiveUnion()noexcept{owned.~Owned();}
};

inline Maybe make(id object,int code){return __objc3_some(Owner,Owner(object,code));}
inline Maybe echo(Maybe value){return value;}
inline int read(const Maybe &value){return __objc3_match(value){case __objc3_case_some(let owner)=>owner.code;case __objc3_case_none=>-1;};}
Maybe message(Maybe value);
inline int exercise() {
  if(live_objects || leases)return 1;
  id object=create();
  {
    auto raw_view=view_echo(view(object));auto raw_copy=__objc3_copy(raw_view);(void)__objc3_copy(raw_view);
    auto raw_callback=[raw_copy]{return __objc3_has_value(raw_copy);};
    if(references(object)!=1 || !raw_callback())return 14;
    ActiveUnion active(object);ActiveUnion active_copy(active);
    if(read(active_copy.owned.value)!=54)return 15;
    auto first=make(object,42);auto copied=__objc3_copy(first);auto moved=__objc3_move(copied);
    Maybe implicit=first;Maybe const_rvalue=static_cast<const Maybe &&>(first);
    auto functional=Maybe(first);const Maybe &extended=Maybe(first);
    int before_heap=references(object),before_heap_leases=leases;
    auto *allocated=new Maybe(first);delete allocated;
    if(references(object)!=before_heap || leases!=before_heap_leases)return 29;
    alignas(Maybe) unsigned char storage[sizeof(Maybe)];
    auto *placed=::new((void *)storage)Maybe(first);placed->~Maybe();
    if(references(object)!=before_heap || leases!=before_heap_leases)return 39;
    int before_discard=references(object);(void)Maybe(first);
    if(read(first)==42)(void)Maybe(first);
    if(references(object)!=before_discard)return 16;
    if(read(implicit)!=42)return 18;
    if(read(const_rvalue)!=42)return 19;
    if(read(functional)!=42)return 27;
    if(read(extended)!=42)return 28;
    std::optional<Maybe> wrapper;wrapper.emplace(first);wrapper.reset();
    std::vector<Maybe> vector;vector.reserve(1);vector.push_back(first);vector.push_back(make(object,64));
    if(read(vector[0])!=42 || read(vector[1])!=64)return 17;vector.clear();
    auto called=echo(__objc3_copy(moved));auto sent=message(__objc3_copy(called));
    auto callback=[sent]{return read(sent);};auto lambda_copy=callback;
    auto block=^int{return read(sent);};void *heap=block_copy(block);
    if(((int(^)(void))heap)()!=42 || lambda_copy()!=42)return 2;
    block_release(heap);
    __block Maybe byref=make(object,43);
    auto byref_block=^int{return read(byref);};void *byref_heap=block_copy(byref_block);
    if(((int(^)(void))byref_heap)()!=43)return 3;
    block_release(byref_heap);
    Record record{{make(object,44),make(object,45)}};Record record_copy(record);
    Maybe values[2]={make(object,46),make(object,47)};
    auto *array=new Maybe[2]{make(object,48),make(object,49)};
    if(read(array[1])!=49 || read(values[0])!=46 || read(record_copy.values[1])!=45)return 4;
    delete[]array;
    auto nested=__objc3_some(Maybe,make(object,50));auto nested_copy=__objc3_copy(nested);
    auto result=__objc3_ok(Owner,BlockOwner,Owner(object,51));auto result_copy=__objc3_copy(result);
    auto failure=__objc3_err(Owner,BlockOwner,BlockOwner(^int{return 52;}));
    auto failure_copy=__objc3_copy(failure);auto failure_moved=__objc3_move(failure_copy);
    if(!__objc3_has_value(nested_copy) || !__objc3_is_ok(result_copy) || __objc3_is_ok(failure_moved))return 5;
  }
  if(leases || references(object)!=1)return 6;
  for(int mode=0;mode!=7;++mode) {
    {
      auto value=make(object,53);int baseline=references(object);
      copies=moves=0;if(mode==1)move_fault=1;else if(mode==0 || mode==6)copy_fault=1;else if(mode==3)copy_fault=2;
      bool caught=false;
      try {
        if(mode==0){(void)Maybe(value);}
        else if(mode==1){auto moved=__objc3_move(value);(void)moved;}
        else if(mode==2){auto block=^int{return read(value);};copies=0;copy_fault=1;void *heap=block_copy(block);block_release(heap);}
        else if(mode==3){Record source{{make(object,54),make(object,55)}};Record copied(source);(void)copied;}
        else if(mode==4){__block Maybe source=make(object,56);auto block=^int{return read(source);};copies=moves=0;copy_fault=move_fault=1;void *heap=block_copy(block);block_release(heap);}
        else if(mode==5){Owner first(object,57),second(object,58),third(object,59);moves=0;move_fault=2;auto *array=new Maybe[3]{__objc3_some(Owner,static_cast<Owner &&>(first)),__objc3_some(Owner,static_cast<Owner &&>(second)),__objc3_some(Owner,static_cast<Owner &&>(third))};delete[]array;}
        else {auto *heap=new Maybe(value);delete heap;}
      }catch(int code){caught=mode==4?(code==901 || code==902):code==(mode==1 || mode==5?902:901);}
      copy_fault=move_fault=0;
      if(!caught || (mode!=1 && references(object)!=baseline))return 20+mode;
    }
    if(leases || references(object)!=1)return 30+mode;
  }
  drop(object);return live_objects || leases?13:0;
}
extern int task_entered;
inline objc3::task<Maybe> task_value(Owner owner,int fail) {
  co_await objc3::task_yield;++task_entered;
  if(fail)throw 903;co_return __objc3_some(Owner,static_cast<Owner &&>(owner));
}
inline auto grouped(id object) {
  return objc3::with_task_group([owner=Owner(object,63)](objc3::task_group_handle group)->objc3::task<Maybe>{auto child=task_value(owner,0).start_child(group);Maybe result=co_await child;co_return result;});
}
inline int exercise_tasks() {
  if(live_objects || leases)return 40;
  id object=create();objc3_executor_t *executor=nullptr;uint32_t ran=0;
  if(objc3_executor_create(1,&executor)!=OBJC3_SERVICE_OK)return 41;
  {
    auto task=task_value(Owner(object,60),0).start(executor);
    if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 42;
    auto result=task.result();task={};if(read(result)!=60)return 43;
  }
  if(leases || references(object)!=1)return 44;
  {
    auto task=grouped(object).start(executor);
    if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 45;
    auto result=task.result();task={};if(read(result)!=63)return 46;
  }
  if(leases || references(object)!=1)return 47;
  {
    auto task=task_value(Owner(object,61),1).start(executor);
    if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 48;
    bool caught=false;try{(void)task.result();}catch(int value){caught=value==903;}
    if(!caught)return 49;task={};
  }
  if(leases || references(object)!=1)return 50;
  {
    const int entered=task_entered;
    auto task=task_value(Owner(object,62),0).start(executor);task.cancel();
    if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 51;
    bool caught=false;try{(void)task.result();}catch(const objc3::task_cancelled &){caught=true;}
    if(!caught || task_entered!=entered)return 52;task={};
  }
  if(leases || references(object)!=1)return 53;
  objc3_executor_release(executor);drop(object);return live_objects || leases?54:0;
}
}
__attribute__((objc_root_class)) @interface FixtureMRCTracked {
  Class isa;@public int count;
}
+(id _Nonnull)alloc;-(id _Nonnull)retain;-(oneway void)release;
-(MRCFixture::Maybe)echo:(MRCFixture::Maybe)value;
@end
@implementation FixtureMRCTracked
+(id _Nonnull)alloc {
  FixtureMRCTracked *_Nonnull value=(FixtureMRCTracked *)class_createInstance(self,0);
  value->count=1;++MRCFixture::live_objects;return value;
}
-(id _Nonnull)retain{++count;return self;}
-(oneway void)release{if(!--count){--MRCFixture::live_objects;object_dispose(self);}}
-(MRCFixture::Maybe)echo:(MRCFixture::Maybe)value{return __objc3_copy(value);}
@end
namespace MRCFixture {
int live_objects,leases,copies,moves,copy_fault,move_fault,task_entered;
void *block_copy(const void *value){return _Block_copy(value);}
void block_release(const void *value){_Block_release(value);}
id _Nonnull create(){return [FixtureMRCTracked alloc];}
id hold(id value){if(value)[value retain];return value;}
void drop(id value){if(value)[value release];}
int references(id value){return ((FixtureMRCTracked *)value)->count;}
Maybe message(Maybe value){FixtureMRCTracked *_Nonnull host=(FixtureMRCTracked *)create();auto result=[host echo:__objc3_copy(value)];drop(host);return result;}
}
)mrc");
  auto provider=request(source,root/"provider.o");
  provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
  provider.module_name="fixture.MRCRecordPayload";provider.package_identity="mrc-record-v1";
  provider.interface_exports={"MRCFixture"};provider.emit_semantic_interface=true;
  auto built=Compile(provider);Success(built);assert(built.interface_artifact && built.interface_artifact->success);
  const auto artifact=*built.interface_artifact;
  const auto interface=root/"MRCRecordPayload.objc3i";
  Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  assert(fs::remove(source));
  for(const char *name:{"a","b"}) {
    auto input=root/(std::string("consumer-")+name+".mm");
    Write(input,"extern \"C\" int check_"+std::string(name)+"(){int result=MRCFixture::exercise();return result?result:MRCFixture::exercise_tasks();}\n");
    auto consumer=request(input,root/(std::string(name)+".o"));
    InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported};Success(Compile(consumer));
  }
  const auto main=root/"main.cpp";
  Write(main,R"(#include <cstdio>
extern "C" int check_a();extern "C" int check_b();
int main(){int a=check_a(),b=check_b();if(a||b)std::fprintf(stderr,"MRC payload failures: %d %d\n",a,b);return a||b;}
)");
  auto entry=request(main,root/"main.o");Success(Compile(entry));
  LinkAndExecute(root,entry.output_path,true,{OBJC3_MRC_TEST_RUNTIME_LIBRARY,"-pthread"});
}
}
int main(int argc,char **argv) {
  assert(argc==1 || argc==3);
  llvm::SmallString<128> temporary;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-native-mrc-record",temporary));
  const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
  unsigned configurations=0;
  for(const char *standard:{"c++20","c++23"})for(const char *optimization:{"-O0","-O2"}) {
    if(argc==3 && (std::string(argv[1])!=standard || std::string(argv[2])!=optimization))continue;
    ++configurations;
    std::cerr<<standard<<' '<<optimization<<'\n';
    Run(root/(std::string(standard)+optimization),standard,optimization);
  }
  assert(configurations==(argc==3?1U:4U));
}
