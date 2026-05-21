#pragma once

#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <utility>
#include <vector>

namespace objc3c::runtime::stdlib_runtime {

inline constexpr std::uint32_t kStdlibRuntimeAbiVersion = 1;
inline constexpr int kHandleKindFactor = 1000000;
inline constexpr int kHandleGenerationFactor = 10000;
inline constexpr int kMaxHandleKind = 99;
inline constexpr int kMaxHandleGeneration = 99;
inline constexpr int kMaxHandleSlot = 9999;
inline constexpr int kMaxStorageElements = 1048576;

enum class HandleTableOwner : std::uint32_t {
  Text = 1,
  Collections = 2,
};

enum class DescriptorKind : std::uint32_t {
  Unknown = 0,
  TextLiteral = 1,
  TextOwnedUtf8 = 2,
  TextBuilder = 3,
  TextScalarIterator = 4,
  CollectionImmutableArray = 10,
  CollectionMutableArray = 11,
  CollectionSlice = 12,
  CollectionMap = 13,
  CollectionSet = 14,
  CollectionIterator = 15,
  CollectionDescriptor = 16,
  ReservedNormalizedText = 20,
  ReservedCollatedText = 21,
  Malformed = 98,
  Stale = 99,
};

enum class LookupStatus {
  Ok,
  InvalidHandle,
  CrossKind,
  StaleHandle,
};

struct RecordHeader {
  HandleTableOwner owner = HandleTableOwner::Text;
  DescriptorKind descriptor_kind = DescriptorKind::Unknown;
  std::uint32_t abi_version = kStdlibRuntimeAbiVersion;
  std::uint64_t reset_generation = 0;
  std::uint32_t handle_generation = 1;
  std::uint64_t mutation_generation = 0;
  bool valid = true;
};

struct DecodedHandle {
  int raw = 0;
  DescriptorKind descriptor_kind = DescriptorKind::Unknown;
  int generation = 0;
  int slot = 0;
  bool well_formed = false;
};

inline int DescriptorKindCode(DescriptorKind kind) {
  return static_cast<int>(kind);
}

inline DescriptorKind DescriptorKindFromCode(int code) {
  switch (code) {
    case 1:
      return DescriptorKind::TextLiteral;
    case 2:
      return DescriptorKind::TextOwnedUtf8;
    case 3:
      return DescriptorKind::TextBuilder;
    case 4:
      return DescriptorKind::TextScalarIterator;
    case 10:
      return DescriptorKind::CollectionImmutableArray;
    case 11:
      return DescriptorKind::CollectionMutableArray;
    case 12:
      return DescriptorKind::CollectionSlice;
    case 13:
      return DescriptorKind::CollectionMap;
    case 14:
      return DescriptorKind::CollectionSet;
    case 15:
      return DescriptorKind::CollectionIterator;
    case 16:
      return DescriptorKind::CollectionDescriptor;
    case 20:
      return DescriptorKind::ReservedNormalizedText;
    case 21:
      return DescriptorKind::ReservedCollatedText;
    case 98:
      return DescriptorKind::Malformed;
    case 99:
      return DescriptorKind::Stale;
    default:
      return DescriptorKind::Unknown;
  }
}

inline int EncodeHandle(DescriptorKind kind, int generation, int slot) {
  const int kind_code = DescriptorKindCode(kind);
  if (kind_code <= 0 || kind_code > kMaxHandleKind || generation <= 0 ||
      generation > kMaxHandleGeneration || slot <= 0 ||
      slot > kMaxHandleSlot) {
    return 0;
  }
  return kind_code * kHandleKindFactor +
         generation * kHandleGenerationFactor + slot;
}

inline DecodedHandle DecodeHandle(int handle) {
  DecodedHandle decoded;
  decoded.raw = handle;
  if (handle <= 0) {
    return decoded;
  }
  const int kind_code = handle / kHandleKindFactor;
  const int remainder = handle % kHandleKindFactor;
  const int generation = remainder / kHandleGenerationFactor;
  const int slot = remainder % kHandleGenerationFactor;
  const DescriptorKind kind = DescriptorKindFromCode(kind_code);
  decoded.descriptor_kind = kind;
  decoded.generation = generation;
  decoded.slot = slot;
  decoded.well_formed = kind != DescriptorKind::Unknown && generation > 0 &&
                        generation <= kMaxHandleGeneration && slot > 0 &&
                        slot <= kMaxHandleSlot;
  return decoded;
}

inline bool IsAcceptedKind(
    DescriptorKind kind,
    std::initializer_list<DescriptorKind> accepted_kinds) {
  return std::find(accepted_kinds.begin(), accepted_kinds.end(), kind) !=
         accepted_kinds.end();
}

inline int NextHandleGeneration(int current_generation) {
  if (current_generation >= kMaxHandleGeneration) {
    return 1;
  }
  return current_generation + 1;
}

inline bool AddWouldOverflowInt(int left, int right) {
  return (right > 0 && left > std::numeric_limits<int>::max() - right) ||
         (right < 0 && left < std::numeric_limits<int>::min() - right);
}

inline bool CountExceedsStorageCapacity(int count) {
  return count > kMaxStorageElements;
}

template <typename Record>
struct LookupResult {
  LookupStatus status = LookupStatus::InvalidHandle;
  DecodedHandle decoded;
  Record *record = nullptr;
};

template <typename Record>
class HandleTable {
 public:
  explicit HandleTable(HandleTableOwner owner) : owner_(owner) {}

  void Reset() {
    stale_record_count_ += LiveRecordCount();
    records_.clear();
    ++reset_generation_;
    current_generation_ = NextHandleGeneration(current_generation_);
  }

  bool CanAllocate() const {
    return records_.size() < static_cast<std::size_t>(kMaxHandleSlot);
  }

  int Store(DescriptorKind kind, Record record) {
    if (!CanAllocate()) {
      return 0;
    }
    record.header.owner = owner_;
    record.header.descriptor_kind = kind;
    record.header.abi_version = kStdlibRuntimeAbiVersion;
    record.header.reset_generation = reset_generation_;
    record.header.handle_generation =
        static_cast<std::uint32_t>(current_generation_);
    record.header.valid = true;
    records_.push_back(std::move(record));
    return EncodeHandle(kind, current_generation_,
                        static_cast<int>(records_.size()));
  }

  LookupResult<Record> Lookup(
      int handle,
      std::initializer_list<DescriptorKind> accepted_kinds) {
    LookupResult<Record> result;
    result.decoded = DecodeHandle(handle);
    if (!result.decoded.well_formed) {
      result.status = LookupStatus::InvalidHandle;
      return result;
    }
    if (!IsAcceptedKind(result.decoded.descriptor_kind, accepted_kinds)) {
      result.status = LookupStatus::CrossKind;
      return result;
    }
    if (result.decoded.generation != current_generation_) {
      result.status = LookupStatus::StaleHandle;
      return result;
    }
    const int slot = result.decoded.slot;
    if (slot <= 0 || slot > static_cast<int>(records_.size())) {
      result.status = LookupStatus::InvalidHandle;
      return result;
    }
    Record &record = records_[static_cast<std::size_t>(slot - 1)];
    if (!record.header.valid ||
        record.header.handle_generation !=
            static_cast<std::uint32_t>(result.decoded.generation)) {
      result.status = LookupStatus::StaleHandle;
      return result;
    }
    if (record.header.descriptor_kind != result.decoded.descriptor_kind) {
      result.status = LookupStatus::CrossKind;
      return result;
    }
    result.status = LookupStatus::Ok;
    result.record = &record;
    return result;
  }

  std::uint64_t reset_generation() const {
    return reset_generation_;
  }

  std::uint32_t handle_generation() const {
    return static_cast<std::uint32_t>(current_generation_);
  }

  int LiveRecordCount() const {
    return static_cast<int>(records_.size());
  }

  int RecordCount(DescriptorKind kind) const {
    return static_cast<int>(std::count_if(
        records_.begin(), records_.end(), [kind](const Record &record) {
          return record.header.valid && record.header.descriptor_kind == kind;
        }));
  }

  template <typename Callback>
  void ForEachLiveRecord(Callback callback) const {
    for (const Record &record : records_) {
      if (record.header.valid) {
        callback(record);
      }
    }
  }

  std::uint64_t stale_record_count() const {
    return stale_record_count_;
  }

 private:
  HandleTableOwner owner_;
  std::uint64_t reset_generation_ = 0;
  int current_generation_ = 1;
  std::uint64_t stale_record_count_ = 0;
  std::vector<Record> records_;
};

}  // namespace objc3c::runtime::stdlib_runtime
