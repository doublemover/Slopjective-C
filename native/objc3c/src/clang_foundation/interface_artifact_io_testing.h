#pragma once

#include <string>

#include "clang_foundation/interface_artifact_io.h"

namespace objc3::clang_foundation::testing {

// Deterministic fault injection for exercising pair rollback after the
// already-staged object has been published. 0 fails before .objc3i publish;
// 1 fails before .objc3i.json publish.
[[nodiscard]] InterfaceArtifactWriteResult
WriteSemanticInterfaceArtifactWithObjectFailingBeforePairPublish(
    const InterfaceEmissionArtifact &artifact,
    const std::string &explicit_source_path,
    const std::string &staged_object_path,
    const std::string &final_object_path,
    unsigned pair_publish_index);

}  // namespace objc3::clang_foundation::testing
