#include "tools/objc3c_frontend_c_api_runner_context.h"

FrontendCApiContextOwner::FrontendCApiContextOwner()
    : context_(objc3c_frontend_c_context_create()) {}

FrontendCApiContextOwner::~FrontendCApiContextOwner() {
  objc3c_frontend_c_context_destroy(context_);
}

bool FrontendCApiContextOwner::valid() const {
  return context_ != nullptr;
}

objc3c_frontend_c_context_t *FrontendCApiContextOwner::get() const {
  return context_;
}
