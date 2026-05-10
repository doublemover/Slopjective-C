#pragma once

#include <string>

struct Objc3VersionedConformanceReportLoweringSummary;

namespace objc3::artifacts::frontend {

std::string BuildRuntimeCapabilityReportJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary);

std::string BuildPublicConformanceReportJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary);

}  // namespace objc3::artifacts::frontend
