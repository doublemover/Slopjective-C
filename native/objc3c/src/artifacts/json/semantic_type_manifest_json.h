#pragma once

#include <iosfwd>

#include "sema/objc3_semantic_passes.h"

namespace objc3::artifacts::json {

void WriteSemanticTypeMetadataHandoffManifestObject(
    std::ostream &out, const Objc3SemanticTypeMetadataHandoff &handoff);

}  // namespace objc3::artifacts::json
