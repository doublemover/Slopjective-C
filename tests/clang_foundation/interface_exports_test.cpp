#include "clang_foundation/compile_session.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
void Write(const fs::path &path, const std::string &value) {
  std::ofstream(path, std::ios::binary | std::ios::trunc) << value;
}
std::string Read(const fs::path &path) {
  std::ifstream stream(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(stream), {}};
}
CompileRequest Request(const fs::path &path, BaseLanguage language,
                       const std::string &standard, bool arc,
                       std::string source) {
  CompileRequest r;
  r.input_path = path.string();
  r.source_text = std::move(source);
  r.language = language;
  r.objc3 = language == BaseLanguage::ObjectiveC ||
            language == BaseLanguage::ObjectiveCxx;
  r.arc = arc;
  if (r.objc3)
    r.objc_runtime = "gnustep-2.2";
  r.target_triple = "x86_64-unknown-linux-gnu";
  r.arguments = {"-std=" + standard, "-fblocks"};
  return r;
}
void Surface(const fs::path &root, BaseLanguage language,
             const std::string &standard, bool arc) {
  bool cpp =
      language == BaseLanguage::Cxx || language == BaseLanguage::ObjectiveCxx;
  const std::string implementation =
      "typedef struct API { int value; } API;\n"
      "int answer(struct API input) { struct BodyOnly { int x; }; "
      "struct BodyOnly local = {0}; return input.value + local.x + 42; }\n"
      "int hidden(void) { return 1; }\n" +
      (cpp ? std::string(
                 "class Private { int value; public: Private():value(1){} };\n"
                 "template<class T> T private_template(T value) { return "
                 "value; }\n")
           : std::string("struct Private { int value; };\n"));
  auto r = Request(root / (cpp ? "surface.mm" : "surface.m"), language,
                   standard, arc, implementation);
  r.emit_semantic_interface = true;
  r.module_name = "Surface";
  r.package_identity = "surface-v1";
  r.interface_exports = {"API", "answer"};
  r.interface_output_path = (root / "Surface.objc3i").string();
  r.emit = EmitKind::Object;
  r.output_path = (root / "surface.o").string();
  auto built = Compile(r);
  assert(built.success);
  const auto &first = *built.interface_artifact;
  assert(first.source_bytes.find("hidden") == std::string::npos);
  assert(first.sidecar_json.find("Private") == std::string::npos);
  assert(first.sidecar_json.find("BodyOnly") == std::string::npos);
  assert(first.sidecar_json.find("private_template") == std::string::npos);
  auto edited = r;
  *edited.source_text += "int another_private_function(void) { return 100; }\n";
  auto changed = Compile(edited);
  assert(changed.success &&
         changed.interface_artifact->sidecar_json == first.sidecar_json);
  auto client = Request(
      root / "client.m", language, standard, arc,
      "int client(void) { struct API input = {0}; return answer(input); }");
  InterfaceImportInput input;
  input.source_path = r.interface_output_path;
  input.locked_identity = first.identity;
  client.imported_interfaces = {input};
  assert(Compile(client).success);
  client.source_text = "int client(void) { return hidden(); }";
  assert(!Compile(client).success);
  client.source_text = "int client(void) { return sizeof(struct Private); }";
  assert(!Compile(client).success);
  for (const auto &selection :
       std::vector<std::vector<std::string>>{{"missing"},
                                             {"API", "missing"},
                                             {"*", "answer"},
                                             {"answer", "answer"},
                                             {"answer"},
                                             {"hidden", ""}}) {
    Write(r.output_path, "prior object");
    Write(r.interface_output_path, "prior interface");
    Write(r.interface_output_path + ".json", "prior sidecar");
    r.interface_exports = selection;
    assert(!Compile(r).success);
    assert(Read(r.output_path) == "prior object");
    assert(Read(r.interface_output_path) == "prior interface");
    assert(Read(r.interface_output_path + ".json") == "prior sidecar");
  }
  r.source_text = "typedef int HiddenAlias; HiddenAlias public_alias(void);";
  r.interface_exports = {"public_alias"};
  assert(!Compile(r).success);
  r.interface_exports = {"HiddenAlias", "public_alias"};
  assert(Compile(r).success);
  r.interface_exports = {"*"};
  assert(Compile(r).success);
}
void CompilerBuiltins(const fs::path &root, const std::string &standard, bool arc) {
  auto r=Request(root/"builtins.mm",BaseLanguage::ObjectiveCxx,standard,arc,
      "inline int public_zero(void *storage){__builtin_memset(storage,0,8);return __builtin_popcount(42);}");
  r.emit_semantic_interface=true;r.module_name="Builtins";r.package_identity="builtins-v1";
  r.interface_exports={"public_zero"};r.interface_output_path=(root/"Builtins.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"builtins.o").string();
  auto built=Compile(r);assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;
  input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"builtin-client.mm",BaseLanguage::ObjectiveCxx,standard,arc,
      "int client(){unsigned long long value=~0ULL;return public_zero(&value);}");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"builtin-client.o").string();assert(Compile(client).success);
  for(const std::string &name:{"__builtin_vendor_copy","memset"}) {
    r.source_text="extern \"C\" void *"+name+"(void*,int,__SIZE_TYPE__);"
        "inline int public_zero(void *storage){"+name+"(storage,0,8);return 0;}";
    auto rejected=Compile(r);assert(!rejected.success && rejected.interface_artifact);
    bool excluded=false;
    for(const auto &diagnostic:rejected.interface_artifact->diagnostics)
      excluded|=diagnostic.code=="O3IFC156";
    assert(excluded);
  }
}
void KeyPathSubscriptBodies(const fs::path &root,const std::string &standard,bool arc) {
  auto r=Request(root/"indexed-provider.mm",BaseLanguage::ObjectiveCxx,standard,arc,R"(
      __attribute__((objc_root_class)) @interface InlinePathRoot @end
      inline int indexed_roundtrip(InlinePathRoot *_Nonnull root,
          WritableKeyPath<InlinePathRoot*,int> path,int value) {
        int assigned=(root[path]=value);int updated=(root[path]+=2);
        return assigned+updated+root[path]++ + ++root[path];
      }
  )");
  r.emit_semantic_interface=true;r.module_name="IndexedBodies";r.package_identity="indexed-bodies-v1";
  r.interface_exports={"InlinePathRoot","indexed_roundtrip"};
  r.interface_output_path=(root/"IndexedBodies.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"indexed-provider.o").string();
  auto built=Compile(r);assert(built.success && built.interface_artifact);
  assert(built.interface_artifact->sidecar_json.find("objc3.keypath-subscripts.v1")!=std::string::npos);
  InterfaceImportInput input;input.source_path=r.interface_output_path;
  input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"indexed-client.mm",BaseLanguage::ObjectiveCxx,standard,arc,R"(
      int client(InlinePathRoot *_Nonnull root,WritableKeyPath<InlinePathRoot*,int> path) {
        const objc3_keypath_descriptor_v2 *metadata=KeyPathMetadata(path);
        return indexed_roundtrip(root,path,10)+(objc3_keypath_validate_v2(metadata)==0);
      }
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"indexed-client.o").string();
  assert(Compile(client).success);
}

void UnnamedLocalBodies(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"unnamed-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
      inline int unnamed_inline(int n) {
        enum {delta=2};
        struct {struct {int n;} nested;int get() const {return nested.n+delta;}} value{{n}};
        using Copy=decltype(value);Copy copied=value;return copied.get();
      }
  )");
  r.emit_semantic_interface=true;r.module_name="UnnamedBodies";r.package_identity="unnamed-bodies-v1";
  r.interface_exports={"unnamed_inline"};r.interface_output_path=(root/"UnnamedBodies.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"unnamed-provider.o").string();
  auto built=Compile(r);assert(built.success && built.interface_artifact);
  assert(built.interface_artifact->sidecar_json.find("objc3.cxx-unnamed-local-types.v1")!=std::string::npos);
  InterfaceImportInput input;input.source_path=r.interface_output_path;
  input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"unnamed-client.cpp",BaseLanguage::Cxx,standard,false,
      "int client(){return unnamed_inline(40);}");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"unnamed-client.o").string();assert(Compile(client).success);
}

void Projection(const fs::path &root, bool producer_arc) {
  auto r = Request(
      root / "projected.m", BaseLanguage::ObjectiveC, "c17", producer_arc,
      "id factory(void) __attribute__((ns_returns_retained));\n"
      "struct Private { id value; }; int hidden(struct Private value);\n");
  r.emit_semantic_interface = true;
  r.projected_arc = !producer_arc;
  r.module_name = "Projected";
  r.package_identity = "projection-v1";
  r.interface_exports = {"factory"};
  auto result = Compile(r);
  assert(result.success && result.interface_artifact->source_bytes.find(
                               "hidden") == std::string::npos);
}
} // namespace
int main() {
  const auto root = fs::temp_directory_path() / "objc3 public export selection";
  fs::remove_all(root);
  fs::create_directories(root);
  Surface(root, BaseLanguage::C, "c17", false);
  for (const auto &standard : {"c++20", "c++23"}) {
    Surface(root, BaseLanguage::Cxx, standard, false);UnnamedLocalBodies(root,standard);
  }
  for (bool arc : {false, true}) {
    for(const auto &standard:{"c++20","c++23"}) {
      CompilerBuiltins(root,standard,arc);KeyPathSubscriptBodies(root,standard,arc);
    }
    Surface(root, BaseLanguage::ObjectiveC, "c17", arc);
    for (const auto &standard : {"c++20", "c++23"})
      Surface(root, BaseLanguage::ObjectiveCxx, standard, arc);
    Projection(root, arc);
  }
  fs::remove_all(root);
}
