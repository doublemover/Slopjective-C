#pragma once

#include "libobjc3c_frontend/c_api.h"

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
