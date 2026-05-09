#pragma once

#include "ast/objc3_ast_declarations.h"
#include "pipeline/frontend_source_closure_replay_keys.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {

inline Objc3FrontendInteropForeignImportSourceClosureSummary
BuildInteropForeignImportSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendInteropForeignImportSourceClosureSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_foreign_declared) {
      ++summary.foreign_callable_sites;
      ++summary.interop_annotation_sites;
      if constexpr (requires { decl.is_prototype; }) {
        if (decl.is_prototype) {
          ++summary.extern_foreign_callable_sites;
        }
      } else if constexpr (requires { decl.has_body; }) {
        if (!decl.has_body) {
          ++summary.extern_foreign_callable_sites;
        }
      }
    }
    if (decl.objc_import_module_declared) {
      ++summary.import_module_annotation_sites;
      ++summary.interop_annotation_sites;
      if (!decl.objc_import_module_name.empty()) {
        ++summary.imported_module_name_sites;
      }
    }
    if (decl.objc_export_header_declared) {
      ++summary.export_header_annotation_sites;
      ++summary.interop_annotation_sites;
      if (!decl.objc_export_header_name.empty()) {
        ++summary.export_header_name_sites;
      }
    }
    if (decl.objc_mixed_image_declared) {
      ++summary.mixed_image_annotation_sites;
      ++summary.interop_annotation_sites;
      if (!decl.objc_mixed_image_name.empty()) {
        ++summary.mixed_image_name_sites;
      }
    }
    if (decl.objc_package_entry_declared) {
      ++summary.package_entry_annotation_sites;
      ++summary.interop_annotation_sites;
      if (!decl.objc_package_entry_name.empty()) {
        ++summary.package_entry_name_sites;
      }
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.foreign_declaration_source_supported = true;
  summary.imported_surface_source_supported = true;
  summary.interop_annotation_source_supported = true;
  summary.deterministic_handoff =
      summary.extern_foreign_callable_sites <= summary.foreign_callable_sites &&
      summary.imported_module_name_sites <=
          summary.import_module_annotation_sites &&
      summary.export_header_name_sites <=
          summary.export_header_annotation_sites &&
      summary.mixed_image_name_sites <=
          summary.mixed_image_annotation_sites &&
      summary.package_entry_name_sites <=
          summary.package_entry_annotation_sites &&
      summary.interop_annotation_sites ==
          summary.foreign_callable_sites +
              summary.import_module_annotation_sites +
              summary.export_header_annotation_sites +
              summary.mixed_image_annotation_sites +
              summary.package_entry_annotation_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildInteropForeignImportSourceClosureReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
