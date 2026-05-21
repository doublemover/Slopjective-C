#include "runtime/stdlib/collections_runtime_state.h"

#include <algorithm>

namespace objc3c::runtime::collections_runtime_internal {

RuntimeStdlibCollectionsState &State() {
  static RuntimeStdlibCollectionsState state;
  return state;
}

namespace {

void RecordStatusCounter(RuntimeStdlibCollectionsState &state, int status) {
  switch (status) {
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE:
      ++state.invalid_handle_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CROSS_KIND_HANDLE:
      ++state.cross_kind_handle_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_STALE_HANDLE:
      ++state.stale_handle_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MALFORMED_DESCRIPTOR:
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_DESCRIPTOR_MISMATCH:
      ++state.malformed_descriptor_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED:
      ++state.capacity_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION:
      ++state.iterator_invalidation_count;
      break;
    default:
      break;
  }
}

bool IsSupportedDescriptorKind(int descriptor_kind) {
  switch (descriptor_kind) {
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_ARRAY:
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_MUTABLE_ARRAY:
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_SLICE:
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_MAP:
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_SET:
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_ITERATOR:
      return true;
    default:
      return false;
  }
}

int DescriptorKindForStorageKind(storage::DescriptorKind kind) {
  switch (kind) {
    case storage::DescriptorKind::CollectionImmutableArray:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_ARRAY;
    case storage::DescriptorKind::CollectionMutableArray:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_MUTABLE_ARRAY;
    case storage::DescriptorKind::CollectionSlice:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_SLICE;
    case storage::DescriptorKind::CollectionMap:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_MAP;
    case storage::DescriptorKind::CollectionSet:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_SET;
    case storage::DescriptorKind::CollectionIterator:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_ITERATOR;
    default:
      return 0;
  }
}

}  // namespace

void RecordCall(RuntimeStdlibCollectionsState &state,
                std::uint64_t &family_count,
                int handle,
                int input_a,
                int input_b,
                int input_c,
                int status,
                int result) {
  ++state.total_call_count;
  ++family_count;
  state.last_handle = handle;
  state.last_input_a = input_a;
  state.last_input_b = input_b;
  state.last_input_c = input_c;
  state.last_status = status;
  state.last_result = result;
  RecordStatusCounter(state, status);
}

int StatusForLookup(storage::LookupStatus status) {
  switch (status) {
    case storage::LookupStatus::Ok:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK;
    case storage::LookupStatus::CrossKind:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CROSS_KIND_HANDLE;
    case storage::LookupStatus::StaleHandle:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_STALE_HANDLE;
    case storage::LookupStatus::InvalidHandle:
    default:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE;
  }
}

bool IsValidDescriptorShape(int descriptor_kind,
                            int key_type,
                            int value_type) {
  if (!IsSupportedDescriptorKind(descriptor_kind)) {
    return false;
  }
  if (descriptor_kind == OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_MAP) {
    return key_type == OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_I32 &&
           value_type == OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_I32;
  }
  return key_type == OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_I32 &&
         value_type == OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_NONE;
}

void AssignDescriptor(CollectionRecord &record,
                      int descriptor_kind,
                      int key_type,
                      int value_type) {
  record.collection_descriptor_kind = descriptor_kind;
  record.collection_descriptor_key_type = key_type;
  record.collection_descriptor_value_type = value_type;
}

void AssignDefaultDescriptor(CollectionRecord &record,
                             storage::DescriptorKind kind) {
  const int descriptor_kind = DescriptorKindForStorageKind(kind);
  const int value_type =
      descriptor_kind == OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_MAP
          ? OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_I32
          : OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_NONE;
  AssignDescriptor(record, descriptor_kind,
                   OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_I32,
                   value_type);
}

bool DescriptorMatchesRecord(const CollectionRecord &descriptor,
                             const CollectionRecord &record) {
  return descriptor.collection_descriptor_kind ==
             record.collection_descriptor_kind &&
         descriptor.collection_descriptor_key_type ==
             record.collection_descriptor_key_type &&
         descriptor.collection_descriptor_value_type ==
             record.collection_descriptor_value_type;
}

CollectionRecord MakeArrayRecord(const int *values,
                                 int count,
                                 storage::DescriptorKind kind) {
  CollectionRecord record;
  record.values.reserve(static_cast<std::size_t>(count));
  for (int index = 0; index < count; ++index) {
    record.values.push_back(values[index]);
  }
  record.count = count;
  record.header.descriptor_kind = kind;
  AssignDefaultDescriptor(record, kind);
  return record;
}

MapEntry *FindMapEntry(CollectionRecord &record, int key) {
  auto iterator = std::find_if(record.entries.begin(), record.entries.end(),
                               [key](const MapEntry &entry) {
                                 return entry.key == key;
                               });
  if (iterator == record.entries.end()) {
    return nullptr;
  }
  return &(*iterator);
}

bool ContainsValue(const std::vector<int> &values, int value) {
  return std::find(values.begin(), values.end(), value) != values.end();
}

bool AppendUniqueValue(std::vector<int> &values, int value) {
  if (ContainsValue(values, value)) {
    return false;
  }
  values.push_back(value);
  return true;
}

bool SumArrayFitsInt(const CollectionRecord &record, int *out) {
  int sum = 0;
  for (int value : record.values) {
    if (storage::AddWouldOverflowInt(sum, value)) {
      return false;
    }
    sum += value;
  }
  *out = sum;
  return true;
}

int ValidateSourceMutation(RuntimeStdlibCollectionsState &state,
                           int handle,
                           storage::DescriptorKind kind,
                           std::uint64_t expected_generation) {
  auto lookup = state.records.Lookup(handle, {kind});
  if (lookup.status != storage::LookupStatus::Ok) {
    return StatusForLookup(lookup.status);
  }
  if (lookup.record->header.mutation_generation != expected_generation) {
    return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION;
  }
  return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK;
}

std::vector<int> SliceValues(const CollectionRecord &record,
                             int start,
                             int count) {
  std::vector<int> values;
  values.reserve(static_cast<std::size_t>(count));
  for (int index = 0; index < count; ++index) {
    values.push_back(record.values[static_cast<std::size_t>(start + index)]);
  }
  return values;
}

int ArrayRecordCount(const RuntimeStdlibCollectionsState &state) {
  return state.records.RecordCount(
             storage::DescriptorKind::CollectionImmutableArray) +
         state.records.RecordCount(
             storage::DescriptorKind::CollectionMutableArray);
}

}  // namespace objc3c::runtime::collections_runtime_internal
