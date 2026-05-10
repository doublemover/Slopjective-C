#include "parse/objc3_message_send_profiles.h"

#include <sstream>

namespace objc3c::parse {

namespace {

#include "parse/objc3_message_send_runtime_profiles_form_key_serialization.inc"

}  // namespace

#include "parse/objc3_message_send_runtime_profiles_nil_receiver_folding.inc"

#include "parse/objc3_message_send_runtime_profiles_super_dispatch.inc"

#include "parse/objc3_message_send_runtime_profiles_method_family_semantics.inc"

#include "parse/objc3_message_send_runtime_profiles_runtime_link_host_link.inc"

}  // namespace objc3c::parse
