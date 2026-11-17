#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>
using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
template<class T>bool Fails(llvm::Expected<T> value){if(value)return false;llvm::consumeError(value.takeError());return true;}
constexpr const char *Source=R"guides(
namespace NativeGuides {
template<class T>struct Scalar{
 T value;
 template<class U>static T convert(U n){if constexpr(__is_pointer(U))return T(*n);else return T(n);}
 template<class U>Scalar(U n,int=0):value(convert(n)){}
};
Scalar(int)->Scalar<long>;
Scalar(short)->Scalar<long>;
Scalar(unsigned,int=0)->Scalar<unsigned>;
explicit Scalar(char)->Scalar<int>;
template<class T>explicit(sizeof(T)>4)Scalar(T)->Scalar<T>;
template<class T>requires(sizeof(T)>1)Scalar(T*)->Scalar<T>;
template<class T>struct Constrained{T value;template<class U>Constrained(U *n):value(*n){}};
template<class T>using Stored=Scalar<T>;
template<class T>concept Wide=sizeof(T)>1;
template<class T>requires(Wide<T>)Constrained(T*)->Constrained<T>;
template<bool B>struct Tag{};
template<int N>struct Choice{int value;template<bool B>Choice(Tag<B>,int n):value(n+N){}};
template<bool B>explicit(B)Choice(Tag<B>,int)->Choice<B?1:0>;
template<int N>struct Pack{int value;template<class... Ts>Pack(Ts...):value(N){}};
template<class... Ts>explicit(sizeof...(Ts)>1)Pack(Ts...)->Pack<sizeof...(Ts)>;
struct Envelope {
 template<class T>struct Cell{T value;template<class U>Cell(U n):value(T(n)){}};
 Cell(int)->Cell<long>;
 template<class T>explicit(sizeof(T)>4)Cell(T)->Cell<T>;
};
template<bool E>struct GuidedFamily {
 template<class T>struct Cell{T value;template<class U>Cell(U n):value(T(n)){}};
 explicit(E)Cell(int)->Cell<long>;
 template<class T>requires(sizeof(T)>1)Cell(T)->Cell<T>;
};
template<class... Ts>struct PackFamily {
 template<class T>struct Cell{T value;template<class U>Cell(U n):value(T(n)){}};
 template<class T>explicit(sizeof...(Ts)>1)Cell(T)->Cell<T>;
};
inline int live=0,destroyed=0;
struct Traced{int value;Traced(int n):value(n){++live;}Traced(const Traced &n):value(n.value){++live;}~Traced(){--live;++destroyed;}};
template<class T>struct Owner{T value;template<class U>Owner(U n):value(n){}};
Owner(int)->Owner<Traced>;
int provider(Scalar<long> value){return int(value.value);}
int provider_owner(Owner<Traced> value){return value.value.value;}
inline int exercise(){
 Scalar a=42;Scalar b(43.0f);Scalar c(44LL);Scalar d('x');Scalar e(45u,0);
 int n=46;Scalar p(&n);Constrained q(&n);Stored<int> stored(50);
 static_assert(__is_same(decltype(a),Scalar<long>));
 static_assert(__is_same(decltype(b),Scalar<float>));
 static_assert(__is_same(decltype(c),Scalar<long long>));
 static_assert(__is_same(decltype(d),Scalar<int>));
 static_assert(__is_same(decltype(e),Scalar<unsigned>));
 static_assert(__is_same(decltype(p),Scalar<int>) && __is_same(decltype(q),Constrained<int>) && Wide<int> && !Wide<char>);
 Choice low(Tag<false>{},47);Choice high(Tag<true>{},47);
 Pack one(1);Pack pair(1,2);Pack zero;
 static_assert(__is_same(decltype(low),Choice<0>) && __is_same(decltype(high),Choice<1>));
 static_assert(__is_same(decltype(one),Pack<1>) && __is_same(decltype(pair),Pack<2>) && __is_same(decltype(zero),Pack<0>));
 Envelope::Cell nested=51;Envelope::Cell nested_explicit(52LL);
 GuidedFamily<false>::Cell loose=53;GuidedFamily<true>::Cell locked(54);
 PackFamily<int>::Cell short_pack=55;PackFamily<int,long>::Cell long_pack(56);
 static_assert(__is_same(decltype(nested),Envelope::Cell<long>) && __is_same(decltype(nested_explicit),Envelope::Cell<long long>));
 static_assert(__is_same(decltype(loose),GuidedFamily<false>::Cell<long>) && __is_same(decltype(locked),GuidedFamily<true>::Cell<long>));
 int before=destroyed;
 {Owner owner(49);static_assert(__is_same(decltype(owner),Owner<Traced>));const int result=provider_owner(owner);if(result!=49 || live!=1 || destroyed!=before+1)return 2;}
 return nested.value!=51 || nested_explicit.value!=52 || loose.value!=53 || locked.value!=54 || short_pack.value!=55 || long_pack.value!=56 || provider(a)!=42 || b.value!=43 || c.value!=44 || d.value!='x' || e.value!=45 || p.value!=46 || q.value!=46 || stored.value!=50 ||
     low.value!=47 || high.value!=48 || one.value!=1 || pair.value!=2 || zero.value!=0 || live!=0 || destroyed!=before+2;
}
}
)guides";
void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");const auto source=root/"provider/module.mm";
 auto request=[&](const fs::path &input,const fs::path &output){return Request(input,output,standard,ownership,"-O2",{});};
 auto publish=[&](const std::string &text,bool owned=true){Write(source,text);auto value=request(source,root/"provider.o");
  value.module_name="fixture.NativeGuides";value.package_identity="native-guides-v1";value.module_owned_names=owned;
  if(owned)value.module_source_root=(root/"provider").string();value.interface_exports={"NativeGuides"};value.emit_semantic_interface=true;
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;};
 const auto artifact=publish(Source),replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 std::string renamed=Source;
 for(const auto &[before,after]:std::vector<std::pair<std::string,std::string>>{{"bool E","bool Enabled"},{"explicit(E)","explicit(Enabled)"},{"Cell(T)","Cell(Element)"},{"Cell<T>","Cell<Element>"},{"int N","int Count"},{"n+N","n+Count"},{"value(N)","value(Count)"},{"class... Ts","class... Arguments"},{"Ts...","Arguments..."},{"sizeof...(Ts)","sizeof...(Arguments)"},{"class T>","class Element>"},{"T value","Element value"},{"static T convert","static Element convert"},{"return T(","return Element("},{"value(T(","value(Element("},{"sizeof(T)","sizeof(Element)"},{"Scalar(T)","Scalar(Element)"},{"Scalar(T*)","Scalar(Element*)"},{"Scalar<T>","Scalar<Element>"},{"Constrained(T*)","Constrained(Element*)"},{"Constrained<T>","Constrained<Element>"},{"Wide<T>","Wide<Element>"},{"bool B","bool Enabled"},{"explicit(B)","explicit(Enabled)"},{"Tag<B>","Tag<Enabled>"},{"Choice<B?","Choice<Enabled?"}})
  for(size_t at=0;(at=renamed.find(before,at))!=std::string::npos;at+=after.size())renamed.replace(at,before.size(),after);
 const auto aliases=publish(renamed);
 if(aliases.identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint){
  auto left=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,SupportedSemanticInterfaceCapabilities());auto right=DecodeSemanticInterfaceSidecar(aliases.sidecar_json,aliases.source_bytes,SupportedSemanticInterfaceCapabilities());assert(left && right);
  for(const auto &row:left->declarations){auto other=std::find_if(right->declarations.begin(),right->declarations.end(),[&](const auto &d){return d.stable_id==row.stable_id;});
   if(other==right->declarations.end())llvm::errs()<<"missing renamed row "<<row.qualified_name<<'\n';
   else if(other->canonical_type_identity!=row.canonical_type_identity)llvm::errs()<<"renamed row "<<row.qualified_name<<"\n"<<row.canonical_type_identity<<"\n"<<other->canonical_type_identity<<'\n';}
 }
 assert(aliases.identity.semantic_fingerprint==artifact.identity.semantic_fingerprint);
 const auto reader=SupportedSemanticInterfaceCapabilities();auto metadata=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(metadata);
 assert(std::count_if(metadata->declarations.begin(),metadata->declarations.end(),HasCxxDeductionGuideIdentity)==15);
 for(const auto &row:metadata->declarations)if(HasCxxDeductionGuideIdentity(row)){assert(row.native_symbol_name.empty());assert(row.export_root.starts_with("NativeGuides::"));assert(row.export_root.find("deduction guide")==std::string::npos);}
 std::vector<llvm::StringRef> old(reader.begin(),reader.end());std::erase(old,kCxxDeductionGuideSemantic);assert(Fails(DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,old)));
 auto missing=*metadata;std::erase(missing.required_semantics,kCxxDeductionGuideSemantic);assert(Fails(EncodeSemanticInterfaceSidecar(missing,reader)));
 for(const char *kind:{"Namespace","Enum","Function","ClassTemplate"}){auto forged=*metadata;auto row=std::find_if(forged.declarations.begin(),forged.declarations.end(),HasCxxDeductionGuideIdentity);assert(row!=forged.declarations.end());row->kind=kind;assert(Fails(EncodeSemanticInterfaceSidecar(forged,reader)));}
 auto named=*metadata;auto named_row=std::find_if(named.declarations.begin(),named.declarations.end(),HasCxxDeductionGuideIdentity);assert(named_row!=named.declarations.end());named_row->native_symbol_name="foreign_guide_entry";assert(Fails(EncodeSemanticInterfaceSidecar(named,reader)));
 for(const auto &[before,after]:std::vector<std::pair<std::string,std::string>>{{"explicit Scalar(char)","Scalar(char)"},{"sizeof(T)>4","sizeof(T)>8"},{"sizeof(T)>1","sizeof(T)>2"},{"Scalar(short)->Scalar<long>","Scalar(short)->Scalar<int>"},{"Cell(int)->Cell<long>","Cell(int)->Cell<int>"},{"explicit(E)Cell(int)","Cell(int)"},{"template<class T>explicit(sizeof...(Ts)>1)Cell(T)","template<class T>explicit(sizeof...(Ts)>2)Cell(T)"}}){
  std::string changed=Source;const auto at=changed.find(before);assert(at!=std::string::npos);changed.replace(at,before.size(),after);
  if(before=="Cell(int)->Cell<long>") {
    const std::string assertion="decltype(nested),Envelope::Cell<long>";
    const auto expected=changed.find(assertion);assert(expected!=std::string::npos);
    changed.replace(expected,assertion.size(),"decltype(nested),Envelope::Cell<int>");
  }
  const auto canonical=publish(changed);assert(canonical.identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint);
  auto mutation=*metadata;mutation.canonical_source_sha256=ComputeInterfaceSourceSha256(canonical.source_bytes);auto encoded=EncodeSemanticInterfaceSidecar(mutation,reader);assert(encoded);
  const auto altered=root/"Altered.objc3i";Write(altered,canonical.source_bytes);Write(altered.string()+".json",*encoded);
  const auto control=root/"mutation.mm";Write(control,"int control(){return 0;}\n");auto bad=request(control,root/"mutation.o");InterfaceImportInput dependency;dependency.source_path=altered.string();dependency.locked_identity=artifact.identity;bad.imported_interfaces={dependency};auto rejected=Compile(bad);assert(!rejected.success);
  assert(std::any_of(rejected.interface_import_diagnostics.begin(),rejected.interface_import_diagnostics.end(),[](const auto &d){return d.code=="O3IFC139";}));
  assert(fs::remove(altered));assert(fs::remove(altered.string()+".json"));
 }
 // Selecting only a class must retain its complete authored deduction set.
 Write(source,"namespace NativeGuides{template<class T>struct Leaf{T value;template<class U>Leaf(U n):value(T(n)){}};Leaf(int)->Leaf<long>;}");
 auto leaf_request=request(source,root/"leaf.o");leaf_request.emit_semantic_interface=true;leaf_request.module_name="fixture.LeafGuides";leaf_request.package_identity="leaf-guides-v1";leaf_request.module_owned_names=true;leaf_request.module_source_root=(root/"provider").string();leaf_request.interface_exports={"NativeGuides::Leaf"};
 auto leaf=Compile(leaf_request);Success(leaf);assert(leaf.interface_artifact && leaf.interface_artifact->success);assert(leaf.interface_artifact->source_bytes.find("Leaf(int)")!=std::string::npos);
 const auto leaf_interface=root/"Leaf.objc3i";Write(leaf_interface,leaf.interface_artifact->source_bytes);Write(leaf_interface.string()+".json",leaf.interface_artifact->sidecar_json);assert(fs::remove(source));
 const auto leaf_consumer=root/"leaf-consumer.mm";Write(leaf_consumer,"NativeGuides::Leaf value(42);static_assert(__is_same(decltype(value),NativeGuides::Leaf<long>));");auto leaf_use=request(leaf_consumer,root/"leaf-consumer.o");InterfaceImportInput leaf_dependency;leaf_dependency.source_path=leaf_interface.string();leaf_dependency.locked_identity=leaf.interface_artifact->identity;leaf_use.imported_interfaces={leaf_dependency};Success(Compile(leaf_use));
 assert(fs::remove(leaf_interface));assert(fs::remove(leaf_interface.string()+".json"));
 // Guide predicates cannot hide excluded dependencies through constant folding.
 Write(source,"namespace Hidden{inline constexpr bool choice=true;}namespace NativeGuides{template<class T>struct HiddenOwner{T value;template<class U>HiddenOwner(U n):value(T(n)){}};explicit(Hidden::choice)HiddenOwner(int)->HiddenOwner<long>;}");
 auto hidden_request=request(source,root/"hidden.o");hidden_request.emit_semantic_interface=true;hidden_request.module_name="fixture.HiddenGuides";hidden_request.package_identity="hidden-guides-v1";hidden_request.interface_exports={"NativeGuides"};
 auto hidden=Compile(hidden_request);assert(!hidden.success && hidden.interface_artifact);assert(std::any_of(hidden.interface_artifact->diagnostics.begin(),hidden.interface_artifact->diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
 Write(source,"namespace NativeGuides{template<class T>struct Leaf{T value;template<class U>Leaf(U n):value(T(n)){}};template<class T>__attribute__((annotate(\"objc3.spi:Testing\")))Leaf(T)->Leaf<T>;}");
 auto private_guide=Compile(hidden_request);assert(!private_guide.success && private_guide.interface_artifact);assert(std::any_of(private_guide.interface_artifact->diagnostics.begin(),private_guide.interface_artifact->diagnostics.end(),[](const auto &d){return d.code=="O3IFC157";}));
 for(const char *invalid:{
     "namespace NativeGuides{template<class T>struct __attribute__((annotate(\"objc3.spi:bad group\"))) Leaf{};Leaf(int)->Leaf<long>;}",
     "namespace NativeGuides{template<class T>struct __attribute__((annotate(\"objc3.spi:Testing\"),annotate(\"objc3.spi:Other\"))) Leaf{};Leaf(int)->Leaf<long>;}",
     "namespace NativeGuides{template<class T>struct __attribute__((annotate(\"objc3.spi:Testing\",1))) Leaf{};Leaf(int)->Leaf<long>;}",
     "namespace __attribute__((annotate(\"objc3.spi:Parent\"))) NativeGuides{template<class T>struct __attribute__((annotate(\"objc3.spi:Testing\"))) Leaf{};Leaf(int)->Leaf<long>;}",
     "namespace NativeGuides{struct Outer{template<class T>struct __attribute__((annotate(\"objc3.spi:Testing\"))) Leaf{};};}"}){
  Write(source,invalid);auto rejected=Compile(hidden_request);assert(!rejected.success && rejected.interface_artifact && !rejected.interface_artifact->success);
 }
 const auto implicit_only=publish("namespace NativeGuides{template<class T>struct Plain{T value;Plain(T n):value(n){}};inline int implicit_only(){Plain value(42);return value.value;}}");
 auto implicit_metadata=DecodeSemanticInterfaceSidecar(implicit_only.sidecar_json,implicit_only.source_bytes,reader);assert(implicit_metadata);assert(std::find(implicit_metadata->required_semantics.begin(),implicit_metadata->required_semantics.end(),kCxxDeductionGuideSemantic)==implicit_metadata->required_semantics.end());
 // Class-template SPI also owns each guide, including leaf-only exports.
 Write(source,"namespace NativeGuides{template<class T>struct __attribute__((annotate(\"objc3.spi:Testing\"))) Leaf{T value;template<class U>Leaf(U n):value(T(n)){}};Leaf(int)->Leaf<long>;}");
 leaf_request.module_name="fixture.SPIGuides";auto spi=Compile(leaf_request);Success(spi);assert(spi.interface_artifact && spi.interface_artifact->success);
 auto spi_metadata=DecodeSemanticInterfaceSidecar(spi.interface_artifact->sidecar_json,spi.interface_artifact->source_bytes,reader);assert(spi_metadata);
 for(const auto &row:spi_metadata->declarations)if(row.kind=="ClassTemplate" || HasCxxDeductionGuideIdentity(row))assert(row.spi_group=="Testing");
 Write(leaf_interface,spi.interface_artifact->source_bytes);Write(leaf_interface.string()+".json",spi.interface_artifact->sidecar_json);assert(fs::remove(source));
 leaf_use.imported_interfaces[0].locked_identity=spi.interface_artifact->identity;assert(!Compile(leaf_use).success);
 leaf_use.imported_interfaces[0].spi_groups={"Testing"};Success(Compile(leaf_use));assert(fs::remove(leaf_interface));assert(fs::remove(leaf_interface.string()+".json"));
 Write(source,"namespace NativeGuides{struct __attribute__((annotate(\"objc3.spi:Testing\"))) HiddenOuter{template<class T>struct Cell{T value;template<class U>Cell(U n):value(T(n)){}};Cell(int)->Cell<long>;};}");
 leaf_request.module_name="fixture.MemberSPIGuides";leaf_request.interface_exports={"NativeGuides::HiddenOuter"};auto member_spi=Compile(leaf_request);Success(member_spi);assert(member_spi.interface_artifact && member_spi.interface_artifact->success);
 auto member_metadata=DecodeSemanticInterfaceSidecar(member_spi.interface_artifact->sidecar_json,member_spi.interface_artifact->source_bytes,reader);assert(member_metadata);
 assert(std::count_if(member_metadata->declarations.begin(),member_metadata->declarations.end(),HasCxxDeductionGuideIdentity)==1);
 for(const auto &row:member_metadata->declarations)if(HasCxxDeductionGuideIdentity(row)){assert(row.spi_group=="Testing");assert(row.export_root=="NativeGuides::HiddenOuter");}
 Write(leaf_interface,member_spi.interface_artifact->source_bytes);Write(leaf_interface.string()+".json",member_spi.interface_artifact->sidecar_json);assert(fs::remove(source));
 Write(leaf_consumer,"NativeGuides::HiddenOuter::Cell value(42);static_assert(__is_same(decltype(value),NativeGuides::HiddenOuter::Cell<long>));");
 leaf_use.imported_interfaces[0].locked_identity=member_spi.interface_artifact->identity;leaf_use.imported_interfaces[0].spi_groups.clear();assert(!Compile(leaf_use).success);leaf_use.imported_interfaces[0].spi_groups={"Testing"};Success(Compile(leaf_use));
 if(ownership){auto directed_member=request(leaf_consumer,root/"member-directed.o");Write(leaf_consumer,"@import fixture.MemberSPIGuides;\nNativeGuides::HiddenOuter::Cell value(42);static_assert(__is_same(decltype(value),NativeGuides::HiddenOuter::Cell<long>));");directed_member.source_directed_imports=true;directed_member.interface_search_paths={root.string()};InterfaceModuleRequest module;module.canonical_name=member_spi.interface_artifact->identity.canonical_name;module.locked_identity=member_spi.interface_artifact->identity;module.initially_visible=false;module.spi_groups={"Testing"};directed_member.module_imports={module};Success(Compile(directed_member));}
 assert(fs::remove(leaf_interface));assert(fs::remove(leaf_interface.string()+".json"));
 publish(Source);assert(fs::remove(source));const auto interface=root/"NativeGuides.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
 auto imported=[&](const fs::path &input,const fs::path &output){auto value=request(input,output);value.imported_interfaces={dependency};return value;};
 for(const char *name:{"a","b"}){const auto input=root/(std::string(name)+".mm");Write(input,"extern \"C\" int check_"+std::string(name)+"(){return NativeGuides::exercise();}\n");Success(Compile(imported(input,root/(std::string(name)+".o"))));}
 unsigned ordinal=0;
 for(const char *text:{"NativeGuides::Envelope::Cell rejected={42LL};","NativeGuides::GuidedFamily<true>::Cell rejected={42};","NativeGuides::PackFamily<int,long>::Cell rejected={42};","NativeGuides::Scalar rejected={'x'};","NativeGuides::Scalar rejected={42LL};","NativeGuides::Choice rejected={NativeGuides::Tag<true>{},42};","NativeGuides::Pack rejected={1,2};","char input='x';NativeGuides::Constrained rejected(&input);"}){
  const auto input=root/("bad-"+std::to_string(ordinal++)+".mm");Write(input,text);auto result=Compile(imported(input,root/"bad.o"));if(result.success)llvm::errs()<<"unexpected guide acceptance: "<<text<<'\n';assert(!result.success);assert(!result.diagnostics.empty());}
 const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}\n");auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
 if(ownership){
  auto directed=[&](const fs::path &input,const fs::path &output){auto value=request(input,output);value.source_directed_imports=true;value.interface_search_paths={root.string()};InterfaceModuleRequest module;module.canonical_name=artifact.identity.canonical_name;module.locked_identity=artifact.identity;module.initially_visible=false;value.module_imports={module};return value;};
  for(const char *name:{"a","b"}){const auto input=root/(std::string("directed-")+name+".mm");Write(input,"@import fixture.NativeGuides;\nextern \"C\" int check_"+std::string(name)+"(){return NativeGuides::exercise();}\n");Success(Compile(directed(input,root/(std::string(name)+".o"))));}LinkAndExecute(root,entry.output_path,true,{});
 }
}
}
int main(){test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-deduction-guides",temporary));const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});for(const char *standard:{"c++20","c++23"})for(int ownership:{0,1,2}){const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}return cases.Finish();}
