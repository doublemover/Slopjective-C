#include "runtime/objc3_runtime.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

namespace {

bool ParseUInt64(const char *text, std::uint64_t &value) {
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

std::string JsonEscape(const char *text) {
  if (text == nullptr) {
    return "";
  }
  std::string out;
  for (const unsigned char c : std::string(text)) {
    switch (c) {
      case '\\':
        out += "\\\\";
        break;
      case '"':
        out += "\\\"";
        break;
      case '\n':
        out += "\\n";
        break;
      case '\r':
        out += "\\r";
        break;
      case '\t':
        out += "\\t";
        break;
      default:
        out.push_back(static_cast<char>(c));
        break;
    }
  }
  return out;
}

struct OwnedSnapshot {
  std::uint64_t registered_image_count = 0;
  std::uint64_t registered_descriptor_total = 0;
  std::uint64_t next_expected_registration_order_ordinal = 0;
  std::uint64_t last_successful_registration_order_ordinal = 0;
  int last_registration_status = 0;
  std::string last_registered_module_name;
  std::string last_registered_translation_unit_identity_key;
  std::string last_rejected_module_name;
  std::string last_rejected_translation_unit_identity_key;
  std::uint64_t last_rejected_registration_order_ordinal = 0;
};

OwnedSnapshot CopySnapshot(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  OwnedSnapshot owned;
  owned.registered_image_count = snapshot.registered_image_count;
  owned.registered_descriptor_total = snapshot.registered_descriptor_total;
  owned.next_expected_registration_order_ordinal =
      snapshot.next_expected_registration_order_ordinal;
  owned.last_successful_registration_order_ordinal =
      snapshot.last_successful_registration_order_ordinal;
  owned.last_registration_status = snapshot.last_registration_status;
  owned.last_registered_module_name =
      snapshot.last_registered_module_name != nullptr
          ? snapshot.last_registered_module_name
          : "";
  owned.last_registered_translation_unit_identity_key =
      snapshot.last_registered_translation_unit_identity_key != nullptr
          ? snapshot.last_registered_translation_unit_identity_key
          : "";
  owned.last_rejected_module_name =
      snapshot.last_rejected_module_name != nullptr
          ? snapshot.last_rejected_module_name
          : "";
  owned.last_rejected_translation_unit_identity_key =
      snapshot.last_rejected_translation_unit_identity_key != nullptr
          ? snapshot.last_rejected_translation_unit_identity_key
          : "";
  owned.last_rejected_registration_order_ordinal =
      snapshot.last_rejected_registration_order_ordinal;
  return owned;
}

void PrintSnapshot(const char *name, const OwnedSnapshot &snapshot,
                   bool emit_comma) {
  std::cout << "\"" << name << "\":{"
            << "\"registered_image_count\":"
            << snapshot.registered_image_count << ","
            << "\"registered_descriptor_total\":"
            << snapshot.registered_descriptor_total << ","
            << "\"next_expected_registration_order_ordinal\":"
            << snapshot.next_expected_registration_order_ordinal << ","
            << "\"last_successful_registration_order_ordinal\":"
            << snapshot.last_successful_registration_order_ordinal << ","
            << "\"last_registration_status\":"
            << snapshot.last_registration_status << ","
            << "\"last_registered_module_name\":\""
            << JsonEscape(snapshot.last_registered_module_name.c_str()) << "\","
            << "\"last_registered_translation_unit_identity_key\":\""
            << JsonEscape(
                   snapshot.last_registered_translation_unit_identity_key.c_str())
            << "\","
            << "\"last_rejected_module_name\":\""
            << JsonEscape(snapshot.last_rejected_module_name.c_str()) << "\","
            << "\"last_rejected_translation_unit_identity_key\":\""
            << JsonEscape(
                   snapshot.last_rejected_translation_unit_identity_key.c_str())
            << "\","
            << "\"last_rejected_registration_order_ordinal\":"
            << snapshot.last_rejected_registration_order_ordinal << "}";
  if (emit_comma) {
    std::cout << ",";
  }
}

}  // namespace

int main(int argc, char **argv) {
  if (argc != 9) {
    return 2;
  }

  const char *module_name = argv[1];
  const char *identity_key = argv[2];
  std::uint64_t order = 0;
  std::uint64_t class_count = 0;
  std::uint64_t protocol_count = 0;
  std::uint64_t category_count = 0;
  std::uint64_t property_count = 0;
  std::uint64_t ivar_count = 0;
  if (!ParseUInt64(argv[3], order) || !ParseUInt64(argv[4], class_count) ||
      !ParseUInt64(argv[5], protocol_count) ||
      !ParseUInt64(argv[6], category_count) ||
      !ParseUInt64(argv[7], property_count) ||
      !ParseUInt64(argv[8], ivar_count)) {
    return 3;
  }

  objc3_runtime_reset_for_testing();

  const objc3_runtime_image_descriptor success_image{
      module_name,
      identity_key,
      order,
      class_count,
      protocol_count,
      category_count,
      property_count,
      ivar_count,
  };
  const objc3_runtime_image_descriptor duplicate_image = success_image;
  const std::string out_of_order_identity =
      std::string(identity_key) + "-out-of-order";
  const objc3_runtime_image_descriptor out_of_order_image{
      "out-of-order-module",
      out_of_order_identity.c_str(),
      order + 2,
      class_count,
      protocol_count,
      category_count,
      property_count,
      ivar_count,
  };
  const objc3_runtime_image_descriptor invalid_image{
      "invalid-module",
      "",
      0,
      class_count,
      protocol_count,
      category_count,
      property_count,
      ivar_count,
  };

  const int success_status = objc3_runtime_register_image(&success_image);
  objc3_runtime_registration_state_snapshot after_success_raw{};
  if (objc3_runtime_copy_registration_state_for_testing(&after_success_raw) !=
      0) {
    return 4;
  }
  const OwnedSnapshot after_success = CopySnapshot(after_success_raw);

  const int duplicate_status = objc3_runtime_register_image(&duplicate_image);
  objc3_runtime_registration_state_snapshot after_duplicate_raw{};
  if (objc3_runtime_copy_registration_state_for_testing(&after_duplicate_raw) !=
      0) {
    return 5;
  }
  const OwnedSnapshot after_duplicate = CopySnapshot(after_duplicate_raw);

  const int out_of_order_status =
      objc3_runtime_register_image(&out_of_order_image);
  objc3_runtime_registration_state_snapshot after_out_of_order_raw{};
  if (objc3_runtime_copy_registration_state_for_testing(
          &after_out_of_order_raw) !=
      0) {
    return 6;
  }
  const OwnedSnapshot after_out_of_order =
      CopySnapshot(after_out_of_order_raw);

  const int invalid_status = objc3_runtime_register_image(&invalid_image);
  objc3_runtime_registration_state_snapshot after_invalid_raw{};
  if (objc3_runtime_copy_registration_state_for_testing(&after_invalid_raw) !=
      0) {
    return 7;
  }
  const OwnedSnapshot after_invalid = CopySnapshot(after_invalid_raw);

  std::cout << "{"
            << "\"success_status\":" << success_status << ","
            << "\"duplicate_status\":" << duplicate_status << ","
            << "\"out_of_order_status\":" << out_of_order_status << ","
            << "\"invalid_status\":" << invalid_status << ","
            << "\"translation_unit_identity_key\":\""
            << JsonEscape(identity_key) << "\","
            << "\"translation_unit_registration_order_ordinal\":" << order
            << ","
            << "\"snapshots\":{";
  PrintSnapshot("after_success", after_success, true);
  PrintSnapshot("after_duplicate", after_duplicate, true);
  PrintSnapshot("after_out_of_order", after_out_of_order, true);
  PrintSnapshot("after_invalid", after_invalid, false);
  std::cout << "}}\n";
  return 0;
}
