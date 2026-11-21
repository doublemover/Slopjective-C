#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
constexpr const char *Source=R"friends(
namespace NativeFriends {
template<class T> concept Number=__is_arithmetic(T);
class Value {
 int value;
public:
 explicit Value(int n):value(n){}
 template<Number T> friend int reveal(const Value &v,T add){return v.value+int(add);}
 template<class T> friend bool operator==(const Value &v,T other) requires Number<T>{return v.value==int(other);}
 template<class... T> friend int sum(const Value &v,T... values){return v.value+(0+...+int(values));}
 template<class T> friend int delayed(const Value&,T);
};
template<class T> inline int delayed(const Value &v,T add){return v.value+int(add)+1;}
template<class T> class Box {
 T value;
public:
 explicit Box(T n):value(n){}
 friend int read(const Box &v) requires Number<T>{return int(v.value);}
 template<Number U> friend int combine(const Box &v,U extra){return int(v.value)+int(extra);}
};
template<class T> class Box<T*> {
 T *value;
public:
 explicit Box(T *n):value(n){}
 friend int read(const Box &v) requires Number<T>{return int(*v.value);}
 template<Number U> friend int combine(const Box &v,U extra){return int(*v.value)+int(extra);}
};
template<class T> struct Outer {
 class Inner {
  T value;
 public:
  explicit Inner(T n):value(n){}
  friend int read(const Inner &v) requires Number<T>{return int(v.value);}
  template<Number U> friend int combine(const Inner &v,U extra){return int(v.value)+int(extra);}
 };
};
struct Reader;
template<class T> class Secret {
 int value;
 friend T;
public: explicit Secret(int n):value(n){}
};
struct Reader {static int read(const Secret<Reader> &v){return v.value;}};
template<class T> struct Base {
 explicit Base(T n):value(n){}
protected:
 using Value=T;
 T value;
 T add() const {return value;}
 T add(T delta) const {return value+delta;}
};
template<class T> class Adaptor: public Base<T> {
public:
 using Base<T>::Base;
 using typename Base<T>::Value;
 using Base<T>::add;
 Value read() const {return value;}
private:
 using Base<T>::value;
};
struct IntOverload {int call(int n) const {return n+1;}};
struct LongOverload {long call(long n) const {return n+2;}};
template<class... Bases> struct Combined: Bases... {using Bases::call...;};
template<class T> using BaseLink=Base<T>;
template<class T> struct AliasAdaptor: BaseLink<T> {
 using BaseLink<T>::BaseLink;
 using BaseLink<T>::add;
};
template<class Parent> struct GenericAdaptor: Parent {using Parent::Parent;};
template<class... Bases> struct TypeProjection: Bases... {using typename Bases::Value...;};
int provider(Value);
inline int exercise(){
 Value v(17);Box<int> i(19);Box<long> l(23);Secret<Reader> s(29);
 int n=31;Box<int*> p(&n);Outer<int>::Inner nested(37);
 return reveal(v,2)!=19 || reveal(v,2L)!=19 || !(v==17) || v==18 ||
     sum(v,1,2,3)!=23 || delayed(v,2)!=20 || read(i)!=19 || read(l)!=23 ||
     combine(i,3)!=22 || combine(l,4L)!=27 || read(p)!=31 || combine(p,1)!=32 ||
     read(nested)!=37 || combine(nested,1)!=38 || Reader::read(s)!=29 || provider(v)!=42;
}
int provider(Value v){return reveal(v,25);}
}
)friends";

void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");const auto source=root/"provider/module.mm";
 auto request=[&](const fs::path &input,const fs::path &output){return Request(input,output,standard,ownership,"-O2",{});};
 auto publish=[&](const std::string &text){
  Write(source,text);auto value=request(source,root/"provider.o");
  value.module_owned_names=true;value.module_source_root=(root/"provider").string();
  value.module_name="fixture.NativeFriends";value.package_identity="friend-templates-v1";
  value.interface_exports={"NativeFriends"};value.emit_semantic_interface=true;
  const auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);
  return *result.interface_artifact;
 };
 const auto artifact=publish(Source);const auto repeated=publish(Source);
 assert(artifact.source_bytes==repeated.source_bytes && artifact.sidecar_json==repeated.sidecar_json);
 assert(artifact.sidecar_json.find(kCxxFriendSemantic)!=std::string::npos);
 assert(artifact.sidecar_json.find(kCxxUsingSemantic)!=std::string::npos);
 for(const auto &[from,to]:std::vector<std::pair<std::string,std::string>>{
     {"return v.value+int(add);","return v.value+int(add)+2;"},
     {"requires Number<T>","requires (Number<T> && sizeof(T)<8)"},
     {"template<Number U>","template<class U>"},
     {"friend T;","friend Reader;"},
     {"private:\n using Base<T>::value;","public:\n using Base<T>::value;"}}){
  std::string changed=Source;const auto at=changed.find(from);assert(at!=std::string::npos);
  changed.replace(at,from.size(),to);assert(publish(changed).identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint);
 }
 auto reject_owners=[&](const std::string &text,const std::vector<std::string> &exports,const char *code){
  Write(source,text);auto value=request(source,root/"bad-provider.o");
  value.module_owned_names=true;value.module_source_root=(root/"provider").string();
  value.module_name="fixture.NativeFriends";value.package_identity="friend-templates-v1";
  value.interface_exports=exports;value.emit_semantic_interface=true;
  const auto result=Compile(value);
  assert(!result.success && result.interface_artifact && std::any_of(
      result.interface_artifact->diagnostics.begin(),result.interface_artifact->diagnostics.end(),
      [&](const auto &d){return d.code==code;}));
 };
 reject_owners("namespace NativeFriends{struct A{template<class T> friend int shared(T);};struct B{template<class T> friend int shared(T);};}",
     {"NativeFriends::A"},"O3IFC156");
 reject_owners("namespace NativeFriends{struct __attribute__((annotate(\"objc3.spi:Testing\"))) A{template<class T> friend int shared(T);};struct B{template<class T> friend int shared(T);};}",
     {"NativeFriends"},"O3IFC157");
 reject_owners(R"(
  namespace NativeFriends {
  template<class T> struct Base {static constexpr int value=42;};
  template<class T> using Hidden=Base<T>;
  template<class T> struct Public: Base<T> {using Hidden<T>::value;};
  })", {"NativeFriends::Base", "NativeFriends::Public"}, "O3IFC156");
 publish(Source);assert(fs::remove(source));
 const auto interface=root/"NativeFriends.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 auto imported=[&](const fs::path &input,const fs::path &output){
  auto value=request(input,output);InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
  value.imported_interfaces={dependency};return value;
 };
 for(const char *name:{"a","b"}){
  const auto input=root/(std::string(name)+".mm");
  Write(input,"extern \"C\" int check_"+std::string(name)+R"(() {
    NativeFriends::Adaptor<int> adapted(40);
    NativeFriends::Adaptor<int>::Value value=2;
    NativeFriends::Combined<NativeFriends::IntOverload,NativeFriends::LongOverload> calls;
    NativeFriends::AliasAdaptor<int> alias(41);
    NativeFriends::GenericAdaptor<NativeFriends::Adaptor<int>> generic(42);
    NativeFriends::TypeProjection<NativeFriends::Adaptor<int>>::Value projected=42;
    return NativeFriends::exercise() || adapted.read()!=40 || adapted.add()!=40 ||
        adapted.add(value)!=42 || calls.call(41)!=42 || calls.call(40L)!=42 ||
        alias.add(1)!=42 || generic.read()!=42 || projected!=42;
  })");
  Success(Compile(imported(input,root/(std::string(name)+".o"))));
 }
 unsigned ordinal=0;
 for(const auto &[text,message]:std::vector<std::pair<std::string,std::string>>{
     {"int bad(){NativeFriends::Value v(1);return reveal(v,\"bad\");}","no matching function"},
     {"int bad(){NativeFriends::Value v(1);return v.value;}","private member"},
     {"int bad(){NativeFriends::Box<void*> v(nullptr);return read(v);}","no matching function"},
     {"int bad(){NativeFriends::Value v(1);return NativeFriends::reveal(v,1);}","no member named"},
     {"int bad(){return NativeFriends::Adaptor<int>(0).value;}","private member"}}){
  const auto input=root/("bad-"+std::to_string(ordinal++)+".mm");Write(input,text);const auto result=Compile(imported(input,root/"bad.o"));
  const bool rejected=!result.success && std::any_of(result.diagnostics.begin(),result.diagnostics.end(),[&](const auto &d){return d.message.find(message)!=std::string::npos;});
  if(!rejected)test::ReportFailure(result);assert(rejected);
 }
 std::vector<llvm::StringRef> reader;
 for(const auto &capability:SupportedSemanticInterfaceCapabilities())if(capability!=kCxxFriendSemantic)reader.push_back(capability);
 reader.push_back("objc3.cxx-friends.v1");auto old=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);
 assert(!old);llvm::consumeError(old.takeError());reader.push_back(kCxxFriendSemantic);
 auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 auto old_using=reader;
 std::erase(old_using,llvm::StringRef(kCxxUsingSemantic));old_using.push_back("objc3.cxx-using.v1");
 auto rejected_using=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,old_using);
 assert(!rejected_using);llvm::consumeError(rejected_using.takeError());
 auto missing=*decoded;std::erase(missing.required_semantics,kCxxFriendSemantic);
 auto invalid=EncodeSemanticInterfaceSidecar(missing,reader);assert(!invalid);llvm::consumeError(invalid.takeError());
 const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
 auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
}
}

int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-friend-templates",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const auto &[standard,ownership]:std::vector<std::pair<const char *,int>>{{"c++20",0},{"c++23",0},{"c++20",1},{"c++23",2}})
  cases.Run(std::string(standard)+"-"+std::to_string(ownership),[&]{Run(root/(std::string(standard)+"-"+std::to_string(ownership)),standard,ownership);});
 return cases.Finish();
}
