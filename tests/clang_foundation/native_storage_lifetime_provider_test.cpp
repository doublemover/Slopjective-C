#include "standard_library_test_support.h"
#include "llvm/ADT/ScopeExit.h"

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
void Run(const fs::path &root,const char *standard,int ownership,
         const char *optimization,bool suppress_implicit_destruction) {
  fs::create_directories(root/"provider");
  std::vector<std::string> flags={"-pthread"};
  if(suppress_implicit_destruction)flags.push_back("-fno-c++-static-destructors");
  auto request=[&](const fs::path &input,const fs::path &output) {
    return Request(input,output,standard,ownership,optimization,flags);
  };
  const auto source=root/"provider/module.mm";
  Write(source,R"storage(
namespace StorageLifetime {
extern int created[10],destroyed[10];
void create(int);void destroy(int);void startup(int);void shutdown(int);
struct Owner {
  int value;
  explicit Owner(int value):value(value){create(value);}
  ~Owner() noexcept {destroy(value);}
};
inline constinit int ready=42;
__attribute__((require_constant_initialization)) inline int attribute_ready=43;
inline __attribute__((init_priority(202),always_destroy)) Owner ordered(2);
inline __attribute__((no_destroy)) Owner retained(4);
inline __attribute__((always_destroy)) Owner finalized(5);
inline Owner &local_retained(){static __attribute__((no_destroy)) Owner value(6);return value;}
inline Owner &local_finalized(){static __attribute__((always_destroy)) Owner value(7);return value;}
inline Owner &thread_finalized(){static thread_local __attribute__((always_destroy)) Owner value(8);return value;}
Owner &provider_local(){return local_retained();}
void first() __attribute__((constructor(201)));
void last() __attribute__((constructor(203)));
void late_shutdown() __attribute__((destructor(203)));
void early_shutdown() __attribute__((destructor(201)));
void verify_exit() __attribute__((destructor(101)));
int created[10],destroyed[10];
int startup_count,startup_trace[3],shutdown_count,shutdown_trace[2];
void create(int value){++created[value];if(value==2)startup(2);}
void destroy(int value){++destroyed[value];}
void startup(int value){if(startup_count>=3)__builtin_trap();startup_trace[startup_count++]=value;}
void shutdown(int value){if(shutdown_count>=2)__builtin_trap();shutdown_trace[shutdown_count++]=value;}
void first(){startup(1);}void last(){startup(3);}
void late_shutdown(){shutdown(3);}void early_shutdown(){shutdown(1);}
void verify_exit(){
  if(startup_count!=3 || startup_trace[0]!=1 || startup_trace[1]!=2 || startup_trace[2]!=3 ||
     shutdown_count!=2 || shutdown_trace[0]!=3 || shutdown_trace[1]!=1)__builtin_trap();
  for(int value=2;value!=9;++value) {
    if(value==3)continue;
    if(created[value]!=(value==8?2:1))__builtin_trap();
    if(destroyed[value]!=(value==4 || value==6?0:value==8?2:1))__builtin_trap();
  }
}
}
)storage");
  auto provider=request(source,root/"provider.o");
  provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
  provider.module_name="fixture.StorageLifetime";provider.package_identity="storage-lifetime-v1";
  provider.interface_exports={"StorageLifetime"};provider.emit_semantic_interface=true;
  auto built=Compile(provider);Success(built);
  assert(built.interface_artifact && built.interface_artifact->success);
  const auto artifact=*built.interface_artifact;
  const auto interface=root/"StorageLifetime.objc3i";
  Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  assert(fs::remove(source));
  for(const char *name:{"a","b"}) {
    const auto consumer_source=root/(std::string("consumer-")+name+".mm");
    Write(consumer_source,std::string(R"storage(
#include <pthread.h>
static void *worker(void *){return StorageLifetime::thread_finalized().value==8?nullptr:(void *)1;}
)storage")+"extern \"C\" int check_"+name+R"storage(() {
  using namespace StorageLifetime;
  if(ready!=42 || attribute_ready!=43 || ordered.value!=2 || retained.value!=4 || finalized.value!=5)return 1;
  if(&local_retained()!=&provider_local() || local_finalized().value!=7)return 2;
  pthread_t thread;void *result=(void *)1;
  if(pthread_create(&thread,nullptr,worker,nullptr) || pthread_join(thread,&result) || result)return 3;
  return 0;
}
)storage"+"extern \"C\" void *address_"+name+"(){return &StorageLifetime::local_retained();}\n");
    auto consumer=request(consumer_source,root/(std::string(name)+".o"));
    InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported};Success(Compile(consumer));
  }
  const auto main=root/"main.cpp";
  Write(main,R"(extern "C" int check_a();extern "C" int check_b();
extern "C" void *address_a();extern "C" void *address_b();
int main(){return check_a() || check_b() || address_a()!=address_b();}
)");
  auto entry=request(main,root/"main.o");Success(Compile(entry));
  LinkAndExecute(root,entry.output_path,ownership!=0,flags);
}
}

int main() {
  llvm::SmallString<128> temporary;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-native-storage-lifetime",temporary));
  const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
  struct Profile {const char *standard;int ownership;const char *optimization;bool suppress;};
  for(const auto &profile:std::vector<Profile>{{"c++20",0,"-O0",false},{"c++23",1,"-O2",false},
       {"c++23",2,"-O2",false},{"c++23",2,"-O2",true}}) {
    std::cerr<<profile.standard<<" ownership="<<profile.ownership<<" suppress="<<profile.suppress<<'\n';
    Run(root/(std::string(profile.standard)+"-"+std::to_string(profile.ownership)+(profile.suppress?"-explicit":"")),
        profile.standard,profile.ownership,profile.optimization,profile.suppress);
  }
}
