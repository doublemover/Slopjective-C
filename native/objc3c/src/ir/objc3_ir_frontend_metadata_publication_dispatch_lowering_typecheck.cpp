#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_typecheck.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRDispatchLoweringTypecheckCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!8 = !{i64 " << static_cast<unsigned long long>(metadata.id_typecheck_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.class_typecheck_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.sel_typecheck_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.object_pointer_typecheck_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.id_class_sel_object_pointer_typecheck_sites_total)
      << ", i1 "
      << (metadata.deterministic_id_class_sel_object_pointer_typecheck_handoff ? 1 : 0) << "}\n";
}
