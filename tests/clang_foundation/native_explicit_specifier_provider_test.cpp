#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>
using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
template<class T>bool Fails(llvm::Expected<T> value){if(value)return false;llvm::consumeError(value.takeError());return true;}
constexpr const char *Source=R"explicit(
namespace ExplicitPolicies {
inline constexpr bool public_choice=false;
struct Implicit {int value;explicit(false) Implicit(int n):value(n){} explicit(false) operator int()const{return value;}};
struct Locked {int value;explicit(true) Locked(int n):value(n){} explicit(true) operator int()const{return value;}};
struct Named {int value;explicit(public_choice) Named(int n):value(n){} explicit(public_choice) operator int()const{return value;}};
struct Detached {int value;explicit(sizeof(int)==4) Detached(int n);explicit(false) operator int()const;int read()const;static Detached make(int);};
inline Detached::Detached(int n):value(n){}
inline Detached::operator int()const{return value;}
inline int Detached::read()const{return value;}
inline Detached Detached::make(int n){return Detached(n);}
template<bool E>struct Toggle {int value;explicit(E) Toggle(int n):value(n){} explicit(E) operator int()const{return value;}};
template<class T>struct Partial {int value;explicit(sizeof(T)>4) Partial(int n):value(n){} explicit(sizeof(T)>4) operator int()const{return value;}};
template<class T>struct Partial<T*> {int value;explicit(false) Partial(int n):value(n){} explicit(false) operator int()const{return value;}};
template<>struct Partial<void> {int value;explicit(true) Partial(int n):value(n){} explicit(true) operator int()const{return value;}};
template<bool E,int N>struct PartialValue{int value;explicit(E) PartialValue(int n):value(n){} explicit(E) operator int()const{return value;}};
template<bool E>struct PartialValue<E,0>{int value;explicit(E) PartialValue(int n):value(n){} explicit(E) operator int()const{return value;}};
template<bool E,int N>inline constexpr int partial_number=E?1:2;
template<bool E>inline constexpr int partial_number<E,0> =E?3:4;
template<bool E>struct Outer {struct Inner{int value;explicit(E) Inner(int n):value(n){} explicit(E) operator int()const{return value;}};};
struct Flexible {
 int value;
 template<class T>explicit(!__is_same(T,int)) Flexible(T n):value(int(n)){}
 template<class T>explicit(!__is_same(T,int)) operator T()const{return T(value);}
};
inline auto make_plain(int n){struct Local{int value;explicit(false) Local(int n):value(n){} explicit(false) operator int()const{return value;}};return Local(n);}
template<bool E>auto make_local(int n){struct Local{int value;explicit(E) Local(int n):value(n){} explicit(E) operator int()const{return value;}};return Local(n);}
inline auto inline_factory=[](int n){struct Cell{int value;explicit(false) Cell(int v):value(v){} explicit(false) operator int()const{return value;}};return Cell(n);};
inline int outlined_destroyed=0;
struct OutlinedBase{virtual ~OutlinedBase();virtual int read()const;};
inline OutlinedBase::~OutlinedBase(){++outlined_destroyed;}
inline int OutlinedBase::read()const{return 57;}
struct OutlinedDerived:OutlinedBase{int read()const override{return 58;}};
int outlined_read(const OutlinedBase &value){return value.read();}
inline int live=0,destroyed=0;
struct Traced {
 int value;
 explicit(false) Traced(int n):value(n){++live;}
 explicit(false) Traced(const Traced &other):value(other.value){++live;}
 ~Traced(){--live;++destroyed;}
};
int provider(Traced value){return value.value;}
inline int exercise(){
 static_assert(__is_convertible(int,Implicit) && __is_convertible(Implicit,int));
 static_assert(!__is_convertible(int,Locked) && !__is_convertible(Locked,int));
 static_assert(__is_convertible(int,Toggle<false>) && !__is_convertible(int,Toggle<true>));
 static_assert(__is_convertible(Partial<int*>,int) && !__is_convertible(Partial<void>,int));
 Implicit a=40;int av=a;Locked b(41);int bv=static_cast<int>(b);Named c=42;int cv=c;
 Toggle<false> d=43;int dv=d;Toggle<true> e(44);int ev=static_cast<int>(e);
 Partial<int> f=45;int fv=f;Partial<long long> g(46);int gv=static_cast<int>(g);
 Partial<int*> h=47;int hv=h;Partial<void> i(48);int iv=static_cast<int>(i);
 Outer<false>::Inner j=49;int jv=j;Outer<true>::Inner k(50);int kv=static_cast<int>(k);
 PartialValue<false,0> vp=36;int vp_value=vp;PartialValue<true,1> vg(37);int vg_value=static_cast<int>(vg);
 Detached detached(39);int detached_value=detached;
 Flexible flexible=51;int flex=flexible;long converted=static_cast<long>(flexible);
 auto local=make_plain(52);int lv=local;auto plain=make_local<false>(53);int pv=plain;
 auto guarded=make_local<true>(54);int tv=static_cast<int>(guarded);
 auto cell=inline_factory(55);int cell_value=cell;
 int retired=outlined_destroyed;
 {OutlinedDerived value;if(outlined_read(value)!=58)return 3;}
 if(outlined_destroyed!=retired+1)return 4;
 int before=destroyed;
 {Traced owner=56;const int copied=provider(owner);if(live!=1 || copied!=56 || destroyed!=before+1)return 2;}
 return partial_number<false,0>!=4 || partial_number<true,1>!=1 || vp_value!=36 || vg_value!=37 || detached_value!=39 || detached.read()!=39 || Detached::make(38).read()!=38 || av!=40 || bv!=41 || cv!=42 || dv!=43 || ev!=44 || fv!=45 || gv!=46 || hv!=47 || iv!=48 ||
     jv!=49 || kv!=50 || flex!=51 || converted!=51 || lv!=52 || pv!=53 || tv!=54 || cell_value!=55 || live!=0 || destroyed!=before+2;
}
}
)explicit";
void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");const auto source=root/"provider/module.mm";
 auto request=[&](const fs::path &input,const fs::path &output){return Request(input,output,standard,ownership,"-O2",{});};
 auto publish=[&](const std::string &text,bool owned=true){Write(source,text);auto value=request(source,root/"provider.o");
  value.module_name="fixture.ExplicitPolicies";value.package_identity="explicit-predicates-v1";value.module_owned_names=owned;
  if(owned)value.module_source_root=(root/"provider").string();value.interface_exports={"ExplicitPolicies"};value.emit_semantic_interface=true;
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;};
 const auto artifact=publish(Source),replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 std::string renamed=Source;
 for(const auto &[before,after]:std::vector<std::pair<std::string,std::string>>{{"bool E","bool Enabled"},{"explicit(E)","explicit(Enabled)"},{"PartialValue<E,0>","PartialValue<Enabled,0>"},{"partial_number<E,0>","partial_number<Enabled,0>"},{"=E?","=Enabled?"}})
  for(size_t at=0;(at=renamed.find(before,at))!=std::string::npos;at+=after.size())renamed.replace(at,before.size(),after);
 const auto aliases=publish(renamed);
 if(aliases.identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint){
  auto left=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,SupportedSemanticInterfaceCapabilities());auto right=DecodeSemanticInterfaceSidecar(aliases.sidecar_json,aliases.source_bytes,SupportedSemanticInterfaceCapabilities());assert(left && right);
  for(const auto &row:left->declarations){auto other=std::find_if(right->declarations.begin(),right->declarations.end(),[&](const auto &d){return d.stable_id==row.stable_id;});
   if(other==right->declarations.end())llvm::errs()<<"missing renamed row "<<row.qualified_name<<'\n';
   else if(other->canonical_type_identity!=row.canonical_type_identity)llvm::errs()<<"renamed row "<<row.qualified_name<<"\n"<<row.canonical_type_identity<<"\n"<<other->canonical_type_identity<<'\n';}
 }
 assert(aliases.identity.semantic_fingerprint==artifact.identity.semantic_fingerprint);
 const auto &reader=SupportedSemanticInterfaceCapabilities();auto metadata=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(metadata);
 assert(std::find(metadata->required_semantics.begin(),metadata->required_semantics.end(),kCxxExplicitSpecifierSemantic)!=metadata->required_semantics.end());
 assert(std::find(metadata->required_semantics.begin(),metadata->required_semantics.end(),kCxxOutOfLineDefinitionSemantic)!=metadata->required_semantics.end());
 auto missing_definitions=*metadata;std::erase(missing_definitions.required_semantics,kCxxOutOfLineDefinitionSemantic);assert(Fails(EncodeSemanticInterfaceSidecar(missing_definitions,reader)));
 std::vector<llvm::StringRef> old_definitions(reader.begin(),reader.end());std::erase(old_definitions,kCxxOutOfLineDefinitionSemantic);assert(Fails(DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,old_definitions)));
 for(const char *kind:{"Namespace","Enum","Var"}){auto forged=*metadata;auto row=std::find_if(forged.declarations.begin(),forged.declarations.end(),HasCxxOutOfLineDefinitionIdentity);assert(row!=forged.declarations.end());row->kind=kind;assert(Fails(EncodeSemanticInterfaceSidecar(forged,reader)));}
 for(const char *name:{"ExplicitPolicies::Implicit","ExplicitPolicies::Locked","ExplicitPolicies::Named","ExplicitPolicies::Detached","ExplicitPolicies::Toggle","ExplicitPolicies::Partial","ExplicitPolicies::PartialValue","ExplicitPolicies::Outer","ExplicitPolicies::Flexible","ExplicitPolicies::make_plain","ExplicitPolicies::make_local","ExplicitPolicies::inline_factory","ExplicitPolicies::Traced"}){
  auto row=std::find_if(metadata->declarations.begin(),metadata->declarations.end(),[&](const auto &d){return d.qualified_name==name;});
  assert(row!=metadata->declarations.end() && HasCxxExplicitSpecifierIdentity(*row));}
 std::vector<llvm::StringRef> old(reader.begin(),reader.end());std::erase(old,kCxxExplicitSpecifierSemantic);assert(Fails(DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,old)));
 auto missing=*metadata;std::erase(missing.required_semantics,kCxxExplicitSpecifierSemantic);assert(Fails(EncodeSemanticInterfaceSidecar(missing,reader)));
 for(const char *kind:{"Namespace","Enum","EnumConstant"}){auto forged=*metadata;auto row=std::find_if(forged.declarations.begin(),forged.declarations.end(),HasCxxExplicitSpecifierIdentity);assert(row!=forged.declarations.end());row->kind=kind;assert(Fails(EncodeSemanticInterfaceSidecar(forged,reader)));}
 const auto excluded=root/"excluded.mm";
 Write(excluded,"namespace HiddenPredicates{inline constexpr bool private_choice=true;}namespace ExplicitPolicies{struct Hidden{explicit(HiddenPredicates::private_choice)Hidden(int){}};}");
 auto hidden=request(excluded,root/"excluded.o");hidden.module_name="fixture.ExcludedPredicates";hidden.package_identity="excluded-predicates-v1";hidden.emit_semantic_interface=true;hidden.interface_exports={"ExplicitPolicies"};
 auto rejected=Compile(hidden);assert(!rejected.success && rejected.interface_artifact);assert(std::any_of(rejected.interface_artifact->diagnostics.begin(),rejected.interface_artifact->diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
 const auto unpublished=root/"unpublished.mm";
 Write(unpublished,"namespace ExplicitPolicies{int hidden_implementation(int value){struct Hidden{int value;explicit(false)Hidden(int n):value(n){}};return Hidden(value).value;}}\n");
 auto unpublished_request=request(unpublished,root/"unpublished.o");unpublished_request.module_name="fixture.UnpublishedPredicates";unpublished_request.package_identity="unpublished-predicates-v1";unpublished_request.emit_semantic_interface=true;
 auto unpublished_result=Compile(unpublished_request);Success(unpublished_result);assert(unpublished_result.interface_artifact && unpublished_result.interface_artifact->success);
 auto unpublished_metadata=DecodeSemanticInterfaceSidecar(unpublished_result.interface_artifact->sidecar_json,unpublished_result.interface_artifact->source_bytes,reader);assert(unpublished_metadata);
 assert(std::find(unpublished_metadata->required_semantics.begin(),unpublished_metadata->required_semantics.end(),kCxxExplicitSpecifierSemantic)==unpublished_metadata->required_semantics.end());
 publish(Source);assert(fs::remove(source));const auto interface=root/"ExplicitPolicies.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
 auto imported=[&](const fs::path &input,const fs::path &output){auto value=request(input,output);value.imported_interfaces={dependency};return value;};
 for(const char *name:{"a","b"}){const auto input=root/(std::string(name)+".mm");Write(input,"extern \"C\" int check_"+std::string(name)+"(){return ExplicitPolicies::exercise();}\n");Success(Compile(imported(input,root/(std::string(name)+".o"))));}
 unsigned ordinal=0;
 for(const char *text:{"ExplicitPolicies::PartialValue<true,0> rejected=42;","ExplicitPolicies::Detached rejected=42;","ExplicitPolicies::Locked rejected=42;","int rejected=ExplicitPolicies::Locked(42);","ExplicitPolicies::Toggle<true> rejected=42;","ExplicitPolicies::Partial<long long> rejected=42;","ExplicitPolicies::Partial<void> rejected=42;","ExplicitPolicies::Outer<true>::Inner rejected=42;","ExplicitPolicies::Flexible rejected=42L;","long rejected=ExplicitPolicies::Flexible(42);","int rejected=ExplicitPolicies::make_local<true>(42);"}){
  const auto input=root/("bad-"+std::to_string(ordinal++)+".mm");Write(input,text);auto result=Compile(imported(input,root/"bad.o"));
  if(result.success)llvm::errs()<<"unexpected explicit conversion: "<<text<<'\n';assert(!result.success);
  assert(std::any_of(result.diagnostics.begin(),result.diagnostics.end(),[](const auto &d){return d.message.find("no viable conversion")!=std::string::npos;}));}
 auto changed=artifact.source_bytes;const std::string predicate="sizeof(T) > 4";const auto at=changed.find(predicate);assert(at!=std::string::npos);changed.replace(at,predicate.size(),"sizeof(T) > 8");
 const auto mutation_source=root/"canonical-mutation.mm";Write(mutation_source,changed);auto canonical_request=request(mutation_source,root/"canonical-mutation.o");canonical_request.emit=EmitKind::Syntax;canonical_request.emit_semantic_interface=true;canonical_request.module_name="fixture.ExplicitPolicies";canonical_request.package_identity="explicit-predicates-v1";canonical_request.interface_exports={"ExplicitPolicies"};
 auto canonical=Compile(canonical_request);Success(canonical);assert(canonical.interface_artifact && canonical.interface_artifact->success);changed=canonical.interface_artifact->source_bytes;
 assert(canonical.interface_artifact->identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint);
 auto mutation=*metadata;mutation.canonical_source_sha256=ComputeInterfaceSourceSha256(changed);auto encoded=EncodeSemanticInterfaceSidecar(mutation,reader);assert(encoded);
 const auto altered=root/"Altered.objc3i";Write(altered,changed);Write(altered.string()+".json",*encoded);
 const auto mutant=root/"mutant.mm";Write(mutant,"int mutation_control(){return 42;}\n");auto bad=imported(mutant,root/"mutant.o");bad.imported_interfaces[0].source_path=altered.string();auto invalid=Compile(bad);assert(!invalid.success);
 assert(std::any_of(invalid.interface_import_diagnostics.begin(),invalid.interface_import_diagnostics.end(),[](const auto &d){return d.code=="O3IFC139";}));
 const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}\n");auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
 assert(!fs::exists(source));assert(fs::remove(root/"Altered.objc3i"));assert(fs::remove(root/"Altered.objc3i.json"));
 if(ownership){
  auto directed=[&](const fs::path &input,const fs::path &output){auto value=request(input,output);value.source_directed_imports=true;value.interface_search_paths={root.string()};
   InterfaceModuleRequest module;module.canonical_name=artifact.identity.canonical_name;module.locked_identity=artifact.identity;module.initially_visible=false;value.module_imports={module};return value;};
  for(const char *name:{"a","b"}){const auto input=root/(std::string("directed-")+name+".mm");Write(input,"@import fixture.ExplicitPolicies;\nextern \"C\" int check_"+std::string(name)+"(){return ExplicitPolicies::exercise();}\n");Success(Compile(directed(input,root/(std::string(name)+".o"))));}
  LinkAndExecute(root,entry.output_path,true,{});
 }
}
}
int main(){test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-explicit-specifiers",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const char *standard:{"c++20","c++23"})for(int ownership:{0,1,2}){const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}return cases.Finish();}
