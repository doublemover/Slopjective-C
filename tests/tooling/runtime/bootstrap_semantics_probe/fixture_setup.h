#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_FIXTURE_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_FIXTURE_SETUP_H_

#include "runtime/public/objc3_runtime_api.h"

#include <cstdint>
#include <cstdlib>
#include <string>

namespace objc3c::runtime::bootstrap_semantics_probe {

inline bool ParseUInt64(const char *text, std::uint64_t &value) {
  if (text == nullptr || text[0] == '\0') {
    return false;
  }
  char *end = nullptr;
  const unsigned long long parsed = std::strtoull(text, &end, 10);
  if (end == nullptr || *end != '\0') {
    return false;
  }
  value = static_cast<std::uint64_t>(parsed);
  return true;
}

struct BootstrapFixture {
  const char *module_name = nullptr;
  const char *identity_key = nullptr;
  std::uint64_t order = 0;
  std::uint64_t class_count = 0;
  std::uint64_t protocol_count = 0;
  std::uint64_t category_count = 0;
  std::uint64_t property_count = 0;
  std::uint64_t ivar_count = 0;
  std::string out_of_order_identity;

  objc3_runtime_image_descriptor SuccessImage() const {
    return {
        module_name,
        identity_key,
        order,
        class_count,
        protocol_count,
        category_count,
        property_count,
        ivar_count,
    };
  }

  objc3_runtime_image_descriptor DuplicateImage() const {
    return SuccessImage();
  }

  objc3_runtime_image_descriptor OutOfOrderImage() const {
    return {
        "out-of-order-module",
        out_of_order_identity.c_str(),
        order + 2,
        class_count,
        protocol_count,
        category_count,
        property_count,
        ivar_count,
    };
  }

  objc3_runtime_image_descriptor InvalidImage() const {
    return {
        "invalid-module",
        "",
        0,
        class_count,
        protocol_count,
        category_count,
        property_count,
        ivar_count,
    };
  }
};

inline bool ParseBootstrapFixture(int argc, char **argv,
                                  BootstrapFixture &fixture) {
  if (argc != 9) {
    return false;
  }

  fixture.module_name = argv[1];
  fixture.identity_key = argv[2];
  if (!ParseUInt64(argv[3], fixture.order) ||
      !ParseUInt64(argv[4], fixture.class_count) ||
      !ParseUInt64(argv[5], fixture.protocol_count) ||
      !ParseUInt64(argv[6], fixture.category_count) ||
      !ParseUInt64(argv[7], fixture.property_count) ||
      !ParseUInt64(argv[8], fixture.ivar_count)) {
    return false;
  }

  fixture.out_of_order_identity =
      std::string(fixture.identity_key) + "-out-of-order";
  return true;
}

}  // namespace objc3c::runtime::bootstrap_semantics_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_FIXTURE_SETUP_H_
