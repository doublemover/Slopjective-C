  enum class CallableBridgeAttributeResult {
    kNotMatched,
    kParsed,
    kFailed,
  };

#include "parse/objc3_parser_attributes_callable_bridge_core.inc"

#include "parse/objc3_parser_attributes_callable_bridge_macro.inc"

#include "parse/objc3_parser_attributes_callable_bridge_interop.inc"

#include "parse/objc3_parser_attributes_callable_bridge_abi.inc"

#include "parse/objc3_parser_attributes_callable_bridge_single_dispatch.inc"
#include "parse/objc3_parser_attributes_callable_bridge_optional_list.inc"
