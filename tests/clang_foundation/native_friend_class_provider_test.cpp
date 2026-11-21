#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
constexpr const char *Source=R"friends(
namespace NativeClassFriends {
template<class T> concept Integral=__is_integral(T);
template<class T> struct Inspector;
template<class T> struct Cell {T value;};
class Value {
 int value;
 template<class U> friend struct Inspector;
 template<class... U> friend struct PackInspector;
 template<template<class> class F> friend struct ShapeInspector;
 template<int... N> friend struct NumberInspector;
 template<Integral U> friend struct ConstrainedInspector;
 template<class U> friend struct Passive;
 friend Inspector<void>;
public: explicit Value(int n):value(n){}
};
template<class T> class Box {
 T value;
 template<class U> friend struct Inspector;
public: explicit Box(T n):value(n){}
};
template<class T> class Box<T*> {
 T *value;
 template<class U> friend struct Inspector;
public: explicit Box(T *n):value(n){}
};
template<class T> class Selected {
 T value;
 friend Inspector<T>;
public: explicit Selected(T n):value(n){}
};
template<class T> using InspectorAlias=Inspector<T>;
template<class T> class Aliased {
 T value;
 friend InspectorAlias<T>;
public: explicit Aliased(T n):value(n){}
};
template<class T> struct Outer {
 class Inner {
  T value;
  template<class U> friend struct Inspector;
 public: explicit Inner(T n):value(n){}
 };
};
template<class U> struct Inspector {
 static int read(const Value &v){return v.value;}
 template<class T> static int box(const Box<T> &v){if constexpr(__is_pointer(T))return int(*v.value);else return int(v.value);}
 template<class T> static int selected(const Selected<T> &v){return int(v.value);}
 template<class T> static int aliased(const Aliased<T> &v){return int(v.value);}
 template<class T> static int nested(const typename Outer<T>::Inner &v){return int(v.value);}
};
template<class... U> struct PackInspector {static int read(const Value &v){return v.value+sizeof...(U);}};
template<template<class> class F> struct ShapeInspector {static int read(const Value &v){return v.value+sizeof(F<int>);}};
template<int... N> struct NumberInspector {static int read(const Value &v){return v.value+(0+...+N);}};
template<Integral U> struct ConstrainedInspector {static int read(const Value &v){return v.value;}};
template<class U> struct Passive {};
template<class U> struct Alternate {};
#if __cplusplus >= 202302L
constexpr int phase(int n){if consteval{return n+1;}else{return n+2;}}
constexpr int inverted(int n){if !consteval{return n+3;}else{return n+4;}}
inline int control(){static_assert(phase(40)==41);static_assert(inverted(40)==44);return phase(1)!=3 || inverted(1)!=4;}
#else
inline int control(){return 0;}
#endif
int provider(Value);
inline int exercise(){
 Value value(17);Box<int> box(19);int n=23;Box<int*> pointer(&n);
 Selected<int> selected(29);Aliased<int> alias(31);Outer<long>::Inner nested(37);
 return Inspector<long>::read(value)!=17 || Inspector<int>::box(box)!=19 || Inspector<long>::box(pointer)!=23 ||
     Inspector<int>::selected(selected)!=29 || Inspector<int>::aliased(alias)!=31 || Inspector<int>::nested<long>(nested)!=37 ||
     PackInspector<int,long>::read(value)!=19 || ShapeInspector<Cell>::read(value)!=17+sizeof(Cell<int>) ||
     NumberInspector<1,2,3>::read(value)!=23 || ConstrainedInspector<int>::read(value)!=17 || control() || provider(value)!=17;
}
int provider(Value value){return Inspector<int>::read(value);}
}
)friends";

void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");const auto source=root/"provider/module.mm";
 auto request=[&](const fs::path &input,const fs::path &output){return Request(input,output,standard,ownership,"-O2",{});};
 auto emit=[&](const std::string &text,const std::vector<std::string> &exports,bool owned=true){
  Write(source,text);auto value=request(source,root/"provider.o");
  value.module_owned_names=owned;if(owned)value.module_source_root=(root/"provider").string();
  value.module_name="fixture.NativeClassFriends";value.package_identity="friend-classes-v1";
  value.interface_exports=exports;value.emit_semantic_interface=true;return Compile(value);
 };
 auto publish=[&](const std::string &text){
  const auto result=emit(text,{"NativeClassFriends"});Success(result);
  assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;
 };
 const auto artifact=publish(Source);const auto repeated=publish(Source);
 assert(artifact.source_bytes==repeated.source_bytes && artifact.sidecar_json==repeated.sidecar_json);
 const auto replayed=emit(artifact.source_bytes,{"NativeClassFriends"},false);Success(replayed);
 assert(replayed.interface_artifact);
 if(replayed.interface_artifact->source_bytes!=artifact.source_bytes)
  std::cerr<<"ORIGINAL_CANONICAL\n"<<artifact.source_bytes<<"REGENERATED_CANONICAL\n"<<replayed.interface_artifact->source_bytes<<"END_CANONICAL\n";
 assert(replayed.interface_artifact->source_bytes==artifact.source_bytes);
 assert(replayed.interface_artifact->identity.semantic_fingerprint==artifact.identity.semantic_fingerprint);
 const std::string constant_body="namespace NativeClassFriends{inline int control(){if constexpr(true)return 1;else return 2;}}";
 const auto constant_only=publish(constant_body);
 assert(constant_only.sidecar_json.find(kCxxConstantEvaluationControlSemantic)!=std::string::npos);
 auto ordinary_body=constant_body;ordinary_body.replace(ordinary_body.find("if constexpr"),12,"if");
 const auto ordinary=publish(ordinary_body);
 assert(ordinary.sidecar_json.find(kCxxConstantEvaluationControlSemantic)==std::string::npos);
 assert(ordinary.identity.semantic_fingerprint!=constant_only.identity.semantic_fingerprint);
 auto unpublished=constant_body;unpublished.erase(unpublished.find("inline "),7);
 assert(publish(unpublished).sidecar_json.find(kCxxConstantEvaluationControlSemantic)==std::string::npos);
 assert(artifact.sidecar_json.find(kCxxFriendSemantic)!=std::string::npos);
 assert(artifact.sidecar_json.find(kCxxConstantEvaluationControlSemantic)!=std::string::npos);
 std::string renamed=Source;
 const std::string before="template<class U> friend struct Passive;";
 renamed.replace(renamed.find(before),before.size(),"template<class Renamed> friend struct Passive;");
 assert(publish(renamed).identity.semantic_fingerprint==artifact.identity.semantic_fingerprint);
 for(const auto &[from,to]:std::vector<std::pair<std::string,std::string>>{
     {"friend struct Passive;","friend struct Alternate;"},
     {"friend Inspector<void>;","friend Inspector<char>;"}}){
  std::string changed=Source;const auto at=changed.find(from);assert(at!=std::string::npos);
  changed.replace(at,from.size(),to);assert(publish(changed).identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint);
 }
 // A template friendship cannot expose an excluded/SPI-only class implicitly.
 for(const auto &[text,exports]:std::vector<std::pair<std::string,std::vector<std::string>>>{
     {"namespace NativeClassFriends{class Value{template<class U>friend struct Inspector;};template<class U>struct Inspector{};}",{"NativeClassFriends::Value"}},
     {"namespace NativeClassFriends{template<class U>struct __attribute__((annotate(\"objc3.spi:Testing\"))) Inspector{};class Value{template<class U>friend struct Inspector;};}",{"NativeClassFriends"}},
     {"namespace NativeClassFriends{template<class T>struct Outer{struct Inner;};class Value{template<class T>friend class Outer<T>::Inner;};}",{"NativeClassFriends"}}}){
  const auto result=emit(text,exports);assert(!result.success && result.interface_artifact && !result.interface_artifact->success);
 }
 publish(Source);assert(fs::remove(source));
 const auto interface=root/"NativeClassFriends.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 auto imported=[&](const fs::path &input,const fs::path &output){
  auto value=request(input,output);InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
  value.imported_interfaces={dependency};return value;
 };
 for(const char *name:{"a","b"}){
  const auto input=root/(std::string(name)+".mm");Write(input,"extern \"C\" int check_"+std::string(name)+"(){return NativeClassFriends::exercise();}");
  Success(Compile(imported(input,root/(std::string(name)+".o"))));
 }
 unsigned ordinal=0;
 for(const auto &[text,message]:std::vector<std::pair<std::string,std::string>>{
     {"int bad(){NativeClassFriends::Value value(1);return value.value;}","private member"},
     {"int bad(){NativeClassFriends::Selected<int> value(1);return NativeClassFriends::Inspector<long>::selected(value);}","private member"},
     {"int bad(){NativeClassFriends::Aliased<int> value(1);return NativeClassFriends::Inspector<long>::aliased(value);}","private member"},
     {"int bad(){NativeClassFriends::Value value(1);return NativeClassFriends::ConstrainedInspector<double>::read(value);}","constraints not satisfied"}}){
  const auto input=root/("bad-"+std::to_string(ordinal++)+".mm");Write(input,text);const auto result=Compile(imported(input,root/"bad.o"));
  const bool rejected=!result.success && std::any_of(result.diagnostics.begin(),result.diagnostics.end(),[&](const auto &d){return d.message.find(message)!=std::string::npos;});
  if(!rejected)test::ReportFailure(result);assert(rejected);
 }
 std::vector<llvm::StringRef> reader;
 for(const auto &capability:SupportedSemanticInterfaceCapabilities())if(capability!=kCxxFriendSemantic)reader.push_back(capability);
 reader.push_back("objc3.cxx-friends.v2");auto old=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);
 assert(!old);llvm::consumeError(old.takeError());reader.push_back(kCxxFriendSemantic);
 auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 auto missing=*decoded;std::erase(missing.required_semantics,kCxxFriendSemantic);
 auto invalid=EncodeSemanticInterfaceSidecar(missing,reader);assert(!invalid);llvm::consumeError(invalid.takeError());
 auto no_control=*decoded;std::erase(no_control.required_semantics,kCxxConstantEvaluationControlSemantic);
 invalid=EncodeSemanticInterfaceSidecar(no_control,reader);assert(!invalid);llvm::consumeError(invalid.takeError());
 const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
 auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
}
}

int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-friend-classes",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const auto &[standard,ownership]:std::vector<std::pair<const char *,int>>{{"c++20",0},{"c++23",0},{"c++20",1},{"c++23",2}})
  cases.Run(std::string(standard)+"-"+std::to_string(ownership),[&]{Run(root/(std::string(standard)+"-"+std::to_string(ownership)),standard,ownership);});
 return cases.Finish();
}
