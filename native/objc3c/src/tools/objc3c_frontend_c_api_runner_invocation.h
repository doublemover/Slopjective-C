#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

class FrontendCApiContextOwner {
 public:
  FrontendCApiContextOwner();
  ~FrontendCApiContextOwner();

  FrontendCApiContextOwner(const FrontendCApiContextOwner &) = delete;
  FrontendCApiContextOwner &operator=(const FrontendCApiContextOwner &) =
      delete;

  bool valid() const;
  objc3c_frontend_c_context_t *get() const;

 private:
  objc3c_frontend_c_context_t *context_ = nullptr;
};

class FrontendCApiRunnerCompileInvocation {
 public:
  explicit FrontendCApiRunnerCompileInvocation(
      const FrontendCApiRunnerOptions &options);

  FrontendCApiRunnerCompileInvocation(
      const FrontendCApiRunnerCompileInvocation &) = delete;
  FrontendCApiRunnerCompileInvocation &operator=(
      const FrontendCApiRunnerCompileInvocation &) = delete;

  const objc3c_frontend_c_compile_options_t *compile_options() const;

 private:
  std::string input_path_text_;
  std::string out_dir_text_;
  std::string clang_path_text_;
  std::string llc_path_text_;
  const FrontendCApiRunnerOptions &runner_options_;
  objc3c_frontend_c_compile_options_t compile_options_ = {};

  void RefreshBorrowedPointers();
};
