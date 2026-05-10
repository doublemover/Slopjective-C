#pragma once

#include <cstdint>
#include <string_view>

#include "diag/objc3_diag_category.h"
#include "diag/objc3_diag_code.h"
#include "diag/objc3_diag_severity.h"
#include "contracts/objc3_diagnostic_owner_contract.h"

struct NativeDiagCodeCatalogEntry {
  char family = '\0';
  Objc3DiagnosticSubsystem subsystem = Objc3DiagnosticSubsystem::kUnknown;
  Objc3DiagnosticCategory category = Objc3DiagnosticCategory::kUnknown;
  const char *summary = "";
  unsigned min_ordinal = 0;
  unsigned max_ordinal = 999;
  std::string_view catalog_owner = kObjc3DiagnosticOwnerContractId;
  bool legacy_positive_allowed = false;
  bool retired_route_allowed = false;
};

const NativeDiagCodeCatalogEntry *FindNativeDiagCodeCatalogEntry(
    std::string_view candidate);
bool NativeDiagCodeIsWithinCatalog(std::string_view candidate);
