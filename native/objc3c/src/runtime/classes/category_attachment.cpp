#include "runtime/classes/category_attachment.h"

namespace objc3c::runtime {

bool RuntimeCategoryAttachmentIsMaterializable(const char *category_name,
                                               const char *class_name) {
  return category_name != nullptr && category_name[0] != '\0' &&
         class_name != nullptr && class_name[0] != '\0';
}

}  // namespace objc3c::runtime
