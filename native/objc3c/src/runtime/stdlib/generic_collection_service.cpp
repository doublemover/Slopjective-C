#include "runtime/public/generic_collection_service.h"

#include "runtime/stdlib/opaque_service_storage.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <utility>

namespace storage = objc3c::runtime::stdlib_runtime;
namespace {
using Status = objc3_generic_collection_status;
constexpr std::uint32_t kArrayKind = 0x47434131U;
constexpr std::uint32_t kViewKind = 0x47435631U;
constexpr std::uint32_t kIteratorKind = 0x47434931U;
constexpr std::uint64_t kMaxElements = 1U << 20;

bool IsAligned(const void *pointer, std::size_t alignment) {
  return pointer != nullptr &&
      (reinterpret_cast<std::uintptr_t>(pointer) & (alignment - 1)) == 0;
}

std::size_t ElementStride(const objc3_value_ops &ops) {
  const auto size = static_cast<std::size_t>(ops.payload_size);
  const auto alignment = static_cast<std::size_t>(ops.payload_alignment);
  const auto effective_size = std::max<std::size_t>(1, size);
  if (effective_size > std::numeric_limits<std::size_t>::max() - (alignment - 1))
    return 0;
  const auto remainder = effective_size % alignment;
  return remainder == 0 ? effective_size : effective_size + alignment - remainder;
}

constexpr std::size_t kChunkElements = 32;
constexpr unsigned kTreeLevel = 2;
constexpr std::size_t kBranchSlots = 32;
static_assert(kMaxElements == (1u << 20));
#ifdef OBJC3_GENERIC_ARRAY_STORAGE_TESTING
std::atomic<std::uint64_t> metadata_slots_copied{0};
std::atomic<std::uint64_t> nodes_allocated{0};
std::atomic<std::uint64_t> chunks_allocated{0};
#endif

struct Chunk final {
  objc3_value_ops ops{};
  std::size_t stride = 1;
  std::size_t alignment = alignof(std::max_align_t);
  std::size_t initialized = 0;
  void *data = nullptr;
  ~Chunk() noexcept {
    if (!data) return;
    for (std::size_t i = initialized; i > 0; --i) {
      try { ops.destroy(static_cast<std::byte *>(data) + (i - 1) * stride); }
      catch (...) {}
    }
    ::operator delete(data, std::align_val_t(alignment));
  }
  void *At(std::size_t index) const {
    return static_cast<std::byte *>(data) + index * stride;
  }
};
std::shared_ptr<Chunk> AllocateChunk(const objc3_value_ops &ops, std::size_t count) {
  auto chunk = std::make_shared<Chunk>();
  chunk->ops = ops;
  chunk->stride = ElementStride(ops);
  chunk->alignment = std::max<std::size_t>(alignof(std::max_align_t), ops.payload_alignment);
  if (!chunk->stride || count > std::numeric_limits<std::size_t>::max() / chunk->stride)
    return nullptr;
  chunk->data = ::operator new(count * chunk->stride,
                               std::align_val_t(chunk->alignment), std::nothrow);
  if (!chunk->data) return nullptr;
#ifdef OBJC3_GENERIC_ARRAY_STORAGE_TESTING
  ++chunks_allocated;
#endif
  return chunk;
}

// Three bounded levels index 32,768 immutable payload chunks. Publication never
// mutates a shared node/chunk: append/set replace one chunk and copy one path.
struct Node final {
  std::array<std::shared_ptr<Node>, kBranchSlots> children{};
  std::array<std::shared_ptr<Chunk>, kBranchSlots> chunks{};
};
std::shared_ptr<Node> NewNode(const std::shared_ptr<Node> &source = {}) {
#ifdef OBJC3_GENERIC_ARRAY_STORAGE_TESTING
  ++nodes_allocated;
  if (source) metadata_slots_copied += 2 * kBranchSlots;
#endif
  return source ? std::make_shared<Node>(*source) : std::make_shared<Node>();
}
std::size_t Slot(std::size_t chunk, unsigned level) {
  return (chunk >> (5 * level)) & (kBranchSlots - 1);
}
void PutUnpublishedChunk(std::shared_ptr<Node> &node, unsigned level,
                         std::size_t index, std::shared_ptr<Chunk> chunk) {
  if (!node) node = NewNode();
  if (!level) { node->chunks[Slot(index, 0)] = std::move(chunk); return; }
  PutUnpublishedChunk(node->children[Slot(index, level)], level - 1, index,
                      std::move(chunk));
}
std::shared_ptr<Node> ReplaceChunk(const std::shared_ptr<Node> &source,
                                  unsigned level, std::size_t index,
                                  const std::shared_ptr<Chunk> &chunk) {
  auto node = NewNode(source);
  if (!level) node->chunks[Slot(index, 0)] = chunk;
  else node->children[Slot(index, level)] = ReplaceChunk(
      source ? source->children[Slot(index, level)] : nullptr,
      level - 1, index, chunk);
  return node;
}
struct Buffer final {
  objc3_value_ops ops{};
  std::size_t stride = 1;
  std::size_t alignment = alignof(std::max_align_t);
  std::size_t count = 0;
  std::shared_ptr<Node> root;
  const std::shared_ptr<Chunk> &ChunkAt(std::size_t index) const {
    const Node *node = root.get();
    for (unsigned level = kTreeLevel; level; --level)
      node = node->children[Slot(index / kChunkElements, level)].get();
    return node->chunks[Slot(index / kChunkElements, 0)];
  }
  void *At(std::size_t index) const { return ChunkAt(index)->At(index % kChunkElements); }
  void MarkInitialized(std::size_t index) { ++ChunkAt(index)->initialized; }
};
std::shared_ptr<Buffer> AllocateBuffer(const objc3_value_ops &ops, std::size_t count) {
  auto result = std::make_shared<Buffer>();
  result->ops = ops;
  result->stride = ElementStride(ops);
  if (!result->stride || count > std::numeric_limits<std::size_t>::max() / result->stride)
    return nullptr;
  result->alignment = std::max<std::size_t>(alignof(std::max_align_t), ops.payload_alignment);
  result->count = count;
  for (std::size_t start = 0; start < count; start += kChunkElements) {
    auto chunk = AllocateChunk(ops, std::min(kChunkElements, count - start));
    if (!chunk) return nullptr;
    PutUnpublishedChunk(result->root, kTreeLevel, start / kChunkElements, std::move(chunk));
  }
  return result;
}

Status CopyElement(const objc3_value_ops &ops, void *destination,
                   const void *source) {
  const auto alignment = static_cast<std::size_t>(ops.payload_alignment);
  if (!IsAligned(destination, alignment))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  if (!source && ops.payload_size != 0)
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  if (source && !IsAligned(source, alignment))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  void *zero_source = nullptr;
  if (!source) {
    zero_source = ::operator new(1, std::align_val_t(alignment), std::nothrow);
    if (!zero_source) return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
    std::memset(zero_source, 0, 1);
    source = zero_source;
  }
  Status result = OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  try {
    const auto status = ops.copy_init(destination, source);
    result = status == OBJC3_SERVICE_OK
        ? OBJC3_GENERIC_COLLECTION_OK
        : status == OBJC3_SERVICE_OUT_OF_MEMORY
            ? OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY
            : OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  }
  catch (...) {
    if (zero_source) ::operator delete(zero_source,
        std::align_val_t(alignment));
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  }
  if (zero_source) ::operator delete(zero_source,
      std::align_val_t(alignment));
  return result;
}

Status ValidateOps(const objc3_value_ops *ops, bool require_copy) {
  if (!ops || ops->struct_size < sizeof(objc3_value_ops))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  if (ops->version != OBJC3_VALUE_OPS_VERSION)
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  if (ops->payload_alignment == 0 ||
      (ops->payload_alignment & (ops->payload_alignment - 1)) != 0 ||
      ops->payload_alignment > std::numeric_limits<std::size_t>::max() ||
      ops->payload_size > std::numeric_limits<std::size_t>::max() ||
      (require_copy && !ops->copy_init) || !ops->move_init || !ops->destroy)
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  return OBJC3_GENERIC_COLLECTION_OK;
}

struct Array final {
  std::mutex mutex;
  std::shared_ptr<Buffer> buffer;
  std::uint64_t generation = 0;
};
struct View final {
  void *owner = nullptr;
  std::uint64_t generation = 0;
  std::size_t start = 0;
  std::size_t count = 0;
  ~View() {
    if (owner) {
      try { (void)ArrayStorage().Release(owner, kArrayKind); }
      catch (...) {}
    }
  }
  static storage::OpaqueServiceStorage<Array> &ArrayStorage();
};
struct Iterator final {
  std::mutex mutex;
  void *owner = nullptr;
  std::uint64_t generation = 0;
  std::size_t position = 0;
  ~Iterator() {
    if (owner) {
      try { (void)ArrayStorage().Release(owner, kArrayKind); }
      catch (...) {}
    }
  }
  static storage::OpaqueServiceStorage<Array> &ArrayStorage();
};

storage::OpaqueServiceStorage<Array> &ArrayStorage() {
  static storage::OpaqueServiceStorage<Array> value;
  return value;
}
storage::OpaqueServiceStorage<View> &ViewStorage() {
  static storage::OpaqueServiceStorage<View> value;
  return value;
}
storage::OpaqueServiceStorage<Iterator> &IteratorStorage() {
  static storage::OpaqueServiceStorage<Iterator> value;
  return value;
}
storage::OpaqueServiceStorage<Array> &View::ArrayStorage() { return ::ArrayStorage(); }
storage::OpaqueServiceStorage<Array> &Iterator::ArrayStorage() { return ::ArrayStorage(); }

template <typename T>
Status MapStatus(typename storage::OpaqueServiceStorage<T>::Status status) {
  using S = typename storage::OpaqueServiceStorage<T>::Status;
  switch (status) {
    case S::Ok: return OBJC3_GENERIC_COLLECTION_OK;
    case S::WrongKind: return OBJC3_GENERIC_COLLECTION_WRONG_KIND;
    case S::StaleHandle: return OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
    default: return OBJC3_GENERIC_COLLECTION_INVALID_HANDLE;
  }
}

Status InspectArray(const void *handle) {
  return MapStatus<Array>(ArrayStorage().Inspect(handle, kArrayKind));
}
Status InspectView(const void *handle) {
  return MapStatus<View>(ViewStorage().Inspect(handle, kViewKind));
}
Status InspectIterator(const void *handle) {
  return MapStatus<Iterator>(IteratorStorage().Inspect(handle, kIteratorKind));
}

Status RetainArrayOwner(void *owner) {
  if (ArrayStorage().Retain(owner, kArrayKind))
    return OBJC3_GENERIC_COLLECTION_OK;
  const Status status = InspectArray(owner);
  return status == OBJC3_GENERIC_COLLECTION_OK
      ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED : status;
}

Status BuildArray(const objc3_value_ops &ops, const void *elements,
                  std::size_t count, bool move, std::shared_ptr<Buffer> &out) {
  if (!move && !ops.copy_init) return OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION;
  if (move && !ops.move_init) return OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION;
  if (count && elements &&
      !IsAligned(elements, static_cast<std::size_t>(ops.payload_alignment)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  auto candidate = AllocateBuffer(ops, count);
  if (!candidate) return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
  void *zero_source = nullptr;
  if (count != 0 && ops.payload_size == 0 && elements == nullptr) {
    zero_source = ::operator new(1, std::align_val_t(candidate->alignment),
                                 std::nothrow);
    if (!zero_source) return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
    std::memset(zero_source, 0, 1);
  }
  const auto *source_bytes = static_cast<const std::byte *>(elements);
  const auto stride = candidate->stride;
  for (std::size_t i = 0; i < count; ++i) {
    const void *source = source_bytes ? source_bytes + i * stride : zero_source;
    Status status;
    try {
      if (move) {
        const auto move_status = ops.move_init(candidate->At(i),
            const_cast<void *>(source));
        status = move_status == OBJC3_SERVICE_OK
            ? OBJC3_GENERIC_COLLECTION_OK
            : move_status == OBJC3_SERVICE_OUT_OF_MEMORY
                ? OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY
                : OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
      } else {
        status = CopyElement(ops, candidate->At(i), source);
      }
    } catch (...) { status = OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
    if (status != OBJC3_GENERIC_COLLECTION_OK) {
      if (zero_source) ::operator delete(zero_source,
          std::align_val_t(candidate->alignment));
      return status;
    }
    candidate->MarkInitialized(i);
  }
  if (zero_source) ::operator delete(zero_source,
      std::align_val_t(candidate->alignment));
  out = std::move(candidate);
  return OBJC3_GENERIC_COLLECTION_OK;
}

Status Snapshot(Array &array, std::shared_ptr<Buffer> &buffer,
                std::uint64_t &generation) {
  std::lock_guard<std::mutex> lock(array.mutex);
  buffer = array.buffer;
  generation = array.generation;
  return buffer ? OBJC3_GENERIC_COLLECTION_OK
                : OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
}

Status Commit(Array &array, std::uint64_t expected,
              std::shared_ptr<Buffer> candidate) {
  std::shared_ptr<Buffer> retired;
  {
    std::lock_guard<std::mutex> lock(array.mutex);
    if (array.generation != expected)
      return OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION;
    if (array.generation == std::numeric_limits<std::uint64_t>::max())
      return OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED;
    retired = std::move(array.buffer);
    array.buffer = std::move(candidate);
    ++array.generation;
  }
  retired.reset();
  return OBJC3_GENERIC_COLLECTION_OK;
}

Status CloneWithReplacement(const std::shared_ptr<Buffer> &source,
                            std::size_t replacement_index,
                            const void *replacement, bool append,
                            bool remove, std::shared_ptr<Buffer> &candidate) {
  const auto &ops = source->ops;
  if (!ops.copy_init) return OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION;
  const std::size_t new_count = remove ? source->count - 1
      : source->count + (append ? 1 : 0);
  if (remove) {
    // Ordered erase shifts the suffix and remains O(N) payload work.
    candidate = AllocateBuffer(ops, new_count);
    if (!candidate) return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
    for (std::size_t target = 0; target < new_count; ++target) {
      const auto *from = source->At(target >= replacement_index ? target + 1 : target);
      const Status status = CopyElement(ops, candidate->At(target), from);
      if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
      candidate->MarkInitialized(target);
    }
    return OBJC3_GENERIC_COLLECTION_OK;
  }
  const auto changed_index = append ? source->count : replacement_index;
  const auto start = (changed_index / kChunkElements) * kChunkElements;
  const auto chunk_count = std::min(kChunkElements, new_count - start);
  auto chunk = AllocateChunk(ops, chunk_count);
  if (!chunk) return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
  for (std::size_t i = 0; i < chunk_count; ++i) {
    const auto index = start + i;
    const void *from = index == changed_index ? replacement : source->At(index);
    const Status status = CopyElement(ops, chunk->At(i), from);
    if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
    ++chunk->initialized;
  }
  candidate = std::make_shared<Buffer>(*source);
  candidate->count = new_count;
  candidate->root = ReplaceChunk(source->root, kTreeLevel,
                                  changed_index / kChunkElements, chunk);

  return OBJC3_GENERIC_COLLECTION_OK;
}

Status GetFromBuffer(const std::shared_ptr<Buffer> &buffer, std::size_t index,
                     void *out_element) {
  if (!out_element && buffer->ops.payload_size != 0)
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  if (index >= buffer->count) return OBJC3_GENERIC_COLLECTION_OUT_OF_BOUNDS;
  if (!buffer->ops.copy_init) return OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION;
  if (out_element) return CopyElement(buffer->ops, out_element, buffer->At(index));
  void *empty_result = ::operator new(1, std::align_val_t(buffer->alignment),
                                      std::nothrow);
  if (!empty_result) return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
  Status status = CopyElement(buffer->ops, empty_result, buffer->At(index));
  if (status == OBJC3_GENERIC_COLLECTION_OK) {
    try { buffer->ops.destroy(empty_result); } catch (...) {}
  }
  ::operator delete(empty_result, std::align_val_t(buffer->alignment));
  return status;
}

Status ViewSnapshot(View &view, std::shared_ptr<Buffer> &buffer) {
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  ArrayStorage().With(view.owner, kArrayKind, [&](Array &array) {
    std::uint64_t generation = 0;
    std::shared_ptr<Buffer> current;
    result = Snapshot(array, current, generation);
    if (result != OBJC3_GENERIC_COLLECTION_OK) return;
    if (generation != view.generation) {
      result = OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION;
      return;
    }
    buffer = std::move(current);
  });
  return result;
}

Status StoreView(void *owner, std::uint64_t generation, std::size_t start,
                 std::size_t count, objc3_generic_view_t *out_view) {
  const Status retain_status = RetainArrayOwner(owner);
  if (retain_status != OBJC3_GENERIC_COLLECTION_OK) return retain_status;
  std::unique_ptr<View> value;
  try {
    value = std::make_unique<View>();
  } catch (const std::bad_alloc &) {
    (void)ArrayStorage().Release(owner, kArrayKind);
    return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
  }
  value->owner = owner;
  value->generation = generation;
  value->start = start;
  value->count = count;
  try {
    void *handle = ViewStorage().Store(std::move(value), kViewKind);
    if (!handle) return ViewStorage().AtCapacity()
        ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED
        : OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
    *out_view = static_cast<objc3_generic_view_t>(handle);
    return OBJC3_GENERIC_COLLECTION_OK;
  } catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
}

Status StoreIterator(void *owner, std::uint64_t generation,
                     objc3_generic_iterator_t *out_iterator) {
  const Status retain_status = RetainArrayOwner(owner);
  if (retain_status != OBJC3_GENERIC_COLLECTION_OK) return retain_status;
  std::unique_ptr<Iterator> value;
  try {
    value = std::make_unique<Iterator>();
  } catch (const std::bad_alloc &) {
    (void)ArrayStorage().Release(owner, kArrayKind);
    return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
  }
  value->owner = owner;
  value->generation = generation;
  try {
    void *handle = IteratorStorage().Store(std::move(value), kIteratorKind);
    if (!handle) return IteratorStorage().AtCapacity()
        ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED
        : OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
    *out_iterator = static_cast<objc3_generic_iterator_t>(handle);
    return OBJC3_GENERIC_COLLECTION_OK;
  } catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
}
}  // namespace

extern "C" Status objc3_generic_array_create(const objc3_value_ops *ops,
    const void *elements, std::uint64_t count, objc3_generic_array_t *out) try {
  if (!IsAligned(out, alignof(objc3_generic_array_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  if (!IsAligned(ops, alignof(objc3_value_ops)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  const auto valid = ValidateOps(ops, false);
  if (valid != OBJC3_GENERIC_COLLECTION_OK) return valid;
  if (!ops->copy_init) return OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION;
  if (count > kMaxElements || (count && ops->payload_size && !elements))
    return count > kMaxElements ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED
                                : OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  try {
    std::shared_ptr<Buffer> buffer;
    Status status = BuildArray(*ops, elements, static_cast<std::size_t>(count),
                               false, buffer);
    if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
    auto array = std::make_unique<Array>();
    array->buffer = std::move(buffer);
    void *handle = ArrayStorage().Store(std::move(array), kArrayKind);
    if (!handle) return ArrayStorage().AtCapacity()
        ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED
        : OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
    *out = static_cast<objc3_generic_array_t>(handle);
    return OBJC3_GENERIC_COLLECTION_OK;
  } catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }

extern "C" Status objc3_generic_array_create_move(const objc3_value_ops *ops,
    void *elements, std::uint64_t count, objc3_generic_array_t *out) try {
  if (!IsAligned(out, alignof(objc3_generic_array_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  if (!IsAligned(ops, alignof(objc3_value_ops)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  const auto valid = ValidateOps(ops, false);
  if (valid != OBJC3_GENERIC_COLLECTION_OK) return valid;
  if (count > kMaxElements || (count && ops->payload_size && !elements))
    return count > kMaxElements ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED
                                : OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  try {
    std::shared_ptr<Buffer> buffer;
    Status status = BuildArray(*ops, elements, static_cast<std::size_t>(count),
                               true, buffer);
    if (status != OBJC3_GENERIC_COLLECTION_OK) return status;
    auto array = std::make_unique<Array>();
    array->buffer = std::move(buffer);
    void *handle = ArrayStorage().Store(std::move(array), kArrayKind);
    if (!handle) return ArrayStorage().AtCapacity()
        ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED
        : OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
    *out = static_cast<objc3_generic_array_t>(handle);
    return OBJC3_GENERIC_COLLECTION_OK;
  } catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }

extern "C" Status objc3_generic_array_retain(objc3_generic_array_t array,
    objc3_generic_array_t *out) try {
  if (!IsAligned(out, alignof(objc3_generic_array_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  if (!ArrayStorage().Retain(array, kArrayKind)) {
    const Status status = InspectArray(array);
    return status == OBJC3_GENERIC_COLLECTION_OK
        ? OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED : status;
  }
  *out = array;
  return OBJC3_GENERIC_COLLECTION_OK;
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_array_release(objc3_generic_array_t array) try {
  return ArrayStorage().Release(array, kArrayKind) ? OBJC3_GENERIC_COLLECTION_OK
                                                   : InspectArray(array);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_array_count(objc3_generic_array_t array,
    std::uint64_t *out) try {
  if (!IsAligned(out, alignof(std::uint64_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = 0;
  bool found = ArrayStorage().With(array, kArrayKind, [&](Array &value) {
    std::shared_ptr<Buffer> buffer; std::uint64_t generation = 0;
    if (Snapshot(value, buffer, generation) == OBJC3_GENERIC_COLLECTION_OK)
      *out = buffer->count;
  });
  return found ? OBJC3_GENERIC_COLLECTION_OK : InspectArray(array);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_array_get(objc3_generic_array_t array,
    std::uint64_t index, void *out) try {
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = ArrayStorage().With(array, kArrayKind, [&](Array &value) {
    std::shared_ptr<Buffer> buffer; std::uint64_t generation = 0;
    result = Snapshot(value, buffer, generation);
    if (result == OBJC3_GENERIC_COLLECTION_OK)
      result = GetFromBuffer(buffer, static_cast<std::size_t>(index), out);
  });
  return found ? result : InspectArray(array);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }

namespace {
template <typename Mutation>
Status Mutate(objc3_generic_array_t handle, Mutation mutation) {
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  try {
    bool found = ArrayStorage().With(handle, kArrayKind, [&](Array &array) {
      std::shared_ptr<Buffer> snapshot; std::uint64_t generation = 0;
      result = Snapshot(array, snapshot, generation);
      if (result != OBJC3_GENERIC_COLLECTION_OK) return;
      std::shared_ptr<Buffer> candidate;
      result = mutation(snapshot, candidate);
      if (result == OBJC3_GENERIC_COLLECTION_OK)
        result = Commit(array, generation, std::move(candidate));
    });
    return found ? result : InspectArray(handle);
  } catch (const std::bad_alloc &) {
    return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY;
  } catch (...) {
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  }
}
}
extern "C" Status objc3_generic_array_set(objc3_generic_array_t array,
    std::uint64_t index, const void *element) try {
  return Mutate(array, [&](const std::shared_ptr<Buffer> &source,
                           std::shared_ptr<Buffer> &candidate) {
    if ((!element && source->ops.payload_size) ||
        (element && !IsAligned(element, static_cast<std::size_t>(
             source->ops.payload_alignment))))
      return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
    if (index >= source->count) return OBJC3_GENERIC_COLLECTION_OUT_OF_BOUNDS;
    return CloneWithReplacement(source, static_cast<std::size_t>(index),
                                element, false, false, candidate);
  });
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_array_append(objc3_generic_array_t array,
    const void *element) try {
  return Mutate(array, [&](const std::shared_ptr<Buffer> &source,
                           std::shared_ptr<Buffer> &candidate) {
    if ((!element && source->ops.payload_size) ||
        (element && !IsAligned(element, static_cast<std::size_t>(
             source->ops.payload_alignment))))
      return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
    if (source->count >= kMaxElements) return OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED;
    return CloneWithReplacement(source, source->count, element, true, false,
                                candidate);
  });
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_array_remove(objc3_generic_array_t array,
    std::uint64_t index) try {
  return Mutate(array, [&](const std::shared_ptr<Buffer> &source,
                           std::shared_ptr<Buffer> &candidate) {
    if (index >= source->count) return OBJC3_GENERIC_COLLECTION_OUT_OF_BOUNDS;
    return CloneWithReplacement(source, static_cast<std::size_t>(index),
                                nullptr, false, true, candidate);
  });
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }

extern "C" Status objc3_generic_array_slice(objc3_generic_array_t array,
    std::uint64_t start, std::uint64_t count, objc3_generic_view_t *out) try {
  if (!IsAligned(out, alignof(objc3_generic_view_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  std::uint64_t generation = 0; std::size_t size = 0;
  bool found = ArrayStorage().With(array, kArrayKind, [&](Array &value) {
    std::shared_ptr<Buffer> buffer;
    result = Snapshot(value, buffer, generation);
    if (result != OBJC3_GENERIC_COLLECTION_OK) return;
    size = buffer->count;
    if (start > size || count > size - static_cast<std::size_t>(start))
      result = OBJC3_GENERIC_COLLECTION_OUT_OF_BOUNDS;
  });
  if (!found) return InspectArray(array);
  if (result != OBJC3_GENERIC_COLLECTION_OK) return result;
  return StoreView(array, generation, static_cast<std::size_t>(start),
                   static_cast<std::size_t>(count), out);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_view_release(objc3_generic_view_t view) try {
  return ViewStorage().Release(view, kViewKind) ? OBJC3_GENERIC_COLLECTION_OK
                                                 : InspectView(view);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_view_count(objc3_generic_view_t view,
    std::uint64_t *out) try {
  if (!IsAligned(out, alignof(std::uint64_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = 0;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = ViewStorage().With(view, kViewKind, [&](View &value) {
    std::shared_ptr<Buffer> buffer;
    result = ViewSnapshot(value, buffer);
    if (result == OBJC3_GENERIC_COLLECTION_OK) *out = value.count;
  });
  return found ? result : InspectView(view);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_view_get(objc3_generic_view_t view,
    std::uint64_t index, void *out) try {
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = ViewStorage().With(view, kViewKind, [&](View &value) {
    if (index >= value.count) { result = OBJC3_GENERIC_COLLECTION_OUT_OF_BOUNDS; return; }
    std::shared_ptr<Buffer> buffer;
    result = ViewSnapshot(value, buffer);
    if (result == OBJC3_GENERIC_COLLECTION_OK)
      result = GetFromBuffer(buffer, value.start + static_cast<std::size_t>(index), out);
    if (result == OBJC3_GENERIC_COLLECTION_OK) {
      std::shared_ptr<Buffer> after_copy;
      result = ViewSnapshot(value, after_copy);
      if (result != OBJC3_GENERIC_COLLECTION_OK && out) {
        try { buffer->ops.destroy(out); } catch (...) {}
      }
    }
  });
  return found ? result : InspectView(view);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_array_iterator(objc3_generic_array_t array,
    objc3_generic_iterator_t *out) try {
  if (!IsAligned(out, alignof(objc3_generic_iterator_t)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *out = nullptr;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  std::uint64_t generation = 0;
  bool found = ArrayStorage().With(array, kArrayKind, [&](Array &value) {
    std::shared_ptr<Buffer> buffer;
    result = Snapshot(value, buffer, generation);
  });
  if (!found) return InspectArray(array);
  if (result != OBJC3_GENERIC_COLLECTION_OK) return result;
  return StoreIterator(array, generation, out);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_iterator_release(objc3_generic_iterator_t iterator) try {
  return IteratorStorage().Release(iterator, kIteratorKind)
      ? OBJC3_GENERIC_COLLECTION_OK : InspectIterator(iterator);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }
extern "C" Status objc3_generic_iterator_next(objc3_generic_iterator_t iterator,
    int *has_value, void *out) try {
  if (!IsAligned(has_value, alignof(int)))
    return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  *has_value = 0;
  Status result = OBJC3_GENERIC_COLLECTION_STALE_HANDLE;
  bool found = IteratorStorage().With(iterator, kIteratorKind, [&](Iterator &value) {
    std::size_t position = 0;
    {
      std::lock_guard<std::mutex> lock(value.mutex);
      position = value.position;
    }
    std::shared_ptr<Buffer> buffer;
    std::uint64_t generation = 0;
    bool owner_found = ArrayStorage().With(value.owner, kArrayKind, [&](Array &array) {
      result = Snapshot(array, buffer, generation);
    });
    if (!owner_found) { result = InspectArray(value.owner); return; }
    if (result != OBJC3_GENERIC_COLLECTION_OK) return;
    if (generation != value.generation) {
      result = OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION; return;
    }
    if (position >= buffer->count) { result = OBJC3_GENERIC_COLLECTION_OK; return; }
    result = GetFromBuffer(buffer, position, out);
    if (result != OBJC3_GENERIC_COLLECTION_OK) return;
    std::shared_ptr<Buffer> current;
    std::uint64_t after_copy_generation = 0;
    bool still_live = ArrayStorage().With(value.owner, kArrayKind, [&](Array &array) {
      result = Snapshot(array, current, after_copy_generation);
    });
    if (!still_live || result != OBJC3_GENERIC_COLLECTION_OK ||
        after_copy_generation != value.generation) {
      if (out) { try { buffer->ops.destroy(out); } catch (...) {} }
      *has_value = 0;
      result = OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION;
      return;
    }
    bool conflict = false;
    {
      std::lock_guard<std::mutex> lock(value.mutex);
      conflict = value.position != position;
      if (!conflict) ++value.position;
    }
    // Copy/destruction and owner checks stay outside the iterator lock. The
    // owner generation check is the successful read's linearization point;
    // only then is the cursor advanced if no concurrent next() won first.
    if (conflict) {
      if (out) { try { buffer->ops.destroy(out); } catch (...) {} }
      result = OBJC3_GENERIC_COLLECTION_CONCURRENT_ADVANCE;
      return;
    }
    *has_value = 1;
  });
  return found ? result : InspectIterator(iterator);
} catch (const std::bad_alloc &) { return OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY; }
  catch (...) { return OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT; }

#ifdef OBJC3_GENERIC_ARRAY_STORAGE_TESTING
extern "C" void objc3_test_array_storage_reset() {
  metadata_slots_copied = 0; nodes_allocated = 0; chunks_allocated = 0;
}
extern "C" std::uint64_t objc3_test_array_metadata_slots() { return metadata_slots_copied; }
extern "C" std::uint64_t objc3_test_array_nodes() { return nodes_allocated; }
extern "C" std::uint64_t objc3_test_array_chunks() { return chunks_allocated; }
#endif
