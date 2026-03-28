#include "runtime/objc3_runtime.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace {

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

}  // namespace

int main() {
  objc3_runtime_registration_state_snapshot snapshot{};
  const int status = objc3_runtime_copy_registration_state_for_testing(&snapshot);
  if (status != 0) {
    return 2;
  }

  std::cout << "{" 
            << "\"copy_status\":" << status << ","
            << "\"registered_image_count\":" << snapshot.registered_image_count
            << ","
            << "\"registered_descriptor_total\":" << snapshot.registered_descriptor_total
            << ","
            << "\"next_expected_registration_order_ordinal\":"
            << snapshot.next_expected_registration_order_ordinal << ","
            << "\"last_successful_registration_order_ordinal\":"
            << snapshot.last_successful_registration_order_ordinal << ","
            << "\"last_registration_status\":" << snapshot.last_registration_status
            << ","
            << "\"last_registered_module_name\":\""
            << JsonEscape(snapshot.last_registered_module_name) << "\","
            << "\"last_registered_translation_unit_identity_key\":\""
            << JsonEscape(snapshot.last_registered_translation_unit_identity_key)
            << "\","
            << "\"last_rejected_module_name\":\""
            << JsonEscape(snapshot.last_rejected_module_name) << "\","
            << "\"last_rejected_translation_unit_identity_key\":\""
            << JsonEscape(snapshot.last_rejected_translation_unit_identity_key)
            << "\","
            << "\"last_rejected_registration_order_ordinal\":"
            << snapshot.last_rejected_registration_order_ordinal << "}\n";
  return 0;
}
