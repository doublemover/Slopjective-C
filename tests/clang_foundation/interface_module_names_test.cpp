#include "clang_foundation/compile_session.h"
#include "clang_foundation/frontend_api.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/module_names.h"
#include "llvm/Support/Error.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
CompileRequest Request(const fs::path &root, BaseLanguage language,
                       const std::string &standard, bool arc) {
  CompileRequest r;
  r.input_path = (root / "module.m").string();
  r.language = language;
  r.objc3 = language == BaseLanguage::ObjectiveC ||
            language == BaseLanguage::ObjectiveCxx;
  r.arc = arc;
  if (r.objc3)
    r.objc_runtime = "gnustep-2.2";
  r.target_triple = "x86_64-unknown-linux-gnu";
  r.arguments = {"-std=" + standard, "-fblocks"};
  r.module_name = "Names.Provider";
  r.package_identity = "names-v1";
  r.module_owned_names = true;
  r.emit_semantic_interface = true;
  r.interface_output_path = (root / "Names.objc3i").string();
  std::ofstream(root / "owned.h") << R"(
    #pragma once
    typedef struct HeaderValue { int value; } HeaderValue;
    int module_value(int value);
    extern int module_global __attribute__((deprecated("old global")));
    #define OWNED_HELPER(name) int name(void) { return 0; }
    OWNED_HELPER(header_helper)
    #ifdef __cplusplus
    inline int private_initializer() { return 1; }
    template<class T> T private_template(T value) {
      static int local_bias = private_initializer();
      return value + local_bias;
    }
    template<int N> int private_variable = N + private_initializer();
    thread_local int private_tls = private_initializer();
    const int &private_reference = private_initializer();
    namespace PrivateNamespace {
      int private_namespace_global = 0;
      int private_namespace_helper() { return private_namespace_global; }
    }
    struct NamedRoot {
      int n;
      NamedRoot(); virtual ~NamedRoot();
      virtual int get() const=0;
      virtual NamedRoot *self();
    };
    struct NamedLeft : virtual NamedRoot { NamedLeft(); int get() const override; };
    struct NamedRight : virtual NamedRoot { NamedRight(); virtual int right() const; };
    struct NamedDiamond final : NamedLeft, NamedRight {
      NamedDiamond(); ~NamedDiamond() override;
      int get() const override final; NamedDiamond *self() override;
    };
    namespace NamedScope {
      struct Point { int n; };
      int increment(Point);
    }
    namespace NamedUsing { using NamedScope::Point; using NamedScope::increment; }
    class NamedFamily {
      struct Hidden { int n; };
    public:
      struct Value { int n; int get() const; };
      using Result=Value;
      enum class Kind : unsigned { Answer=42 };
    };
    struct NamedPrefix { long prefix; };
    class NamedValue {
      int storage;
    public:
      explicit NamedValue(int);
      NamedValue(const NamedValue&);
      ~NamedValue();
      int get() const;
      explicit operator bool() const;
    };
    class NamedDerived : public NamedPrefix, public NamedValue {
    public: explicit NamedDerived(int);
    };
    struct NamedDefaults : NamedPrefix {
      int n;
      NamedDefaults() noexcept(false)=default;
      NamedDefaults(const NamedDefaults&) noexcept=default;
      ~NamedDefaults()=default;
    };
    #endif
  )";
  r.source_text = R"(
    #include "owned.h"
    extern int module_global;
    int module_value(int value);
    int module_value(int value) { 
      #ifdef __cplusplus
      return private_template<int>(value) + private_template<long>(module_global) + private_variable<0> + private_tls + private_reference + header_helper() + PrivateNamespace::private_namespace_helper();
      #else
      return value + module_global + header_helper();
      #endif
     }
    int module_global = 42;
    int implementation_helper(void) { return 1; }
  )";
  if (language == BaseLanguage::Cxx || language == BaseLanguage::ObjectiveCxx)
    *r.source_text += R"(
      int NamedFamily::Value::get() const { return n; }
      int NamedScope::increment(Point point) { return point.n + 1; }
      NamedRoot::NamedRoot() : n(42) {}
      NamedRoot::~NamedRoot() {}
      NamedRoot *NamedRoot::self() { return this; }
      NamedLeft::NamedLeft() {}
      int NamedLeft::get() const { return n; }
      NamedRight::NamedRight() {}
      int NamedRight::right() const { return n; }
      NamedDiamond::NamedDiamond() {}
      NamedDiamond::~NamedDiamond() {}
      int NamedDiamond::get() const { return n; }
      NamedDiamond *NamedDiamond::self() { return this; }
      NamedValue::NamedValue(int n) : storage(n) {}
      NamedValue::NamedValue(const NamedValue &v) : storage(v.storage) {}
      NamedValue::~NamedValue() { storage = -1; }
      int NamedValue::get() const { return storage; }
      NamedValue::operator bool() const { return storage >= 0; }
      NamedDerived::NamedDerived(int n) : NamedPrefix{1}, NamedValue(n) {}
      int class_user() {
        NamedValue a(42); NamedValue b(a); NamedDerived derived(42);
        NamedDefaults d{}; d.n=42; NamedDefaults copied(d);
        return b.get() + derived.get() + copied.n;
      }
    )";
  if (r.objc3)
    *r.source_text += R"(
    @protocol Root
    - (int)value;
    @end
    __attribute__((objc_root_class)) @interface Root <Root> { @public int slot; }
    - (int)value;
    - (int)directValue __attribute__((objc_direct));
    @end
    @implementation Root
    - (int)value { return slot; }
    - (int)directValue { return slot + 1; }
    @end
    @protocol Protocol
    - (int)value;
    @end
    @interface Child : Root <Protocol> @end
    @implementation Child
    - (int)value { return [super value] + 1; }
    @end
    @interface Root (Extension)
    - (int)extra;
    @end
    @implementation Root (Extension)
    - (int)extra { return [self directValue]; }
    @end
  )";
  r.interface_exports = {"module_value", "module_global", "HeaderValue"};
  if (language == BaseLanguage::Cxx || language == BaseLanguage::ObjectiveCxx) {
    *r.source_text += R"(
      struct MemberValue { int slot; int get() const; int operator()(int) const; };
      int MemberValue::get() const { return slot; }
      int MemberValue::operator()(int n) const { return slot + n; }
    )";
    r.interface_exports.push_back("MemberValue");
    r.interface_exports.insert(
        r.interface_exports.end(),
        {"NamedValue", "NamedPrefix", "NamedDerived", "NamedDefaults",
         "NamedRoot", "NamedLeft", "NamedRight", "NamedDiamond", "NamedScope", "NamedFamily", "NamedUsing"});
  }

  if (r.objc3)
    r.interface_exports.insert(r.interface_exports.end(),
                               {"Root", "Protocol", "Child", "Extension"});
  return r;
}
void Success(const CompileResult &r) {
  if (!r.success) {
    std::cerr << r.failure << '\n';
    for (const auto &d : r.diagnostics)
      std::cerr << d.message << '\n';
    if (r.interface_artifact)
      for (const auto &d : r.interface_artifact->diagnostics)
        std::cerr << d.code << ' ' << d.message << '\n';
  }
  assert(r.success);
}
const std::vector<llvm::StringRef> semantics = {
    "objc3.canonical-function-effects.v1",
    "objc3.native-carrier-types.v1",
    "objc3.native-return-ownership.v1",
    "objc3.direct-dispatch.v1",
    "objc3.cxx-data-interfaces.v1",
    "objc3.cxx-member-interfaces.v1",
    "objc3.cxx-class-lifecycles.v1",
    "objc3.cxx-class-structure.v1",
    "objc3.cxx-class-dispatch.v1",
    "objc3.cxx-namespaces.v1",
    "objc3.cxx-nested-types.v1",
    "objc3.cxx-using.v1",
    "objc3.ownership-projection.v1",
    "objc3.api-availability.v1",
    "objc3.api-partitions.v1",
    "objc3.module-owned-names.v1"};
void Check(const fs::path &root, BaseLanguage language,
           const std::string &standard, bool arc) {
  auto request = Request(root, language, standard, arc);
  request.emit = EmitKind::LLVM;
  request.output_path = (root / "provider.ll").string();
  auto built = Compile(request);
  Success(built);
  auto sidecar = DecodeSemanticInterfaceSidecar(
      built.interface_artifact->sidecar_json,
      built.interface_artifact->source_bytes, semantics);
  assert(sidecar);
  for (const auto &d : sidecar->declarations) {
    if (d.kind == "CXXRecord" || d.kind == "Function" || IsCxxMemberDeclarationKind(d.kind) ||
        d.kind == "Var" || d.kind == "ObjCInterface" ||
        d.kind == "ObjCProtocol" || d.kind == "ObjCCategory") {
      assert(!d.native_symbol_name.empty());
      assert(d.native_symbol_name ==
             BuildModuleOwnedName(request.module_name, request.package_identity,
                                  d.stable_id));
    }
  }
  std::ifstream input(request.output_path);
  std::string ir((std::istreambuf_iterator<char>(input)), {});
  for (const auto &d : sidecar->declarations) {
    if (d.kind == "ObjCProtocol")
      continue; // Protocol metadata is emitted on use.
    if (d.kind != "CXXRecord" &&
        d.canonical_type_identity.find(";pure=1") == std::string::npos &&
        !d.native_symbol_name.empty() &&
        d.canonical_type_identity.find(";defaulted=1;") == std::string::npos)
      assert(ir.find(d.native_symbol_name) != std::string::npos);
  }
  if (request.objc3) {
    assert(ir.find("_OBJC_CLASS_Root") == std::string::npos);
    assert(ir.find("__objc_ivar_offset_Root.") == std::string::npos);
    assert(ir.find("_i_Root__directValue") == std::string::npos);
  }
  if (language == BaseLanguage::Cxx || language == BaseLanguage::ObjectiveCxx) {
    bool constructor = false, destructor = false;
    for (const auto &d : sidecar->declarations) {
      if (d.kind == "CXXConstructor" &&
          d.canonical_type_identity.find(";defaulted=1;") ==
              std::string::npos) {
        constructor = true;
        // Abstract bases have no complete-object constructor entry.
        const auto owner = d.qualified_name.substr(0, d.qualified_name.rfind("::"));
        const bool abstract = std::any_of(
            sidecar->declarations.begin(), sidecar->declarations.end(),
            [&](const auto &record) {
              return record.kind == "CXXRecord" && record.qualified_name == owner &&
                     record.canonical_type_identity.find(";abstract=1") != std::string::npos;
            });
        if (!abstract)
          assert(ir.find(d.native_symbol_name + "_ctor_0") != std::string::npos);
        assert(ir.find(d.native_symbol_name + "_ctor_1") != std::string::npos);
      }
      if (d.kind == "CXXDestructor" &&
          d.canonical_type_identity.find(";defaulted=1;") ==
              std::string::npos) {
        destructor = true;
        assert(ir.find(d.native_symbol_name + "_dtor_1") != std::string::npos);
        assert(ir.find(d.native_symbol_name + "_dtor_2") != std::string::npos);
        if (d.canonical_type_identity.find(";virtual=1;") != std::string::npos)
          assert(ir.find(d.native_symbol_name + "_dtor_0") != std::string::npos);
      }
    }
    assert(constructor && destructor);
    for (const auto &d : sidecar->declarations)
      if (d.kind == "CXXRecord" && d.qualified_name == "NamedDiamond") {
        assert(ir.find("_ZTV") != std::string::npos);
        assert(ir.find("_ZTT") != std::string::npos);
        assert(ir.find(d.native_symbol_name) != std::string::npos);
      }
    assert(ir.find("private_template") == std::string::npos);
    assert(ir.find("private_variable") == std::string::npos);
    assert(ir.find("private_initializer") == std::string::npos);
    assert(ir.find("private_tls") == std::string::npos);
    assert(ir.find("private_reference") == std::string::npos);
    assert(ir.find("private_namespace_helper") == std::string::npos);
    assert(ir.find("private_namespace_global") == std::string::npos);
  }
  const auto original = built.interface_artifact->identity;
  auto relocated = request;
  relocated.input_path = (root / "moved-provider.m").string();
  auto moved = Compile(relocated);
  Success(moved);
  assert(moved.interface_artifact->identity.semantic_fingerprint ==
         original.semantic_fingerprint);
  auto package = request;
  package.package_identity = "names-v2";
  auto renamed = Compile(package);
  Success(renamed);
  assert(renamed.interface_artifact->identity.semantic_fingerprint !=
         original.semantic_fingerprint);
  if (language == BaseLanguage::Cxx || language == BaseLanguage::ObjectiveCxx) {
    auto renamed_sidecar = DecodeSemanticInterfaceSidecar(
        renamed.interface_artifact->sidecar_json,
        renamed.interface_artifact->source_bytes, semantics);
    assert(renamed_sidecar);
    for (const auto &type : sidecar->declarations)
      if (type.kind == "CXXRecord" && type.qualified_name == "NamedDiamond") {
        auto renamed_type = std::find_if(
            renamed_sidecar->declarations.begin(), renamed_sidecar->declarations.end(),
            [&](const auto &d) { return d.stable_id == type.stable_id; });
        assert(renamed_type != renamed_sidecar->declarations.end());
        assert(!type.native_symbol_name.empty() &&
               renamed_type->native_symbol_name != type.native_symbol_name);
        std::ifstream renamed_input(package.output_path);
        std::string renamed_ir((std::istreambuf_iterator<char>(renamed_input)), {});
        assert(renamed_ir.find(renamed_type->native_symbol_name) != std::string::npos);
        assert(renamed_ir.find(type.native_symbol_name) == std::string::npos);
      }
  }
  Success(Compile(request));
  auto consumer = Request(root, language, standard, arc);
  consumer.module_owned_names = false;
  consumer.emit_semantic_interface = false;
  consumer.interface_exports.clear();
  consumer.interface_output_path.clear();
  consumer.module_name.clear();
  consumer.package_identity.clear();
  consumer.source_text =
      "int client(void) { return module_value(module_global); }";
  if (request.objc3)
    consumer.source_text = "int client(Child *value, id<Root> protocol) { "
                           "return module_value(module_global) + [value "
                           "directValue] + [value extra] + [protocol value]; }";
  if (language == BaseLanguage::Cxx || language == BaseLanguage::ObjectiveCxx)
    *consumer.source_text +=
        " int structure_client() { NamedDerived v(42); NamedDefaults d{}; "
        "d.n=42; NamedDefaults copy(d); return v.get()+copy.n; }"
        " int dispatch_client() { NamedDiamond d; NamedRoot *r=&d; "
        "NamedRight *right=dynamic_cast<NamedRight*>(r); "
        "return r->get()+right->right()+(r->self()==r); }"
        " int namespace_client() { return NamedScope::increment(NamedScope::Point{41}); }"
        " int nested_client() { NamedFamily::Result value{42}; return value.get(); }"
        " int using_client() { NamedUsing::Point p{41}; return NamedUsing::increment(p); }";
  InterfaceImportInput imported;
  imported.source_path = request.interface_output_path;
  imported.locked_identity = original;
  consumer.imported_interfaces = {imported};
  Success(Compile(consumer));
  const auto physical_header = root / "provider-artifact.h";
  fs::copy_file(request.interface_output_path, physical_header,
                fs::copy_options::overwrite_existing);
  fs::copy_file(request.interface_output_path + ".json",
                physical_header.string() + ".json",
                fs::copy_options::overwrite_existing);
  auto owned_consumer = consumer;
  owned_consumer.module_owned_names = true;
  owned_consumer.module_source_root = root.string();
  owned_consumer.module_name = "Names.Consumer";
  owned_consumer.package_identity = "consumer-v1";
  owned_consumer.emit_semantic_interface = true;
  owned_consumer.interface_output_path = (root / "Consumer.objc3i").string();
  owned_consumer.interface_exports = {"client"};
  owned_consumer.imported_interfaces[0].source_path = physical_header.string();
  Success(Compile(owned_consumer));
  auto outside_root = request;
  outside_root.module_name += ".__ownership_producer";
  assert(!Compile(outside_root).success);
  outside_root = request;
  outside_root.module_source_root = (root / "elsewhere").string();
  assert(!Compile(outside_root).success);
  fs::create_directories(root / "owned-source");
  std::ofstream(root / "foreign.h") << "int foreign_api(int value);\n";
  auto foreign = request;
  foreign.input_path = (root / "owned-source/foreign-client.m").string();
  foreign.module_source_root = (root / "owned-source").string();
  foreign.interface_exports = {"module_value"};
  foreign.source_text =
      "#include \"../foreign.h\"\nint foreign_api(int value);\nint "
      "module_value(int value) { return foreign_api(value); }\n";
  Success(Compile(foreign));
  foreign.interface_exports = {"foreign_api"};
  foreign.source_text = "#include \"../foreign.h\"\nint foreign_api(int value) "
                        "{ return value; }\n";
  assert(!Compile(foreign).success);
  auto conflict = request;
  conflict.source_text = "int module_value(void) __asm__(\"foreign_name\");";
  conflict.interface_exports = {"module_value"};
  assert(!Compile(conflict).success);
  conflict.source_text = "int main(void) { return 0; }";
  conflict.interface_exports = {"main"};
  assert(!Compile(conflict).success);
  if (language == BaseLanguage::Cxx || language == BaseLanguage::ObjectiveCxx) {
    conflict.source_text =
        "struct __attribute__((abi_tag(\"foreign_tag\"))) Tagged { int n; };";
    conflict.interface_exports = {"Tagged"};
    assert(!Compile(conflict).success);
    conflict.source_text = "namespace Q { extern \"C\" int api(); } "
                           "namespace R { extern \"C\" int api(); }";
    conflict.interface_exports = {"Q", "R"};
    assert(!Compile(conflict).success);
    conflict.source_text = "inline int module_global = 42;";
    conflict.interface_exports = {"module_global"};
    assert(Compile(conflict).success);
  }
  auto tampered = *sidecar;
  auto named =
      std::find_if(tampered.declarations.begin(), tampered.declarations.end(),
                   [](const auto &d) { return !d.native_symbol_name.empty(); });
  named->native_symbol_name = "__objc3_m_forged";
  auto encoded = EncodeSemanticInterfaceSidecar(tampered, semantics);
  assert(!encoded);
  llvm::consumeError(encoded.takeError());
  tampered = *sidecar;
  tampered.required_semantics.erase(std::find(
      tampered.required_semantics.begin(), tampered.required_semantics.end(),
      "objc3.module-owned-names.v1"));
  encoded = EncodeSemanticInterfaceSidecar(tampered, semantics);
  assert(!encoded);
  llvm::consumeError(encoded.takeError());
  if (request.objc3) {
    auto projected = request;
    projected.source_text = R"(
      #ifdef __cplusplus
      extern "C" {
      #endif
      int module_value(int value) { return value; }
      #ifdef __cplusplus
      }
      #endif
    )";
    projected.interface_exports = {"module_value"};
    projected.projected_arc = !arc;
    auto projection = Compile(projected);
    Success(projection);
    auto projected_client = consumer;
    projected_client.arc = !arc;
    projected_client.source_text =
        "int client(void) { return module_value(42); }";
    projected_client.imported_interfaces[0].locked_identity =
        projection.interface_artifact->identity;
    Success(Compile(projected_client));
  }
  if (language == BaseLanguage::C) {
    const std::string input = (root / "embedding.c").string();
    const std::string directory = root.string();
    const std::string body = "int embedded(void) { return 42; }";
    const char *arguments[] = {"-std=c17"};
    objc3c_frontend_v3_options options{};
    options.struct_size = sizeof(options);
    options.abi_version = OBJC3C_FRONTEND_V3_ABI;
    options.base.struct_size = sizeof(options.base);
    options.base.abi_version = OBJC3C_FRONTEND_V2_ABI;
    options.base.input_path = input.c_str();
    options.base.source = body.c_str();
    options.base.source_size = body.size();
    options.base.target_triple = "x86_64-unknown-linux-gnu";
    options.base.language = 0;
    options.base.arguments = arguments;
    options.base.argument_count = 1;
    options.base.module_name = "Names.Embedding";
    options.base.package_identity = "embedding-v1";
    options.module_owned_names = 1;
    options.module_source_root = directory.c_str();
    objc3c_frontend_v2_result *result = nullptr;
    assert(objc3c_frontend_v3_compile(&options, &result) == 0);
    objc3c_frontend_v2_result_destroy(result);
    options.module_owned_names = 2;
    assert(objc3c_frontend_v3_compile(&options, &result) == 2 && !result);
    options.module_owned_names = 0;
    assert(objc3c_frontend_v3_compile(&options, &result) == 2 && !result);
    options.module_owned_names = 1;
    options.struct_size =
        offsetof(objc3c_frontend_v3_options, module_source_root) + 1;
    assert(objc3c_frontend_v3_compile(&options, &result) == 2 && !result);
  }
}
} // namespace
int main() {
  const auto root =
      fs::temp_directory_path() /
      ("objc3-names-" +
       std::to_string(
           std::chrono::steady_clock::now().time_since_epoch().count()));
  fs::create_directories(root);
  Check(root, BaseLanguage::C, "c17", false);
  for (const auto &standard : {"c++20", "c++23"})
    Check(root, BaseLanguage::Cxx, standard, false);
  for (bool arc : {false, true}) {
    Check(root, BaseLanguage::ObjectiveC, "c17", arc);
    for (const auto &standard : {"c++20", "c++23"})
      Check(root, BaseLanguage::ObjectiveCxx, standard, arc);
  }
  fs::remove_all(root);
}
