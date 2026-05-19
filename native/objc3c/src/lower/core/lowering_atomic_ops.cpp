#include "lower/core/lowering_atomic_ops.h"

namespace {

const char *AtomicMemoryOrderToken(Objc3AtomicMemoryOrder order) {
  switch (order) {
    case Objc3AtomicMemoryOrder::Relaxed:
      return kObjc3AtomicMemoryOrderRelaxed;
    case Objc3AtomicMemoryOrder::Acquire:
      return kObjc3AtomicMemoryOrderAcquire;
    case Objc3AtomicMemoryOrder::Release:
      return kObjc3AtomicMemoryOrderRelease;
    case Objc3AtomicMemoryOrder::AcqRel:
      return kObjc3AtomicMemoryOrderAcqRel;
    case Objc3AtomicMemoryOrder::SeqCst:
    default:
      // Unknown enum values stay fail-closed at the strongest source token.
      return kObjc3AtomicMemoryOrderSeqCst;
  }
}

}  // namespace

bool TryParseObjc3AtomicMemoryOrder(const std::string &token,
                                    Objc3AtomicMemoryOrder &order) {
  if (token == kObjc3AtomicMemoryOrderRelaxed) {
    order = Objc3AtomicMemoryOrder::Relaxed;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderAcquire) {
    order = Objc3AtomicMemoryOrder::Acquire;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderRelease) {
    order = Objc3AtomicMemoryOrder::Release;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderAcqRel || token == "acquire_release") {
    order = Objc3AtomicMemoryOrder::AcqRel;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderSeqCst) {
    order = Objc3AtomicMemoryOrder::SeqCst;
    return true;
  }
  // Unknown source spellings are rejected instead of lowered permissively.
  return false;
}

const char *Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder order) {
  switch (order) {
    case Objc3AtomicMemoryOrder::Relaxed:
      return "monotonic";
    case Objc3AtomicMemoryOrder::Acquire:
      return "acquire";
    case Objc3AtomicMemoryOrder::Release:
      return "release";
    case Objc3AtomicMemoryOrder::AcqRel:
      return "acq_rel";
    case Objc3AtomicMemoryOrder::SeqCst:
    default:
      // Unknown enum values stay fail-closed at the strongest LLVM ordering.
      return "seq_cst";
  }
}

std::string Objc3AtomicMemoryOrderMappingReplayKey() {
  return std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Relaxed)) +
         "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Relaxed) +
         ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Acquire)) +
         "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Acquire) +
         ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Release)) +
         "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Release) +
         ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::AcqRel)) +
         "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::AcqRel) +
         ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::SeqCst)) +
         "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::SeqCst);
}
