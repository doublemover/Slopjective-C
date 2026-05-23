#pragma once

#include "runtime/stdlib/collections_runtime_contract.h"
#include "runtime/stdlib/stdlib_runtime_storage.h"

#include <cstdint>
#include <mutex>
#include <vector>

namespace objc3c::runtime::collections_runtime_internal {

namespace storage = objc3c::runtime::stdlib_runtime;

struct MapEntry {
  int key = 0;
  int value = 0;
};

struct CollectionDescriptorShape {
  int kind = 0;
  int key_type = 0;
  int value_type = 0;
};

struct CollectionRecord {
  storage::RecordHeader header;
  std::vector<int> values;
  std::vector<MapEntry> entries;
  int collection_descriptor_kind = 0;
  int collection_descriptor_key_type = 0;
  int collection_descriptor_value_type = 0;
  int array_handle = 0;
  storage::DescriptorKind array_handle_kind = storage::DescriptorKind::Unknown;
  std::uint64_t expected_array_mutation_generation = 0;
  int start = 0;
  int count = 0;
  int iterator_position = 0;
  int iterator_source_handle = 0;
  storage::DescriptorKind iterator_source_kind =
      storage::DescriptorKind::Unknown;
  std::uint64_t expected_mutation_generation = 0;
};

struct RuntimeStdlibCollectionsState {
  std::mutex mutex;
  storage::HandleTable<CollectionRecord> records{
      storage::HandleTableOwner::Collections};
  std::uint64_t total_call_count = 0;
  std::uint64_t array_create_call_count = 0;
  std::uint64_t array_query_call_count = 0;
  std::uint64_t map_create_call_count = 0;
  std::uint64_t map_query_call_count = 0;
  std::uint64_t map_mutation_call_count = 0;
  std::uint64_t set_create_call_count = 0;
  std::uint64_t set_query_call_count = 0;
  std::uint64_t set_mutation_call_count = 0;
  std::uint64_t slice_create_call_count = 0;
  std::uint64_t slice_query_call_count = 0;
  std::uint64_t iterator_create_call_count = 0;
  std::uint64_t iterator_query_call_count = 0;
  std::uint64_t descriptor_create_call_count = 0;
  std::uint64_t descriptor_query_call_count = 0;
  std::uint64_t status_call_count = 0;
  std::uint64_t mutation_generation = 0;
  std::uint64_t invalid_handle_failure_count = 0;
  std::uint64_t cross_kind_handle_failure_count = 0;
  std::uint64_t stale_handle_failure_count = 0;
  std::uint64_t malformed_descriptor_failure_count = 0;
  std::uint64_t descriptor_mismatch_failure_count = 0;
  std::uint64_t capacity_failure_count = 0;
  std::uint64_t iterator_invalidation_count = 0;
  int last_handle = 0;
  int last_input_a = 0;
  int last_input_b = 0;
  int last_input_c = 0;
  int last_status = OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK;
  int last_result = 0;
  int last_descriptor_handle = 0;
  int last_descriptor_status = OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK;
  int last_descriptor_result = 0;
  CollectionDescriptorShape last_descriptor_actual;
  CollectionDescriptorShape last_descriptor_expected;
};

RuntimeStdlibCollectionsState &State();
void RecordCall(RuntimeStdlibCollectionsState &state,
                std::uint64_t &family_count,
                int handle,
                int input_a,
                int input_b,
                int input_c,
                int status,
                int result);
int StatusForLookup(storage::LookupStatus status);
CollectionDescriptorShape MakeDescriptorShape(int descriptor_kind,
                                              int key_type,
                                              int value_type);
CollectionDescriptorShape DescriptorShapeFromRecord(
    const CollectionRecord &record);
CollectionDescriptorShape DefaultDescriptorShapeForKind(
    storage::DescriptorKind kind);
bool DescriptorShapeMatches(CollectionDescriptorShape left,
                            CollectionDescriptorShape right);
void RecordDescriptorEvent(RuntimeStdlibCollectionsState &state,
                           int descriptor_handle,
                           CollectionDescriptorShape actual,
                           CollectionDescriptorShape expected,
                           int status,
                           int result);
bool IsValidDescriptorShape(int descriptor_kind, int key_type, int value_type);
void AssignDescriptor(CollectionRecord &record,
                      int descriptor_kind,
                      int key_type,
                      int value_type);
void AssignDefaultDescriptor(CollectionRecord &record,
                             storage::DescriptorKind kind);
bool DescriptorMatchesRecord(const CollectionRecord &descriptor,
                             const CollectionRecord &record);
CollectionRecord MakeArrayRecord(const int *values,
                                 int count,
                                 storage::DescriptorKind kind);
MapEntry *FindMapEntry(CollectionRecord &record, int key);
bool ContainsValue(const std::vector<int> &values, int value);
bool AppendUniqueValue(std::vector<int> &values, int value);
bool SumArrayFitsInt(const CollectionRecord &record, int *out);
int ValidateSourceMutation(RuntimeStdlibCollectionsState &state,
                           int handle,
                           storage::DescriptorKind kind,
                           std::uint64_t expected_generation);
std::vector<int> SliceValues(const CollectionRecord &record,
                             int start,
                             int count);
int ArrayRecordCount(const RuntimeStdlibCollectionsState &state);

}  // namespace objc3c::runtime::collections_runtime_internal
