#pragma once

namespace objc3c::runtime {

const char *RuntimeErrorCatchKindName(int catch_kind);
const char *RuntimeForeignExceptionKindName(int foreign_kind);
bool RuntimeForeignExceptionKindIsSupported(int foreign_kind);

}  // namespace objc3c::runtime
