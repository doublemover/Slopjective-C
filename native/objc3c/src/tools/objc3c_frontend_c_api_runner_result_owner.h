#pragma once

#include "libobjc3c_frontend/c_api.h"

class FrontendCApiCompileResultOwner {
 public:
  FrontendCApiCompileResultOwner() = default;
  ~FrontendCApiCompileResultOwner();

  FrontendCApiCompileResultOwner(const FrontendCApiCompileResultOwner &) =
      delete;
  FrontendCApiCompileResultOwner &operator=(
      const FrontendCApiCompileResultOwner &) = delete;

  objc3c_frontend_c_compile_result_t *out_param();
  const objc3c_frontend_c_compile_result_t &view() const;

 private:
  objc3c_frontend_c_compile_result_t result_ = {};
};
