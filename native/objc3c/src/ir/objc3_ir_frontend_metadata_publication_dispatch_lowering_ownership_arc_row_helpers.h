#pragma once

#include <cstddef>
#include <initializer_list>
#include <iosfwd>

void EmitObjc3IRDispatchOwnershipArcCounterRow(
    const char *metadata_node_id, std::initializer_list<std::size_t> counters,
    bool deterministic_handoff, std::ostringstream &out);
