#include <objc3c/frontend_api.h>
#include <objc3c/runtime/objc3_runtime_services.h>
#include <objc3c/runtime/objc3_task.hpp>
#include <cstring>
#include <iostream>

objc3::task<int> embedded_value() {
  co_await objc3::task_yield;
  co_return 42;
}
objc3::task<int> embedded_transform(objc3::task_handle<int> input) {
  co_return (co_await input) + 1;
}

int main(int argc, char **argv) {
  if (argc != 2) return 2;
  objc3c_frontend_v3_options options{};
  options.struct_size = sizeof(options);
  options.abi_version = OBJC3C_FRONTEND_V3_ABI;
  options.base.struct_size = sizeof(options.base);
  options.base.abi_version = OBJC3C_FRONTEND_V2_ABI;
  options.base.input_path = "memory.c";
  options.base.source = "int compute(void) { return 42; }";
  options.base.source_size = std::strlen(options.base.source);
  options.compiler_sdk_root = argv[1];
  objc3c_frontend_v2_result *result = nullptr;
  int status = objc3c_frontend_v3_compile(&options, &result);
  bool good = status == 0 && result && objc3c_frontend_v2_result_success(result);
  objc3c_frontend_v2_result_destroy(result);
  if (!good) return 1;
  options.base.source = "int broken( {";
  options.base.source_size = std::strlen(options.base.source);
  result = nullptr;
  status = objc3c_frontend_v3_compile(&options, &result);
  good = status == 1 && result && !objc3c_frontend_v2_result_success(result);
  objc3c_frontend_v2_result_destroy(result);
  if (!good) return 1;
  objc3_executor_t *executor = nullptr;
  if (objc3_executor_create(1, &executor) != OBJC3_SERVICE_OK) return 1;
  int invoked = 0;
  if (objc3_executor_enqueue(executor, [](void *context) {
        ++*static_cast<int *>(context);
      }, &invoked) != OBJC3_SERVICE_OK) return 1;
  uint32_t count = 0;
  auto pumped = objc3_executor_pump(executor, 1, &count);
  if (pumped != OBJC3_SERVICE_OK || invoked != 1 || count != 1) return 1;
  {
    auto child = embedded_value().start(executor);
    auto parent = embedded_transform(child).start(executor);
    if (objc3_executor_pump(executor, 16, &count) != OBJC3_SERVICE_OK) return 1;
    if (parent.result() != 43) return 1;
  }
  objc3_executor_release(executor);
  std::cout << "installed embedding and runtime services: PASS\n";
}
