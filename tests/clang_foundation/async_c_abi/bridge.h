#ifndef ASYNC_C_ABI_BRIDGE_H
#define ASYNC_C_ABI_BRIDGE_H

#include "objc3_runtime_services.h"

struct bridge_pair {
  int left;
  double right;
};
struct __attribute__((aligned(256))) bridge_wide {
  unsigned char bytes[256];
};
typedef __complex__ double bridge_complex;

#ifdef __cplusplus
extern "C" {
#endif
objc3_async_frame bridge_c_int(int value, int mode, int *destroyed);
objc3_async_frame bridge_c_pair(struct bridge_pair, int *destroyed);
objc3_async_frame bridge_c_wide(int *destroyed);
objc3_async_frame bridge_c_complex(bridge_complex, int *destroyed);
objc3_async_frame bridge_c_void(int *destroyed);
objc3_async_frame bridge_c_chain(objc3_async_frame *child, int *destroyed);
int bridge_c_copy_count(void);
int bridge_c_consume_cpp(objc3_executor_t *executor);
objc3_async_frame bridge_cpp_int(int value);
objc3_async_frame bridge_cpp_pair(struct bridge_pair);
objc3_async_frame bridge_cpp_void(void);
#ifdef __OBJC__
typedef int (^bridge_block)(int);
objc3_async_frame bridge_c_object(int value, int nil_value, int *destroyed);
objc3_async_frame bridge_c_block(int value, int nil_value, int *destroyed);
objc3_async_frame bridge_cpp_object(int value);
objc3_async_frame bridge_cpp_block(int value);
int bridge_object_number(void *value);
int bridge_object_destructions(void);
#endif
#ifdef __cplusplus
}
#endif

#endif
