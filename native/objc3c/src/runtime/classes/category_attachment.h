#pragma once

namespace objc3c::runtime {

struct RealizedClassNode;
struct RuntimeState;

bool RuntimeCategoryAttachmentIsMaterializable(const char *category_name,
                                               const char *class_name);
bool AttachRealizedCategoryRecordsUnlocked(RuntimeState &state,
                                           RealizedClassNode &node);

}  // namespace objc3c::runtime
