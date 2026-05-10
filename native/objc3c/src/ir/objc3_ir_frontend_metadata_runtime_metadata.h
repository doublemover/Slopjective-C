#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_frontend_metadata_runtime_bootstrap.h"
#include "ir/objc3_ir_frontend_metadata_runtime_bundles.h"
#include "ir/objc3_ir_frontend_metadata_runtime_export.h"
#include "ir/objc3_ir_frontend_metadata_runtime_member_storage.h"
#include "ir/objc3_ir_frontend_metadata_runtime_sections.h"
#include "ir/objc3_ir_frontend_metadata_runtime_source_closure.h"

struct Objc3IRFrontendRuntimeMetadata
    : Objc3IRFrontendRuntimeBootstrapMetadata,
      Objc3IRFrontendRuntimeExportMetadata,
      Objc3IRFrontendRuntimeMemberStorageMetadata,
      Objc3IRFrontendRuntimeSectionsMetadata,
      Objc3IRFrontendRuntimeSourceClosureMetadata {
  std::string runtime_metadata_source_ownership_contract_id;
  std::string runtime_metadata_source_schema;
  std::string runtime_metadata_ivar_source_model;
  std::size_t runtime_metadata_class_record_count = 0;
  std::size_t runtime_metadata_protocol_record_count = 0;
  std::size_t runtime_metadata_category_interface_record_count = 0;
  std::size_t runtime_metadata_category_implementation_record_count = 0;
  std::size_t runtime_metadata_property_record_count = 0;
  std::size_t runtime_metadata_method_record_count = 0;
  std::size_t runtime_metadata_ivar_record_count = 0;
  bool frontend_owns_runtime_metadata_source_records = false;
  bool runtime_metadata_source_records_ready_for_lowering = false;
  bool native_runtime_library_present = false;
  bool runtime_metadata_source_boundary_fail_closed = false;
  bool runtime_link_test_only = true;
  bool deterministic_runtime_metadata_source_schema = false;
};
