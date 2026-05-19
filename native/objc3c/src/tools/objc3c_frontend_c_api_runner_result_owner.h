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

  objc3c_frontend_c_result_t **out_param();
  bool valid() const;
  const objc3c_frontend_c_compile_result_t &view() const;

 private:
  void RefreshSnapshot() const;

  objc3c_frontend_c_result_t *result_ = nullptr;
  mutable objc3c_frontend_c_compile_result_t snapshot_ = {};
};
