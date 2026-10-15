#include "runtime/stdlib/generic_associative_service.h"

#include "runtime/services/value_storage.h"
#include "runtime/stdlib/opaque_service_storage.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

namespace storage = objc3c::runtime::stdlib_runtime;
namespace {
using Status = objc3_generic_collection_status;
using Payload = objc3::services::ValueStorage;
constexpr std::uint32_t kMapKind = 0x474d5031U;
constexpr std::uint32_t kSetKind = 0x47535431U;
constexpr std::uint32_t kIteratorKind = 0x47414931U;
constexpr std::uint64_t kMaxEntries = 1U << 20;

bool IsAligned(const void *pointer, std::size_t alignment) {
  return pointer != nullptr &&
      (reinterpret_cast<std::uintptr_t>(pointer) & (alignment - 1)) == 0;
}

Status MapServiceStatus(objc3_service_status status) {
  if (status == OBJC3_SERVICE_OK) return OBJC3_GENERIC_COLLECTION_OK;
  if (status == OBJC3_SERVICE_OUT_OF_MEMORY)
    return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
  return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
}

Status ValidateValueOps(const objc3_value_ops &ops) {
  if (ops.struct_size < sizeof(objc3_value_ops) ||
      ops.version != OBJC3_VALUE_OPS_VERSION || ops.payload_alignment == 0 ||
      (ops.payload_alignment & (ops.payload_alignment - 1)) != 0 ||
      ops.payload_alignment > std::numeric_limits<std::size_t>::max() ||
      ops.payload_size > std::numeric_limits<std::size_t>::max() ||
      !ops.move_init || !ops.destroy)
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  return OBJC3_GENERIC_COLLECTION_OK;
}

std::size_t Stride(const objc3_value_ops &ops) {
  const std::size_t alignment = static_cast<std::size_t>(ops.payload_alignment);
  const std::size_t size = std::max<std::size_t>(1,
      static_cast<std::size_t>(ops.payload_size));
  if (size > std::numeric_limits<std::size_t>::max() - (alignment - 1))
    return 0;
  const std::size_t remainder = size % alignment;
  return remainder == 0 ? size : size + alignment - remainder;
}

Status CopyPayload(Payload &destination, const void *source,
                   const objc3_value_ops &ops) {
  if (!ops.copy_init) return OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION;
  if (!source && ops.payload_size != 0)
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  if (source &&
      !IsAligned(source, static_cast<std::size_t>(ops.payload_alignment)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  void *empty_source = nullptr;
  if (!source) {
    empty_source = ::operator new(1, std::align_val_t(
        static_cast<std::size_t>(ops.payload_alignment)), std::nothrow);
    if (!empty_source) return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
    *static_cast<unsigned char *>(empty_source) = 0;
    source = empty_source;
  }
  objc3_service_status result = OBJC3_SERVICE_INVALID_ARGUMENT;
  try { result = destination.Copy(source, &ops); }
  catch (const std::bad_alloc &) { result = OBJC3_SERVICE_OUT_OF_MEMORY; }
  catch (...) { result = OBJC3_SERVICE_INVALID_ARGUMENT; }
  if (empty_source) ::operator delete(empty_source, std::align_val_t(
      static_cast<std::size_t>(ops.payload_alignment)));
  return MapServiceStatus(result);
}

Status CopyOut(const objc3_value_ops &ops, const void *source, void *out) {
  const std::size_t alignment = static_cast<std::size_t>(ops.payload_alignment);
  if (!out && ops.payload_size != 0)
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  if (out && !IsAligned(out, alignment))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  if (!ops.copy_init) return OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION;
  if (out) {
    objc3_service_status result;
    try { result = ops.copy_init(out, source); }
    catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
    return MapServiceStatus(result);
  }
  void *temporary = ::operator new(1, std::align_val_t(alignment), std::nothrow);
  if (!temporary) return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
  objc3_service_status result;
  try { result = ops.copy_init(temporary, source); }
  catch (...) {
    ::operator delete(temporary, std::align_val_t(alignment));
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  }
  if (result == OBJC3_SERVICE_OK) {
    try { ops.destroy(temporary); } catch (...) {}
  }
  ::operator delete(temporary, std::align_val_t(alignment));
  return MapServiceStatus(result);
}

struct Entry final {
  Payload key;
  Payload value;
  std::uint64_t hash = 0;
};
struct Snapshot final { std::vector<Entry> entries; };
struct AssociativeRecord final {
  std::mutex mutex;
  std::uint32_t kind = 0;
  bool is_set = false;
  objc3_generic_map_descriptor map_descriptor{};
  objc3_generic_set_descriptor set_descriptor{};
  std::shared_ptr<Snapshot> snapshot;
  std::uint64_t generation = 0;
};
storage::OpaqueServiceStorage<AssociativeRecord> &AssociativeStorage() {
  static storage::OpaqueServiceStorage<AssociativeRecord> value;
  return value;
}
struct AssocIterator final {
  std::mutex mutex;
  void *owner = nullptr;
  std::uint32_t owner_kind = 0;
  std::uint64_t generation = 0;
  std::size_t position = 0;
  ~AssocIterator() {
    if (owner) {
      try { (void)AssociativeStorage().Release(owner, owner_kind); }
      catch (...) {}
    }
  }
};
struct PendingHandle final {
  void *handle = nullptr;
  std::uint32_t kind = 0;
  bool keep = false;
  ~PendingHandle() {
    if (!keep && handle) {
      try { (void)AssociativeStorage().Release(handle, kind); }
      catch (...) {}
    }
  }
};
storage::OpaqueServiceStorage<AssocIterator> &IteratorStorage() {
  static storage::OpaqueServiceStorage<AssocIterator> value;
  return value;
}

Status Inspect(std::uint32_t kind, const void *handle) {
  using S = storage::OpaqueServiceStorage<AssociativeRecord>::Status;
  const auto status = AssociativeStorage().Inspect(handle, kind);
  switch (status) {
    case S::Ok: return OBJC3_GENERIC_COLLECTION_OK;
    case S::WrongKind: return OBJC3_GENERIC_COLLECTION_WRONG_KIND;
    case S::StaleHandle: return OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
    default: return OBJC3_GENERIC_COLLECTION_INVALID_HANDLE;
  }
}
Status InspectIterator(const void *handle) {
  using S = storage::OpaqueServiceStorage<AssocIterator>::Status;
  const auto status = IteratorStorage().Inspect(handle, kIteratorKind);
  switch (status) {
    case S::Ok: return OBJC3_GENERIC_COLLECTION_OK;
    case S::WrongKind: return OBJC3_GENERIC_COLLECTION_WRONG_KIND;
    case S::StaleHandle: return OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
    default: return OBJC3_GENERIC_COLLECTION_INVALID_HANDLE;
  }
}

Status ValidateMapDescriptor(const objc3_generic_map_descriptor *descriptor) {
  if (!IsAligned(descriptor, alignof(objc3_generic_map_descriptor)) ||
      descriptor->struct_size < sizeof(*descriptor) ||
      descriptor->version != OBJC3_GENERIC_ASSOCIATIVE_DESCRIPTOR_VERSION ||
      !descriptor->hash || !descriptor->equal)
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  Status status = ValidateValueOps(descriptor->key_ops);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  return ValidateValueOps(descriptor->value_ops);
}
Status ValidateSetDescriptor(const objc3_generic_set_descriptor *descriptor) {
  if (!IsAligned(descriptor, alignof(objc3_generic_set_descriptor)) ||
      descriptor->struct_size < sizeof(*descriptor) ||
      descriptor->version != OBJC3_GENERIC_ASSOCIATIVE_DESCRIPTOR_VERSION ||
      !descriptor->hash || !descriptor->equal)
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  return ValidateValueOps(descriptor->key_ops);
}

const objc3_value_ops &KeyOps(const AssociativeRecord &record) {
  return record.is_set ? record.set_descriptor.key_ops
                       : record.map_descriptor.key_ops;
}
const objc3_value_ops &ValueOps(const AssociativeRecord &record) {
  return record.map_descriptor.value_ops;
}
objc3_generic_hash_fn HashFn(const AssociativeRecord &record) {
  return record.is_set ? record.set_descriptor.hash : record.map_descriptor.hash;
}
objc3_generic_equal_fn EqualFn(const AssociativeRecord &record) {
  return record.is_set ? record.set_descriptor.equal : record.map_descriptor.equal;
}

Status ComputeHash(const AssociativeRecord &record, const void *key,
                   std::uint64_t &out_hash) {
  const auto &ops = KeyOps(record);
  if (!key && ops.payload_size != 0)
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  if (key &&
      !IsAligned(key, static_cast<std::size_t>(ops.payload_alignment)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  void *empty = nullptr;
  if (!key) {
    empty = ::operator new(1, std::align_val_t(
        static_cast<std::size_t>(ops.payload_alignment)), std::nothrow);
    if (!empty) return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
    *static_cast<unsigned char *>(empty) = 0;
    key = empty;
  }
  objc3_service_status status = OBJC3_SERVICE_INVALID_ARGUMENT;
  try { status = HashFn(record)(key, &out_hash); }
  catch (...) { status = OBJC3_SERVICE_INVALID_ARGUMENT; }
  if (empty) ::operator delete(empty, std::align_val_t(
      static_cast<std::size_t>(ops.payload_alignment)));
  return MapServiceStatus(status);
}

Status KeysEqual(const AssociativeRecord &record, const void *left,
                 const void *right, int &out_equal) {
  objc3_service_status status = OBJC3_SERVICE_INVALID_ARGUMENT;
  out_equal = 0;
  try { status = EqualFn(record)(left, right, &out_equal); }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
  if (status != OBJC3_SERVICE_OK) return MapServiceStatus(status);
  if (out_equal != 0 && out_equal != 1)
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  return OBJC3_GENERIC_COLLECTION_OK;
}

Status SnapshotRecord(AssociativeRecord &record,
                      std::shared_ptr<Snapshot> &snapshot,
                      std::uint64_t &generation) {
  std::lock_guard<std::mutex> lock(record.mutex);
  snapshot = record.snapshot;
  generation = record.generation;
  return snapshot ? OBJC3_GENERIC_COLLECTION_OK
                  : OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
}

Status FindEntry(const AssociativeRecord &record,
                 const std::shared_ptr<Snapshot> &snapshot, const void *key,
                 std::uint64_t hash, std::size_t &index, bool &found) {
  found = false;
  const void *query = key;
  void *empty = nullptr;
  const auto &ops = KeyOps(record);
  if (!query) {
    empty = ::operator new(1, std::align_val_t(
        static_cast<std::size_t>(ops.payload_alignment)), std::nothrow);
    if (!empty) return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
    *static_cast<unsigned char *>(empty) = 0;
    query = empty;
  }
  for (std::size_t i = 0; i < snapshot->entries.size(); ++i) {
    int equal = 0;
    Status status = KeysEqual(record, snapshot->entries[i].key.data(), query,
                              equal);
    if (status != OBJC3_GENERIC_COLLECTION_OK) {
      if (empty) ::operator delete(empty, std::align_val_t(
          static_cast<std::size_t>(ops.payload_alignment)));
      return status;
    }
    if (!equal) continue;
    if (snapshot->entries[i].hash != hash) {
      if (empty) ::operator delete(empty, std::align_val_t(
          static_cast<std::size_t>(ops.payload_alignment)));
      return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
    }
    index = i;
    found = true;
    break;
  }
  if (empty) ::operator delete(empty, std::align_val_t(
      static_cast<std::size_t>(ops.payload_alignment)));
  return OBJC3_GENERIC_COLLECTION_OK;
}

Status CopyEntry(Entry &destination, const Entry &source,
                 const AssociativeRecord &record, bool with_value,
                 const void *replacement_value = nullptr) {
  Status status = CopyPayload(destination.key, source.key.data(), KeyOps(record));
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  destination.hash = source.hash;
  if (record.is_set || !with_value) return OBJC3_GENERIC_COLLECTION_OK;
  return CopyPayload(destination.value,
      replacement_value ? replacement_value : source.value.data(), ValueOps(record));
}

Status Commit(AssociativeRecord &record, std::uint64_t expected,
              std::shared_ptr<Snapshot> replacement) {
  std::shared_ptr<Snapshot> retired;
  {
    std::lock_guard<std::mutex> lock(record.mutex);
    if (record.generation != expected)
      return OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION;
    if (record.generation == std::numeric_limits<std::uint64_t>::max())
      return OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED;
    retired = std::move(record.snapshot);
    record.snapshot = std::move(replacement);
    ++record.generation;
  }
  retired.reset();
  return OBJC3_GENERIC_COLLECTION_OK;
}

Status Insert(AssociativeRecord &record, const void *key, const void *value,
              int *out_inserted) {
  std::shared_ptr<Snapshot> before;
  std::uint64_t generation = 0;
  Status status = SnapshotRecord(record, before, generation);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  std::uint64_t hash = 0;
  status = ComputeHash(record, key, hash);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  std::size_t found_index = 0;
  bool found = false;
  status = FindEntry(record, before, key, hash, found_index, found);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  if (found && record.is_set) {
    if (out_inserted) *out_inserted = 0;
    return OBJC3_GENERIC_COLLECTION_OK;
  }
  if ((!found && !KeyOps(record).copy_init) ||
      (!record.is_set && !ValueOps(record).copy_init))
    return OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION;
  if (!found && before->entries.size() >= kMaxEntries)
    return OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED;
  auto candidate = std::make_shared<Snapshot>();
  candidate->entries.reserve(before->entries.size() + (found ? 0 : 1));
  for (std::size_t i = 0; i < before->entries.size(); ++i) {
    Entry entry;
    const bool replace_value = found && i == found_index && !record.is_set;
    status = CopyEntry(entry, before->entries[i], record, true,
                       replace_value ? value : nullptr);
    if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
    candidate->entries.push_back(std::move(entry));
  }
  if (!found) {
    Entry entry;
    status = CopyPayload(entry.key, key, KeyOps(record));
    if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
    entry.hash = hash;
    if (!record.is_set) {
      status = CopyPayload(entry.value, value, ValueOps(record));
      if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
    }
    candidate->entries.push_back(std::move(entry));
  }
  status = Commit(record, generation, std::move(candidate));
  if (status == OBJC3_GENERIC_COLLECTION_OK && out_inserted)
    *out_inserted = found ? 0 : 1;
  return status;
}

Status Remove(AssociativeRecord &record, const void *key, int *out_removed) {
  std::shared_ptr<Snapshot> before;
  std::uint64_t generation = 0;
  Status status = SnapshotRecord(record, before, generation);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  std::uint64_t hash = 0;
  status = ComputeHash(record, key, hash);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  std::size_t found_index = 0;
  bool found = false;
  status = FindEntry(record, before, key, hash, found_index, found);
  if (status != OBJC3_GENERIC_COLLECTION_OK || !found) return status;
  if (before->entries.size() > 1 &&
      (!KeyOps(record).copy_init || (!record.is_set && !ValueOps(record).copy_init)))
    return OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION;
  auto candidate = std::make_shared<Snapshot>();
  candidate->entries.reserve(before->entries.size() - 1);
  for (std::size_t i = 0; i < before->entries.size(); ++i) {
    if (i == found_index) continue;
    Entry entry;
    status = CopyEntry(entry, before->entries[i], record, true);
    if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
    candidate->entries.push_back(std::move(entry));
  }
  status = Commit(record, generation, std::move(candidate));
  if (status == OBJC3_GENERIC_COLLECTION_OK && out_removed) *out_removed = 1;
  return status;
}

Status StoreRecord(std::unique_ptr<AssociativeRecord> record,
                   std::uint32_t kind, void **out) {
  void *handle = AssociativeStorage().Store(std::move(record), kind);
  if (!handle) return AssociativeStorage().AtCapacity()
      ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED
      : OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
  *out = handle;
  return OBJC3_GENERIC_COLLECTION_OK;
}

template <typename Descriptor>
Status CreateEmpty(const Descriptor &descriptor, std::uint32_t kind,
                   bool is_set, void **out) {
  auto record = std::make_unique<AssociativeRecord>();
  record->kind = kind;
  record->is_set = is_set;
  if constexpr (std::is_same_v<Descriptor, objc3_generic_set_descriptor>)
    record->set_descriptor = descriptor;
  else
    record->map_descriptor = descriptor;
  record->snapshot = std::make_shared<Snapshot>();
  return StoreRecord(std::move(record), kind, out);
}

Status StoreIterator(void *owner, std::uint32_t owner_kind,
                     std::uint64_t generation,
                     objc3_generic_associative_iterator_t *out) {
  if (!AssociativeStorage().Retain(owner, owner_kind)) {
    Status status = Inspect(owner_kind, owner);
    return status == OBJC3_GENERIC_COLLECTION_OK
        ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED : status;
  }
  std::unique_ptr<AssocIterator> iterator;
  try { iterator = std::make_unique<AssocIterator>(); }
  catch (const std::bad_alloc &) {
    (void)AssociativeStorage().Release(owner, owner_kind);
    return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
  }
  iterator->owner = owner;
  iterator->owner_kind = owner_kind;
  iterator->generation = generation;
  void *handle = IteratorStorage().Store(std::move(iterator), kIteratorKind);
  if (!handle) return IteratorStorage().AtCapacity()
      ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED
      : OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
  *out = static_cast<objc3_generic_associative_iterator_t>(handle);
  return OBJC3_GENERIC_COLLECTION_OK;
}

Status Count(std::uint32_t kind, const void *handle, std::uint64_t *out) {
  if (!IsAligned(out, alignof(std::uint64_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = 0;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = AssociativeStorage().With(handle, kind, [&](AssociativeRecord &record) {
    std::shared_ptr<Snapshot> snapshot; std::uint64_t generation = 0;
    result = SnapshotRecord(record, snapshot, generation);
    if (result == OBJC3_GENERIC_COLLECTION_OK) *out = snapshot->entries.size();
  });
  return found ? result : Inspect(kind, handle);
}

Status IteratorNext(AssocIterator &iterator, int *out_has, void *out_key,
                    void *out_value) {
  std::size_t position = 0;
  {
    std::lock_guard<std::mutex> lock(iterator.mutex);
    position = iterator.position;
  }
  std::shared_ptr<Snapshot> snapshot;
  std::uint64_t generation = 0;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool owner_found = AssociativeStorage().With(iterator.owner,
      iterator.owner_kind, [&](AssociativeRecord &record) {
        result = SnapshotRecord(record, snapshot, generation);
      });
  if (!owner_found) return Inspect(iterator.owner_kind, iterator.owner);
  if (result != OBJC3_GENERIC_COLLECTION_OK) return result;
  if (generation != iterator.generation)
    return OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION;
  if (position >= snapshot->entries.size()) return OBJC3_GENERIC_COLLECTION_OK;

  objc3_value_ops key_ops{};
  objc3_value_ops value_ops{};
  bool is_set = false;
  owner_found = AssociativeStorage().With(iterator.owner, iterator.owner_kind,
      [&](AssociativeRecord &record) {
        key_ops = KeyOps(record);
        is_set = record.is_set;
        if (!is_set) value_ops = ValueOps(record);
      });
  if (!owner_found) return Inspect(iterator.owner_kind, iterator.owner);
  result = CopyOut(key_ops, snapshot->entries[position].key.data(), out_key);
  if (result != OBJC3_GENERIC_COLLECTION_OK) return result;
  bool key_owned_by_caller = out_key != nullptr;
  bool value_owned_by_caller = false;
  if (!is_set) {
    result = CopyOut(value_ops, snapshot->entries[position].value.data(), out_value);
    if (result != OBJC3_GENERIC_COLLECTION_OK) {
      if (key_owned_by_caller) { try { key_ops.destroy(out_key); } catch (...) {} }
      return result;
    }
    value_owned_by_caller = out_value != nullptr;
  }

  std::shared_ptr<Snapshot> after;
  std::uint64_t after_generation = 0;
  bool still_live = AssociativeStorage().With(iterator.owner,
      iterator.owner_kind, [&](AssociativeRecord &record) {
        result = SnapshotRecord(record, after, after_generation);
      });
  if (!still_live || result != OBJC3_GENERIC_COLLECTION_OK ||
      after_generation != iterator.generation) {
    if (key_owned_by_caller) { try { key_ops.destroy(out_key); } catch (...) {} }
    if (value_owned_by_caller) { try { value_ops.destroy(out_value); } catch (...) {} }
    return OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION;
  }
  bool conflict = false;
  {
    std::lock_guard<std::mutex> lock(iterator.mutex);
    conflict = iterator.position != position;
    if (!conflict) ++iterator.position;
  }
  if (conflict) {
    if (key_owned_by_caller) { try { key_ops.destroy(out_key); } catch (...) {} }
    if (value_owned_by_caller) { try { value_ops.destroy(out_value); } catch (...) {} }
    return OBJC3_GENERIC_COLLECTION_CONCURRENT_ADVANCE;
  }
  *out_has = 1;
  return OBJC3_GENERIC_COLLECTION_OK;
}

Status IteratorNextApi(objc3_generic_associative_iterator_t iterator,
                       int *out_has, void *out_key, void *out_value) {
  if (!IsAligned(out_has, alignof(int)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out_has = 0;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = IteratorStorage().With(iterator, kIteratorKind,
      [&](AssocIterator &value) {
        result = IteratorNext(value, out_has, out_key, out_value);
      });
  return found ? result : InspectIterator(iterator);
}

template <typename Handle>
Status RetainRecord(std::uint32_t kind, Handle handle, Handle *out) {
  if (!IsAligned(out, alignof(Handle)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  if (!AssociativeStorage().Retain(handle, kind)) {
    Status result = Inspect(kind, handle);
    return result == OBJC3_GENERIC_COLLECTION_OK
        ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED : result;
  }
  *out = handle;
  return OBJC3_GENERIC_COLLECTION_OK;
}

}  // namespace

extern "C" Status objc3_generic_map_create_empty(
    const objc3_generic_map_descriptor *descriptor,
    objc3_generic_map_t *out) try {
  if (!IsAligned(out, alignof(objc3_generic_map_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  Status status = ValidateMapDescriptor(descriptor);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  void *handle = nullptr;
  status = CreateEmpty(*descriptor, kMapKind, false, &handle);
  if (status == OBJC3_GENERIC_COLLECTION_OK)
    *out = static_cast<objc3_generic_map_t>(handle);
  return status;
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }

extern "C" Status objc3_generic_map_create(
    const objc3_generic_map_descriptor *descriptor, const void *keys,
    const void *values, std::uint64_t count, objc3_generic_map_t *out) try {
  if (!IsAligned(out, alignof(objc3_generic_map_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  Status status = ValidateMapDescriptor(descriptor);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  if (count > kMaxEntries) return OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED;
  const std::size_t key_stride = Stride(descriptor->key_ops);
  const std::size_t value_stride = Stride(descriptor->value_ops);
  if (key_stride == 0 || value_stride == 0 ||
      (key_stride != 0 && count > std::numeric_limits<std::size_t>::max() / key_stride) ||
      (value_stride != 0 && count > std::numeric_limits<std::size_t>::max() / value_stride) ||
      (count && descriptor->key_ops.payload_size && !keys) ||
      (count && descriptor->value_ops.payload_size && !values) ||
      (keys && count &&
       !IsAligned(keys, static_cast<std::size_t>(descriptor->key_ops.payload_alignment))) ||
      (values && count &&
       !IsAligned(values, static_cast<std::size_t>(descriptor->value_ops.payload_alignment))))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  objc3_generic_map_t map = nullptr;
  void *map_handle = nullptr;
  status = CreateEmpty(*descriptor, kMapKind, false, &map_handle);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  map = static_cast<objc3_generic_map_t>(map_handle);
  PendingHandle cleanup{map, kMapKind, false};
  for (std::size_t i = 0; i < static_cast<std::size_t>(count); ++i) {
    const auto *key = keys ? static_cast<const std::byte *>(keys) + i * key_stride : nullptr;
    const auto *value = values ? static_cast<const std::byte *>(values) + i * value_stride : nullptr;
    int inserted = 0;
    bool success = AssociativeStorage().With(map, kMapKind,
        [&](AssociativeRecord &record) { status = Insert(record, key, value, &inserted); });
    if (!success) status = Inspect(kMapKind, map);
    if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  }
  *out = map;
  cleanup.keep = true;
  return OBJC3_GENERIC_COLLECTION_OK;
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }

extern "C" Status objc3_generic_map_retain(objc3_generic_map_t map,
    objc3_generic_map_t *out) try {
  return RetainRecord(kMapKind, map, out);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_map_release(objc3_generic_map_t map) try {
  return AssociativeStorage().Release(map, kMapKind)
      ? OBJC3_GENERIC_COLLECTION_OK : Inspect(kMapKind, map);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_map_count(objc3_generic_map_t map,
    std::uint64_t *out) try {
  return Count(kMapKind, map, out);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_map_insert(objc3_generic_map_t map,
    const void *key, const void *value, int *out_inserted) try {
  if (!IsAligned(out_inserted, alignof(int)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out_inserted = 0;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = AssociativeStorage().With(map, kMapKind,
      [&](AssociativeRecord &record) { result = Insert(record, key, value, out_inserted); });
  return found ? result : Inspect(kMapKind, map);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_map_lookup(objc3_generic_map_t map,
    const void *key, int *out_has, void *out_value) try {
  if (!IsAligned(out_has, alignof(int)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out_has = 0;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = AssociativeStorage().With(map, kMapKind,
      [&](AssociativeRecord &record) {
        std::shared_ptr<Snapshot> snapshot; std::uint64_t generation = 0;
        result = SnapshotRecord(record, snapshot, generation);
        if (result != OBJC3_GENERIC_COLLECTION_OK) return;
        std::uint64_t hash = 0;
        result = ComputeHash(record, key, hash);
        if (result != OBJC3_GENERIC_COLLECTION_OK) return;
        std::size_t index = 0; bool present = false;
        result = FindEntry(record, snapshot, key, hash, index, present);
        if (result != OBJC3_GENERIC_COLLECTION_OK || !present) return;
        *out_has = 1;
        result = CopyOut(ValueOps(record), snapshot->entries[index].value.data(),
                         out_value);
      });
  return found ? result : Inspect(kMapKind, map);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_map_remove(objc3_generic_map_t map,
    const void *key, int *out_removed) try {
  if (!IsAligned(out_removed, alignof(int)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out_removed = 0;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = AssociativeStorage().With(map, kMapKind,
      [&](AssociativeRecord &record) { result = Remove(record, key, out_removed); });
  return found ? result : Inspect(kMapKind, map);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_map_iterator(objc3_generic_map_t map,
    objc3_generic_associative_iterator_t *out) try {
  if (!IsAligned(out, alignof(objc3_generic_associative_iterator_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  std::uint64_t generation = 0;
  bool found = AssociativeStorage().With(map, kMapKind,
      [&](AssociativeRecord &record) {
        std::shared_ptr<Snapshot> snapshot;
        result = SnapshotRecord(record, snapshot, generation);
      });
  if (!found) return Inspect(kMapKind, map);
  if (result != OBJC3_GENERIC_COLLECTION_OK) return result;
  return StoreIterator(map, kMapKind, generation, out);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }

extern "C" Status objc3_generic_set_create_empty(
    const objc3_generic_set_descriptor *descriptor,
    objc3_generic_set_t *out) try {
  if (!IsAligned(out, alignof(objc3_generic_set_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  Status status = ValidateSetDescriptor(descriptor);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  void *handle = nullptr;
  status = CreateEmpty(*descriptor, kSetKind, true, &handle);
  if (status == OBJC3_GENERIC_COLLECTION_OK)
    *out = static_cast<objc3_generic_set_t>(handle);
  return status;
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_set_create(const objc3_generic_set_descriptor *descriptor,
    const void *values, std::uint64_t count, objc3_generic_set_t *out) try {
  if (!IsAligned(out, alignof(objc3_generic_set_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  Status status = ValidateSetDescriptor(descriptor);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  const std::size_t stride = Stride(descriptor->key_ops);
  if (count > kMaxEntries) return OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED;
  if (stride == 0 || count > std::numeric_limits<std::size_t>::max() / stride ||
      (count && descriptor->key_ops.payload_size && !values) ||
      (values && count &&
       !IsAligned(values, static_cast<std::size_t>(descriptor->key_ops.payload_alignment))))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  objc3_generic_set_t set = nullptr;
  void *set_handle = nullptr;
  status = CreateEmpty(*descriptor, kSetKind, true, &set_handle);
  if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  set = static_cast<objc3_generic_set_t>(set_handle);
  PendingHandle cleanup{set, kSetKind, false};
  for (std::size_t i = 0; i < static_cast<std::size_t>(count); ++i) {
    const auto *value = values ? static_cast<const std::byte *>(values) + i * stride : nullptr;
    int inserted = 0;
    bool success = AssociativeStorage().With(set, kSetKind,
        [&](AssociativeRecord &record) { status = Insert(record, value, nullptr, &inserted); });
    if (!success) status = Inspect(kSetKind, set);
    if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
  }
  *out = set;
  cleanup.keep = true;
  return OBJC3_GENERIC_COLLECTION_OK;
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_set_retain(objc3_generic_set_t set,
    objc3_generic_set_t *out) try {
  return RetainRecord(kSetKind, set, out);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_set_release(objc3_generic_set_t set) try {
  return AssociativeStorage().Release(set, kSetKind)
      ? OBJC3_GENERIC_COLLECTION_OK : Inspect(kSetKind, set);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_set_count(objc3_generic_set_t set,
    std::uint64_t *out) try {
  return Count(kSetKind, set, out);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_set_insert(objc3_generic_set_t set,
    const void *value, int *out_inserted) try {
  if (!IsAligned(out_inserted, alignof(int)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out_inserted = 0;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = AssociativeStorage().With(set, kSetKind,
      [&](AssociativeRecord &record) { result = Insert(record, value, nullptr, out_inserted); });
  return found ? result : Inspect(kSetKind, set);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_set_contains(objc3_generic_set_t set,
    const void *value, int *out_contains) try {
  if (!IsAligned(out_contains, alignof(int)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out_contains = 0;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = AssociativeStorage().With(set, kSetKind,
      [&](AssociativeRecord &record) {
        std::shared_ptr<Snapshot> snapshot; std::uint64_t generation = 0;
        result = SnapshotRecord(record, snapshot, generation);
        if (result != OBJC3_GENERIC_COLLECTION_OK) return;
        std::uint64_t hash = 0;
        result = ComputeHash(record, value, hash);
        if (result != OBJC3_GENERIC_COLLECTION_OK) return;
        std::size_t index = 0; bool present = false;
        result = FindEntry(record, snapshot, value, hash, index, present);
        if (result == OBJC3_GENERIC_COLLECTION_OK) *out_contains = present ? 1 : 0;
      });
  return found ? result : Inspect(kSetKind, set);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_set_remove(objc3_generic_set_t set,
    const void *value, int *out_removed) try {
  if (!IsAligned(out_removed, alignof(int)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out_removed = 0;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = AssociativeStorage().With(set, kSetKind,
      [&](AssociativeRecord &record) { result = Remove(record, value, out_removed); });
  return found ? result : Inspect(kSetKind, set);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_set_iterator(objc3_generic_set_t set,
    objc3_generic_associative_iterator_t *out) try {
  if (!IsAligned(out, alignof(objc3_generic_associative_iterator_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  std::uint64_t generation = 0;
  bool found = AssociativeStorage().With(set, kSetKind,
      [&](AssociativeRecord &record) {
        std::shared_ptr<Snapshot> snapshot;
        result = SnapshotRecord(record, snapshot, generation);
      });
  if (!found) return Inspect(kSetKind, set);
  if (result != OBJC3_GENERIC_COLLECTION_OK) return result;
  return StoreIterator(set, kSetKind, generation, out);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_associative_iterator_release(
    objc3_generic_associative_iterator_t iterator) try {
  return IteratorStorage().Release(iterator, kIteratorKind)
      ? OBJC3_GENERIC_COLLECTION_OK : InspectIterator(iterator);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_associative_iterator_next(
    objc3_generic_associative_iterator_t iterator, int *out_has,
    void *out_key, void *out_value) try {
  return IteratorNextApi(iterator, out_has, out_key, out_value);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
