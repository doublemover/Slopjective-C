#pragma once

#include "probe_state.h"
#include "runtime/classes/metaclass_graph.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace metaclass_graph_root_class {

inline void CaptureFailClosedDiagnostics(FailClosedDiagnostics &diagnostics) {
  std::string reason;
  diagnostics.root_super_metadata_rejected =
      RuntimeMetaclassMetadataIsConsistent(
          "BrokenRoot", "class:BrokenRoot", "metaclass:BrokenRoot",
          "class:UnexpectedBase", "", true, &reason)
          ? 0
          : 1;
  diagnostics.root_super_metadata_reason = reason;

  reason.clear();
  diagnostics.subclass_missing_super_metadata_rejected =
      RuntimeMetaclassMetadataIsConsistent(
          "BrokenChild", "class:BrokenChild", "metaclass:BrokenChild",
          "class:RootObject", "", false, &reason)
          ? 0
          : 1;
  diagnostics.subclass_missing_super_metadata_reason = reason;

  reason.clear();
  diagnostics.subclass_metaclass_link_mismatch_rejected =
      RuntimeMetaclassSuperclassLinkIsConsistent(
          "BrokenChild", "class:RootObject", "metaclass:OtherRoot",
          "class:RootObject", "metaclass:RootObject", &reason)
          ? 0
          : 1;
  diagnostics.subclass_metaclass_link_mismatch_reason = reason;
}

}  // namespace metaclass_graph_root_class
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
