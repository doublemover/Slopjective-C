#include "tools/objc3c_frontend_c_api_runner_result_lifetime.h"

FrontendCApiCompileResultGuard::~FrontendCApiCompileResultGuard() {
  objc3c_frontend_c_result_destroy(result);
}
