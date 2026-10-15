#include "clang_foundation/compile_session.h"

#include <filesystem>
#include <iostream>
#include <string_view>

namespace {
int RunCompiler(int argc, char **argv) {
  using namespace objc3::clang_foundation;
  if (argc < 2) {
    std::cerr
        << "usage: objc3c-clang <source> [--objc3] [--objcxx] [--emit-object "
           "path] "
           "[--emit-llvm path] [--arc] [--profile name] [--strict-concurrency] "
           "[--emit-interface path.objc3i --module name --package identity] "
           "[--target triple] [--runtime abi] [--module-search-path dir] "
           "[--import-module name fingerprint package "
           "[--private-import] [--reexport-import]] [--source-imports] "
           "[--emit-dependency-record path] "
           "[-- <clang arguments>]\n";
    return 2;
  }
  CompileRequest request;
  std::string dependency_record_path;
  request.input_path = argv[1];
  if (request.input_path.empty() || request.input_path.starts_with("--")) {
    std::cerr << "a source path must precede compiler options\n";
    return 2;
  }
  auto extension =
      std::filesystem::path(request.input_path).extension().string();
  if (extension == ".mm")
    request.language = BaseLanguage::ObjectiveCxx;
  else if (extension == ".c")
    request.language = BaseLanguage::C;
  else if (extension == ".cc" || extension == ".cpp" || extension == ".cxx")
    request.language = BaseLanguage::Cxx;
  else if (extension == ".objc3")
    request.objc3 = true;
  for (int i = 2; i < argc; ++i) {
    std::string argument = argv[i];
    auto has_values = [&](int count) {
      if (argc - i - 1 < count)
        return false;
      for (int offset = 1; offset <= count; ++offset)
        if (!*argv[i + offset] ||
            std::string_view(argv[i + offset]).starts_with("--"))
          return false;
      return true;
    };
    if (argument == "--") {
      while (++i < argc)
        request.arguments.emplace_back(argv[i]);
      break;
    }
    if (argument == "--objc3")
      request.objc3 = true;
    else if (argument == "--source-imports")
      request.source_directed_imports = true;
    else if (argument == "--emit-dependency-record" && has_values(1)) {
      dependency_record_path = argv[++i];
    } else if (argument == "--private-import" &&
               !request.module_imports.empty()) {
      request.module_imports.back().public_import = false;
    } else if (argument == "--reexport-import" &&
               !request.module_imports.empty()) {
      request.module_imports.back().reexported = true;
    } else if (argument == "--module-search-path" && has_values(1)) {
      request.interface_search_paths.emplace_back(argv[++i]);
    } else if (argument == "--import-module" && has_values(3)) {
      InterfaceModuleRequest module;
      module.canonical_name = argv[++i];
      module.locked_identity.canonical_name = module.canonical_name;
      module.locked_identity.semantic_fingerprint = argv[++i];
      module.locked_identity.package_identity = argv[++i];
      module.public_import = true;
      request.module_imports.push_back(std::move(module));
    } else if (argument == "--objcxx")
      request.language = BaseLanguage::ObjectiveCxx;
    else if (argument == "--arc")
      request.arc = true;
    else if (argument == "--strict-concurrency")
      request.strict_concurrency = true;
    else if ((argument == "--emit-object" || argument == "--emit-llvm" ||
              argument == "--target" || argument == "--runtime" ||
              argument == "--profile" || argument == "--emit-interface" ||
              argument == "--module" || argument == "--package") &&
             has_values(1)) {
      std::string value = argv[++i];
      if (argument == "--target")
        request.target_triple = value;
      else if (argument == "--runtime")
        request.objc_runtime = value;
      else if (argument == "--module")
        request.module_name = value;
      else if (argument == "--package")
        request.package_identity = value;
      else if (argument == "--emit-interface") {
        request.emit_semantic_interface = true;
        request.interface_output_path = value;
      } else if (argument == "--profile") {
        if (value == "compatibility")
          request.profile = SemanticProfile::Compatibility;
        else if (value == "strict")
          request.profile = SemanticProfile::Strict;
        else if (value == "strict-concurrency")
          request.profile = SemanticProfile::StrictConcurrency;
        else if (value == "strict-system")
          request.profile = SemanticProfile::StrictSystem;
        else {
          std::cerr << "unknown profile: " << value << '\n';
          return 2;
        }
      } else {
        request.emit =
            argument == "--emit-object" ? EmitKind::Object : EmitKind::LLVM;
        request.output_path = value;
      }
    } else {
      std::cerr << "unknown or incomplete option: " << argument << '\n';
      return 2;
    }
  }
  const bool objc = request.language == BaseLanguage::ObjectiveC ||
                    request.language == BaseLanguage::ObjectiveCxx;
  if (((request.objc3 || request.arc || !request.objc_runtime.empty()) && !objc) ||
      (!request.objc3 && request.profile != SemanticProfile::Compatibility) ||
      (request.strict_concurrency && !request.objc3) ||
      (request.emit_semantic_interface &&
       (request.module_name.empty() || request.package_identity.empty()))) {
    std::cerr << "language mode or semantic interface options are inconsistent\n";
    return 2;
  }
  if ((!request.module_imports.empty() &&
       request.interface_search_paths.empty()) ||
      (request.source_directed_imports &&
       (!request.objc3 ||
        (request.language != BaseLanguage::ObjectiveC &&
         request.language != BaseLanguage::ObjectiveCxx) ||
        request.module_imports.empty())) ||
      (!dependency_record_path.empty() && request.module_imports.empty())) {
    std::cerr << "module imports require search paths; source imports require "
                 "an ObjC3 Objective-C or Objective-C++ source and a locked "
                 "module registry; "
                 "dependency records require imports\n";
    return 2;
  }
  if (!dependency_record_path.empty()) {
    const auto record_path =
        std::filesystem::weakly_canonical(dependency_record_path);
    auto aliases_record = [&](const std::string &path) {
      if (path.empty())
        return false;
      if (std::filesystem::weakly_canonical(path) == record_path)
        return true;
      std::error_code error;
      return std::filesystem::equivalent(path, record_path, error) && !error;
    };
    if (aliases_record(request.input_path) ||
        aliases_record(request.output_path) ||
        aliases_record(request.interface_output_path) ||
        (!request.interface_output_path.empty() &&
         aliases_record(request.interface_output_path + ".json"))) {
      std::cerr << "dependency record path conflicts with a source or compiler "
                   "output\n";
      return 2;
    }
  }
  request.dependency_record_output_path = dependency_record_path;
  auto result = Compile(request);
  for (const auto &diagnostic : result.diagnostics)
    std::cerr << diagnostic.expansion.file << ':' << diagnostic.expansion.line
              << ':' << diagnostic.expansion.column << ": "
              << diagnostic.message << '\n';
  if (!result.success)
    std::cerr << result.failure << '\n';
  if (result.interface_artifact) {
    for (const auto &diagnostic : result.interface_artifact->diagnostics)
      std::cerr << diagnostic.code << ": " << diagnostic.message << '\n';
  }
  return result.success ? 0 : 1;
}
} // namespace

int main(int argc, char **argv) {
  try {
    return RunCompiler(argc, argv);
  } catch (const std::exception &error) {
    std::cerr << "compiler host failure: " << error.what() << '\n';
    return 3;
  }
}
