#include "ir/objc3_ir_frontend_metadata_publication_runtime_object.h"

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_class_metaclass.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_member_table.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_pools_retention_closeout.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category.h"

void EmitObjc3IRRuntimeMetadataObjectPublicationNodes(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count, std::ostringstream &out) {
  EmitObjc3IRRuntimeLayoutPolicyMetadataNode(runtime_metadata_layout_policy, out);
  EmitObjc3IRRuntimeClassMetaclassMetadataNode(metadata, out);
  EmitObjc3IRRuntimeProtocolCategoryMetadataNode(metadata, out);
  EmitObjc3IRRuntimeMemberTableMetadataNode(metadata, out);
  EmitObjc3IRRuntimePoolRetentionCloseoutMetadataNodes(
      metadata,
      runtime_metadata_linker_anchor_symbol,
      runtime_metadata_discovery_root_symbol,
      selector_pool_global_count,
      runtime_string_pool_global_count,
      out);
}
