#include "clang_foundation/source_identity.h"
namespace objc3::clang_foundation {
const char *CompiledFrontendSourceIdentity() {
  return OBJC3C_FRONTEND_INPUT_SHA256;
}
}
