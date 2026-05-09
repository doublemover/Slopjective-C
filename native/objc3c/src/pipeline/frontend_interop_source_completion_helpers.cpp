#include "pipeline/frontend_interop_source_completion_helpers.h"

#include "pipeline/frontend_source_closure_replay_keys.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
BuildInteropCppSwiftInteropAnnotationSourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_swift_name_declared) {
      ++summary.swift_name_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
      if (!decl.objc_swift_name.empty()) {
        ++summary.named_annotation_payload_sites;
      }
    }
    if (decl.objc_swift_private_declared) {
      ++summary.swift_private_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
    }
    if (decl.objc_cxx_name_declared) {
      ++summary.cpp_name_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
      if (!decl.objc_cxx_name.empty()) {
        ++summary.named_annotation_payload_sites;
      }
    }
    if (decl.objc_header_name_declared) {
      ++summary.header_name_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
      if (!decl.objc_header_name.empty()) {
        ++summary.named_annotation_payload_sites;
      }
    }
    if (decl.objc_abi_align_declared) {
      ++summary.abi_alignment_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
    }
    if (decl.objc_foreign_type_declared) {
      ++summary.foreign_type_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
      if (!decl.objc_foreign_type_name.empty()) {
        ++summary.named_annotation_payload_sites;
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

  summary.swift_annotation_source_supported = true;
  summary.cpp_annotation_source_supported = true;
  summary.interop_metadata_source_supported = true;
  summary.deterministic_handoff =
      summary.named_annotation_payload_sites ==
          summary.swift_name_annotation_sites +
              summary.cpp_name_annotation_sites +
              summary.header_name_annotation_sites +
              summary.foreign_type_annotation_sites &&
      summary.interop_metadata_annotation_sites ==
          summary.swift_name_annotation_sites +
              summary.swift_private_annotation_sites +
              summary.cpp_name_annotation_sites +
              summary.header_name_annotation_sites +
              summary.abi_alignment_annotation_sites +
              summary.foreign_type_annotation_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildInteropCppSwiftInteropAnnotationSourceCompletionReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
