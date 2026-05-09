#include "ir/objc3_ir_receiver_identity_contracts.h"

int NextNonZeroReceiverIdentityValue(std::size_t ordinal, int salt) {
  constexpr int kBase = 1024;
  constexpr int kStride = 17;
  return kBase + static_cast<int>(ordinal * kStride) + salt;
}

int BuildInstanceReceiverIdentityValue(int class_identity) {
  return class_identity == 0 ? 0 : class_identity + 1;
}

int BuildClassReceiverIdentityValue(int class_identity) {
  return class_identity == 0 ? 0 : class_identity + 2;
}
