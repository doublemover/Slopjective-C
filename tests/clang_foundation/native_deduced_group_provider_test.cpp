#include "standard_library_test_support.h"
#include "llvm/ADT/ScopeExit.h"
#include <tuple>
using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
void Run(const fs::path &root,const char *standard,int ownership,const char *optimization) {
  fs::create_directories(root/"provider");
  fs::create_directories(root/"include/objc3c");
  fs::create_directory_symlink(OBJC3_GROUP_TEST_RUNTIME_HEADERS,root/"include/objc3c/runtime");
  auto request=[&](const fs::path &input,const fs::path &output) {
    auto value=Request(input,output,standard,ownership,optimization,{});
    value.arguments.insert(value.arguments.end(),{"-fexceptions","-fobjc-exceptions","-fobjc-arc-exceptions","-fcoro-aligned-allocation"});
    value.arguments.push_back("-I"+(root/"include").string());
    return value;
  };
  const auto source=root/"provider/module.mm";
  Write(source,R"group(
#include <objc3c/runtime/objc3_task.hpp>
namespace PublishedGroup {
extern int live,entered;
int live=0,entered=0;
struct Value {
 int value;
 explicit Value(int value):value(value){++live;}
 Value(const Value &other):value(other.value){++live;}
 Value(Value &&other) noexcept:value(other.value){++live;}
 ~Value() noexcept{--live;}
};
inline objc3::task<int> leaf(int value){co_await objc3::task_yield;co_return value;}
inline objc3::task<int> callback(objc3::task_group_handle group){++entered;auto child=leaf(42).start_child(group);co_return co_await child;}
inline auto make(){return objc3::with_task_group(callback);}
inline auto captured(int value){return objc3::with_task_group([value](objc3::task_group_handle group)->objc3::task<int>{++entered;auto child=leaf(value).start_child(group);co_return co_await child;});}
inline objc3::task<Value> record_leaf(int value){co_await objc3::task_yield;co_return Value(value);}
inline auto record(int value){return objc3::with_task_group([value](objc3::task_group_handle group)->objc3::task<Value>{auto child=record_leaf(value).start_child(group);co_return co_await child;});}
inline objc3::task<int> failure_leaf(){throw 777;co_return 0;}
inline auto failure(){return objc3::with_task_group([](objc3::task_group_handle group)->objc3::task<int>{auto child=failure_leaf().start_child(group);co_return co_await child;});}
}
)group");
  auto provider=request(source,root/"provider.o");provider.module_owned_names=true;
  provider.module_source_root=(root/"provider").string();provider.module_name="fixture.DeducedGroup";
  provider.package_identity="deduced-group-v1";provider.interface_exports={"PublishedGroup"};provider.emit_semantic_interface=true;
  auto built=Compile(provider);Success(built);assert(built.interface_artifact && built.interface_artifact->success);
  const auto artifact=*built.interface_artifact;
  const auto interface=root/"DeducedGroup.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  assert(fs::remove(source));
  for(const char *name:{"a","b"}) {
    const auto input=root/(std::string("consumer-")+name+".mm");
    Write(input,"extern \"C\" int check_"+std::string(name)+R"group((){
      static_assert(std::is_same_v<decltype(PublishedGroup::make()),objc3::task<int>>);
      static_assert(std::is_same_v<decltype(PublishedGroup::record(42)),objc3::task<PublishedGroup::Value>>);
      objc3_executor_t *executor=nullptr;
      if(objc3_executor_create(1,&executor)!=OBJC3_SERVICE_OK)return 1;
      uint32_t ran=0;
      auto task=PublishedGroup::make().start(executor);
      if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK || task.result()!=42)return 2;
      task={};
      auto captured=PublishedGroup::captured(43).start(executor);
      if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK || captured.result()!=43)return 3;
      captured={};
      {
        auto record=PublishedGroup::record(44).start(executor);
        if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 4;
        auto result=record.result();record={};if(result.value!=44)return 5;
      }
      if(PublishedGroup::live)return 6;
      auto failure=PublishedGroup::failure().start(executor);
      if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 7;
      bool caught=false;try{(void)failure.result();}catch(int value){if(value!=777)return 8;caught=true;}
      if(!caught)return 9;failure={};
      const auto entered=PublishedGroup::entered;
      auto stopped=PublishedGroup::captured(99).start(executor);stopped.cancel();
      if(objc3_executor_pump(executor,UINT32_MAX,&ran)!=OBJC3_SERVICE_OK)return 10;
      caught=false;try{(void)stopped.result();}catch(const objc3::task_cancelled &){caught=true;}
      if(!caught || PublishedGroup::entered!=entered)return 11;stopped={};
      if(PublishedGroup::live)return 12;
      objc3_executor_release(executor);return 0;
    })group");
    auto consumer=request(input,root/(std::string(name)+".o"));
    InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported};Success(Compile(consumer));
  }
  const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}\n");
  auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,true,{OBJC3_GROUP_TEST_RUNTIME_LIBRARY,"-pthread"});
}
}
int main() {
  llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-native-deduced-group",temporary));
  const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
  for(const auto &[standard,ownership,opt]:std::vector<std::tuple<const char *,int,const char *>>{{"c++20",1,"-O0"},{"c++20",2,"-O0"},{"c++23",1,"-O2"},{"c++23",2,"-O2"}}) {
    std::cerr<<standard<<" ownership="<<ownership<<'\n';Run(root/(std::string(standard)+"-"+std::to_string(ownership)),standard,ownership,opt);
  }
}
