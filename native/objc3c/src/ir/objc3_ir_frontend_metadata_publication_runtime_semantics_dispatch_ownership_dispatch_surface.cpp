#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_dispatch_surface.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRDispatchSurfaceClassificationMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!66 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_instance_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_class_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_super_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_direct_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_dynamic_sites)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_instance_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_class_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_super_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_direct_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_dynamic_entrypoint_family)
      << "\", i1 "
      << (metadata.deterministic_dispatch_surface_classification_handoff ? 1 : 0)
      << "}\n";
}
