#include "clang_foundation/compile_session.h"
#include "clang_foundation/frontend_api.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <random>

int main() {
  using namespace objc3::clang_foundation;
  CompileRequest request;
  request.input_path = "objc3c-session-memory.c";
  request.language = BaseLanguage::C;
  request.source_text = "typedef struct Pair { double x; long long y; } Pair; "
                        "Pair make_pair(double x, long long y) { return (Pair){x,y}; }";
  auto parsed = Compile(request);
  assert(parsed.success && !parsed.declarations.empty());
  assert(parsed.compiler_identity.find("ObjC3-patches=") != std::string::npos);
  assert(!parsed.target_triple.empty());
  auto target_request = request;
  target_request.language = BaseLanguage::ObjectiveC;
  target_request.input_path = "objc3c-session-target.m";
  target_request.source_text = "@interface NativeTarget @end";
  target_request.arguments = {"--target=x86_64-unknown-linux-gnu"};
  auto linux_target = Compile(target_request);
  assert(linux_target.success && linux_target.runtime_abi == "gnustep-2.0");
  target_request.arguments = {"--target=x86_64-apple-macosx14.0"};
  auto apple_target = Compile(target_request);
  assert(apple_target.success && apple_target.runtime_abi.starts_with("macosx"));
  target_request.arguments = {"--target=x86_64-unknown-linux-gnu", "-fobjc-runtime=gcc"};
  auto explicit_runtime = Compile(target_request);
  assert(explicit_runtime.success && explicit_runtime.runtime_abi == "gcc");
  auto explicit_standard = request;
  explicit_standard.arguments = {"-std=c23"};
  explicit_standard.source_text = "constexpr int value = 7; static_assert(value == 7);";
  assert(Compile(explicit_standard).success);
  auto cxx_request = request;
  cxx_request.language = BaseLanguage::Cxx;
  cxx_request.input_path = "objc3c-session-native.cc";
  cxx_request.source_text = "template<class T> concept Sized = sizeof(T) > 0; "
      "static_assert(Sized<int>); "
      "int native_exception() { try { throw 7; } catch (int x) { return x; } }";
  assert(Compile(cxx_request).success);
  auto effect_request = request;
  effect_request.language = BaseLanguage::ObjectiveCxx;
  effect_request.input_path = "objc3c-session-effects.mm";
  effect_request.objc3 = true;
  effect_request.source_text = "struct Error {}; typedef Error ErrorAlias; "
      "void asynchronous() __objc3_async; "
      "void throwing() __objc3_throws(ErrorAlias); "
      "typedef void TypedCallback(void) __objc3_throws(ErrorAlias); "
      "TypedCallback *callback; "
      "void native_noexcept() noexcept;";
  auto observed_effects = Compile(effect_request);
  assert(observed_effects.success);
  bool saw_async = false, saw_typed = false, saw_native = false;
  bool saw_callback = false, saw_callback_typedef = false;
  for (const auto &declaration : observed_effects.declarations) {
    if (declaration.qualified_name == "asynchronous") {
      saw_async = true;
      assert(declaration.effects.is_async && !declaration.effects.is_throwing);
    } else if (declaration.qualified_name == "throwing") {
      saw_typed = true;
      assert(!declaration.effects.is_async && declaration.effects.is_throwing);
      assert(declaration.effects.throws_kind == "typed");
      assert(declaration.effects.error_type_identity == "Error");
    } else if (declaration.qualified_name == "native_noexcept") {
      saw_native = true;
      assert(!declaration.effects.is_async && !declaration.effects.is_throwing);
    } else if (declaration.qualified_name == "callback" ||
               declaration.qualified_name == "TypedCallback") {
      if (declaration.qualified_name == "callback") saw_callback = true;
      else saw_callback_typedef = true;
      assert(declaration.effects.is_throwing &&
             declaration.effects.throws_kind == "typed" &&
             declaration.effects.error_type_identity == "Error");
    }
  }
  assert(saw_async && saw_typed && saw_native && saw_callback &&
         saw_callback_typedef);
  auto carrier_request = effect_request;
  carrier_request.input_path = "objc3c-session-carriers.mm";
  carrier_request.source_text =
      "typedef __objc3_optional(int) A; typedef __objc3_optional(long) B; "
      "void consume(A *); void consume(B *);";
  auto carrier_symbols = Compile(carrier_request);
  assert(carrier_symbols.success);
  std::string first_carrier_symbol;
  unsigned carrier_overloads = 0;
  for (const auto &declaration : carrier_symbols.declarations) {
    if (declaration.qualified_name != "consume") continue;
    ++carrier_overloads;
    if (first_carrier_symbol.empty())
      first_carrier_symbol = declaration.stable_id;
    else
      assert(first_carrier_symbol != declaration.stable_id);
  }
  assert(carrier_overloads == 2);
  // Both facades must drive the same native expression/defer pipeline. The
  // selected zero remains present; the zero-valued error remains an error.
  constexpr char carrier_code[] =
      "extern void record(int); "
      "typedef __objc3_optional(int) MaybeInt; "
      "typedef __objc3_result(int, long long) IntResult; "
      "int carrier_presence(void) { "
      "__objc3_defer { record(9); } "
      "MaybeInt value = __objc3_some(int, 0); "
      "MaybeInt copied = __objc3_none(int); copied = value; "
      "IntResult error = __objc3_err(int, long long, 0); "
      "return __objc3_has_value(copied) && !__objc3_is_ok(error); }";
  auto carrier_values = request;
  carrier_values.language = BaseLanguage::ObjectiveC;
  carrier_values.objc3 = true;
  carrier_values.input_path = "objc3c-session-carrier-values.m";
  carrier_values.emit = EmitKind::LLVM;
  carrier_values.output_path = "objc3c-session-carrier-values.ll";
  carrier_values.arguments = {"-std=c17", "-O1"};
  carrier_values.source_text = carrier_code;
  auto carrier_lowered = Compile(carrier_values);
  assert(carrier_lowered.success);
  auto read_ir = [](const std::filesystem::path &path) {
    std::ifstream stream(path);
    return std::string(std::istreambuf_iterator<char>(stream), {});
  };
  const auto native_carrier_ir = read_ir(carrier_values.output_path);
  assert(native_carrier_ir.find("@carrier_presence(") != std::string::npos);
  assert(native_carrier_ir.find("call void @record(i32 noundef 9)") !=
             std::string::npos ||
         native_carrier_ir.find("call void @record(i32 9)") != std::string::npos);
  const char *carrier_arguments[] = {"-std=c17", "-O1"};
  objc3c_frontend_v2_options carrier_options{};
  carrier_options.struct_size = sizeof(carrier_options);
  carrier_options.abi_version = OBJC3C_FRONTEND_V2_ABI;
  carrier_options.input_path = carrier_values.input_path.c_str();
  carrier_options.source = carrier_code;
  carrier_options.source_size = sizeof(carrier_code) - 1;
  carrier_options.output_path = "objc3c-session-carrier-c-api.ll";
  carrier_options.arguments = carrier_arguments;
  carrier_options.argument_count = 2;
  carrier_options.language = 2;
  carrier_options.emit = 2;
  carrier_options.objc3 = 1;
  objc3c_frontend_v2_result *carrier_result = nullptr;
  assert(objc3c_frontend_v2_compile(&carrier_options, &carrier_result) == 0);
  assert(carrier_result && objc3c_frontend_v2_result_success(carrier_result));
  assert(carrier_lowered.compiler_identity ==
         objc3c_frontend_v2_result_identity(carrier_result));
  assert(native_carrier_ir == read_ir(carrier_options.output_path));
  objc3c_frontend_v2_result_destroy(carrier_result);
  // A failed native query preserves a prior emitted artifact through the C ABI.
  constexpr char invalid_query[] =
      "void bad(void) { (void)__objc3_has_value(42); }";
  carrier_options.source = invalid_query;
  carrier_options.source_size = sizeof(invalid_query) - 1;
  carrier_result = nullptr;
  assert(objc3c_frontend_v2_compile(&carrier_options, &carrier_result) == 1);
  assert(carrier_result && !objc3c_frontend_v2_result_success(carrier_result));
  assert(objc3c_frontend_v2_diagnostic_count(carrier_result) != 0);
  assert(native_carrier_ir == read_ir(carrier_options.output_path));
  objc3c_frontend_v2_result_destroy(carrier_result);
  std::filesystem::remove(carrier_values.output_path);
  std::filesystem::remove(carrier_options.output_path);
  effect_request.emit = EmitKind::LLVM;
  effect_request.output_path = "objc3c-session-unlowered.ll";
  effect_request.source_text = "void asynchronous() __objc3_async {}";
  {
    std::ofstream prior(effect_request.output_path);
    prior << "preserved prior output";
  }
  auto rejected_effect = Compile(effect_request);
  assert(!rejected_effect.success && !rejected_effect.diagnostics.empty());
  {
    std::ifstream preserved(effect_request.output_path);
    assert(std::string(std::istreambuf_iterator<char>(preserved), {}) ==
           "preserved prior output");
  }
  std::filesystem::remove(effect_request.output_path);
  auto objcxx_request = request;
  objcxx_request.language = BaseLanguage::ObjectiveCxx;
  objcxx_request.input_path = "objc3c-session-native.mm";
  objcxx_request.arc = true;
  objcxx_request.objc3 = true;
  objcxx_request.emit = EmitKind::LLVM;
  objcxx_request.output_path = "objc3c-session-native.ll";
  objcxx_request.source_text = "extern \"C\" void cpp_cleanup(); "
      "struct Guard { ~Guard() { cpp_cleanup(); } }; "
      "extern \"C\" id native_identity(id value) { Guard guard; id strong = value; "
      "try { throw 7; } catch (int) {} return strong; }";
  auto native_lowered = Compile(objcxx_request);
  assert(native_lowered.success);
  {
    std::ifstream native_ir(objcxx_request.output_path);
    const std::string ir(std::istreambuf_iterator<char>(native_ir), {});
    assert(ir.find("ptr @native_identity(") != std::string::npos);
    assert(ir.find("llvm.objc.storeStrong") != std::string::npos);
    assert(ir.find("call void @cpp_cleanup(") != std::string::npos ||
           ir.find("invoke void @cpp_cleanup(") != std::string::npos);
    assert(ir.find("__CxxFrameHandler3") != std::string::npos);
  }
  std::filesystem::remove(objcxx_request.output_path);
  request.emit = EmitKind::Object;
  request.output_path = "objc3c-session-memory.obj";
  auto emitted = Compile(request);
  assert(emitted.success && std::filesystem::file_size(request.output_path) > 0);
  assert(parsed.declarations.size() == emitted.declarations.size());
  for (size_t i = 0; i < parsed.declarations.size(); ++i) {
    assert(parsed.declarations[i].stable_id == emitted.declarations[i].stable_id);
    assert(parsed.declarations[i].canonical_type_identity ==
           emitted.declarations[i].canonical_type_identity);
  }
  std::ifstream input(request.output_path, std::ios::binary);
  std::string before((std::istreambuf_iterator<char>(input)), {});
  input.close();
  request.source_text = "int broken( {";
  auto failed = Compile(request);
  assert(!failed.success && !failed.diagnostics.empty());
  std::ifstream preserved(request.output_path, std::ios::binary);
  std::string after((std::istreambuf_iterator<char>(preserved)), {});
  assert(before == after);
  preserved.close();
  std::filesystem::remove(request.output_path);

  // Invocation-relative paths and interface publication share the object
  // transaction. An invalid sidecar destination must preserve the object.
  std::filesystem::path directory;
  std::random_device nonce;
  for (unsigned attempt = 0; attempt < 32; ++attempt) {
    auto candidate = std::filesystem::temp_directory_path() /
        ("objc3c-session-" + std::to_string(nonce()) + "-" + std::to_string(nonce()));
    if (std::filesystem::create_directory(candidate)) {
      directory = std::move(candidate);
      break;
    }
  }
  assert(!directory.empty());
  std::filesystem::create_directory(directory / "include path");
  {
    std::ofstream header(directory / "include path" / "native header.h");
    header << "#define HEADER_VALUE 4\n";
    std::ofstream response(directory / "flags with spaces.rsp");
    response << "-DRESPONSE_VALUE=9 -I\"include path\" -std=c23 "
                "--target=x86_64-unknown-linux-gnu\n";
  }
  auto response_request = request;
  response_request.emit = EmitKind::Syntax;
  response_request.output_path.clear();
  response_request.working_directory = directory.string();
  response_request.arguments = {"@flags with spaces.rsp"};
  response_request.source_text = "#include \"native header.h\"\n"
      "constexpr int response_value = RESPONSE_VALUE + HEADER_VALUE;\n"
      "static_assert(response_value == 13);\n";
  assert(Compile(response_request).success);
  std::filesystem::remove(directory / "flags with spaces.rsp");
  std::filesystem::remove(directory / "include path" / "native header.h");
  std::filesystem::remove(directory / "include path");
  auto interface_request = request;
  interface_request.working_directory = directory.string();
  interface_request.output_path = "transaction.obj";
  interface_request.source_text = "int visible_api(int x) { return x + 1; }";
  interface_request.emit_semantic_interface = true;
  interface_request.module_name = "session.fixture";
  interface_request.package_identity = "session-test";
  interface_request.interface_output_path = "transaction.objc3i";
  auto transaction = Compile(interface_request);
  assert(transaction.success);
  assert(std::filesystem::is_regular_file(directory / "transaction.obj"));
  assert(std::filesystem::is_regular_file(directory / "transaction.objc3i"));
  assert(std::filesystem::is_regular_file(directory / "transaction.objc3i.json"));
  auto read_bytes = [](const std::filesystem::path &path) {
    std::ifstream stream(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(stream), {});
  };
  const auto prior_object = read_bytes(directory / "transaction.obj");
  const auto prior_interface = read_bytes(directory / "transaction.objc3i");
  std::filesystem::remove(directory / "transaction.objc3i.json");
  std::filesystem::create_directory(directory / "transaction.objc3i.json");
  interface_request.source_text = "int visible_api(int x) { return x + 2; }";
  assert(!Compile(interface_request).success);
  assert(read_bytes(directory / "transaction.obj") == prior_object);
  assert(read_bytes(directory / "transaction.objc3i") == prior_interface);
  for (const char *name : {"transaction.obj", "transaction.objc3i",
                           "transaction.objc3i.json"})
    std::filesystem::remove(directory / name);
  std::filesystem::remove(directory);

  constexpr char code[] = "int await = 0; int defer(int x) { return x + await; }";
  objc3c_frontend_v2_options options{};
  options.struct_size = sizeof(options);
  options.abi_version = OBJC3C_FRONTEND_V2_ABI;
  options.input_path = "objc3c-session-memory.m";
  options.source = code;
  options.source_size = sizeof(code) - 1;
  options.language = 2;
  options.objc3 = 1;
  objc3c_frontend_v2_result *result = nullptr;
  assert(objc3c_frontend_v2_compile(&options, &result) == 0);
  assert(result && objc3c_frontend_v2_result_success(result));
  objc3c_frontend_v2_result_destroy(result);
  options.abi_version = 99;
  assert(objc3c_frontend_v2_compile(&options, &result) == 2 && !result);
}
