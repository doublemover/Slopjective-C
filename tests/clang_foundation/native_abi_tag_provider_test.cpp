#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
constexpr const char *Source=R"abi(
namespace NativeABITags {
struct __attribute__((abi_tag("zeta","alpha"))) Value;
struct __attribute__((abi_tag("alpha","zeta"))) Value {int number;};
template<class T> struct __attribute__((abi_tag("container"))) Box {
  T value;
  template<class U> __attribute__((abi_tag("member"))) U get()const{return U(value);}
  template<class U> __attribute__((abi_tag("member_data"))) inline static U data=U(42);
};
template<class T> struct __attribute__((abi_tag("container"))) Box<T *> {
  T *value;
  T get()const{return *value;}
};
template<> struct __attribute__((abi_tag("container"))) Box<double> {
  double value;
  int get()const{return int(value);}
};
template<class T> union __attribute__((abi_tag("choice"))) Choice {
  T value;
  unsigned long inactive;
  explicit Choice(T input):value(input){}
};
template<class T> __attribute__((abi_tag("function"))) inline T identity(T value){return value;}
template<class T> __attribute__((abi_tag("variable"))) inline int data=42;
template<class T> __attribute__((abi_tag("variable"))) inline int data<T *> =42;
template<> __attribute__((abi_tag("variable"))) inline int data<double> =42;
template<> __attribute__((abi_tag("function"))) inline long identity<long>(long value){return value;}
template<> template<> __attribute__((abi_tag("member"))) inline long Box<int>::get<long>()const{return long(value);}
template<> template<> __attribute__((abi_tag("member_data"))) inline int Box<int>::data<int> =42;
inline auto local_type(int input){struct __attribute__((abi_tag("local"))) Local {int number;};return Local{input};}
struct __attribute__((abi_tag("base"))) Base {
  virtual ~Base()=default;
  virtual int get()const=0;
};
struct __attribute__((abi_tag("derived"))) Derived:Base {
  int number;
  explicit Derived(int input):number(input){}
  int get()const override{return number;}
};
Value provider(Value input);
int consume(Value input);
Base *polymorphic(int input);
void destroy(Base *value);
Value provider(Value input){return input;}
int consume(Value input){return input.number;}
Base *polymorphic(int input){return new Derived(input);}
void destroy(Base *value){delete value;}
inline int run() {
  Value input{42};
  if(consume(provider(input))!=42)return 1;
  Box<int> box{42};int number=42;Box<int *> pointer{&number};Box<double> special{42.0};
  if(box.get<long>()!=42 || pointer.get()!=42 || special.get()!=42)return 2;
  Choice<int> choice(42);if(choice.value!=42 || identity(42)!=42)return 3;
  if(data<int>!=42 || data<int *>!=42 || data<double>!=42 || Box<int>::data<long>!=42 || Box<int>::data<int>!=42)return 4;
  if(identity(42L)!=42 || local_type(42).number!=42)return 6;
  Base *base=polymorphic(42);auto *derived=dynamic_cast<Derived *>(base);
  bool valid=derived && derived->number==42 && base->get()==42;
  destroy(base);return valid?0:5;
}
}
)abi";

void Run(const fs::path &root,const char *standard,int ownership) {
  fs::create_directories(root/"provider");
  const auto source=root/"provider/module.mm";
  auto request=[&](const fs::path &input,const fs::path &output) {
    return Request(input,output,standard,ownership,"-O0",{});
  };
  auto provider_request=[&] {
    auto provider=request(source,root/"provider.o");
    provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
    provider.module_name="fixture.NativeABITags";provider.package_identity="native-abi-tag-v1";
    provider.interface_exports={"NativeABITags"};provider.emit_semantic_interface=true;
    return provider;
  };
  auto publish=[&](const std::string &text) {
    Write(source,text);const auto built=Compile(provider_request());Success(built);
    assert(built.interface_artifact && built.interface_artifact->success);return *built.interface_artifact;
  };
  const auto artifact=publish(Source);
  assert(artifact.sidecar_json.find(kNativeDeclarationAttributeSemantic)!=std::string::npos);
  for(const auto *name:{"alpha","zeta","container","member","member_data","choice","function","variable","derived","base"})
    assert(artifact.source_bytes.find(std::string("\"")+name+"\"")!=std::string::npos);
  const auto repeated=publish(Source);
  assert(repeated.source_bytes==artifact.source_bytes && repeated.sidecar_json==artifact.sidecar_json);
  auto changed=std::string(Source);auto at=changed.find("\"zeta\",\"alpha\"");assert(at!=std::string::npos);
  changed.replace(at,std::string("\"zeta\",\"alpha\"").size(),"\"alpha\",\"zeta\"");
  assert(publish(changed).identity.semantic_fingerprint==artifact.identity.semantic_fingerprint);
  changed=Source;
  for(at=0;(at=changed.find("\"container\"",at))!=std::string::npos;at+=14)
    changed.replace(at,11,"\"container_v2\"");
  assert(publish(changed).identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint);
  for(const std::string declaration:{
      "struct __attribute__((abi_tag(\"vendor\",\"__objc3_m_foreign\"))) Forged {int value;};",
      "template<class T>struct __attribute__((abi_tag(\"vendor\",\"__objc3_m_foreign\"))) Forged {T value;};",
      "template<class T>__attribute__((abi_tag(\"vendor\",\"__objc3_m_foreign\"))) T forged(T value){return value;}",
      "template<class T>__attribute__((abi_tag(\"vendor\",\"__objc3_m_foreign\"))) inline T forged{};",
      "struct __attribute__((abi_tag(\"vendor\",\"__objc3_m_first\",\"__objc3_m_second\"))) Forged {int value;};"}) {
    Write(source,"namespace NativeABITags{"+declaration+"}");
    const auto rejected=Compile(provider_request());assert(!rejected.success);
  }
  publish(Source);assert(fs::remove(source));
  const auto interface=root/"NativeABITags.objc3i";
  Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  for(const char *name:{"a","b"}) {
    const auto input=root/(std::string(name)+".mm");
    Write(input,"extern \"C\" int check_"+std::string(name)+"(){return NativeABITags::run();}");
    auto consumer=request(input,root/(std::string(name)+".o"));InterfaceImportInput imported;
    imported.source_path=interface.string();imported.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported};Success(Compile(consumer));
  }
  std::vector<llvm::StringRef> reader;
  for(const auto &capability:SupportedSemanticInterfaceCapabilities())
    if(capability!=kNativeDeclarationAttributeSemantic)reader.push_back(capability);
  reader.push_back("objc3.native-declaration-attributes.v4");
  auto unsupported=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);
  assert(!unsupported);llvm::consumeError(unsupported.takeError());
  const auto main=root/"main.cpp";
  Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
  auto entry=request(main,root/"main.o");Success(Compile(entry));
  LinkAndExecute(root,entry.output_path,ownership!=0,{});
}
}

int main() {
  test::IsolatedCases cases;
  llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-native-abi-tags",temporary));
  const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
  for(const auto &[standard,ownership]:std::vector<std::pair<const char *,int>>{
      {"c++20",0},{"c++23",0},{"c++20",1},{"c++23",2}})
    cases.Run(std::string(standard)+"-"+std::to_string(ownership),[&]{
      Run(root/(std::string(standard)+"-"+std::to_string(ownership)),standard,ownership);
    });
  return cases.Finish();
}
