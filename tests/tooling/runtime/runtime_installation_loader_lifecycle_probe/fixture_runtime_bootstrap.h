#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_FIXTURE_RUNTIME_BOOTSTRAP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_FIXTURE_RUNTIME_BOOTSTRAP_H_

#include "runtime/public/objc3_runtime_api.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstddef>
#include <cstdint>

#ifndef OBJC3_RUNTIME_FIXTURE_MODULE_NAME
#error "OBJC3_RUNTIME_FIXTURE_MODULE_NAME must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_TRANSLATION_UNIT_IDENTITY_KEY
#error "OBJC3_RUNTIME_FIXTURE_TRANSLATION_UNIT_IDENTITY_KEY must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_REGISTRATION_ORDER_ORDINAL
#error "OBJC3_RUNTIME_FIXTURE_REGISTRATION_ORDER_ORDINAL must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT
#error "OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT
#error "OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT
#error "OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT
#error "OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT
#error "OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT must be provided by the generated fixture config"
#endif

namespace objc3c::runtime::installation_loader_lifecycle_probe {

template <std::size_t EntryCount>
struct ProbeAggregate {
  std::uint64_t count;
  const void *entries[EntryCount];
};

template <std::size_t EntryCount>
const objc3_runtime_pointer_aggregate *RuntimeAggregate(
    const ProbeAggregate<EntryCount> &aggregate) {
  return reinterpret_cast<const objc3_runtime_pointer_aggregate *>(&aggregate);
}

struct FixtureRuntimeBootstrap {
  int class_descriptor_slots[4] = {1, 2, 3, 4};
  int protocol_descriptor_slots[2] = {5, 6};
  int category_descriptor_slots[2] = {7, 8};
  int discovery_padding_slots[2] = {9, 10};
  unsigned char image_local_init_state = 0;

  objc3_runtime_image_descriptor compiled_image_descriptor{
      OBJC3_RUNTIME_FIXTURE_MODULE_NAME,
      OBJC3_RUNTIME_FIXTURE_TRANSLATION_UNIT_IDENTITY_KEY,
      OBJC3_RUNTIME_FIXTURE_REGISTRATION_ORDER_ORDINAL,
      OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT,
      OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT,
      OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT,
      OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT,
      OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT,
  };

  ProbeAggregate<4> class_root{
      OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT,
      {
          &class_descriptor_slots[0],
          &class_descriptor_slots[1],
          &class_descriptor_slots[2],
          &class_descriptor_slots[3],
      },
  };

  ProbeAggregate<2> protocol_root{
      OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT,
      {
          &protocol_descriptor_slots[0],
          &protocol_descriptor_slots[1],
      },
  };

  ProbeAggregate<2> category_root{
      OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT,
      {
          &category_descriptor_slots[0],
          &category_descriptor_slots[1],
      },
  };

  ProbeAggregate<1> property_root{
      OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT,
      {nullptr},
  };

  ProbeAggregate<1> ivar_root{
      OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT,
      {nullptr},
  };

  ProbeAggregate<6> valid_discovery_root{
      6u,
      {
          &compiled_image_descriptor,
          &class_root,
          &protocol_root,
          &category_root,
          &property_root,
          &ivar_root,
      },
  };

  const void *valid_linker_anchor_target = &valid_discovery_root;
  const void *invalid_linker_anchor_target = &class_root;

  ProbeAggregate<6> invalid_discovery_root{
      6u,
      {
          &compiled_image_descriptor,
          &class_root,
          &protocol_root,
          &property_root,
          &ivar_root,
          &discovery_padding_slots[0],
      },
  };

  objc3_runtime_registration_table MakeInvalidAnchorTable() {
    return {
        2u,
        12u,
        &compiled_image_descriptor,
        RuntimeAggregate(valid_discovery_root),
        &invalid_linker_anchor_target,
        RuntimeAggregate(class_root),
        RuntimeAggregate(protocol_root),
        RuntimeAggregate(category_root),
        RuntimeAggregate(property_root),
        RuntimeAggregate(ivar_root),
        nullptr,
        nullptr,
        nullptr,
        &image_local_init_state,
    };
  }

  objc3_runtime_registration_table MakeInvalidDiscoveryRootTable() {
    return {
        2u,
        12u,
        &compiled_image_descriptor,
        RuntimeAggregate(invalid_discovery_root),
        &valid_linker_anchor_target,
        RuntimeAggregate(class_root),
        RuntimeAggregate(protocol_root),
        RuntimeAggregate(category_root),
        RuntimeAggregate(property_root),
        RuntimeAggregate(ivar_root),
        nullptr,
        nullptr,
        nullptr,
        &image_local_init_state,
    };
  }
};

}  // namespace objc3c::runtime::installation_loader_lifecycle_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_FIXTURE_RUNTIME_BOOTSTRAP_H_
