#include "artifacts/objc3_frontend_artifact_source_shape_plan.h"

#include <string>
#include <utility>

#include "ast/objc3_ast_constant_evaluator.h"

namespace {

using objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure;

void AddPostPipelineFailure(
    std::vector<Objc3FrontendArtifactPostPipelineFailure> &failures,
    const char *code,
    std::string message) {
  Objc3FrontendArtifactPostPipelineFailure failure;
  failure.present = true;
  failure.code = code == nullptr ? "" : code;
  failure.message = std::move(message);
  failures.push_back(std::move(failure));
}

}  // namespace

Objc3FrontendArtifactSourceShapePlan
BuildObjc3FrontendArtifactSourceShapePlan(
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3FrontendArtifactSourceShapePlan plan;
  const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff =
      pipeline_result.sema_type_metadata_handoff;
  for (const auto &interface_metadata :
       type_metadata_handoff.interfaces_lexicographic) {
    for (const auto &method_metadata :
         interface_metadata.methods_lexicographic) {
      if (method_metadata.is_class_method) {
        ++plan.interface_implementation_method_counts
              .interface_class_method_symbols;
      } else {
        ++plan.interface_implementation_method_counts
              .interface_instance_method_symbols;
      }
    }
  }
  for (const auto &implementation_metadata :
       type_metadata_handoff.implementations_lexicographic) {
    for (const auto &method_metadata :
         implementation_metadata.methods_lexicographic) {
      if (method_metadata.is_class_method) {
        ++plan.interface_implementation_method_counts
              .implementation_class_method_symbols;
      } else {
        ++plan.interface_implementation_method_counts
              .implementation_instance_method_symbols;
      }
      if (method_metadata.has_definition) {
        ++plan.interface_implementation_method_counts
              .implementation_methods_with_body;
      }
    }
  }
  if (!ResolveObjc3GlobalInitializerValues(program.globals,
                                           plan.resolved_global_values) ||
      plan.resolved_global_values.size() != program.globals.size()) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: global initializer failed const evaluation");
  }
  return plan;
}
