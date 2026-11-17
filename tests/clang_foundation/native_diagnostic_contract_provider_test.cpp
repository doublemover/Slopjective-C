#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_emitter.h"
#include "llvm/ADT/ScopeExit.h"
#include "llvm/Support/JSON.h"
#include <algorithm>
#include <tuple>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
constexpr const char *Source=R"contract(
namespace DiagnosticContracts {
struct API {
  [[deprecated("old field: use replacement")]] int legacy;
  int replacement;
  [[deprecated("old method: use current")]] int old() const {return 41;}
  int current() const {return 42;}
  __attribute__((unavailable("member unavailable"))) int forbidden() const;
  template<class T> [[deprecated("old member template")]] T convert(T value) const {return value;}
  template<class T> using Alias [[deprecated("old member alias")]]=T;
  enum State {current_state=1, old_state [[deprecated("old enumerator")]]=2};
  struct [[deprecated("old nested record")]] Nested {int value;};
};
template<class T> struct Generic {
  [[deprecated("old generic field")]] T old;
  T current;
  [[deprecated("old generic method")]] T read() const {return current;}
  __attribute__((unavailable("generic unavailable"))) T forbidden() const;
};
template<class T> using Alias [[deprecated("old namespace alias")]]=T;
template<class T> struct [[deprecated("old class template")]] Obsolete {T value;};
template<class T> [[deprecated("old variable template")]] inline constexpr T old_value=T(3);
template<class T> struct __attribute__((availability(macos,introduced=12.0,deprecated=14.0,message="use new generic")))
Versioned {T value;};
struct PlatformAPI {
  __attribute__((availability(macos,introduced=12.0,deprecated=14.0,replacement="current",message="old platform method")))
  int old() const;
  __attribute__((availability(ios,introduced=16.0,obsoleted=19.0,strict,message="old mobile method")))
  int mobile() const;
};
__attribute__((error("blocked call: use \"supported\"\\next"))) int blocked();
__attribute__((warning("discouraged call: use supported"))) int discouraged();
[[deprecated("old redeclared function")]] int redeclared(int);
inline int redeclared(int value){return value;}
__attribute__((error("blocked redeclaration"))) int redeclared_error();
int redeclared_error();
struct Diagnostics {
  __attribute__((error("blocked member call"))) int blocked() const;
  template<class T> __attribute__((warning("discouraged template call"))) T discouraged(T) const;
};
inline int local() {
  [[maybe_unused]] int quiet=11;
  [[deprecated("old local binding")]] int old=7;
  return 42;
}
inline int run() {API api;api.replacement=42;Generic<int> generic;generic.current=42;return api.current()+generic.current+local();}
}
)contract";

bool HasDiagnostic(const CompileResult &result,const std::string &text) {
  return std::any_of(result.diagnostics.begin(),result.diagnostics.end(),[&](const auto &d){return d.message.find(text)!=std::string::npos;});
}

void Run(const fs::path &root,const char *standard,int ownership) {
  fs::create_directories(root/"provider");
  auto request=[&](const fs::path &input,const fs::path &output) {
    return Request(input,output,standard,ownership,"-O0",{});
  };
  const auto source=root/"provider/module.mm";
  auto publish=[&](const std::string &text) {
    Write(source,text);
    auto provider=request(source,root/"provider.o");
    provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
    provider.module_name="fixture.DiagnosticContracts";provider.package_identity="diagnostic-contract-v1";
    provider.interface_exports={"DiagnosticContracts"};provider.emit_semantic_interface=true;
    auto built=Compile(provider);Success(built);
    assert(built.interface_artifact && built.interface_artifact->success);
    return *built.interface_artifact;
  };
  const auto artifact=publish(Source);
  assert(artifact.sidecar_json.find(kNativeDeclarationAttributeSemantic)!=std::string::npos);
  for(const auto *spelling:{"old member alias","old enumerator","old generic field","old namespace alias",
      "old variable template","old nested record","old platform method","old mobile method",
      "blocked member call","old local binding","maybe_unused"})
    assert(artifact.source_bytes.find(spelling)!=std::string::npos);
  const auto repeated=publish(Source);
  assert(repeated.source_bytes==artifact.source_bytes && repeated.sidecar_json==artifact.sidecar_json);
  const auto local_only=publish("namespace DiagnosticContracts{inline int local(){[[deprecated(\"local only\")]] int old=1;return 0;}}");
  assert(local_only.sidecar_json.find(kNativeDeclarationAttributeSemantic)!=std::string::npos);
  const auto local_changed=publish("namespace DiagnosticContracts{inline int local(){[[deprecated(\"changed local only\")]] int old=1;return 0;}}");
  assert(local_only.identity.semantic_fingerprint!=local_changed.identity.semantic_fingerprint);
  const auto unpublished=publish("namespace DiagnosticContracts{int local(){[[deprecated(\"private local only\")]] int old=1;return 0;}}");
  assert(unpublished.sidecar_json.find(kNativeDeclarationAttributeSemantic)==std::string::npos);
  for(const auto &[before,after]:std::vector<std::pair<std::string,std::string>>{
      {"old field: use replacement","changed field contract"},
      {"old member template","changed member template"},
      {"old member alias","changed member alias"},
      {"old generic field","changed generic field"},
      {"old nested record","changed nested record"},
      {"old enumerator","changed enumerator"},
      {"old namespace alias","changed alias"},
      {"old class template","changed class template"},
      {"old variable template","changed variable template"},
      {"old redeclared function","changed redeclaration"},
      {"blocked redeclaration","changed inherited diagnostic"},
      {"introduced=12.0,deprecated=14.0,message=","introduced=13.0,deprecated=15.0,message="},
      {"replacement=\"current\"","replacement=\"replacement\""},
      {"obsoleted=19.0","obsoleted=20.0"},
      {"warning(\"discouraged call: use supported\")","error(\"discouraged call: use supported\")"},
      {"blocked member call","changed blocked member"},
      {"old local binding","changed local binding"}}) {
    std::string changed=Source;const auto at=changed.find(before);assert(at!=std::string::npos);
    changed.replace(at,before.size(),after);
    assert(publish(changed).identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint);
  }
  // Restore the exact provider before deleting all source from the import path.
  publish(Source);assert(fs::remove(source));
  const auto interface=root/"DiagnosticContracts.objc3i";
  Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  auto imported_request=[&](const fs::path &input,const fs::path &output) {
    auto consumer=request(input,output);InterfaceImportInput imported;
    imported.source_path=interface.string();imported.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported};return consumer;
  };
  for(const char *name:{"a","b"}) {
    const auto input=root/(std::string(name)+".mm");
    Write(input,"extern \"C\" int check_"+std::string(name)+"(){return DiagnosticContracts::run()!=126;}");
    Success(Compile(imported_request(input,root/(std::string(name)+".o"))));
  }
  unsigned ordinal=0;
  for(const auto &[expression,message]:std::vector<std::pair<std::string,std::string>>{
      {"DiagnosticContracts::API value{};return value.legacy;","old field: use replacement"},
      {"DiagnosticContracts::API value{};return value.old();","old method: use current"},
      {"DiagnosticContracts::API value{};return value.forbidden();","member unavailable"},
      {"DiagnosticContracts::API value{};return value.convert(3);","old member template"},
      {"DiagnosticContracts::API::Alias<int> value=3;return value;","old member alias"},
      {"return DiagnosticContracts::API::old_state;","old enumerator"},
      {"DiagnosticContracts::API::Nested value{3};return value.value;","old nested record"},
      {"DiagnosticContracts::Generic<int> value{};return value.old;","old generic field"},
      {"DiagnosticContracts::Generic<int> value{};return value.read();","old generic method"},
      {"DiagnosticContracts::Generic<int> value{};return value.forbidden();","generic unavailable"},
      {"DiagnosticContracts::Alias<int> value=3;return value;","old namespace alias"},
      {"DiagnosticContracts::Obsolete<int> value{3};return value.value;","old class template"},
      {"return DiagnosticContracts::old_value<int>;","old variable template"},
      {"return DiagnosticContracts::redeclared(3);","old redeclared function"},
      {"return DiagnosticContracts::redeclared_error();","blocked redeclaration"},
      {"return DiagnosticContracts::blocked();","blocked call: use \"supported\"\\next"},
      {"return DiagnosticContracts::discouraged();","discouraged call: use supported"},
      {"DiagnosticContracts::Diagnostics value;return value.blocked();","blocked member call"},
      {"DiagnosticContracts::Diagnostics value;return value.discouraged(3);","discouraged template call"}}) {
    const auto input=root/("bad-"+std::to_string(ordinal++)+".mm");Write(input,"int bad(){"+expression+"}");
    auto consumer=imported_request(input,root/"bad.o");
    consumer.arguments.push_back("-Werror=deprecated-declarations");
    consumer.arguments.push_back("-Werror=attribute-warning");
    const auto rejected=Compile(consumer);
    if(rejected.success || !HasDiagnostic(rejected,message))test::ReportFailure(rejected);
    assert(!rejected.success && HasDiagnostic(rejected,message));
  }
  std::vector<llvm::StringRef> reader;
  for(const auto &capability:SupportedSemanticInterfaceCapabilities())
    if(capability!=kNativeDeclarationAttributeSemantic)reader.push_back(capability);
  reader.push_back("objc3.native-declaration-attributes.v3");
  auto unsupported=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);
  assert(!unsupported);llvm::consumeError(unsupported.takeError());
  // A sidecar cannot silently drop the requirement while retaining its marker.
  auto json=llvm::json::parse(artifact.sidecar_json);assert(json);
  auto *semantics=json->getAsObject()->getArray("required_semantics");assert(semantics);
  for(auto it=semantics->begin();it!=semantics->end();++it)
    if(it->getAsString()==kNativeDeclarationAttributeSemantic){semantics->erase(it);break;}
  std::string malformed;llvm::raw_string_ostream output(malformed);output<<*json;
  reader.clear();for(const auto &capability:SupportedSemanticInterfaceCapabilities())reader.push_back(capability);
  auto missing=DecodeSemanticInterfaceSidecar(malformed,artifact.source_bytes,reader);
  assert(!missing);llvm::consumeError(missing.takeError());
  const auto main=root/"main.cpp";
  Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
  auto entry=request(main,root/"main.o");Success(Compile(entry));
  LinkAndExecute(root,entry.output_path,ownership!=0,{});
}
}

int main() {
  test::IsolatedCases cases;
  llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-native-diagnostics",temporary));
  const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
  for(const auto &[standard,ownership]:std::vector<std::pair<const char *,int>>{
      {"c++20",0},{"c++23",0},{"c++20",1},{"c++23",2}})
    cases.Run(std::string(standard)+"-"+std::to_string(ownership),[&]{
      Run(root/(std::string(standard)+"-"+std::to_string(ownership)),standard,ownership);
    });
  return cases.Finish();
}
