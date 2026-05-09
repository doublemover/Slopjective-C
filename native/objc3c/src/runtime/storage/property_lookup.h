#pragma once

namespace objc3c::runtime {

struct RealizedClassNode;
struct RealizedPropertyAccessor;
struct RuntimeState;

const RealizedPropertyAccessor *FindRuntimePropertyAccessorByNameUnlocked(
    RuntimeState &state,
    const RealizedClassNode &start_node,
    const char *property_name,
    const RealizedClassNode *&resolved_node,
    bool &inherited,
    bool &used_cache);

}  // namespace objc3c::runtime
