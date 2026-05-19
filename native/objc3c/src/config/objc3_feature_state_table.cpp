#include "config/objc3_feature_state_table.h"

#include <array>
#include <cstddef>

#include "config/objc3_feature_state_core_data.h"
#include "config/objc3_rejected_feature_state_data.h"

namespace objc3c::config {
namespace {

inline constexpr std::size_t kCanonicalFeatureStateEntryCount =
    kImplementedFeatureStateDataCount + kRejectedFeatureStateDataCount +
    kReservedFeatureStateDataCount;

void AppendFeatureStates(
    std::array<LanguageFeatureState, kCanonicalFeatureStateEntryCount> &entries,
    std::size_t &next, std::span<const LanguageFeatureState> source) {
  for (const LanguageFeatureState &entry : source) {
    entries[next++] = entry;
  }
}

std::array<LanguageFeatureState, kCanonicalFeatureStateEntryCount>
BuildCanonicalFeatureStateEntries() {
  std::array<LanguageFeatureState, kCanonicalFeatureStateEntryCount> entries{};
  std::size_t next = 0;
  AppendFeatureStates(entries, next, ImplementedFeatureStateData());
  AppendFeatureStates(entries, next, RejectedFeatureStateData());
  AppendFeatureStates(entries, next, ReservedFeatureStateData());
  return entries;
}

const std::array<LanguageFeatureState, kCanonicalFeatureStateEntryCount>
    kCanonicalFeatureStateEntries = BuildCanonicalFeatureStateEntries();

}  // namespace

std::span<const LanguageFeatureState> CanonicalFeatureStateTable() {
  return std::span<const LanguageFeatureState>(
      kCanonicalFeatureStateEntries.data(), kCanonicalFeatureStateEntries.size());
}

}  // namespace objc3c::config
