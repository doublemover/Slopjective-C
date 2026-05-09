#include "lower/contracts/lowering_artifact_publication.h"

#include <sstream>

namespace {

std::string BuildRelativeArtifactPath(const std::string &emit_prefix,
                                      const std::string &suffix) {
  return emit_prefix + suffix;
}

}  // namespace

Objc3LoweringArtifactPlan Objc3BuildLoweringArtifactPlan(
    const std::string &output_directory, const std::string &emit_prefix,
    bool emit_ir, bool emit_object, bool emit_manifest,
    bool emit_runtime_metadata) {
  Objc3LoweringArtifactPlan plan;
  plan.output_directory = output_directory;
  plan.emit_prefix = emit_prefix.empty() ? "module" : emit_prefix;
  plan.emit_ir = emit_ir;
  plan.emit_object = emit_object;
  plan.emit_manifest = emit_manifest;
  plan.emit_runtime_metadata = emit_runtime_metadata;
  plan.ir_relative_path = BuildRelativeArtifactPath(plan.emit_prefix, ".ll");
  plan.object_relative_path =
      BuildRelativeArtifactPath(plan.emit_prefix, ".obj");
  plan.manifest_relative_path =
      BuildRelativeArtifactPath(plan.emit_prefix, ".manifest.json");
  plan.runtime_metadata_relative_path =
      BuildRelativeArtifactPath(plan.emit_prefix, ".runtime-metadata.bin");
  return plan;
}

std::string Objc3LoweringArtifactPlanReplayKey(
    const Objc3LoweringArtifactPlan &plan) {
  std::ostringstream out;
  out << "out=" << plan.output_directory << ";prefix=" << plan.emit_prefix
      << ";ir=" << (plan.emit_ir ? "true" : "false")
      << ";object=" << (plan.emit_object ? "true" : "false")
      << ";manifest=" << (plan.emit_manifest ? "true" : "false")
      << ";runtime_metadata="
      << (plan.emit_runtime_metadata ? "true" : "false")
      << ";ir_path=" << plan.ir_relative_path
      << ";object_path=" << plan.object_relative_path
      << ";manifest_path=" << plan.manifest_relative_path
      << ";runtime_metadata_path=" << plan.runtime_metadata_relative_path;
  return out.str();
}
