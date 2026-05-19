#pragma once

namespace objc3c::runtime {

bool RuntimeCatchFilterCanMatch(int error_value, int catch_all);
bool RuntimeCatchKindIsSupported(int catch_kind);
bool RuntimeCatchFilterMatches(int error_value, int catch_kind, int catch_all);

}  // namespace objc3c::runtime
