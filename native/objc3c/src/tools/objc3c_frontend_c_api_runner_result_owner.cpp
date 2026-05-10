#include "tools/objc3c_frontend_c_api_runner_result_owner.h"

FrontendCApiCompileResultOwner::~FrontendCApiCompileResultOwner() {
  objc3c_frontend_c_result_destroy(&result_);
}

objc3c_frontend_c_compile_result_t *FrontendCApiCompileResultOwner::out_param() {
  return &result_;
}

const objc3c_frontend_c_compile_result_t &FrontendCApiCompileResultOwner::view()
    const {
  return result_;
}
