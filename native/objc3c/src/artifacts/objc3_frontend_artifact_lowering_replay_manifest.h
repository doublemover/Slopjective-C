#pragma once

#include <initializer_list>
#include <iosfwd>
#include <string>

namespace objc3::artifacts::frontend {

struct LoweringReplayManifestEntry {
  std::string manifest_key;
  std::string replay_key;
  std::string contract_id;
  bool deterministic = false;
  std::string contract_field_name = "lane_contract";
};

void WriteLoweringReplayManifestEntries(
    std::ostream &manifest,
    std::initializer_list<LoweringReplayManifestEntry> entries);

}  // namespace objc3::artifacts::frontend
