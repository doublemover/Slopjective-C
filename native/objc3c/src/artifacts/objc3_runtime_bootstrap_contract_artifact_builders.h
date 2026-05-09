#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend::runtime_bootstrap_contracts {

struct RuntimeStartupBootstrapInvariantArtifactBuilder {
  [[nodiscard]] static std::string BuildReplayKey(
      const Objc3RuntimeStartupBootstrapInvariantSummary &summary);

  [[nodiscard]] static Objc3RuntimeStartupBootstrapInvariantSummary BuildSummary(
      const Objc3RuntimeTranslationUnitRegistrationManifestSummary
          &registration_manifest);

  [[nodiscard]] static std::string BuildSummaryJson(
      const Objc3RuntimeStartupBootstrapInvariantSummary &summary);
};

struct RuntimeBootstrapSemanticsArtifactBuilder {
  [[nodiscard]] static std::string BuildReplayKey(
      const Objc3RuntimeBootstrapSemanticsSummary &summary);

  [[nodiscard]] static Objc3RuntimeBootstrapSemanticsSummary BuildSummary(
      const Objc3RuntimeStartupBootstrapInvariantSummary &bootstrap_invariants,
      const Objc3RuntimeTranslationUnitRegistrationManifestSummary
          &registration_manifest);

  [[nodiscard]] static std::string BuildSummaryJson(
      const Objc3RuntimeBootstrapSemanticsSummary &summary);
};

}  // namespace objc3::artifacts::frontend::runtime_bootstrap_contracts
