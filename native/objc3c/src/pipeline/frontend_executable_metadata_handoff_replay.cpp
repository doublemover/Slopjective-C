#include "pipeline/frontend_executable_metadata_handoff_replay.h"

#include <sstream>

#include "pipeline/frontend_executable_metadata_handoff_replay_owners.h"

std::string BuildExecutableMetadataLoweringHandoffReplayKey(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  std::ostringstream out;
  objc3_frontend_executable_metadata_handoff_replay::
      AppendExecutableMetadataLoweringHandoffReplayFields(out, surface);
  return out.str();
}

std::string BuildExecutableMetadataTypedLoweringHandoffReplayKey(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface) {
  std::ostringstream out;
  objc3_frontend_executable_metadata_handoff_replay::
      AppendExecutableMetadataTypedLoweringHandoffReplayHeader(out, surface);
  const Objc3ExecutableMetadataSourceGraph &graph = surface.source_graph;
  objc3_frontend_executable_metadata_handoff_replay::
      AppendExecutableMetadataTypedClassReplayRecords(out, graph);
  objc3_frontend_executable_metadata_handoff_replay::
      AppendExecutableMetadataTypedProtocolCategoryReplayRecords(out, graph);
  objc3_frontend_executable_metadata_handoff_replay::
      AppendExecutableMetadataTypedMemberReplayRecords(out, graph);
  objc3_frontend_executable_metadata_handoff_replay::
      AppendExecutableMetadataTypedEdgeReplayRecords(out, graph);
  return out.str();
}
