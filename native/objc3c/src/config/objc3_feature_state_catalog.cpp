#include "config/objc3_feature_state_catalog.h"

#include <array>
#include <cstddef>

#include "config/objc3_feature_state_core_data.h"
#include "config/objc3_rejected_feature_state_data.h"
#include "contracts/objc3_config_capability_contract.h"

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

std::span<const LanguageFeatureState> CanonicalFeatureStateEntries() {
  return std::span<const LanguageFeatureState>(
      kCanonicalFeatureStateEntries.data(), kCanonicalFeatureStateEntries.size());
}

}  // namespace

std::span<const LanguageFeatureState> CanonicalFeatureStates() {
  return CanonicalFeatureStateEntries();
}

const LanguageFeatureState *FindCanonicalFeatureState(
    std::string_view feature) {
  for (const auto &state : CanonicalFeatureStateEntries()) {
    if (feature == state.feature) {
      return &state;
    }
  }
  return nullptr;
}

const char *CanonicalFeatureStateCatalogContractId() {
  return objc3c::contracts::kObjc3CanonicalFeatureStateCatalogContractId;
}

}  // namespace objc3c::config
