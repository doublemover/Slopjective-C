#include "runtime/stdlib/opaque_service_storage.h"
#include "runtime/stdlib/stdlib_service_contract.h"

#include <algorithm>
#include <atomic>
#include <unordered_map>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <utility>
#include <vector>

namespace storage = objc3c::runtime::stdlib_runtime;

namespace {
struct MapEntryV2 { std::int32_t key = 0; std::int32_t value = 0; };
#ifdef OBJC3_RUNTIME_I32_INDEX_TESTING
std::atomic<uint64_t> index_hash_calls{0}, index_equal_calls{0};
#endif
struct I32Hash {
  std::size_t operator()(std::int32_t value) const noexcept {
#ifdef OBJC3_RUNTIME_I32_INDEX_TESTING
    ++index_hash_calls;
#endif
    return std::hash<std::int32_t>{}(value);
  }
};
struct I32Equal {
  bool operator()(std::int32_t left, std::int32_t right) const noexcept {
#ifdef OBJC3_RUNTIME_I32_INDEX_TESTING
    ++index_equal_calls;
#endif
    return left == right;
  }
};
using I32Index = std::unordered_map<std::int32_t, std::size_t, I32Hash, I32Equal>;
// Average constant-time membership/append, with ordered payloads retained in
// their existing vectors. Ordered removal repairs shifted slots in O(N).
template <class T>
void AppendIndexed(std::vector<T> &values, I32Index &index,
                   std::int32_t key, T value) {
  values.push_back(value);
  try { index.emplace(key, values.size() - 1); }
  catch (...) { values.pop_back(); throw; }
}
template <class T, class Key>
void EraseIndexed(std::vector<T> &values, I32Index &index,
                  I32Index::iterator found, Key key) {
  const auto position = found->second;
  index.erase(found);
  values.erase(values.begin() + position);
  for (std::size_t i = position; i < values.size(); ++i)
    index.find(key(values[i]))->second = i;
}

struct CollectionV2Value {
  std::mutex mutex;
  std::uint32_t kind = 0;
  std::uint64_t mutation_generation = 0;
  std::vector<std::int32_t> values;
  std::vector<MapEntryV2> entries;
  std::unique_ptr<I32Index> key_index;
  void *owner_handle = nullptr;
  std::uint32_t owner_kind = 0;
  void *owner_registry = nullptr;
  bool (*owner_release_callback)(void *, const void *, std::uint32_t) noexcept =
      nullptr;
  std::uint64_t expected_mutation_generation = 0;
  std::size_t start = 0;
  std::size_t count = 0;
  std::size_t position = 0;
  bool map_iterator = false;

  ~CollectionV2Value() noexcept {
    const auto release = owner_release_callback;
    void *registry = owner_registry;
    void *owner = owner_handle;
    const std::uint32_t owner_type = owner_kind;
    owner_release_callback = nullptr;
    owner_registry = nullptr;
    owner_handle = nullptr;
    owner_kind = 0;
    if (release != nullptr && owner != nullptr) {
      (void)release(registry, owner, owner_type);
    }
  }
};
storage::OpaqueServiceStorage<CollectionV2Value> &CollectionV2Storage() {
  static storage::OpaqueServiceStorage<CollectionV2Value> storage;
  return storage;
}
constexpr std::uint32_t kCollectionV2Kind = 2;
constexpr std::uint32_t kArrayI32V2Kind = 20;
constexpr std::uint32_t kSliceI32V2Kind = 21;
constexpr std::uint32_t kIteratorI32V2Kind = 22;
constexpr std::uint32_t kMapI32I32V2Kind = 23;
constexpr std::uint32_t kSetI32V2Kind = 24;
constexpr std::size_t kCollectionV2MaxElements = 1U << 20;

bool ReleaseCollectionOwnerLease(void *registry, const void *owner,
                                 std::uint32_t owner_kind) noexcept {
  if (registry == nullptr) return false;
  try {
    return static_cast<storage::OpaqueServiceStorage<CollectionV2Value> *>(
        registry)->Release(owner, owner_kind);
  } catch (...) {
    return false;
  }
}

bool RetainCollectionOwner(CollectionV2Value &view, void *owner,
                           std::uint32_t owner_kind) {
  auto &registry = CollectionV2Storage();
  if (registry.Retain(owner, owner_kind) == nullptr) return false;
  view.owner_registry = &registry;
  view.owner_handle = owner;
  view.owner_kind = owner_kind;
  view.owner_release_callback = &ReleaseCollectionOwnerLease;
  return true;
}

int CollectionV2RegistryStatus(
    storage::OpaqueServiceStorage<CollectionV2Value>::Status status) {
  using RegistryStatus =
      storage::OpaqueServiceStorage<CollectionV2Value>::Status;
  switch (status) {
    case RegistryStatus::Ok: return OBJC3_STDLIB_V2_OK;
    case RegistryStatus::WrongKind: return OBJC3_STDLIB_V2_WRONG_KIND;
    case RegistryStatus::StaleHandle: return OBJC3_STDLIB_V2_STALE_HANDLE;
    case RegistryStatus::InvalidHandle:
    default: return OBJC3_STDLIB_V2_INVALID_HANDLE;
  }
}

int StoreCollectionV2(std::unique_ptr<CollectionV2Value> value,
                      std::uint32_t kind, void **out_handle) {
  if (out_handle == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_handle = nullptr;
  value->kind = kind;
  void *handle = CollectionV2Storage().Store(std::move(value), kind);
  if (handle == nullptr) {
    return CollectionV2Storage().AtCapacity()
        ? OBJC3_STDLIB_V2_CAPACITY_EXCEEDED : OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
  *out_handle = handle;
  return OBJC3_STDLIB_V2_OK;
}

int ReleaseCollectionV2(void *handle, std::uint32_t kind) {
  if (!CollectionV2Storage().Release(handle, kind)) {
    return CollectionV2RegistryStatus(
        CollectionV2Storage().Inspect(handle, kind));
  }
  return OBJC3_STDLIB_V2_OK;
}

}  // namespace

extern "C" objc3_collection_v2_t
objc3_runtime_stdlib_collections_v2_create_i32(const int32_t *values,
                                                uint64_t count) {
  if (count > (1U << 20) || (values == nullptr && count != 0)) return nullptr;
  try {
    auto value = std::make_unique<CollectionV2Value>();
    value->kind = kCollectionV2Kind;
    if (count != 0) {
      value->values.assign(values, values + static_cast<std::size_t>(count));
    }
    return static_cast<objc3_collection_v2_t>(CollectionV2Storage().Store(
        std::move(value), kCollectionV2Kind));
  } catch (const std::bad_alloc &) {
    return nullptr;
  }
}

extern "C" objc3_collection_v2_t
objc3_runtime_stdlib_collections_v2_retain(
    objc3_collection_v2_t collection) {
  return static_cast<objc3_collection_v2_t>(
      CollectionV2Storage().Retain(collection, kCollectionV2Kind));
}

extern "C" void objc3_runtime_stdlib_collections_v2_release(
    objc3_collection_v2_t collection) {
  (void)CollectionV2Storage().Release(collection, kCollectionV2Kind);
}

extern "C" int objc3_runtime_stdlib_collections_v2_count(
    objc3_collection_v2_t collection, uint64_t *out_count) {
  if (out_count == nullptr) return 0;
  return CollectionV2Storage().With(collection, kCollectionV2Kind,
      [&](CollectionV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        *out_count = static_cast<uint64_t>(value.values.size());
      });
}

extern "C" int objc3_runtime_stdlib_collections_v2_get(
    objc3_collection_v2_t collection, uint64_t index, int32_t *out_value) {
  if (out_value == nullptr) return 0;
  bool found = false;
  const bool valid = CollectionV2Storage().With(collection, kCollectionV2Kind,
      [&](CollectionV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        if (index < value.values.size()) {
          *out_value = value.values[static_cast<std::size_t>(index)];
          found = true;
        }
      });
  return valid && found;
}

extern "C" int objc3_runtime_stdlib_array_i32_v2_create(
    const int32_t *values, uint64_t count, objc3_array_i32_v2_t *out_array) {
  if (out_array == nullptr || (values == nullptr && count != 0))
    return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_array = nullptr;
  if (count > kCollectionV2MaxElements) return OBJC3_STDLIB_V2_CAPACITY_EXCEEDED;
  try {
    auto value = std::make_unique<CollectionV2Value>();
    if (count) value->values.assign(values, values + static_cast<std::size_t>(count));
    void *handle = nullptr;
    const int status = StoreCollectionV2(std::move(value), kArrayI32V2Kind,
                                         &handle);
    if (status == OBJC3_STDLIB_V2_OK)
      *out_array = static_cast<objc3_array_i32_v2_t>(handle);
    return status;
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
}

extern "C" int objc3_runtime_stdlib_array_i32_v2_retain(
    objc3_array_i32_v2_t array, objc3_array_i32_v2_t *out_array) {
  if (out_array == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_array = nullptr;
  if (CollectionV2Storage().Retain(array, kArrayI32V2Kind) == nullptr)
    return CollectionV2RegistryStatus(
        CollectionV2Storage().Inspect(array, kArrayI32V2Kind));
  *out_array = array;
  return OBJC3_STDLIB_V2_OK;
}

extern "C" int objc3_runtime_stdlib_array_i32_v2_release(
    objc3_array_i32_v2_t array) {
  return ReleaseCollectionV2(array, kArrayI32V2Kind);
}

extern "C" int objc3_runtime_stdlib_array_i32_v2_count(
    objc3_array_i32_v2_t array, uint64_t *out_count) {
  if (out_count == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_count = 0;
  const bool found = CollectionV2Storage().With(array, kArrayI32V2Kind,
      [&](CollectionV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        *out_count = value.values.size();
      });
  return found ? OBJC3_STDLIB_V2_OK : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(array, kArrayI32V2Kind));
}

extern "C" int objc3_runtime_stdlib_array_i32_v2_get(
    objc3_array_i32_v2_t array, uint64_t index, int32_t *out_value) {
  if (out_value == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_value = 0;
  bool in_bounds = false;
  const bool found = CollectionV2Storage().With(array, kArrayI32V2Kind,
      [&](CollectionV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        if (index >= value.values.size()) return;
        *out_value = value.values[static_cast<std::size_t>(index)];
        in_bounds = true;
      });
  if (!found) return CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(array, kArrayI32V2Kind));
  return in_bounds ? OBJC3_STDLIB_V2_OK : OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
}

extern "C" int objc3_runtime_stdlib_array_i32_v2_set(
    objc3_array_i32_v2_t array, uint64_t index, int32_t value_to_set) {
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = CollectionV2Storage().With(array, kArrayI32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        if (index >= record.values.size()) {
          status = OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
          return;
        }
        if (record.values[static_cast<std::size_t>(index)] != value_to_set) {
          record.values[static_cast<std::size_t>(index)] = value_to_set;
          ++record.mutation_generation;
        }
      });
  return found ? status : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(array, kArrayI32V2Kind));
}

extern "C" int objc3_runtime_stdlib_array_i32_v2_append(
    objc3_array_i32_v2_t array, int32_t value_to_append) {
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = CollectionV2Storage().With(array, kArrayI32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        if (record.values.size() >= kCollectionV2MaxElements) {
          status = OBJC3_STDLIB_V2_CAPACITY_EXCEEDED;
          return;
        }
        try {
          record.values.push_back(value_to_append);
          ++record.mutation_generation;
        } catch (const std::bad_alloc &) {
          status = OBJC3_STDLIB_V2_OUT_OF_MEMORY;
        }
      });
  return found ? status : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(array, kArrayI32V2Kind));
}

extern "C" int objc3_runtime_stdlib_array_i32_v2_remove(
    objc3_array_i32_v2_t array, uint64_t index, int32_t *out_value) {
  if (out_value == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_value = 0;
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = CollectionV2Storage().With(array, kArrayI32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        if (index >= record.values.size()) {
          status = OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
          return;
        }
        auto position = record.values.begin() + static_cast<std::ptrdiff_t>(index);
        *out_value = *position;
        record.values.erase(position);
        ++record.mutation_generation;
      });
  return found ? status : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(array, kArrayI32V2Kind));
}

extern "C" int objc3_runtime_stdlib_array_i32_v2_slice(
    objc3_array_i32_v2_t array, uint64_t start, uint64_t count,
    objc3_slice_i32_v2_t *out_slice) {
  if (out_slice == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_slice = nullptr;
  std::unique_ptr<CollectionV2Value> slice;
  try {
    slice = std::make_unique<CollectionV2Value>();
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
  slice->owner_handle = array;
  slice->owner_kind = kArrayI32V2Kind;
  int status = OBJC3_STDLIB_V2_OK;
  if (!CollectionV2Storage().With(array, kArrayI32V2Kind,
      [&](CollectionV2Value &owner) {
        std::lock_guard<std::mutex> lock(owner.mutex);
        if (start > owner.values.size() || count > owner.values.size() - start) {
          status = OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
          return;
        }
        slice->start = static_cast<std::size_t>(start);
        slice->count = static_cast<std::size_t>(count);
        slice->expected_mutation_generation = owner.mutation_generation;
      })) return CollectionV2RegistryStatus(
          CollectionV2Storage().Inspect(array, kArrayI32V2Kind));
  if (status != OBJC3_STDLIB_V2_OK) return status;
  if (!RetainCollectionOwner(*slice, array, kArrayI32V2Kind))
    return CollectionV2RegistryStatus(
        CollectionV2Storage().Inspect(array, kArrayI32V2Kind));
  void *handle = nullptr;
  try {
    status = StoreCollectionV2(std::move(slice), kSliceI32V2Kind, &handle);
  } catch (const std::bad_alloc &) {
    status = OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
  if (status != OBJC3_STDLIB_V2_OK) {
    return status;
  }
  *out_slice = static_cast<objc3_slice_i32_v2_t>(handle);
  return OBJC3_STDLIB_V2_OK;
}

extern "C" int objc3_runtime_stdlib_slice_i32_v2_release(
    objc3_slice_i32_v2_t slice) {
  return ReleaseCollectionV2(slice, kSliceI32V2Kind);
}

extern "C" int objc3_runtime_stdlib_slice_i32_v2_count(
    objc3_slice_i32_v2_t slice, uint64_t *out_count) {
  if (out_count == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_count = 0;
  void *owner_handle = nullptr;
  std::uint64_t start = 0;
  std::uint64_t count = 0;
  std::uint64_t expected = 0;
  const bool found = CollectionV2Storage().With(slice, kSliceI32V2Kind,
      [&](CollectionV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        owner_handle = value.owner_handle;
        start = value.start;
        count = value.count;
        expected = value.expected_mutation_generation;
      });
  if (!found) return CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(slice, kSliceI32V2Kind));
  int status = OBJC3_STDLIB_V2_OK;
  if (!CollectionV2Storage().With(owner_handle, kArrayI32V2Kind,
      [&](CollectionV2Value &owner) {
        std::lock_guard<std::mutex> lock(owner.mutex);
        if (owner.mutation_generation != expected) {
          status = OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION;
          return;
        }
        if (start > owner.values.size() || count > owner.values.size() - start) {
          status = OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
          return;
        }
        *out_count = count;
      })) return CollectionV2RegistryStatus(
          CollectionV2Storage().Inspect(owner_handle, kArrayI32V2Kind));
  return status;
}

extern "C" int objc3_runtime_stdlib_slice_i32_v2_get(
    objc3_slice_i32_v2_t slice, uint64_t index, int32_t *out_value) {
  if (out_value == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_value = 0;
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = CollectionV2Storage().With(slice, kSliceI32V2Kind,
      [&](CollectionV2Value &view) {
        std::lock_guard<std::mutex> view_lock(view.mutex);
        if (index >= view.count) {
          status = OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
          return;
        }
        if (!CollectionV2Storage().With(view.owner_handle, kArrayI32V2Kind,
            [&](CollectionV2Value &owner) {
              std::lock_guard<std::mutex> owner_lock(owner.mutex);
              if (owner.mutation_generation !=
                  view.expected_mutation_generation) {
                status = OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION;
              } else if (view.start > owner.values.size() ||
                         view.count > owner.values.size() - view.start) {
                status = OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
              } else {
                *out_value = owner.values[view.start + index];
              }
            })) {
          status = CollectionV2RegistryStatus(CollectionV2Storage().Inspect(
              view.owner_handle, kArrayI32V2Kind));
        }
      });
  return found ? status : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(slice, kSliceI32V2Kind));
}

extern "C" int objc3_runtime_stdlib_array_i32_v2_iterator(
    objc3_array_i32_v2_t array, objc3_iterator_i32_v2_t *out_iterator) {
  if (out_iterator == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_iterator = nullptr;
  std::unique_ptr<CollectionV2Value> iterator;
  try {
    iterator = std::make_unique<CollectionV2Value>();
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
  iterator->owner_handle = array;
  iterator->owner_kind = kArrayI32V2Kind;
  if (!CollectionV2Storage().With(array, kArrayI32V2Kind,
      [&](CollectionV2Value &owner) {
        std::lock_guard<std::mutex> lock(owner.mutex);
        iterator->expected_mutation_generation = owner.mutation_generation;
      })) return CollectionV2RegistryStatus(
          CollectionV2Storage().Inspect(array, kArrayI32V2Kind));
  if (!RetainCollectionOwner(*iterator, array, kArrayI32V2Kind))
    return CollectionV2RegistryStatus(
        CollectionV2Storage().Inspect(array, kArrayI32V2Kind));
  void *handle = nullptr;
  try {
    const int status = StoreCollectionV2(std::move(iterator),
                                         kIteratorI32V2Kind, &handle);
    if (status != OBJC3_STDLIB_V2_OK) {
      return status;
    }
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
  *out_iterator = static_cast<objc3_iterator_i32_v2_t>(handle);
  return OBJC3_STDLIB_V2_OK;
}

extern "C" int objc3_runtime_stdlib_iterator_i32_v2_release(
    objc3_iterator_i32_v2_t iterator) {
  return ReleaseCollectionV2(iterator, kIteratorI32V2Kind);
}

extern "C" int objc3_runtime_stdlib_iterator_i32_v2_next(
    objc3_iterator_i32_v2_t iterator, int *out_has_value,
    int32_t *out_value) {
  if (out_has_value == nullptr || out_value == nullptr)
    return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_has_value = 0;
  *out_value = 0;
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = CollectionV2Storage().With(iterator, kIteratorI32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> iterator_lock(record.mutex);
        if (record.map_iterator || record.owner_handle == nullptr) {
          status = OBJC3_STDLIB_V2_WRONG_KIND;
          return;
        }
        if (!CollectionV2Storage().With(record.owner_handle, record.owner_kind,
            [&](CollectionV2Value &owner) {
              std::lock_guard<std::mutex> owner_lock(owner.mutex);
              if (owner.mutation_generation !=
                  record.expected_mutation_generation) {
                status = OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION;
              } else if (record.position < owner.values.size()) {
                *out_value = owner.values[record.position++];
                *out_has_value = 1;
              }
            })) {
          status = CollectionV2RegistryStatus(CollectionV2Storage().Inspect(
              record.owner_handle, record.owner_kind));
        }
      });
  return found ? status : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(iterator, kIteratorI32V2Kind));
}

extern "C" int objc3_runtime_stdlib_map_i32_i32_v2_create(
    objc3_map_i32_i32_v2_t *out_map) {
  if (out_map == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_map = nullptr;
  try {
    auto value = std::make_unique<CollectionV2Value>();
    value->key_index = std::make_unique<I32Index>();
    void *handle = nullptr;
    const int status = StoreCollectionV2(std::move(value), kMapI32I32V2Kind,
                                         &handle);
    if (status == OBJC3_STDLIB_V2_OK)
      *out_map = static_cast<objc3_map_i32_i32_v2_t>(handle);
    return status;
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
}

extern "C" int objc3_runtime_stdlib_map_i32_i32_v2_release(
    objc3_map_i32_i32_v2_t map) {
  return ReleaseCollectionV2(map, kMapI32I32V2Kind);
}

extern "C" int objc3_runtime_stdlib_map_i32_i32_v2_count(
    objc3_map_i32_i32_v2_t map, uint64_t *out_count) {
  if (out_count == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_count = 0;
  const bool found = CollectionV2Storage().With(map, kMapI32I32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        *out_count = record.entries.size();
      });
  return found ? OBJC3_STDLIB_V2_OK : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(map, kMapI32I32V2Kind));
}

extern "C" int objc3_runtime_stdlib_map_i32_i32_v2_contains(
    objc3_map_i32_i32_v2_t map, int32_t key, int *out_found) {
  if (out_found == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_found = 0;
  const bool found = CollectionV2Storage().With(map, kMapI32I32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        *out_found = record.key_index->find(key) != record.key_index->end();
      });
  return found ? OBJC3_STDLIB_V2_OK : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(map, kMapI32I32V2Kind));
}

extern "C" int objc3_runtime_stdlib_map_i32_i32_v2_get(
    objc3_map_i32_i32_v2_t map, int32_t key, int *out_found,
    int32_t *out_value) {
  if (out_found == nullptr || out_value == nullptr)
    return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_found = 0;
  *out_value = 0;
  const bool found = CollectionV2Storage().With(map, kMapI32I32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        auto item = record.key_index->find(key);
        if (item != record.key_index->end()) {
          *out_found = 1;
          *out_value = record.entries[item->second].value;
        }
      });
  return found ? OBJC3_STDLIB_V2_OK : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(map, kMapI32I32V2Kind));
}

extern "C" int objc3_runtime_stdlib_map_i32_i32_v2_insert_count(
    objc3_map_i32_i32_v2_t map, int32_t key, int32_t value, uint64_t *out_count) {
  if (!out_count) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_count = 0;
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = CollectionV2Storage().With(map, kMapI32I32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        auto item = record.key_index->find(key);
        if (item != record.key_index->end()) {
          if (record.entries[item->second].value != value) {
            record.entries[item->second].value = value;
            ++record.mutation_generation;
          }
          *out_count = record.entries.size();
          return;
        }
        if (record.entries.size() >= kCollectionV2MaxElements) {
          status = OBJC3_STDLIB_V2_CAPACITY_EXCEEDED;
          return;
        }
        try {
          AppendIndexed(record.entries, *record.key_index, key, MapEntryV2{key, value});
          ++record.mutation_generation;
          *out_count = record.entries.size();
        } catch (const std::bad_alloc &) {
          status = OBJC3_STDLIB_V2_OUT_OF_MEMORY;
        }
      });
  return found ? status : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(map, kMapI32I32V2Kind));
}

extern "C" int objc3_runtime_stdlib_map_i32_i32_v2_insert(
    objc3_map_i32_i32_v2_t map, int32_t key, int32_t value) {
  uint64_t count = 0;
  return objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, key, value, &count);
}

extern "C" int objc3_runtime_stdlib_map_i32_i32_v2_remove(
    objc3_map_i32_i32_v2_t map, int32_t key, int *out_removed) {
  if (out_removed == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_removed = 0;
  const bool found = CollectionV2Storage().With(map, kMapI32I32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        auto item = record.key_index->find(key);
        if (item != record.key_index->end()) {
          EraseIndexed(record.entries, *record.key_index, item,
                       [](const MapEntryV2 &entry) { return entry.key; });
          *out_removed = 1;
          ++record.mutation_generation;
        }
      });
  return found ? OBJC3_STDLIB_V2_OK : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(map, kMapI32I32V2Kind));
}

extern "C" int objc3_runtime_stdlib_map_i32_i32_v2_iterator(
    objc3_map_i32_i32_v2_t map, objc3_iterator_i32_v2_t *out_iterator) {
  if (out_iterator == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_iterator = nullptr;
  std::unique_ptr<CollectionV2Value> iterator;
  try {
    iterator = std::make_unique<CollectionV2Value>();
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
  iterator->owner_handle = map;
  iterator->owner_kind = kMapI32I32V2Kind;
  iterator->map_iterator = true;
  if (!CollectionV2Storage().With(map, kMapI32I32V2Kind,
      [&](CollectionV2Value &owner) {
        std::lock_guard<std::mutex> lock(owner.mutex);
        iterator->expected_mutation_generation = owner.mutation_generation;
      })) return CollectionV2RegistryStatus(
          CollectionV2Storage().Inspect(map, kMapI32I32V2Kind));
  if (!RetainCollectionOwner(*iterator, map, kMapI32I32V2Kind))
    return CollectionV2RegistryStatus(
        CollectionV2Storage().Inspect(map, kMapI32I32V2Kind));
  void *handle = nullptr;
  try {
    const int status = StoreCollectionV2(std::move(iterator),
                                         kIteratorI32V2Kind, &handle);
    if (status != OBJC3_STDLIB_V2_OK) {
      return status;
    }
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
  *out_iterator = static_cast<objc3_iterator_i32_v2_t>(handle);
  return OBJC3_STDLIB_V2_OK;
}

extern "C" int objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(
    objc3_iterator_i32_v2_t iterator, int *out_has_value, int32_t *out_key,
    int32_t *out_value) {
  if (out_has_value == nullptr || out_key == nullptr || out_value == nullptr)
    return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_has_value = 0;
  *out_key = 0;
  *out_value = 0;
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = CollectionV2Storage().With(iterator, kIteratorI32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> iterator_lock(record.mutex);
        if (!record.map_iterator || record.owner_handle == nullptr) {
          status = OBJC3_STDLIB_V2_WRONG_KIND;
          return;
        }
        if (!CollectionV2Storage().With(record.owner_handle,
                kMapI32I32V2Kind, [&](CollectionV2Value &owner) {
              std::lock_guard<std::mutex> owner_lock(owner.mutex);
              if (owner.mutation_generation !=
                  record.expected_mutation_generation) {
                status = OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION;
              } else if (record.position < owner.entries.size()) {
                *out_key = owner.entries[record.position].key;
                *out_value = owner.entries[record.position].value;
                *out_has_value = 1;
                ++record.position;
              }
            })) {
          status = CollectionV2RegistryStatus(CollectionV2Storage().Inspect(
              record.owner_handle, kMapI32I32V2Kind));
        }
      });
  return found ? status : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(iterator, kIteratorI32V2Kind));
}

extern "C" int objc3_runtime_stdlib_set_i32_v2_create(
    objc3_set_i32_v2_t *out_set) {
  if (out_set == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_set = nullptr;
  try {
    auto value = std::make_unique<CollectionV2Value>();
    value->key_index = std::make_unique<I32Index>();
    void *handle = nullptr;
    const int status = StoreCollectionV2(std::move(value), kSetI32V2Kind,
                                         &handle);
    if (status == OBJC3_STDLIB_V2_OK)
      *out_set = static_cast<objc3_set_i32_v2_t>(handle);
    return status;
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
}

extern "C" int objc3_runtime_stdlib_set_i32_v2_release(
    objc3_set_i32_v2_t set) {
  return ReleaseCollectionV2(set, kSetI32V2Kind);
}

extern "C" int objc3_runtime_stdlib_set_i32_v2_count(
    objc3_set_i32_v2_t set, uint64_t *out_count) {
  if (out_count == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_count = 0;
  const bool found = CollectionV2Storage().With(set, kSetI32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        *out_count = record.values.size();
      });
  return found ? OBJC3_STDLIB_V2_OK : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(set, kSetI32V2Kind));
}

extern "C" int objc3_runtime_stdlib_set_i32_v2_contains(
    objc3_set_i32_v2_t set, int32_t value, int *out_found) {
  if (out_found == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_found = 0;
  const bool found = CollectionV2Storage().With(set, kSetI32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        *out_found = record.key_index->find(value) != record.key_index->end();
      });
  return found ? OBJC3_STDLIB_V2_OK : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(set, kSetI32V2Kind));
}

extern "C" int objc3_runtime_stdlib_set_i32_v2_insert(
    objc3_set_i32_v2_t set, int32_t value, int *out_inserted) {
  if (out_inserted == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_inserted = 0;
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = CollectionV2Storage().With(set, kSetI32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        if (record.key_index->find(value) != record.key_index->end()) return;
        if (record.values.size() >= kCollectionV2MaxElements) {
          status = OBJC3_STDLIB_V2_CAPACITY_EXCEEDED;
          return;
        }
        try {
          AppendIndexed(record.values, *record.key_index, value, value);
          ++record.mutation_generation;
          *out_inserted = 1;
        } catch (const std::bad_alloc &) {
          status = OBJC3_STDLIB_V2_OUT_OF_MEMORY;
        }
      });
  return found ? status : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(set, kSetI32V2Kind));
}

extern "C" int objc3_runtime_stdlib_set_i32_v2_remove(
    objc3_set_i32_v2_t set, int32_t value, int *out_removed) {
  if (out_removed == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_removed = 0;
  const bool found = CollectionV2Storage().With(set, kSetI32V2Kind,
      [&](CollectionV2Value &record) {
        std::lock_guard<std::mutex> lock(record.mutex);
        auto item = record.key_index->find(value);
        if (item != record.key_index->end()) {
          EraseIndexed(record.values, *record.key_index, item,
                       [](std::int32_t entry) { return entry; });
          ++record.mutation_generation;
          *out_removed = 1;
        }
      });
  return found ? OBJC3_STDLIB_V2_OK : CollectionV2RegistryStatus(
      CollectionV2Storage().Inspect(set, kSetI32V2Kind));
}

extern "C" int objc3_runtime_stdlib_set_i32_v2_iterator(
    objc3_set_i32_v2_t set, objc3_iterator_i32_v2_t *out_iterator) {
  if (out_iterator == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_iterator = nullptr;
  std::unique_ptr<CollectionV2Value> iterator;
  try {
    iterator = std::make_unique<CollectionV2Value>();
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
  iterator->owner_handle = set;
  iterator->owner_kind = kSetI32V2Kind;
  if (!CollectionV2Storage().With(set, kSetI32V2Kind,
      [&](CollectionV2Value &owner) {
        std::lock_guard<std::mutex> lock(owner.mutex);
        iterator->expected_mutation_generation = owner.mutation_generation;
      })) return CollectionV2RegistryStatus(
          CollectionV2Storage().Inspect(set, kSetI32V2Kind));
  if (!RetainCollectionOwner(*iterator, set, kSetI32V2Kind))
    return CollectionV2RegistryStatus(
        CollectionV2Storage().Inspect(set, kSetI32V2Kind));
  void *handle = nullptr;
  try {
    const int status = StoreCollectionV2(std::move(iterator),
                                         kIteratorI32V2Kind, &handle);
    if (status != OBJC3_STDLIB_V2_OK) {
      return status;
    }
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
  *out_iterator = static_cast<objc3_iterator_i32_v2_t>(handle);
  return OBJC3_STDLIB_V2_OK;
}

#ifdef OBJC3_RUNTIME_I32_INDEX_TESTING
extern "C" void objc3_test_i32_index_reset() { index_hash_calls = 0; index_equal_calls = 0; }
extern "C" uint64_t objc3_test_i32_index_hash_calls() { return index_hash_calls.load(); }
extern "C" uint64_t objc3_test_i32_index_equal_calls() { return index_equal_calls.load(); }
#endif
