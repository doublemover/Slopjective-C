#include "artifacts/interop/interop_bridge_artifacts.h"

#include <algorithm>
#include <sstream>
#include <utility>
#include <vector>

namespace objc3::artifacts::interop {
namespace {

std::string BuildInteropBridgeCType(ValueType type, unsigned pointer_depth,
                                    bool object_pointer_type_spelling) {
  std::string base;
  switch (type) {
    case ValueType::I32:
      base = "int32_t";
      break;
    case ValueType::Bool:
      base = "bool";
      break;
    case ValueType::Void:
      base = "void";
      break;
    case ValueType::ObjCId:
    case ValueType::ObjCClass:
    case ValueType::ObjCSel:
    case ValueType::ObjCProtocol:
    case ValueType::ObjCInstancetype:
    case ValueType::ObjCObjectPtr:
    case ValueType::Function:
    default:
      base = "void";
      pointer_depth = std::max(pointer_depth, 1u);
      break;
  }
  if (object_pointer_type_spelling) {
    base = "void";
    pointer_depth = std::max(pointer_depth, 1u);
  }
  for (unsigned i = 0; i < pointer_depth; ++i) {
    base += "*";
  }
  return base;
}

std::string BuildInteropBridgeReturnType(const FunctionDecl &function) {
  return BuildInteropBridgeCType(function.return_type,
                                function.has_return_pointer_declarator
                                    ? function.return_pointer_declarator_depth
                                    : 0u,
                                function.return_object_pointer_type_spelling);
}

std::string BuildInteropBridgeParameterType(const FuncParam &param) {
  return BuildInteropBridgeCType(param.type,
                                param.has_pointer_declarator
                                    ? param.pointer_declarator_depth
                                    : 0u,
                                param.object_pointer_type_spelling);
}

std::vector<const FunctionDecl *> BuildSortedForeignFunctions(
    const Objc3Program &program) {
  std::vector<const FunctionDecl *> foreign_functions;
  for (const auto &function : program.functions) {
    if (function.objc_foreign_declared) {
      foreign_functions.push_back(&function);
    }
  }
  std::sort(foreign_functions.begin(), foreign_functions.end(),
            [](const FunctionDecl *lhs, const FunctionDecl *rhs) {
              return lhs->name < rhs->name;
            });
  return foreign_functions;
}

Objc3InteropBridgeArtifactInputs BuildInteropBridgeInputs(
    const Objc3Program &program,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary &bridge_summary) {
  Objc3InteropBridgeArtifactInputs inputs;
  inputs.contract_id = bridge_summary.contract_id;
  inputs.module_name = summary.module_name;
  inputs.header_artifact_relative_path =
      bridge_summary.header_artifact_relative_path;
  inputs.module_artifact_relative_path =
      bridge_summary.module_artifact_relative_path;
  inputs.bridge_artifact_relative_path =
      bridge_summary.bridge_artifact_relative_path;
  inputs.runtime_generation_ready = bridge_summary.runtime_generation_ready;
  inputs.cross_module_packaging_ready =
      bridge_summary.cross_module_packaging_ready;
  inputs.deterministic = bridge_summary.deterministic;
  inputs.replay_key = bridge_summary.replay_key;
  inputs.local_import_module_names_lexicographic =
      bridge_summary.local_import_module_names_lexicographic;

  const std::vector<const FunctionDecl *> foreign_functions =
      BuildSortedForeignFunctions(program);
  inputs.foreign_callables.reserve(foreign_functions.size());
  for (const FunctionDecl *function : foreign_functions) {
    Objc3InteropBridgeCallableArtifact callable;
    callable.name = function->name;
    callable.return_type = BuildInteropBridgeReturnType(*function);
    callable.objc_import_module_name = function->objc_import_module_name;
    callable.objc_import_module_declared =
        function->objc_import_module_declared;
    callable.objc_header_name = function->objc_header_name;
    callable.objc_header_name_declared = function->objc_header_name_declared;
    callable.objc_cxx_name = function->objc_cxx_name;
    callable.objc_cxx_name_declared = function->objc_cxx_name_declared;
    callable.objc_swift_name = function->objc_swift_name;
    callable.objc_swift_name_declared = function->objc_swift_name_declared;
    callable.objc_export_header_name = function->objc_export_header_name;
    callable.objc_export_header_declared =
        function->objc_export_header_declared;
    callable.objc_abi_alignment_bytes = function->objc_abi_alignment_bytes;
    callable.objc_abi_align_declared = function->objc_abi_align_declared;
    callable.objc_foreign_type_name = function->objc_foreign_type_name;
    callable.objc_foreign_type_declared =
        function->objc_foreign_type_declared;
    callable.objc_mixed_image_name = function->objc_mixed_image_name;
    callable.objc_mixed_image_declared = function->objc_mixed_image_declared;
    callable.objc_package_entry_name = function->objc_package_entry_name;
    callable.objc_package_entry_declared =
        function->objc_package_entry_declared;
    callable.parameters.reserve(function->params.size());
    for (const FuncParam &param : function->params) {
      callable.parameters.push_back({
          BuildInteropBridgeParameterType(param),
          param.name,
      });
    }
    inputs.foreign_callables.push_back(std::move(callable));
  }
  return inputs;
}

}  // namespace

std::string BuildInteropBridgeHeaderArtifactText(
    const Objc3InteropBridgeArtifactInputs &inputs) {
  std::ostringstream out;
  out << "/* Part 11 bridge header: generated by objc3c-native. */\n"
      << "#pragma once\n"
      << "#include <stdbool.h>\n"
      << "#include <stdint.h>\n\n"
      << "/* module: " << inputs.module_name << " */\n"
      << "/* replay: " << inputs.replay_key << " */\n";
  for (const auto &module_name :
       inputs.local_import_module_names_lexicographic) {
    out << "/* objc_import_module: " << module_name << " */\n";
  }
  if (!inputs.local_import_module_names_lexicographic.empty()) {
    out << "\n";
  }
  for (const Objc3InteropBridgeCallableArtifact &callable :
       inputs.foreign_callables) {
    out << "/* objc_foreign";
    if (callable.objc_header_name_declared &&
        !callable.objc_header_name.empty()) {
      out << " header_name=" << callable.objc_header_name;
    }
    if (callable.objc_cxx_name_declared && !callable.objc_cxx_name.empty()) {
      out << " cxx_name=" << callable.objc_cxx_name;
    }
    if (callable.objc_swift_name_declared &&
        !callable.objc_swift_name.empty()) {
      out << " swift_name=" << callable.objc_swift_name;
    }
    if (callable.objc_import_module_declared &&
        !callable.objc_import_module_name.empty()) {
      out << " import_module=" << callable.objc_import_module_name;
    }
    if (callable.objc_export_header_declared &&
        !callable.objc_export_header_name.empty()) {
      out << " export_header=" << callable.objc_export_header_name;
    }
    if (callable.objc_abi_align_declared) {
      out << " abi_align=" << callable.objc_abi_alignment_bytes;
    }
    if (callable.objc_foreign_type_declared &&
        !callable.objc_foreign_type_name.empty()) {
      out << " foreign_type=" << callable.objc_foreign_type_name;
    }
    if (callable.objc_mixed_image_declared &&
        !callable.objc_mixed_image_name.empty()) {
      out << " mixed_image=" << callable.objc_mixed_image_name;
    }
    if (callable.objc_package_entry_declared &&
        !callable.objc_package_entry_name.empty()) {
      out << " package_entry=" << callable.objc_package_entry_name;
    }
    out << " */\n";
    out << callable.return_type << " " << callable.name << "(";
    for (std::size_t index = 0; index < callable.parameters.size(); ++index) {
      const auto &param = callable.parameters[index];
      out << param.type << " " << param.name;
      if (index + 1 != callable.parameters.size()) {
        out << ", ";
      }
    }
    if (callable.parameters.empty()) {
      out << "void";
    }
    out << ");\n\n";
  }
  return out.str();
}

std::string BuildInteropBridgeHeaderArtifactText(
    const Objc3Program &program,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary &bridge_summary) {
  return BuildInteropBridgeHeaderArtifactText(
      BuildInteropBridgeInputs(program, summary, bridge_summary));
}

std::string BuildInteropBridgeModuleArtifactText(
    const Objc3InteropBridgeArtifactInputs &inputs) {
  std::ostringstream out;
  out << "module " << inputs.module_name << "_objc3_interop_bridge {\n"
      << "  header \"" << inputs.header_artifact_relative_path << "\"\n"
      << "  export *\n"
      << "}\n";
  return out.str();
}

std::string BuildInteropBridgeModuleArtifactText(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary &bridge_summary) {
  Objc3InteropBridgeArtifactInputs inputs;
  inputs.module_name = summary.module_name;
  inputs.header_artifact_relative_path =
      bridge_summary.header_artifact_relative_path;
  return BuildInteropBridgeModuleArtifactText(inputs);
}

std::string BuildInteropBridgeArtifactJson(
    const Objc3Program &program,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary &bridge_summary) {
  return BuildInteropBridgeArtifactJson(
      BuildInteropBridgeInputs(program, summary, bridge_summary));
}

}  // namespace objc3::artifacts::interop
