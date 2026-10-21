#pragma once
int shared_helper(void);
extern int shared_global;
#ifdef __cplusplus
#define PRIVATE_TEMPLATE_BIAS 1
inline int private_initializer() { return PRIVATE_TEMPLATE_BIAS; }
template<class T> T private_template(T value) {
  static int local_bias = private_initializer();
  return value + local_bias;
}
template<int N> int private_variable = N + private_initializer();
thread_local int private_tls = private_initializer();
const int &private_reference = private_initializer();
namespace PrivateNamespace {
int private_namespace_global = 0;
int private_namespace_helper() { return private_namespace_global; }
}

#endif

#define PRIVATE_BIAS 0

#ifdef __OBJC__
#include <objc/runtime.h>
#include <objc/objc-arc.h>
id class_createInstance(Class type, size_t extra) __attribute__((ns_returns_retained));
#endif
