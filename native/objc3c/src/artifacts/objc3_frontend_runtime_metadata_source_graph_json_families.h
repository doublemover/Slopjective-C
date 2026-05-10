#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

namespace objc3::artifacts::frontend::runtime_metadata_source_graph_json {

void WriteSourceGraphIdentityJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph);

void WriteSourceGraphNodeCountJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph);

void WriteSourceGraphClosureJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph);

void WriteSourceGraphClassMetaclassNodeJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph);

void WriteSourceGraphProtocolCategoryNodeJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph);

void WriteSourceGraphMemberNodeJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph);

void WriteSourceGraphOwnerEdgeReadinessJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph);

}  // namespace objc3::artifacts::frontend::runtime_metadata_source_graph_json
