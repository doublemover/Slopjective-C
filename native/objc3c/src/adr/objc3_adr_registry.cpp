#include "adr/objc3_adr_registry.h"

#include <array>

namespace objc3c::adr {

namespace {

constexpr std::array<Objc3AdrRecord, 4> kFrontendAdrRecords = {{
    {.number = 1,
     .slug = "layered-frontend-boundaries",
     .title = "Layered Frontend Boundaries",
     .status = Objc3AdrStatus::Accepted,
     .date = "2026-02-27",
     .deciders = "objc3c native maintainers",
     .related_surfaces = "objc3c.frontend.layeredboundaries.v1"},
    {.number = 2,
     .slug = "cli-vs-library-separation",
     .title = "CLI and Library Separation",
     .status = Objc3AdrStatus::Accepted,
     .date = "2026-02-27",
     .deciders = "objc3c native maintainers",
     .related_surfaces =
         "objc3c.frontend.layeredboundaries.v1;objc3c.frontend.clilibrarysplit.v1"},
    {.number = 3,
     .slug = "diagnostics-determinism-contract",
     .title = "Deterministic Diagnostics Contract",
     .status = Objc3AdrStatus::Accepted,
     .date = "2026-02-27",
     .deciders = "objc3c native maintainers",
     .related_surfaces =
         "objc3c.frontend.layeredboundaries.v1;objc3c.frontend.diagnosticsdeterminism.probe.v1;objc3c.frontend.diagnosticsdeterminism.closeout.v1"},
    {.number = 4,
     .slug = "frontend-result-and-sema-ownership-cutover",
     .title = "Frontend Result and Sema Ownership Cutover",
     .status = Objc3AdrStatus::Accepted,
     .date = "2026-05-09",
     .deciders = "objc3c native maintainers",
     .related_surfaces =
         "objc3c.frontend.capi.resultownership.v1;objc3c.frontend.artifactpublication.v1;objc3c.sema.featureclaims.v1;objc3c.sema.passowners.v1"},
}};

bool SurfaceListContains(std::string_view surface_list,
                         std::string_view related_surface) {
  if (related_surface.empty()) {
    return false;
  }
  std::size_t begin = 0;
  while (begin <= surface_list.size()) {
    const std::size_t end = surface_list.find(';', begin);
    const std::string_view item =
        end == std::string_view::npos
            ? surface_list.substr(begin)
            : surface_list.substr(begin, end - begin);
    if (item == related_surface) {
      return true;
    }
    if (end == std::string_view::npos) {
      break;
    }
    begin = end + 1;
  }
  return false;
}

}  // namespace

std::span<const Objc3AdrRecord> Objc3FrontendAdrRecords() {
  return kFrontendAdrRecords;
}

const Objc3AdrRecord *FindObjc3FrontendAdrByNumber(uint16_t number) {
  for (const Objc3AdrRecord &record : kFrontendAdrRecords) {
    if (record.number == number) {
      return &record;
    }
  }
  return nullptr;
}

const Objc3AdrRecord *FindObjc3FrontendAdrBySurface(
    std::string_view related_surface) {
  for (const Objc3AdrRecord &record : kFrontendAdrRecords) {
    if (SurfaceListContains(record.related_surfaces, related_surface)) {
      return &record;
    }
  }
  return nullptr;
}

bool Objc3FrontendAdrRegistryIsWellFormed() {
  uint16_t previous_number = 0;
  for (const Objc3AdrRecord &record : kFrontendAdrRecords) {
    if (!Objc3AdrRecordIsWellFormed(record) ||
        record.number <= previous_number) {
      return false;
    }
    previous_number = record.number;
  }
  return true;
}

}  // namespace objc3c::adr
