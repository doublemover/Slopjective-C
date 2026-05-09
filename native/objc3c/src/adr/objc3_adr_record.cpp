#include "adr/objc3_adr_record.h"

namespace objc3c::adr {

std::string_view Objc3AdrStatusName(Objc3AdrStatus status) {
  switch (status) {
    case Objc3AdrStatus::Accepted:
      return "Accepted";
  }
  return "Unknown";
}

std::string_view Objc3AdrFileName(const Objc3AdrRecord &record) {
  if (record.number == 1) {
    return "ADR-0001-layered-frontend-boundaries.md";
  }
  if (record.number == 2) {
    return "ADR-0002-cli-vs-library-separation.md";
  }
  if (record.number == 3) {
    return "ADR-0003-diagnostics-determinism-contract.md";
  }
  if (record.number == 4) {
    return "ADR-0004-frontend-result-and-sema-ownership-cutover.md";
  }
  return {};
}

bool Objc3AdrRecordIsWellFormed(const Objc3AdrRecord &record) {
  return record.number != 0 && !record.slug.empty() && !record.title.empty() &&
         !record.date.empty() && !record.deciders.empty() &&
         !record.related_surfaces.empty() &&
         !Objc3AdrFileName(record).empty();
}

}  // namespace objc3c::adr
