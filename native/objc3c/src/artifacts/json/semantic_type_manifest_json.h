#pragma once

#include <iosfwd>

struct Objc3SemanticTypeMetadataHandoff;

namespace objc3::artifacts::json {

void WriteSemanticTypeMetadataHandoffManifestObject(
    std::ostream &out, const Objc3SemanticTypeMetadataHandoff &handoff);

}  // namespace objc3::artifacts::json
