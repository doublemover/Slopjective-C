#pragma once

#include <cstddef>
#include <vector>

#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactFunctionManifest {
  std::vector<const FunctionDecl *> manifest_functions;
  std::size_t scalar_return_i32 = 0;
  std::size_t scalar_return_bool = 0;
  std::size_t scalar_return_void = 0;
  std::size_t scalar_param_i32 = 0;
  std::size_t scalar_param_bool = 0;
  std::size_t vector_signature_functions = 0;
  std::size_t vector_return_signatures = 0;
  std::size_t vector_param_signatures = 0;
  std::size_t vector_i32_signatures = 0;
  std::size_t vector_bool_signatures = 0;
  std::size_t vector_lane2_signatures = 0;
  std::size_t vector_lane4_signatures = 0;
  std::size_t vector_lane8_signatures = 0;
  std::size_t vector_lane16_signatures = 0;
};

Objc3FrontendArtifactFunctionManifest
BuildObjc3FrontendArtifactFunctionManifest(
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result);
