#include "ir/objc3_ir_runtime_object_metadata_emission.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_runtime_object_metadata_symbols.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

#include "objc3_ir_runtime_object_metadata_protocol_bundle_emission.inc"
#include "objc3_ir_runtime_object_metadata_category_bundle_emission.inc"
