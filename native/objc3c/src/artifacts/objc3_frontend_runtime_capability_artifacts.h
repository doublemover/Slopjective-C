#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "pipeline/results/report_dto.h"
#include "token/objc3_token_contract.h"

namespace objc3::artifacts::frontend {

std::string BuildRuntimeCapabilityReportJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary);

std::string BuildPublicConformanceReportJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary);

}  // namespace objc3::artifacts::frontend
