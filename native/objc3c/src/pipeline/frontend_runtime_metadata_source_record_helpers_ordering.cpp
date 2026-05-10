#include "pipeline/frontend_runtime_metadata_source_record_helpers_owners.h"

#include <algorithm>

#include "pipeline/frontend_metadata_handoff_ordering.h"

namespace objc3c::pipeline::orchestration {

void SortRuntimeMetadataSourceRecordSet(
    Objc3RuntimeMetadataSourceRecordSet &records) {
  std::sort(records.classes_lexicographic.begin(),
            records.classes_lexicographic.end(),
            IsClassSourceRecordLess);
  std::sort(records.protocols_lexicographic.begin(),
            records.protocols_lexicographic.end(),
            IsProtocolSourceRecordLess);
  std::sort(records.categories_lexicographic.begin(),
            records.categories_lexicographic.end(),
            IsCategorySourceRecordLess);
  std::sort(records.properties_lexicographic.begin(),
            records.properties_lexicographic.end(),
            IsPropertySourceRecordLess);
  std::sort(records.methods_lexicographic.begin(),
            records.methods_lexicographic.end(),
            IsMethodSourceRecordLess);
  std::sort(records.ivars_lexicographic.begin(),
            records.ivars_lexicographic.end(),
            IsIvarSourceRecordLess);
}

bool IsRuntimeMetadataSourceRecordSetDeterministic(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return std::is_sorted(records.classes_lexicographic.begin(),
                        records.classes_lexicographic.end(),
                        IsClassSourceRecordLess) &&
         std::is_sorted(records.protocols_lexicographic.begin(),
                        records.protocols_lexicographic.end(),
                        IsProtocolSourceRecordLess) &&
         std::is_sorted(records.categories_lexicographic.begin(),
                        records.categories_lexicographic.end(),
                        IsCategorySourceRecordLess) &&
         std::is_sorted(records.properties_lexicographic.begin(),
                        records.properties_lexicographic.end(),
                        IsPropertySourceRecordLess) &&
         std::is_sorted(records.methods_lexicographic.begin(),
                        records.methods_lexicographic.end(),
                        IsMethodSourceRecordLess) &&
         std::is_sorted(records.ivars_lexicographic.begin(),
                        records.ivars_lexicographic.end(),
                        IsIvarSourceRecordLess);
}

}  // namespace objc3c::pipeline::orchestration
