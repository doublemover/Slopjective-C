#include "standard_library_test_support.h"
#include "llvm/ADT/ScopeExit.h"
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
    auto value=Request(input,output,standard,ownership,optimization,{});
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
  auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
}
}
int main() {
  llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-native-emission-policy",temporary));
  const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
  for(const auto &[standard,ownership,opt]:std::vector<std::tuple<const char *,int,const char *>>{{"c++20",0,"-O0"},{"c++23",1,"-O2"},{"c++23",2,"-O2"}}) {
    std::cerr<<standard<<" ownership="<<ownership<<'\n';Run(root/(std::string(standard)+"-"+std::to_string(ownership)),standard,ownership,opt);
  }
}
