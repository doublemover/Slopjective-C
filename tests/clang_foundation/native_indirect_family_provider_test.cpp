#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include "llvm/Support/JSON.h"
#include <algorithm>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
constexpr const char *Source=R"family(
namespace NativeIndirect {
struct Object {int references;char payload[8];};
typedef Object *Ref __attribute__((objc_retainable_family(Execution)));
typedef borrowed const char *Loan;
extern int live,destroyed,uses;
extern bool throwing;
Ref create() __attribute__((cf_returns_retained));
Ref hold(Ref) __attribute__((objc_family_retain(Execution)));
Ref hold_other(Ref) __attribute__((objc_family_retain(Execution)));
Ref hold_nothrow(Ref) noexcept __attribute__((objc_family_retain(Execution)));
void drop(Ref) __attribute__((objc_family_release(Execution)));
void drop_other(Ref) __attribute__((objc_family_release(Execution)));
Loan bytes(Ref owner) __attribute__((objc_returns_borrowed(owner_index=0)));
void use(const char *value __attribute__((noescape)));
using Hold=Ref(*)(Ref);using Drop=void(*)(Ref);
inline constexpr Hold global_hold=hold;
inline constexpr Drop global_drop=drop;
struct Table {Hold hold;Drop drop;};
inline constexpr Table table{hold,drop};
inline Hold select(int which){return which?hold:hold_other;}
inline Hold nested(int which){return select(which);}
inline int balanced(Ref owner,int which){
 Loan value=bytes(owner);Hold const acquire=which?hold:hold_other;Drop const release=which?drop:drop_other;
 Ref alias=acquire(owner);release(owner);use(value);
 if(alias!=owner || owner->references!=1)return 1;
 global_hold(owner);global_drop(owner);use(value);
 table.hold(owner);table.drop(owner);use(value);
 nested(which)(owner);global_drop(owner);use(value);
 return owner->references!=1;
}
inline int guarded(Ref owner,int which,bool fail){
 Loan value=bytes(owner);hold_nothrow(owner);throwing=fail;
 try{nested(which)(owner);}
 catch(int error){throwing=false;if(error!=91)return 1;global_drop(owner);use(value);return owner->references!=1;}
 throwing=false;global_drop(owner);global_drop(owner);use(value);return owner->references!=1;
}
inline int exercise(){
 if(live)return 1;
 const int before_uses=uses,before_destroyed=destroyed;
 Ref owner=create();
 for(int which=0;which!=2;++which)
  if(balanced(owner,which) || guarded(owner,which,false) || guarded(owner,which,true))return 2;
 drop(owner);return live || destroyed!=before_destroyed+1 || uses!=before_uses+12;
}
int live=0,destroyed=0,uses=0;bool throwing=false;
Ref create(){++live;return new Object{1,{'a','l','i','v','e',0}};}
Ref hold(Ref value){if(throwing)throw 91;++value->references;return value;}
Ref hold_other(Ref value){if(throwing)throw 91;++value->references;return value;}
Ref hold_nothrow(Ref value)noexcept{++value->references;return value;}
void drop(Ref value){if(!--value->references){--live;++destroyed;delete value;}}
void drop_other(Ref value){drop(value);}
Loan bytes(Ref owner) __attribute__((objc_returns_borrowed(owner_index=0))){return owner->payload;}
void use(const char *value __attribute__((noescape))){if(!live || value[0]!='a')throw 92;++uses;}
}
)family";

void Run(const fs::path &root,const char *standard,int ownership) {
 fs::create_directories(root/"provider");
 auto request=[&](const fs::path &input,const fs::path &output) {
  auto value=Request(input,output,standard,ownership,"-O2",{
      "-fexceptions","-fcxx-exceptions","-fobjc-exceptions"});
  value.profile=SemanticProfile::StrictSystem;return value;
 };
 const auto source=root/"provider/module.mm";Write(source,Source);
 auto provider=request(source,root/"provider.o");
 provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
 provider.module_name="fixture.NativeIndirect";provider.package_identity="indirect-family-v1";
 provider.interface_exports={"NativeIndirect"};provider.emit_semantic_interface=true;
 const auto built=Compile(provider);Success(built);assert(built.interface_artifact && built.interface_artifact->success);
 const auto artifact=*built.interface_artifact;
 assert(artifact.sidecar_json.find(kManualFamilyCallProofSemantic)!=std::string::npos);
 const auto interface=root/"NativeIndirect.objc3i";
 Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);assert(fs::remove(source));
 auto imported_request=[&](const fs::path &input,const fs::path &output) {
  auto consumer=request(input,output);InterfaceImportInput imported;
  imported.source_path=interface.string();imported.locked_identity=artifact.identity;
  consumer.imported_interfaces={imported};return consumer;
 };
 for(const char *name:{"a","b"}) {
  const auto input=root/(std::string(name)+".mm");
  Write(input,"extern \"C\" int check_"+std::string(name)+"(){return NativeIndirect::exercise();}");
  Success(Compile(imported_request(input,root/(std::string(name)+".o"))));
 }
 unsigned ordinal=0;
 for(const auto &body:std::vector<std::string>{
     "NativeIndirect::Loan value=NativeIndirect::bytes(owner);NativeIndirect::global_drop(owner);NativeIndirect::use(value);",
     "NativeIndirect::Loan value=NativeIndirect::bytes(owner);NativeIndirect::Drop const drop=which?NativeIndirect::drop:opaque;drop(owner);NativeIndirect::use(value);",
     "NativeIndirect::Loan value=NativeIndirect::bytes(owner);try{NativeIndirect::global_hold(owner);}catch(...){NativeIndirect::drop(owner);NativeIndirect::use(value);}"}) {
  const auto input=root/("bad-"+std::to_string(ordinal++)+".mm");
  Write(input,"void bad(NativeIndirect::Ref owner,int which,NativeIndirect::Drop opaque){"+body+"}");
  const auto rejected=Compile(imported_request(input,root/"bad.o"));
  const bool expected=!rejected.success && std::any_of(rejected.diagnostics.begin(),rejected.diagnostics.end(),
      [](const auto &d){return d.message.find("after its owner lifetime ended")!=std::string::npos;});
  if(!expected){std::cerr<<"Unexpected negative case "<<ordinal<<", success="<<rejected.success<<'\n';test::ReportFailure(rejected);}
  assert(expected);
 }
 std::vector<llvm::StringRef> reader;
 for(const auto &capability:SupportedSemanticInterfaceCapabilities())
  if(capability!=kManualFamilyCallProofSemantic)reader.push_back(capability);
 auto unsupported=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);
 assert(!unsupported);llvm::consumeError(unsupported.takeError());
 reader.push_back(kManualFamilyCallProofSemantic);
 auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 auto missing=*decoded;std::erase(missing.required_semantics,kManualFamilyCallProofSemantic);
 auto invalid=EncodeSemanticInterfaceSidecar(missing,reader);assert(!invalid);llvm::consumeError(invalid.takeError());
 const auto main=root/"main.cpp";
 Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
 auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,true,{});
}
}

int main() {
 test::IsolatedCases cases;
 llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-indirect-family",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const auto &[standard,ownership]:std::vector<std::pair<const char *,int>>{
     {"c++20",1},{"c++20",2},{"c++23",1},{"c++23",2}})
  cases.Run(std::string(standard)+"-"+std::to_string(ownership),[&]{
   Run(root/(std::string(standard)+"-"+std::to_string(ownership)),standard,ownership);
  });
 return cases.Finish();
}
