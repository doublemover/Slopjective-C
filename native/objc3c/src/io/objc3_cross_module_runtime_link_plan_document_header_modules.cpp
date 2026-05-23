#include "io/objc3_cross_module_runtime_link_plan_document_header_modules.h"

#include <ostream>

#include "io/objc3_process_json_helpers.h"

namespace {

void EmitReadyField(std::ostream &out, const char *name, bool ready) {
  out << "  \"" << name << "\": " << (ready ? "true" : "false") << ",\n";
}

}  // namespace

void EmitObjc3CrossModuleRuntimeLinkPlanHeaderModuleSections(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanSections &sections) {
  out << "  \"module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(sections.module_names_lexicographic,
                                      "    ")
      << ",\n"
      << "  \"module_image_count\": "
      << sections.module_names_lexicographic.size() << ",\n"
      << "  \"direct_import_input_count\": "
      << sections.direct_import_surface_artifact_paths.size() << ",\n"
      << "  \"error_handling_imported_module_count\": "
      << sections.imported_error_handling_module_names_lexicographic.size()
      << ",\n"
      << "  \"concurrency_actor_imported_module_count\": "
      << sections.imported_concurrency_actor_module_names_lexicographic.size()
      << ",\n"
      << "  \"scheduler_task_imported_module_count\": "
      << sections.imported_scheduler_task_module_names_lexicographic.size()
      << ",\n"
      << "  \"interop_ffi_imported_module_count\": "
      << sections.imported_interop_ffi_module_names_lexicographic.size()
      << ",\n"
      << "  \"foreign_abi_imported_module_count\": "
      << sections.imported_foreign_abi_module_names_lexicographic.size()
      << ",\n"
      << "  \"interop_header_module_bridge_imported_module_count\": "
      << sections
             .imported_interop_header_module_bridge_module_names_lexicographic
             .size()
      << ",\n"
      << "  \"metaprogramming_host_cache_imported_module_count\": "
      << sections.imported_metaprogramming_host_cache_module_names_lexicographic
             .size()
      << ",\n"
      << "  \"direct_import_surface_artifact_paths\": "
      << BuildIndentedStringArrayJson(
             sections.direct_import_surface_artifact_paths, "    ")
      << ",\n"
      << "  \"error_handling_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections.imported_error_handling_module_names_lexicographic,
             "    ")
      << ",\n"
      << "  \"concurrency_actor_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections.imported_concurrency_actor_module_names_lexicographic,
             "    ")
      << ",\n"
      << "  \"scheduler_task_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections.imported_scheduler_task_module_names_lexicographic,
             "    ")
      << ",\n"
      << "  \"interop_ffi_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections.imported_interop_ffi_module_names_lexicographic, "    ")
      << ",\n"
      << "  \"foreign_abi_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections.imported_foreign_abi_module_names_lexicographic, "    ")
      << ",\n"
      << "  \"interop_header_module_bridge_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections
                 .imported_interop_header_module_bridge_module_names_lexicographic,
             "    ")
      << ",\n"
      << "  \"metaprogramming_host_cache_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections
                 .imported_metaprogramming_host_cache_module_names_lexicographic,
             "    ")
      << ",\n";

  EmitReadyField(out,
                 "error_handling_cross_module_preservation_ready",
                 !sections.imported_error_handling_module_names_lexicographic
                      .empty());
  EmitReadyField(
      out,
      "concurrency_actor_cross_module_isolation_ready",
      !sections.imported_concurrency_actor_module_names_lexicographic.empty());
  EmitReadyField(
      out,
      "actor_mailbox_cross_module_runtime_expansion_ready",
      !sections.imported_concurrency_actor_module_names_lexicographic.empty());
  EmitReadyField(
      out,
      "scheduler_task_cross_module_runtime_replay_ready",
      !sections.imported_scheduler_task_module_names_lexicographic.empty());
  EmitReadyField(out,
                 "interop_ffi_cross_module_packaging_ready",
                 !sections.imported_interop_ffi_module_names_lexicographic
                      .empty());
  EmitReadyField(out,
                 "foreign_abi_cross_module_runtime_closure_ready",
                 !sections.imported_foreign_abi_module_names_lexicographic
                      .empty());
  EmitReadyField(
      out,
      "interop_header_module_bridge_cross_module_packaging_ready",
      !sections.imported_interop_header_module_bridge_module_names_lexicographic
           .empty());
  EmitReadyField(
      out,
      "metaprogramming_host_cache_cross_module_preservation_ready",
      !sections.imported_metaprogramming_host_cache_module_names_lexicographic
           .empty());
  EmitReadyField(
      out,
      "macro_package_replay_cross_module_runtime_closure_ready",
      !sections.imported_metaprogramming_host_cache_module_names_lexicographic
           .empty());
}
