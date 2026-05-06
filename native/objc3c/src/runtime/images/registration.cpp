#include "runtime/images/registration.h"

#include "runtime/images/image_descriptor.h"

namespace objc3c::runtime {

bool RuntimeRegistrationTableShapeIsSupported(
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image) {
  return registration_table != nullptr &&
         registration_table->abi_version == 2 &&
         registration_table->pointer_field_count == 12 &&
         registration_table->image_descriptor != nullptr &&
         RuntimeImageDescriptorsMatch(registration_table->image_descriptor,
                                      image) &&
         registration_table->discovery_root != nullptr &&
         registration_table->linker_anchor != nullptr &&
         registration_table->class_descriptor_root != nullptr &&
         registration_table->protocol_descriptor_root != nullptr &&
         registration_table->category_descriptor_root != nullptr &&
         registration_table->property_descriptor_root != nullptr &&
         registration_table->ivar_descriptor_root != nullptr &&
         registration_table->image_local_init_state != nullptr;
}

}  // namespace objc3c::runtime
