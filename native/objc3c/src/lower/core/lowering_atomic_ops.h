#pragma once

#include <cstdint>
#include <string>

inline constexpr const char *kObjc3AtomicMemoryOrderRelaxed = "relaxed";
inline constexpr const char *kObjc3AtomicMemoryOrderAcquire = "acquire";
inline constexpr const char *kObjc3AtomicMemoryOrderRelease = "release";
inline constexpr const char *kObjc3AtomicMemoryOrderAcqRel = "acq_rel";
inline constexpr const char *kObjc3AtomicMemoryOrderSeqCst = "seq_cst";

enum class Objc3AtomicMemoryOrder : std::uint8_t {
  Relaxed = 0,
  Acquire = 1,
  Release = 2,
  AcqRel = 3,
  SeqCst = 4,
};

bool TryParseObjc3AtomicMemoryOrder(const std::string &token,
                                    Objc3AtomicMemoryOrder &order);
const char *Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder order);
std::string Objc3AtomicMemoryOrderMappingReplayKey();
