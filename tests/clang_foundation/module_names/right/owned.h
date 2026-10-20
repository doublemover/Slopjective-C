#pragma once
int shared_helper(void);
extern int shared_global;
#ifdef __cplusplus
#define PRIVATE_TEMPLATE_BIAS 2
template<class T> T private_template(T value) { return value + PRIVATE_TEMPLATE_BIAS; }
template<int N> int private_variable = N + PRIVATE_TEMPLATE_BIAS;
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
