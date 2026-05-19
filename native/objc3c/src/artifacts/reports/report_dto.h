#pragma once

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::reports {

using VersionedConformanceReportLoweringSummary =
    ::Objc3VersionedConformanceReportLoweringSummary;
using MachineReadableConformanceReportContractSummary =
    ::Objc3ToolingMachineReadableConformanceReportContractSummary;
using FeatureAwareConformanceReportEmissionSummary =
    ::Objc3ToolingFeatureAwareConformanceReportEmissionSummary;
using CorpusShardingReleaseEvidencePackagingSummary =
    ::Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary;

[[nodiscard]] inline bool IsReady(
    const VersionedConformanceReportLoweringSummary &summary) {
  return ::IsReadyObjc3VersionedConformanceReportLoweringSummary(summary);
}

[[nodiscard]] inline bool IsReady(
    const MachineReadableConformanceReportContractSummary &summary) {
  return ::IsReadyObjc3ToolingMachineReadableConformanceReportContractSummary(
      summary);
}

[[nodiscard]] inline bool IsReady(
    const FeatureAwareConformanceReportEmissionSummary &summary) {
  return ::IsReadyObjc3ToolingFeatureAwareConformanceReportEmissionSummary(
      summary);
}

[[nodiscard]] inline bool IsReady(
    const CorpusShardingReleaseEvidencePackagingSummary &summary) {
  return ::IsReadyObjc3ToolingCorpusShardingReleaseEvidencePackagingSummary(
      summary);
}

}  // namespace objc3::artifacts::reports
