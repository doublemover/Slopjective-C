#include "runtime/classes/receiver_identity.h"

#include "runtime/dispatch/dispatch_family.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/runtime_instance_records.h"

namespace objc3c::runtime {

namespace {

constexpr std::uint64_t kReceiverIdentityBase = 1024;
constexpr std::uint64_t kReceiverIdentityStride = 17;

}  // namespace

std::uint64_t RuntimeReceiverIdentityBase() {
  return kReceiverIdentityBase;
}

std::uint64_t RuntimeReceiverIdentityStride() {
  return kReceiverIdentityStride;
}

std::uint64_t BuildReceiverBaseIdentity(std::size_t ordinal) {
  return kReceiverIdentityBase +
         static_cast<std::uint64_t>(ordinal) * kReceiverIdentityStride;
}

std::uint64_t BuildInstanceReceiverIdentity(std::uint64_t base_identity) {
  return base_identity + 1u;
}

std::uint64_t BuildClassReceiverIdentity(std::uint64_t base_identity) {
  return base_identity + 2u;
}

bool IsRuntimeReceiverBaseIdentity(std::uint64_t base_identity) {
  return base_identity >= kReceiverIdentityBase &&
         ((base_identity - kReceiverIdentityBase) %
          kReceiverIdentityStride) == 0;
}

bool DecodeReceiverIdentity(const RuntimeState &state, int receiver,
                            std::uint64_t &base_identity,
                            DispatchFamily &family,
                            std::uint64_t &normalized_receiver_identity) {
  if (receiver <= 0) {
    return false;
  }
  const auto runtime_instance_it =
      state.runtime_instances_by_receiver.find(receiver);
  if (runtime_instance_it != state.runtime_instances_by_receiver.end()) {
    base_identity = runtime_instance_it->second.base_identity;
    family = DispatchFamily::Instance;
    normalized_receiver_identity =
        runtime_instance_it->second.normalized_receiver_identity != 0u
            ? runtime_instance_it->second.normalized_receiver_identity
            : BuildInstanceReceiverIdentity(base_identity);
    return true;
  }
  const std::int64_t signed_receiver = receiver;
  if (signed_receiver <
      static_cast<std::int64_t>(RuntimeReceiverIdentityBase())) {
    return false;
  }
  const std::int64_t delta =
      signed_receiver - static_cast<std::int64_t>(RuntimeReceiverIdentityBase());
  const std::int64_t ordinal =
      delta / static_cast<std::int64_t>(RuntimeReceiverIdentityStride());
  const std::int64_t salt =
      delta % static_cast<std::int64_t>(RuntimeReceiverIdentityStride());
  if (ordinal < 0) {
    return false;
  }
  base_identity = static_cast<std::uint64_t>(
      static_cast<std::int64_t>(RuntimeReceiverIdentityBase()) +
      ordinal * static_cast<std::int64_t>(RuntimeReceiverIdentityStride()));
  switch (salt) {
    case 0:
    case 2:
      family = DispatchFamily::Class;
      normalized_receiver_identity = BuildClassReceiverIdentity(base_identity);
      return true;
    case 1:
      family = DispatchFamily::Instance;
      normalized_receiver_identity =
          BuildInstanceReceiverIdentity(base_identity);
      return true;
    default:
      return false;
  }
}

const RealizedClassNode *FindRealizedClassNodeByBaseIdentityUnlocked(
    const RuntimeState &state, std::uint64_t base_identity) {
  const auto class_name_it =
      state.realized_class_name_by_base_identity.find(base_identity);
  if (class_name_it == state.realized_class_name_by_base_identity.end()) {
    return nullptr;
  }
  const auto node_indexes_it =
      state.realized_class_node_indices_by_name.find(class_name_it->second);
  if (node_indexes_it == state.realized_class_node_indices_by_name.end()) {
    return nullptr;
  }
  for (const std::size_t node_index : node_indexes_it->second) {
    if (node_index >= state.realized_class_nodes.size()) {
      continue;
    }
    const RealizedClassNode &node = state.realized_class_nodes[node_index];
    if (node.base_identity == base_identity) {
      return &node;
    }
  }
  return nullptr;
}

}  // namespace objc3c::runtime
