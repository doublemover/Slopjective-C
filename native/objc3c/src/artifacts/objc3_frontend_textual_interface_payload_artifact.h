#pragma once

#include <filesystem>
#include <string>

#include "ast/objc3_ast.h"
#include "pipeline/results/compile_options.h"
#include "pipeline/results/pipeline_result_model.h"

namespace objc3::artifacts::frontend {

inline constexpr const char *kObjc3StandaloneTextualInterfacePayloadSchemaId =
    "objc3c-standalone-textual-interface-payload-v1";
inline constexpr const char *kObjc3StandaloneTextualInterfacePayloadKind =
    "objc3c.standalone_textual_interface_payload.v1";

[[nodiscard]] std::string BuildObjc3StandaloneTextualInterfacePayloadArtifact(
    const std::filesystem::path &input_path,
    const Objc3Program &program,
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result);

}  // namespace objc3::artifacts::frontend
