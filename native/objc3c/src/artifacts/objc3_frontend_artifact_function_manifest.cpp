#include "artifacts/objc3_frontend_artifact_function_manifest.h"

#include <string>
#include <unordered_set>

Objc3FrontendArtifactFunctionManifest
BuildObjc3FrontendArtifactFunctionManifest(
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3FrontendArtifactFunctionManifest manifest;
  manifest.manifest_functions.reserve(program.functions.size());
  std::unordered_set<std::string> manifest_function_names;
  for (const auto &fn : program.functions) {
    if (manifest_function_names.insert(fn.name).second) {
      manifest.manifest_functions.push_back(&fn);
    }
  }

  for (const auto &entry : pipeline_result.integration_surface.functions) {
    const FunctionInfo &signature = entry.second;
    if (signature.return_type == ValueType::Bool) {
      ++manifest.scalar_return_bool;
    } else if (signature.return_type == ValueType::Void) {
      ++manifest.scalar_return_void;
    } else {
      ++manifest.scalar_return_i32;
    }
    for (const ValueType param_type : signature.param_types) {
      if (param_type == ValueType::Bool) {
        ++manifest.scalar_param_bool;
      } else {
        ++manifest.scalar_param_i32;
      }
    }
  }

  for (const FunctionDecl *fn : manifest.manifest_functions) {
    bool has_vector_signature = false;
    if (fn->return_vector_spelling) {
      has_vector_signature = true;
      ++manifest.vector_return_signatures;
      if (fn->return_vector_base_spelling == kObjc3SimdVectorBaseBool) {
        ++manifest.vector_bool_signatures;
      } else {
        ++manifest.vector_i32_signatures;
      }
      if (fn->return_vector_lane_count == 2u) {
        ++manifest.vector_lane2_signatures;
      } else if (fn->return_vector_lane_count == 4u) {
        ++manifest.vector_lane4_signatures;
      } else if (fn->return_vector_lane_count == 8u) {
        ++manifest.vector_lane8_signatures;
      } else if (fn->return_vector_lane_count == 16u) {
        ++manifest.vector_lane16_signatures;
      }
    }
    for (const FuncParam &param : fn->params) {
      if (!param.vector_spelling) {
        continue;
      }
      has_vector_signature = true;
      ++manifest.vector_param_signatures;
      if (param.vector_base_spelling == kObjc3SimdVectorBaseBool) {
        ++manifest.vector_bool_signatures;
      } else {
        ++manifest.vector_i32_signatures;
      }
      if (param.vector_lane_count == 2u) {
        ++manifest.vector_lane2_signatures;
      } else if (param.vector_lane_count == 4u) {
        ++manifest.vector_lane4_signatures;
      } else if (param.vector_lane_count == 8u) {
        ++manifest.vector_lane8_signatures;
      } else if (param.vector_lane_count == 16u) {
        ++manifest.vector_lane16_signatures;
      }
    }
    if (has_vector_signature) {
      ++manifest.vector_signature_functions;
    }
  }

  return manifest;
}
