#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
constexpr const char *Source=R"union(
namespace UnionTemplates {
template<class T> union Plain {
  T value;
  unsigned char bytes[sizeof(T)];
  constexpr explicit Plain(T input):value(input){}
  constexpr T get() const {return value;}
  template<class U> constexpr U convert() const {return U(value);}
};
template<class T> union Plain<T *> {
  T *pointer;
  unsigned long bits;
  explicit Plain(T *input):pointer(input){}
  T get() const {return *pointer;}
};
template<> union Plain<double> {
  double value;
  unsigned long bits;
  constexpr explicit Plain(double input):value(input){}
  constexpr long get() const {return long(value);}
};
template<class T,int Alignment=alignof(T)> union alignas(Alignment) Aligned {
  T value;
  unsigned char bytes[sizeof(T)];
  explicit Aligned(T input):value(input){}
};
template<class T> struct Enclosing {
  template<class U> union Inner {
    U value;
    T alternate;
    explicit Inner(U input):value(input){}
    U get() const {return value;}
  };
  Inner<T> data;
};
struct Counts {inline static int live=0,copies=0,moves=0,destroyed=0;inline static bool fail_copy=false;};
struct Owner {
  int value;
  explicit Owner(int input):value(input){++Counts::live;}
  Owner(const Owner &other):value(other.value){if(Counts::fail_copy)throw 91;++Counts::live;++Counts::copies;}
  Owner(Owner &&other)noexcept:value(other.value){other.value=-1;++Counts::live;++Counts::moves;}
  ~Owner(){--Counts::live;++Counts::destroyed;}
};
struct MoveOnly {
  Owner value;
  explicit MoveOnly(int input):value(input){}
  MoveOnly(const MoveOnly &)=delete;
  MoveOnly(MoveOnly &&)=default;
};
template<class T> union Owned {
  T value;
  unsigned long inactive;
  explicit Owned(T input):value(static_cast<T &&>(input)){}
  Owned(const Owned &other):value(other.value){}
  Owned(Owned &&other)noexcept:value(static_cast<T &&>(other.value)){}
  ~Owned()noexcept{value.~T();}
  T &get(){return value;}
  const T &get()const{return value;}
};
template<class T> union Projected {
  struct {T value;T get()const{return value;}} first;
  struct {T value;T get()const{return value;}} second;
  explicit Projected(T input):first{input}{}
  T get()const{return first.get();}
};
using Native=Plain<long>;
Native provider(long value);
long consume(Native value);
using Owning=Owned<Owner>;
Owning owning_provider(int value);
int owning_consume(Owning value);
Native provider(long value){return Native(value);}
long consume(Native value){return value.get();}
Owning owning_provider(int value){return Owning(Owner(value));}
int owning_consume(Owning value){return value.get().value;}
inline int run() {
  static_assert(__is_union(Plain<int>) && __is_union(Plain<int *>) && __is_union(Plain<double>));
  static_assert(__is_union(Owned<Owner>) && __is_union(Enclosing<long>::Inner<int>));
  static_assert(sizeof(Plain<int>)==sizeof(int) && alignof(Aligned<int,64>)==64);
  static_assert(Plain<int>(42).get()==42);
  int value=42;Plain<int *> pointer(&value);Plain<double> special(42.0);
  Enclosing<long>::Inner<int> nested(42);Projected<int> projected(42);
  if(pointer.get()!=42 || special.get()!=42 || nested.get()!=42 || projected.get()!=42)return 1;
  if(consume(provider(42))!=42 || Plain<int>(42).convert<long>()!=42)return 2;
  if(Counts::live!=0)return 3;
  Counts::copies=Counts::moves=Counts::destroyed=0;
  {
    Owned<Owner> first(Owner(42));
    Owned<Owner> second(first);
    Owned<Owner> third(static_cast<Owned<Owner> &&>(second));
    if(first.get().value!=42 || third.get().value!=42 || second.get().value!=-1)return 4;
    if(Counts::live!=3 || Counts::copies!=1 || Counts::moves<2)return 5;
    Counts::fail_copy=true;
    try {Owned<Owner> failed(first);return 7;}catch(int error){if(error!=91)return 8;}
    Counts::fail_copy=false;
    if(Counts::live!=3)return 9;
  }
  if(Counts::live!=0 || Counts::destroyed<4)return 6;
  {
    auto owning=owning_provider(42);
    if(owning_consume(static_cast<Owning &&>(owning))!=42)return 10;
    Owned<MoveOnly> unique(MoveOnly(42));
    Owned<MoveOnly> moved(static_cast<Owned<MoveOnly> &&>(unique));
    if(moved.get().value.value!=42 || unique.get().value.value!=-1)return 11;
  }
  if(Counts::live!=0)return 12;
  return 0;
}
}
)union";

void Run(const fs::path &root,const char *standard,int ownership) {
  fs::create_directories(root/"provider");
  auto request=[&](const fs::path &input,const fs::path &output) {
    return Request(input,output,standard,ownership,"-O2",{"-fno-elide-constructors"});
  };
  const auto source=root/"provider/module.mm";
  auto publish=[&](const std::string &text) {
    Write(source,text);auto provider=request(source,root/"provider.o");
    provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
    provider.module_name="fixture.UnionTemplates";provider.package_identity="union-templates-v1";
    provider.interface_exports={"UnionTemplates"};provider.emit_semantic_interface=true;
    const auto built=Compile(provider);Success(built);
    assert(built.interface_artifact && built.interface_artifact->success);return *built.interface_artifact;
  };
  const auto artifact=publish(Source);
  assert(artifact.sidecar_json.find(kCxxClassTemplateSemantic)!=std::string::npos);
  auto changed=std::string(Source);auto at=changed.find("unsigned long inactive;");assert(at!=std::string::npos);
  changed.replace(at,std::string("unsigned long inactive;").size(),"unsigned long inactive[3];");
  assert(publish(changed).identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint);
  publish(Source);assert(fs::remove(source));
  const auto interface=root/"UnionTemplates.objc3i";
  Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  for(const char *name:{"a","b"}) {
    const auto input=root/(std::string(name)+".mm");
    Write(input,"extern \"C\" int check_"+std::string(name)+"(){return UnionTemplates::run();}");
    auto consumer=request(input,root/(std::string(name)+".o"));
    InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported};Success(Compile(consumer));
  }
  std::vector<llvm::StringRef> reader;
  for(const auto &capability:SupportedSemanticInterfaceCapabilities())
    if(capability!=kCxxClassTemplateSemantic)reader.push_back(capability);
  reader.push_back("objc3.cxx-class-templates.v1");
  auto unsupported=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);
  assert(!unsupported);llvm::consumeError(unsupported.takeError());
  reader.clear();for(const auto &capability:SupportedSemanticInterfaceCapabilities())reader.push_back(capability);
  auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
  auto forged=*decoded;bool changed_owner=false;
  for(auto &declaration:forged.declarations)
    if(declaration.kind=="ClassTemplate" && declaration.qualified_name=="UnionTemplates::Plain") {
      declaration.kind="Namespace";changed_owner=true;break;
    }
  assert(changed_owner);
  auto invalid_owner=EncodeSemanticInterfaceSidecar(forged,reader);
  assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
  const auto main=root/"main.cpp";
  Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
  auto entry=request(main,root/"main.o");Success(Compile(entry));
  LinkAndExecute(root,entry.output_path,ownership!=0,{});
}
}

int main() {
  test::IsolatedCases cases;
  llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-native-union-templates",temporary));
  const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
  for(const auto &[standard,ownership]:std::vector<std::pair<const char *,int>>{
      {"c++20",0},{"c++23",0},{"c++20",1},{"c++23",2}})
    cases.Run(std::string(standard)+"-"+std::to_string(ownership),[&]{
      Run(root/(std::string(standard)+"-"+std::to_string(ownership)),standard,ownership);
    });
  return cases.Finish();
}
