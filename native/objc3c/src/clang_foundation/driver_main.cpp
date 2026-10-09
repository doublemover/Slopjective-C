#include "clang_foundation/compile_session.h"

#include <filesystem>
#include <iostream>

int main(int argc, char **argv) {
  using namespace objc3::clang_foundation;
  CompileRequest request;
  if (argc < 2) {
    std::cerr << "usage: objc3c-clang <source> [--objc3] [--objcxx] [--emit-object path] "
                 "[--emit-llvm path] [--arc] [--profile name] [--strict-concurrency] "
                 "[--emit-interface path.objc3i --module name --package identity] "
                 "[--target triple] [--runtime abi] [-- <clang arguments>]\n";
    return 2;
  }
  request.input_path = argv[1];
  auto extension = std::filesystem::path(request.input_path).extension().string();
  if (extension == ".mm") request.language = BaseLanguage::ObjectiveCxx;
  else if (extension == ".c") request.language = BaseLanguage::C;
  else if (extension == ".cc" || extension == ".cpp" || extension == ".cxx") request.language = BaseLanguage::Cxx;
  else if (extension == ".objc3") request.objc3 = true;
  for (int i = 2; i < argc; ++i) {
    std::string argument = argv[i];
    if (argument == "--") {
      while (++i < argc) request.arguments.emplace_back(argv[i]);
      break;
    }
    if (argument == "--objc3") request.objc3 = true;
    else if (argument == "--objcxx") request.language = BaseLanguage::ObjectiveCxx;
    else if (argument == "--arc") request.arc = true;
    else if (argument == "--strict-concurrency") request.strict_concurrency = true;
    else if ((argument == "--emit-object" || argument == "--emit-llvm" ||
              argument == "--target" || argument == "--runtime" || argument == "--profile" ||
              argument == "--emit-interface" || argument == "--module" || argument == "--package") && i + 1 < argc) {
      std::string value = argv[++i];
      if (argument == "--target") request.target_triple = value;
      else if (argument == "--runtime") request.objc_runtime = value;
      else if (argument == "--module") request.module_name = value;
      else if (argument == "--package") request.package_identity = value;
      else if (argument == "--emit-interface") {
        request.emit_semantic_interface = true;
        request.interface_output_path = value;
      } else if (argument == "--profile") {
        if (value == "compatibility") request.profile = SemanticProfile::Compatibility;
        else if (value == "strict") request.profile = SemanticProfile::Strict;
        else if (value == "strict-concurrency") request.profile = SemanticProfile::StrictConcurrency;
        else if (value == "strict-system") request.profile = SemanticProfile::StrictSystem;
        else { std::cerr << "unknown profile: " << value << '\n'; return 2; }
      }
      else {
        request.emit = argument == "--emit-object" ? EmitKind::Object : EmitKind::LLVM;
        request.output_path = value;
      }
    } else {
      std::cerr << "unknown or incomplete option: " << argument << '\n';
      return 2;
    }
  }
  try {
    auto result = Compile(request);
    for (const auto &diagnostic : result.diagnostics)
      std::cerr << diagnostic.expansion.file << ':' << diagnostic.expansion.line << ':'
                << diagnostic.expansion.column << ": " << diagnostic.message << '\n';
    if (!result.success) std::cerr << result.failure << '\n';
    if (result.interface_artifact) {
      for (const auto &diagnostic : result.interface_artifact->diagnostics)
        std::cerr << diagnostic.code << ": " << diagnostic.message << '\n';
    }
    return result.success ? 0 : 1;
  } catch (const std::exception &error) {
    std::cerr << "compiler host failure: " << error.what() << '\n';
    return 3;
  }
}
