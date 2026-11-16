#include "thread_safety_test_source.h"
#include "standard_library_test_support.h"
#include "llvm/ADT/ScopeExit.h"
#include "clang_foundation/interface_codec.h"
#include <set>
#include <sstream>
#include <tuple>
using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
std::string Read(const fs::path &path){std::ifstream in(path);assert(in.good());std::ostringstream out;out<<in.rdbuf();return out.str();}
void CheckIR(const fs::path &path,const std::string &quiet,const std::string &checked) {
  const auto ir=Read(path);
  auto attributes=[&](const std::string &symbol) {
    const std::string target="@"+symbol+"(";
    std::string line;
    for(size_t at=0;(at=ir.find("define ",at))!=std::string::npos;++at) {
      auto end=ir.find('\n',at);auto candidate=ir.substr(at,end-at);
      if(candidate.find(target)!=std::string::npos){line=std::move(candidate);break;}
    }
    assert(!line.empty());
    auto number=line.find(" #");assert(number!=std::string::npos);
    auto last=line.find_first_not_of("0123456789",number+2);
    auto group="attributes #"+line.substr(number+2,last-number-2)+" = ";
    auto at=ir.find(group);assert(at!=std::string::npos);
    return ir.substr(at,ir.find('\n',at)-at);
  };
  const auto quietAttributes=attributes(quiet),checkedAttributes=attributes(checked);
  assert(quietAttributes.find("disable_sanitizer_instrumentation")!=std::string::npos);
  assert(quietAttributes.find("noprofile")!=std::string::npos);
  assert(quietAttributes.find("sanitize_address")==std::string::npos);
  assert(quietAttributes.find("sspreq")==std::string::npos);
  assert(checkedAttributes.find("sanitize_address")!=std::string::npos);
  assert(checkedAttributes.find("sspreq")!=std::string::npos);
  assert(ir.find("section \".text.objc3.policy\"")!=std::string::npos);
  assert(ir.find("@llvm.used")!=std::string::npos);
}
void Run(const fs::path &root,const char *standard,int ownership,const char *optimization) {
  fs::create_directories(root/"provider");
  auto request=[&](const fs::path &input,const fs::path &output) {
    auto value=Request(input,output,standard,ownership,optimization,{"-fsanitize=address"});
    value.arguments.push_back("-fstack-protector-all");return value;
  };
  const auto source=root/"provider/module.mm";
  Write(source,R"policy(
namespace EmissionPolicy {
extern int storage __attribute__((weak,section(".data.objc3.policy")));
int storage=17;
__attribute__((weak)) int fallback(){return 5;}
__attribute__((used,retain,noinline,section(".text.objc3.policy"),no_sanitize("address","undefined"),disable_sanitizer_instrumentation,no_profile_instrument_function,no_instrument_function,no_stack_protector,nodebug))
inline int quiet(const int *value){return *value+storage;}
__attribute__((used,noinline)) inline int checked(const int *value){return *value;}
}
)policy");
  auto provider=request(source,root/"provider.o");provider.module_owned_names=true;
  provider.module_source_root=(root/"provider").string();provider.module_name="fixture.EmissionPolicy";
  provider.package_identity="emission-policy-v1";provider.interface_exports={"EmissionPolicy"};provider.emit_semantic_interface=true;
  auto built=Compile(provider);Success(built);assert(built.interface_artifact && built.interface_artifact->success);
  const auto artifact=*built.interface_artifact;
  std::string fallback,quiet,checked;
  for(const auto &declaration:built.declarations) {
    if(declaration.qualified_name=="EmissionPolicy::fallback")fallback=declaration.native_symbol_name;
    if(declaration.qualified_name=="EmissionPolicy::quiet")quiet=declaration.native_symbol_name;
    if(declaration.qualified_name=="EmissionPolicy::checked")checked=declaration.native_symbol_name;
  }
  assert(!fallback.empty() && !quiet.empty() && !checked.empty());
  auto ir=provider;ir.emit=EmitKind::LLVM;ir.output_path=(root/"provider.ll").string();Success(Compile(ir));
  CheckIR(root/"provider.ll",quiet,checked);
  const auto interface=root/"EmissionPolicy.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  assert(fs::remove(source));
  for(const char *name:{"a","b"}) {
    const auto input=root/(std::string("consumer-")+name+".mm");
    Write(input,"extern \"C\" int check_"+std::string(name)+R"policy((){int value=3;return EmissionPolicy::quiet(&value)!=20 || EmissionPolicy::checked(&value)!=3 || EmissionPolicy::fallback()!=31;})policy");
    auto consumer=request(input,root/(std::string(name)+".o"));
    InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported};Success(Compile(consumer));
    consumer.emit=EmitKind::LLVM;consumer.output_path=(root/(std::string(name)+".ll")).string();Success(Compile(consumer));
    CheckIR(root/(std::string(name)+".ll"),quiet,checked);
  }
  const auto main=root/"main.cpp";
  Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();\nextern \"C\" int override_policy() __asm__(\""+fallback+"\");\nextern \"C\" int override_policy(){return 31;}\nint main(){return check_a() || check_b();}\n");
  auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{"-fsanitize=address"});
}
void RunThreadSafety(const fs::path &root,const char *standard,int ownership,const char *optimization) {
 fs::create_directories(root/"provider");
 auto request=[&](const fs::path &input,const fs::path &output){return Request(input,output,standard,ownership,optimization,{"-Wthread-safety","-Werror=thread-safety"});};
 const auto source=root/"provider/module.mm";Write(source,kNativeThreadSafetySource);
 auto provider=request(source,root/"provider.o");provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();provider.module_name="fixture.ThreadSafety";provider.package_identity="thread-safety-v1";provider.interface_exports={"ThreadSafety"};provider.emit_semantic_interface=true;
 auto built=Compile(provider);Success(built);assert(built.interface_artifact && built.interface_artifact->success);
 const auto artifact=*built.interface_artifact;assert(artifact.sidecar_json.find(kNativeThreadSafetySemantic)!=std::string::npos);
 const auto interface=root/"ThreadSafety.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);assert(fs::remove(source));
 auto imported_request=[&](const fs::path &input,const fs::path &output){auto consumer=request(input,output);InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;consumer.imported_interfaces={imported};return consumer;};
 for(const char *name:{"a","b"}) {
  const auto input=root/(std::string("consumer-")+name+".mm");Write(input,"extern \"C\" int check_"+std::string(name)+R"thread((){
   ThreadSafety::Counter counter;
   ThreadSafety::acquire(&counter.mutex);counter.bump();int first=counter.read();ThreadSafety::release(&counter.mutex);
   if(first!=1 || counter.mutex.state!=0)return 1;
   if(ThreadSafety::run(counter)!=2 || counter.mutex.state!=0)return 2;
   if(counter.mutex.try_lock()){counter.bump();int value=counter.read();counter.mutex.unlock();if(value!=3)return 3;}
   {ThreadSafety::Scope guard(counter.mutex);if(counter.read()!=3)return 4;}
   ThreadSafety::Box<int> box;{ThreadSafety::Scope guard(box.mutex);if(box.read()!=0)return 5;}
   counter.mutex.lock_shared();int shared=counter.read();counter.mutex.unlock_shared();if(shared!=3)return 6;
   if(counter.mutex.try_shared()){int shared=counter.read();counter.mutex.unlock_shared();if(shared!=3)return 7;}
   counter.get_mutex().lock();counter.mutex.assert_held();int asserted=counter.read();counter.mutex.unlock_any();if(asserted!=3)return 8;
   counter.mutex.lock_shared();counter.mutex.assert_shared();int shared_asserted=counter.read();counter.mutex.unlock_shared();if(shared_asserted!=3)return 9;
   return counter.mutex.state!=0;
  })thread");Success(Compile(imported_request(input,root/(std::string(name)+".o"))));
 }
 const auto bad=root/"unsafe.mm";Write(bad,"int missing_lock(ThreadSafety::Counter &counter){return counter.read();}");
 auto rejected=Compile(imported_request(bad,root/"unsafe.o"));assert(!rejected.success);
 assert(std::any_of(rejected.diagnostics.begin(),rejected.diagnostics.end(),[](const auto &d){return d.message.find("requires holding mutex")!=std::string::npos;}));
 const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
 auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
}

void RunSwiftABI(const fs::path &root,const char *standard,int ownership,const char *optimization) {
  fs::create_directories(root/"provider");
  const auto source=root/"provider/module.mm";
  Write(source,R"swift(
namespace SwiftABI {
struct Pair {long first;long second;};
__attribute__((swiftcall,noinline)) int context(int value,void *state __attribute__((swift_context))) {return value+*static_cast<int *>(state);}
__attribute__((swiftcall,noinline)) void result(Pair *out __attribute__((swift_indirect_result)),int value) {out->first=value;out->second=value+2;}
__attribute__((swiftcall,noinline)) void erased(void *out __attribute__((swift_indirect_result)),int value) {auto *pair=static_cast<Pair *>(out);pair->first=value;pair->second=value+2;}
__attribute__((swiftcall,noinline)) void reference(Pair &out __attribute__((swift_indirect_result)),int value) {out.first=value;out.second=value+2;}
__attribute__((swiftcall,noinline)) int checked(int value,void *state __attribute__((swift_context)),void **error __attribute__((swift_error_result))) {*error=value<0?state:nullptr;return value<0?0:value+*static_cast<int *>(state);}
__attribute__((swiftasynccall,noinline)) int resume(void *state __attribute__((swift_async_context))) {return *static_cast<int *>(state);}
typedef int Callback(int,void * __attribute__((swift_context))) __attribute__((swiftcall));
inline int invoke(Callback *callback,int value,void *state){return callback(value,state);}
}
)swift");
  auto provider=Request(source,root/"provider.o",standard,ownership,optimization,{});
  provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
  provider.module_name="fixture.SwiftABI";provider.package_identity="swift-abi-v1";
  provider.interface_exports={"SwiftABI"};provider.emit_semantic_interface=true;
  auto built=Compile(provider);Success(built);assert(built.interface_artifact && built.interface_artifact->success);
  const auto artifact=*built.interface_artifact;
  assert(artifact.sidecar_json.find("objc3.native-declaration-attributes.v3")!=std::string::npos);
  const auto interface=root/"SwiftABI.objc3i";
  Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  assert(fs::remove(source));
  for(const char *name:{"a","b"}) {
    const auto input=root/(std::string("consumer-")+name+".mm");
    Write(input,"extern \"C\" int check_"+std::string(name)+R"swift((){
      int state=2;void *error=nullptr;SwiftABI::Pair pair{};
      if(SwiftABI::invoke(&SwiftABI::context,40,&state)!=42)return 1;
      SwiftABI::result(&pair,40);if(pair.first!=40 || pair.second!=42)return 2;
      SwiftABI::erased(&pair,41);if(pair.first!=41 || pair.second!=43)return 5;
      SwiftABI::reference(pair,42);if(pair.first!=42 || pair.second!=44)return 6;
      if(SwiftABI::checked(40,&state,&error)!=42 || error)return 3;
      if(SwiftABI::resume(&state)!=2)return 4;
      return SwiftABI::checked(-1,&state,&error)!=0 || error!=&state;
    })swift");
    auto consumer=Request(input,root/(std::string(name)+".o"),standard,ownership,optimization,{});
    InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported};Success(Compile(consumer));
    consumer.emit=EmitKind::LLVM;consumer.output_path=(root/(std::string(name)+".ll")).string();Success(Compile(consumer));
    const auto ir=Read(consumer.output_path);
    assert(ir.find("swiftcc")!=std::string::npos && ir.find("swiftself")!=std::string::npos);
    assert(ir.find("swifterror")!=std::string::npos && ir.find("sret(")!=std::string::npos);
    assert(ir.find("swifttailcc")!=std::string::npos && ir.find("swiftasync")!=std::string::npos);
    assert(ir.find("sret(ptr)")==std::string::npos);
    assert(ir.find("sret([0 x i8])")!=std::string::npos);
  }
  Write(root/"main.cpp","extern \"C\" int check_a();extern \"C\" int check_b();int main(){int first=check_a();return first?first:check_b();}");
  auto main=Request(root/"main.cpp",root/"main.o",standard,ownership,optimization,{});Success(Compile(main));
  LinkAndExecute(root,main.output_path,ownership!=0,{});
}

void RunTargetPolicy(const fs::path &root,const char *standard,int ownership,const char *optimization) {
  fs::create_directories(root/"provider");
  auto request=[&](const fs::path &input,const fs::path &output){return Request(input,output,standard,ownership,optimization,{});};
  const auto source=root/"provider/module.mm";
  Write(source,R"target(
namespace TargetPolicy {
__attribute__((target("sse2"),min_vector_width(128),noinline)) inline int specialized(int value){return value+1;}
__attribute__((target_clones("default","sse4.2"))) inline int cloned(int value){return value+2;}
__attribute__((target_clones("default","sse4.2"))) int declared(int);
__attribute__((target("default"))) inline int default_only(int value){return value+5;}
__attribute__((target("default"))) inline int versioned(int value){return value+6;}
__attribute__((target("sse4.2"))) inline int versioned(int value){return value+7;}

template<class T>__attribute__((target("sse2"),min_vector_width(128),noinline)) inline int generic(T value){return static_cast<int>(value)+3;}
struct Record {__attribute__((target("sse2"),min_vector_width(128),noinline)) int method(int value)const{return value+4;}};
int external_caller(int value){return specialized(value)+cloned(value)+generic(value)+Record().method(value);}
}
)target");
  auto provider=request(source,root/"provider.o");provider.module_owned_names=true;
  provider.module_source_root=(root/"provider").string();provider.module_name="fixture.TargetPolicy";
  provider.package_identity="target-policy-v1";provider.interface_exports={"TargetPolicy"};provider.emit_semantic_interface=true;
  auto built=Compile(provider);Success(built);assert(built.interface_artifact && built.interface_artifact->success);
  const auto artifact=*built.interface_artifact;
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,supported);assert(decoded);
  std::set<std::string> variants,families;
  for(const auto &declaration:decoded->declarations)if(declaration.qualified_name=="TargetPolicy::versioned") {
    assert(declaration.canonical_type_identity.find(";native-cpu-variant:v1;")!=std::string::npos);
    variants.insert(declaration.stable_id);families.insert(declaration.native_symbol_name);
  }
  assert(variants.size()==2 && families.size()==1);

  auto check_ir=[&](const fs::path &path) {
    const auto ir=Read(path);
    assert(ir.find("ifunc i32")!=std::string::npos);
    assert(ir.find(".resolver")!=std::string::npos);
    assert(ir.find("+sse2")!=std::string::npos && ir.find("+sse4.2")!=std::string::npos);
    assert(ir.find("\"min-legal-vector-width\"=\"128\"")!=std::string::npos);
  };
  auto ir=provider;ir.emit=EmitKind::LLVM;ir.output_path=(root/"provider.ll").string();Success(Compile(ir));check_ir(root/"provider.ll");
  const auto interface=root/"TargetPolicy.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  assert(fs::remove(source));
  for(const char *name:{"a","b"}) {
    const auto input=root/(std::string("consumer-")+name+".mm");
    Write(input,"extern \"C\" int check_"+std::string(name)+R"target((){
      return TargetPolicy::specialized(41)!=42 || TargetPolicy::cloned(40)!=42 ||
        TargetPolicy::generic(39)!=42 || TargetPolicy::Record().method(38)!=42 ||
        TargetPolicy::external_caller(1)!=14 || TargetPolicy::default_only(37)!=42 ||
        TargetPolicy::versioned(36)!=(__builtin_cpu_supports("sse4.2")?43:42);
    })target");
    auto consumer=request(input,root/(std::string(name)+".o"));
    InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported};Success(Compile(consumer));
    consumer.emit=EmitKind::LLVM;consumer.output_path=(root/(std::string(name)+".ll")).string();Success(Compile(consumer));check_ir(root/(std::string(name)+".ll"));
  }
  const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
  auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
}

}
int main() {
  test::IsolatedCases cases;
  llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-native-emission-policy",temporary));
  const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
  for(const auto &[standard,ownership,opt]:std::vector<std::tuple<const char *,int,const char *>>{{"c++20",0,"-O0"},{"c++23",1,"-O2"},{"c++23",2,"-O2"}}) {
    std::cerr<<standard<<" ownership="<<ownership<<'\n';cases.Run(std::string(standard)+"-"+std::to_string(ownership),[&]{Run(root/(std::string(standard)+"-"+std::to_string(ownership)),standard,ownership,opt);});
  }
  for(const auto &[standard,ownership,opt]:std::vector<std::tuple<const char *,int,const char *>>{{"c++20",0,"-O0"},{"c++23",1,"-O2"},{"c++23",2,"-O2"}})
    cases.Run(std::string("swift-")+standard+"-"+std::to_string(ownership),[&]{RunSwiftABI(root/(std::string("swift-")+standard+"-"+std::to_string(ownership)),standard,ownership,opt);});
  for(const auto &[standard,ownership,opt]:std::vector<std::tuple<const char *,int,const char *>>{{"c++20",0,"-O0"},{"c++23",1,"-O2"},{"c++23",2,"-O2"}})
    cases.Run(std::string("target-")+standard+"-"+std::to_string(ownership),[&]{RunTargetPolicy(root/(std::string("target-")+standard+"-"+std::to_string(ownership)),standard,ownership,opt);});
  for(const auto &[standard,ownership,opt]:std::vector<std::tuple<const char *,int,const char *>>{{"c++20",0,"-O0"},{"c++23",1,"-O2"},{"c++23",2,"-O2"}})
    cases.Run(std::string("thread-")+standard+"-"+std::to_string(ownership),[&]{RunThreadSafety(root/(std::string("thread-")+standard+"-"+std::to_string(ownership)),standard,ownership,opt);});
  return cases.Finish();
}
