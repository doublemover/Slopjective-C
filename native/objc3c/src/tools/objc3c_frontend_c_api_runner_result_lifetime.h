#pragma once

#include "libobjc3c_frontend/c_api.h"

struct FrontendCApiCompileResultGuard {
  objc3c_frontend_c_compile_result_t *result = nullptr;

  ~FrontendCApiCompileResultGuard();
};
