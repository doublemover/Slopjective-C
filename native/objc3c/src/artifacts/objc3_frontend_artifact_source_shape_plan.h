#pragma once

#include <cstddef>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactInterfaceImplementationMethodCounts {
  std::size_t interface_class_method_symbols = 0;
  std::size_t interface_instance_method_symbols = 0;
  std::size_t implementation_class_method_symbols = 0;
  std::size_t implementation_instance_method_symbols = 0;
  std::size_t implementation_methods_with_body = 0;
};

struct Objc3FrontendArtifactSourceShapePlan {
  Objc3FrontendArtifactInterfaceImplementationMethodCounts
      interface_implementation_method_counts;
  std::vector<int> resolved_global_values;
  std::vector<
      objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure>
      post_pipeline_failures;
};

Objc3FrontendArtifactSourceShapePlan
BuildObjc3FrontendArtifactSourceShapePlan(
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result);
