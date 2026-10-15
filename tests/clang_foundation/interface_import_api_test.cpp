#include "clang_foundation/frontend_api.h"
#include "clang_foundation/interface_artifact_io.h"
#include "clang_foundation/interface_codec.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <random>
#include <sstream>
#include <string>
#include <vector>

// Exercise the actual CLI entry without shell quoting, shared argv buffers,
// or subprocess-specific exit handling.
#define main objc3c_import_cli_main
#include "../../native/objc3c/src/clang_foundation/driver_main.cpp"
#undef main

namespace {
using namespace objc3::clang_foundation;

objc3c_frontend_v3_options Options(const std::string &source) {
  objc3c_frontend_v3_options options{};
  options.struct_size = sizeof(options);
  options.abi_version = OBJC3C_FRONTEND_V3_ABI;
  options.base.struct_size = sizeof(options.base);
  options.base.abi_version = OBJC3C_FRONTEND_V2_ABI;
  options.base.input_path = "api-consumer.m";
  options.base.source = source.data();
  options.base.source_size = source.size();
  options.base.target_triple = "x86_64-unknown-linux-gnu";
  options.base.language = 2;
  options.base.objc3 = 1;
  return options;
}

void Invalid(const objc3c_frontend_v3_options &options) {
  auto *result = reinterpret_cast<objc3c_frontend_v2_result *>(uintptr_t{1});
  assert(objc3c_frontend_v3_compile(&options, &result) == 2);
  assert(!result);
}

int InvokeCLI(std::vector<std::string> arguments,
              std::string *diagnostics = nullptr) {
  std::vector<char *> argv;
  for (auto &argument : arguments)
    argv.push_back(argument.data());
  argv.push_back(nullptr);
  std::ostringstream errors;
  auto *previous = std::cerr.rdbuf(errors.rdbuf());
  const int status =
      objc3c_import_cli_main(static_cast<int>(arguments.size()), argv.data());
  std::cerr.rdbuf(previous);
  if (diagnostics)
    *diagnostics = errors.str();
  return status;
}

std::string ReadBytes(const std::filesystem::path &path) {
  std::ifstream input(path, std::ios::binary);
  return std::string(std::istreambuf_iterator<char>(input), {});
}

struct TestDirectory {
  std::filesystem::path path;
  TestDirectory() {
    std::random_device random;
    for (unsigned attempt = 0; attempt < 32; ++attempt) {
      auto candidate = std::filesystem::temp_directory_path() /
                       ("objc3c-import-api-" + std::to_string(random()) + "-" +
                        std::to_string(random()));
      if (std::filesystem::create_directory(candidate)) {
        path = std::move(candidate);
        return;
      }
    }
    assert(false && "could not create an isolated import API test directory");
  }
  ~TestDirectory() {
    std::error_code ignored;
    std::filesystem::remove_all(path, ignored);
  }
};
} // namespace

int main() {
  using namespace objc3::clang_foundation;
  std::string source = "int api_plain(void) { return 7; }\n";
  const auto valid = Options(source);
  auto changed = valid;
  changed.struct_size =
      offsetof(objc3c_frontend_v3_options, source_directed_imports) - 1;
  Invalid(changed);
  changed = valid;
  changed.abi_version = 2;
  Invalid(changed);
  changed = valid;
  changed.base.struct_size = sizeof(changed.base) - 1;
  Invalid(changed);
  changed = valid;
  changed.base.abi_version = 3;
  Invalid(changed);
  changed = valid;
  changed.base.input_path = nullptr;
  Invalid(changed);
  changed = valid;
  changed.base.input_path = "";
  Invalid(changed);
  changed = valid;
  changed.base.source = nullptr;
  Invalid(changed);
  changed = valid;
  changed.base.language = 4;
  Invalid(changed);
  changed = valid;
  changed.base.emit = 3;
  Invalid(changed);
  changed = valid;
  changed.base.profile = 4;
  Invalid(changed);
  changed = valid;
  changed.base.objc3 = 2;
  Invalid(changed);
  changed = valid;
  changed.base.arc = 2;
  Invalid(changed);
  changed = valid;
  changed.base.strict_concurrency = 2;
  Invalid(changed);
  changed = valid;
  changed.base.emit_semantic_interface = 2;
  Invalid(changed);
  changed = valid;
  changed.base.language = 0;
  Invalid(changed);
  changed = valid;
  changed.base.objc3 = 0;
  changed.base.profile = 1;
  Invalid(changed);
  changed = valid;
  changed.base.objc3 = 0;
  changed.base.strict_concurrency = 1;
  Invalid(changed);
  changed = valid;
  changed.base.emit = 1;
  Invalid(changed);
  changed = valid;
  changed.base.emit = 2;
  changed.base.output_path = "";
  Invalid(changed);
  changed = valid;
  changed.base.emit_semantic_interface = 1;
  Invalid(changed);
  changed = valid;
  changed.base.argument_count = 1;
  Invalid(changed);
  const char *null_arguments[] = {nullptr};
  changed = valid;
  changed.base.arguments = null_arguments;
  changed.base.argument_count = 1;
  Invalid(changed);
  changed = valid;
  changed.interface_search_path_count = 1;
  Invalid(changed);
  const char *null_paths[] = {nullptr};
  changed = valid;
  changed.interface_search_paths = null_paths;
  changed.interface_search_path_count = 1;
  Invalid(changed);
  const char *empty_paths[] = {""};
  changed.interface_search_paths = empty_paths;
  Invalid(changed);
  changed = valid;
  changed.import_count = 1;
  Invalid(changed);
  changed = valid;
  changed.source_directed_imports = 2;
  Invalid(changed);
  changed = valid;
  changed.source_directed_imports = 1;
  Invalid(changed);
  objc3c_frontend_v2_result *result = nullptr;
  assert(objc3c_frontend_v3_compile(nullptr, &result) == 2 && !result);
  assert(objc3c_frontend_v3_compile(&valid, nullptr) == 2);
  assert(!objc3c_frontend_v3_result_dependency_record(nullptr));

  // The formerly complete V3 prefix omits the optional source-import flag.
  // Poisoning that omitted field proves it is neither validated nor enabled.
  changed = valid;
  changed.struct_size =
      offsetof(objc3c_frontend_v3_options, source_directed_imports);
  changed.source_directed_imports = 2;
  assert(objc3c_frontend_v3_compile(&changed, &result) == 0);
  assert(result && !objc3c_frontend_v3_result_dependency_record(result));
  objc3c_frontend_v2_result_destroy(result);
  changed = valid;
  changed.struct_size = sizeof(changed) + 32;
  assert(objc3c_frontend_v3_compile(&changed, &result) == 0);
  objc3c_frontend_v2_result_destroy(result);
  auto empty_source = valid.base;
  empty_source.source = "";
  empty_source.source_size = 0;
  assert(objc3c_frontend_v2_compile(&empty_source, &result) == 0);
  objc3c_frontend_v2_result_destroy(result);

  TestDirectory directory;
  CompileRequest producer;
  producer.input_path = (directory.path / "absent-provider.m").string();
  producer.source_text = "typedef struct ApiValue { int field; } ApiValue;\n"
                         "int api_value(ApiValue value);\n";
  producer.language = BaseLanguage::ObjectiveC;
  producer.objc3 = true;
  producer.target_triple = "x86_64-unknown-linux-gnu";
  producer.emit_semantic_interface = true;
  producer.module_name = "fixture.Api";
  producer.package_identity = "api-provider-package";
  auto built = Compile(producer);
  assert(built.success && built.interface_artifact &&
         built.interface_artifact->success);
  assert(
      WriteSemanticInterfaceArtifact(*built.interface_artifact,
                                     (directory.path / "Api.objc3i").string())
          .success);
  const auto &identity = built.interface_artifact->identity;
  const std::string search_path = directory.path.string();
  const char *search_paths[] = {search_path.c_str()};
  objc3c_frontend_v3_import imported{};
  imported.struct_size = sizeof(imported);
  imported.canonical_name = identity.canonical_name.c_str();
  imported.semantic_fingerprint = identity.semantic_fingerprint.c_str();
  imported.package_identity = identity.package_identity.c_str();
  imported.public_import = 1;
  source =
      "int use_api(void) { ApiValue value = {5}; return api_value(value); }\n";
  auto options = Options(source);
  options.interface_search_paths = search_paths;
  options.interface_search_path_count = 1;
  options.imports = &imported;
  options.import_count = 1;
  changed = options;
  changed.interface_search_path_count = 0;
  Invalid(changed);
  changed = options;
  changed.imports = nullptr;
  Invalid(changed);
  auto saved_import = imported;
  imported.struct_size = sizeof(imported) - 1;
  Invalid(options);
  imported = saved_import;
  imported.canonical_name = nullptr;
  Invalid(options);
  imported = saved_import;
  imported.canonical_name = "";
  Invalid(options);
  imported = saved_import;
  imported.semantic_fingerprint = nullptr;
  Invalid(options);
  imported = saved_import;
  imported.semantic_fingerprint = "";
  Invalid(options);
  imported = saved_import;
  imported.package_identity = nullptr;
  Invalid(options);
  imported = saved_import;
  imported.package_identity = "";
  Invalid(options);
  imported = saved_import;
  imported.public_import = 2;
  Invalid(options);
  imported = saved_import;
  imported.reexported = 2;
  Invalid(options);
  imported = saved_import;
  changed = options;
  changed.source_directed_imports = 1;
  changed.base.objc3 = 0;
  Invalid(changed);
  changed = options;
  changed.source_directed_imports = 1;
  changed.base.language = 1;
  changed.base.objc3 = 0;
  Invalid(changed);
  assert(objc3c_frontend_v3_compile(&options, &result) == 0);
  assert(result && objc3c_frontend_v2_result_success(result));
  const char *borrowed_record =
      objc3c_frontend_v3_result_dependency_record(result);
  assert(borrowed_record);
  const std::string record_snapshot = borrowed_record;
  assert(record_snapshot.find(identity.semantic_fingerprint) !=
         std::string::npos);
  assert(record_snapshot.find(identity.package_identity) != std::string::npos);
  const char *borrowed_identity = objc3c_frontend_v2_result_identity(result);
  const std::string identity_snapshot = borrowed_identity;
  changed = options;
  changed.struct_size = offsetof(objc3c_frontend_v3_options, source_directed_imports);
  changed.source_directed_imports = 1;
  objc3c_frontend_v2_result *legacy_result = nullptr;
  assert(objc3c_frontend_v3_compile(&changed, &legacy_result) == 0);
  assert(objc3c_frontend_v3_result_dependency_record(legacy_result));
  objc3c_frontend_v2_result_destroy(legacy_result);

  // Independent results retain their owned strings after input mutation and
  // another compile. Private direct imports remain usable by this consumer.
  source.assign("changed input buffers");
  imported.public_import = 0;
  const std::string private_source = "int private_use(void) { ApiValue value = "
                                     "{3}; return api_value(value); }\n";
  options.base.source = private_source.data();
  options.base.source_size = private_source.size();
  options.base.emit_semantic_interface = 1;
  options.base.module_name = "fixture.Consumer";
  options.base.package_identity = "api-consumer-package";
  objc3c_frontend_v2_result *private_result = nullptr;
  assert(objc3c_frontend_v3_compile(&options, &private_result) == 0);
  auto private_sidecar = DecodeSemanticInterfaceSidecar(
      objc3c_frontend_v2_interface_sidecar(private_result),
      objc3c_frontend_v2_interface_source(private_result), {});
  assert(private_sidecar && private_sidecar->dependencies.size() == 1);
  assert(!private_sidecar->dependencies[0].public_import);
  assert(!private_sidecar->dependencies[0].reexported);
  objc3c_frontend_v2_result_destroy(private_result);
  assert(record_snapshot == borrowed_record &&
         identity_snapshot == borrowed_identity);
  objc3c_frontend_v2_result_destroy(result);

  imported.public_import = 1;
  imported.reexported = 1;
  const std::string directed_source = "@import fixture.Api;\n" + private_source;
  options.base.source = directed_source.data();
  options.base.source_size = directed_source.size();
  options.source_directed_imports = 1;
  assert(objc3c_frontend_v3_compile(&options, &result) == 0);
  auto reexported_sidecar = DecodeSemanticInterfaceSidecar(
      objc3c_frontend_v2_interface_sidecar(result),
      objc3c_frontend_v2_interface_source(result), {});
  assert(reexported_sidecar && reexported_sidecar->dependencies.size() == 1);
  assert(reexported_sidecar->dependencies[0].public_import);
  assert(reexported_sidecar->dependencies[0].reexported);
  objc3c_frontend_v2_result_destroy(result);
  options.base.emit_semantic_interface = 0;
  options.base.source = private_source.data();
  options.base.source_size = private_source.size();
  assert(objc3c_frontend_v3_compile(&options, &result) == 1);
  assert(result && !objc3c_frontend_v3_result_dependency_record(result));
  objc3c_frontend_v2_result_destroy(result);
  // Incomplete lock values must never absorb a later facade option, and
  // rejected requests must not reach file-backed compilation.
  for (const auto &arguments : std::vector<std::vector<std::string>>{
           {"objc3c-clang"},
           {"objc3c-clang", ""},
           {"objc3c-clang", "--objc3"},
           {"objc3c-clang", "absent.m", "--private-import"},
           {"objc3c-clang", "absent.m", "--reexport-import"},
           {"objc3c-clang", "absent.m", "--module-search-path", ""},
           {"objc3c-clang", "absent.m", "--emit-dependency-record", ""},
           {"objc3c-clang", "absent.m", "--import-module", "fixture.Api",
            "fingerprint"},
           {"objc3c-clang", "absent.m", "--import-module", "fixture.Api",
            "--source-imports", "pkg"},
           {"objc3c-clang", "absent.m", "--import-module", "fixture.Api",
            "fingerprint", "--objc3"},
           {"objc3c-clang", "absent.m", "--import-module", "fixture.Api",
            "fingerprint", "pkg"},
           {"objc3c-clang", "absent.m", "--source-imports"},
           {"objc3c-clang", "absent.c", "--objc3"},
           {"objc3c-clang", "absent.m", "--profile", "strict"},
           {"objc3c-clang", "absent.m", "--emit-interface", "interface.objc3i"},
           {"objc3c-clang", "absent.m", "--emit-dependency-record",
            "record.json"}})
    assert(InvokeCLI(arguments) == 2);

  const auto consumer_path = directory.path / "cli consumer.m";
  {
    std::ofstream output(consumer_path, std::ios::binary);
    output << directed_source;
  }
  const auto source_before_alias = ReadBytes(consumer_path);
  assert(source_before_alias == directed_source);
  const auto dependency_path = directory.path / "dependency record.json";
  std::vector<std::string> cli = {"objc3c-clang",
                                  consumer_path.string(),
                                  "--objc3",
                                  "--target",
                                  "x86_64-unknown-linux-gnu",
                                  "--module-search-path",
                                  search_path,
                                  "--import-module",
                                  identity.canonical_name,
                                  identity.semantic_fingerprint,
                                  identity.package_identity,
                                  "--source-imports",
                                  "--emit-dependency-record"};
  auto alias = cli;
  alias.push_back((directory.path / "." / consumer_path.filename()).string());
  std::string errors;
  assert(InvokeCLI(alias, &errors) == 2);
  assert(errors.find("path conflicts") != std::string::npos);
  assert(ReadBytes(consumer_path) == source_before_alias);
  const auto object_path = directory.path / "preserved.obj";
  {
    std::ofstream output(object_path);
    output << "prior object";
  }
  alias = cli;
  alias.push_back(object_path.string());
  alias.insert(alias.end(), {"--emit-object", object_path.string()});
  assert(InvokeCLI(alias) == 2 && ReadBytes(object_path) == "prior object");
  cli.push_back(dependency_path.string());
  {
    std::ofstream output(dependency_path);
    output << "prior record";
  }
  assert(InvokeCLI(cli) == 0);
  const auto successful_record = ReadBytes(dependency_path);
  assert(successful_record.find(identity.semantic_fingerprint) !=
         std::string::npos);
  {
    std::ofstream output(consumer_path, std::ios::binary);
    output << "int broken( {\n";
  }
  assert(InvokeCLI(cli) == 1);
  assert(ReadBytes(dependency_path) == successful_record);
  {
    std::ofstream output(consumer_path, std::ios::binary);
    output << directed_source;
  }
  const auto blocked_record = directory.path / "blocked record";
  std::filesystem::create_directory(blocked_record);
  {
    std::ofstream output(blocked_record / "sentinel");
    output << "preserved";
  }
  cli.back() = blocked_record.string();
  assert(InvokeCLI(cli) == 1);
  assert(ReadBytes(blocked_record / "sentinel") == "preserved");

  const auto provider_source_path = directory.path / "Api.objc3i";
  const auto provider_sidecar_path = directory.path / "Api.objc3i.json";
  for (const auto &provider_path : {provider_source_path, provider_sidecar_path}) {
    const auto provider_bytes = ReadBytes(provider_path);
    auto overwrite_provider = cli;
    overwrite_provider.back() = provider_path.string();
    assert(InvokeCLI(overwrite_provider) == 1);
    assert(ReadBytes(provider_path) == provider_bytes);
  }

  // A record failure must preserve the object and interface pair too.
  const auto consumer_interface = directory.path / "Consumer.objc3i";
  const auto consumer_sidecar = directory.path / "Consumer.objc3i.json";
  auto products_cli = cli;
  products_cli.insert(products_cli.end(), {
      "--emit-object", object_path.string(),
      "--emit-interface", consumer_interface.string(),
      "--module", "Consumer", "--package", "consumer-package"});
  auto write_prior = [&](const std::filesystem::path &path, const char *bytes) {
    std::ofstream output(path, std::ios::binary);
    output << bytes;
    output.close();
    assert(output);
  };
  write_prior(object_path, "prior object");
  write_prior(consumer_interface, "prior interface");
  write_prior(consumer_sidecar, "prior sidecar");
  assert(InvokeCLI(products_cli) == 1);
  assert(ReadBytes(object_path) == "prior object");
  assert(ReadBytes(consumer_interface) == "prior interface");
  assert(ReadBytes(consumer_sidecar) == "prior sidecar");
  assert(ReadBytes(blocked_record / "sentinel") == "preserved");

  // A late interface failure restores the already published dependency record.
  products_cli[cli.size() - 1] = dependency_path.string();
  std::filesystem::remove(consumer_sidecar);
  std::filesystem::create_directory(consumer_sidecar);
  assert(InvokeCLI(products_cli) == 1);
  assert(ReadBytes(dependency_path) == successful_record);
  assert(ReadBytes(object_path) == "prior object");
  assert(ReadBytes(consumer_interface) == "prior interface");
  assert(std::filesystem::is_directory(consumer_sidecar));
  std::filesystem::remove(consumer_sidecar);
  assert(InvokeCLI(products_cli) == 0);
  assert(ReadBytes(object_path) != "prior object");
  assert(ReadBytes(consumer_interface) != "prior interface");
  assert(ReadBytes(dependency_path).find(identity.semantic_fingerprint) != std::string::npos);

  // An unused registry emits no record; that rejection must preserve products.
  write_prior(consumer_path, "int local(void) { return 7; }\n");
  const auto prior_object = ReadBytes(object_path);
  const auto prior_interface = ReadBytes(consumer_interface);
  const auto prior_sidecar = ReadBytes(consumer_sidecar);
  const auto prior_record = ReadBytes(dependency_path);
  assert(InvokeCLI(products_cli) == 1);
  assert(ReadBytes(object_path) == prior_object);
  assert(ReadBytes(consumer_interface) == prior_interface);
  assert(ReadBytes(consumer_sidecar) == prior_sidecar);
  assert(ReadBytes(dependency_path) == prior_record);
  for (const auto &entry : std::filesystem::directory_iterator(directory.path))
    assert(entry.path().filename().string().find(".objc3i-txn-") ==
           std::string::npos);
}
