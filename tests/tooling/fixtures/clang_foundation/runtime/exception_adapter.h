#pragma once
#ifdef __cplusplus
extern "C" {
#endif
id adapter_make(int key) __attribute__((ns_returns_retained));
void adapter_cleaned(void);
void adapter_throw_c(int key);
void adapter_throw_cpp(int key, int mode);
int adapter_uncaught(void);
#ifdef __cplusplus
}
#endif
