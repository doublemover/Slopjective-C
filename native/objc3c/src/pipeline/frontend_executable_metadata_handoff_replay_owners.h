#pragma once

#include <sstream>

#include "pipeline/frontend_executable_metadata_handoff_replay.h"

namespace objc3_frontend_executable_metadata_handoff_replay {

void AppendExecutableMetadataLoweringHandoffReplayFields(
    std::ostringstream &out,
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface);

void AppendExecutableMetadataTypedLoweringHandoffReplayHeader(
    std::ostringstream &out,
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface);

void AppendExecutableMetadataTypedClassReplayRecords(
    std::ostringstream &out,
    const Objc3ExecutableMetadataSourceGraph &graph);

void AppendExecutableMetadataTypedProtocolCategoryReplayRecords(
    std::ostringstream &out,
    const Objc3ExecutableMetadataSourceGraph &graph);

void AppendExecutableMetadataTypedMemberReplayRecords(
    std::ostringstream &out,
    const Objc3ExecutableMetadataSourceGraph &graph);

void AppendExecutableMetadataTypedEdgeReplayRecords(
    std::ostringstream &out,
    const Objc3ExecutableMetadataSourceGraph &graph);

}  // namespace objc3_frontend_executable_metadata_handoff_replay
