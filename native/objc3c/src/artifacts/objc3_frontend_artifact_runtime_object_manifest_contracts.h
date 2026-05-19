#pragma once

#include <cstddef>

namespace objc3::artifacts::frontend {

inline constexpr const char *kRuntimeObjectExecutableRealizationRecordsContractId =
    "objc3c.executable.realization.records.v1";
inline constexpr const char *kRuntimeObjectClassRealizationContractId =
    "objc3c.runtime.class.realization.freeze.v1";
inline constexpr const char *kRuntimeObjectMetaclassGraphRootClassContractId =
    "objc3c.runtime.metaclass.graph.root.class.baseline.v1";
inline constexpr const char
    *kRuntimeObjectCategoryAttachmentProtocolConformanceContractId =
        "objc3c.runtime.category.attachment.protocol.conformance.v1";
inline constexpr const char *kRuntimeObjectPropertyMetadataReflectionContractId =
    "objc3c.runtime.property.metadata.reflection.v1";
inline constexpr const char
    *kRuntimeObjectBackedObjectOwnershipAttributeSurfaceContractId =
        "objc3c.runtime.backed.object.ownership.attribute.surface.v1";

struct Objc3RuntimeDispatchTableReflectionRecordLoweringFields {
  std::size_t message_send_sites = 0;
};

}  // namespace objc3::artifacts::frontend
