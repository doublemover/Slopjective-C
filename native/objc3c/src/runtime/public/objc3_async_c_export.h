#ifndef OBJC3_ASYNC_C_EXPORT_H
#define OBJC3_ASYNC_C_EXPORT_H

#include "objc3_async_c.h"

#if defined(__OBJC__) && defined(__BLOCKS__)

#ifdef __cplusplus
#define __OBJC3_ASYNC_C_ZERO {}
#else
#define __OBJC3_ASYNC_C_ZERO {0}
#endif

#define __OBJC3_ASYNC_C_LOAD_VALUE(Result)                                      \
  (*(const Result *)__objc3_outcome->payload)
#define __OBJC3_ASYNC_C_LOAD_REFERENCE(Result)                                  \
  __extension__ ({                                                            \
    __unsafe_unretained Result __objc3_c_raw_reference;                        \
    __builtin_memcpy(&__objc3_c_raw_reference, __objc3_outcome->payload,         \
                     sizeof(__objc3_c_raw_reference));                         \
    __objc3_c_raw_reference;                                                  \
  })

#define __OBJC3_ASYNC_C_DEFINE(Name, Result, Frame, Checked, Executor, Actor,   \
                              Kind, Lifetime, Load, ...)                       \
  __OBJC3_ASYNC_C_LINKAGE void Name##_async_c(                                  \
      __VA_OPT__(__VA_ARGS__,) void * _Nullable __objc3_context,                \
      Name##_completion_t _Nonnull __objc3_completion) {                        \
    if (!__objc3_completion) __builtin_trap();                                 \
    typedef Result __objc3_result_type;                                        \
    objc3_async_frame __objc3_frame = (Frame);                                  \
    __objc3_async_c_check_result(&__objc3_frame, Kind,                           \
        __builtin_objc3_type_identity((Lifetime __objc3_result_type *)0),       \
        sizeof(__objc3_result_type), __alignof__(__objc3_result_type));          \
    __objc3_async_c_submit(&__objc3_frame, (Executor), (Actor), (Checked),      \
        ^(const objc3_completion *__objc3_outcome) {                            \
          const __objc3_result_type __objc3_c_empty = __OBJC3_ASYNC_C_ZERO;      \
          const __objc3_result_type __objc3_c_callback_value =                 \
              __objc3_outcome->kind == OBJC3_COMPLETION_SUCCESS                \
                  ? Load(__objc3_result_type) : __objc3_c_empty;               \
          __objc3_completion(__objc3_context,                                  \
              (objc_async_completion_status_t)__objc3_outcome->kind,           \
              __objc3_c_callback_value,                                        \
              __objc3_async_c_completion_error(__objc3_outcome));               \
        });                                                                    \
  }

/* Frame is the native async call with the original arguments forwarded.
 * Checked is 1 for an async throws entry and 0 for an async entry. Executor
 * and Actor select an explicit destination, or pass null for both to use
 * the global executor. Use an ABI-stable C value or a typed object/Block. */
#define OBJC3_ASYNC_C_DEFINE_VALUE(Name, Result, Frame, Checked, Executor, Actor, ...) \
  __OBJC3_ASYNC_C_DEFINE(Name, Result, Frame, Checked, Executor, Actor,           \
      OBJC3_ASYNC_VALUE, , __OBJC3_ASYNC_C_LOAD_VALUE __VA_OPT__(, __VA_ARGS__))

#define OBJC3_ASYNC_C_DEFINE_OBJECT(Name, Result, Frame, Checked, Executor, Actor, ...) \
  __OBJC3_ASYNC_C_DEFINE(Name, Result, Frame, Checked, Executor, Actor,            \
      OBJC3_ASYNC_OBJECT, __unsafe_unretained,                                   \
      __OBJC3_ASYNC_C_LOAD_REFERENCE __VA_OPT__(, __VA_ARGS__))

#define OBJC3_ASYNC_C_DEFINE_BLOCK(Name, Result, Frame, Checked, Executor, Actor, ...) \
  __OBJC3_ASYNC_C_DEFINE(Name, Result, Frame, Checked, Executor, Actor,           \
      OBJC3_ASYNC_BLOCK, __unsafe_unretained,                                   \
      __OBJC3_ASYNC_C_LOAD_REFERENCE __VA_OPT__(, __VA_ARGS__))

#define OBJC3_ASYNC_C_DEFINE_VOID(Name, Frame, Checked, Executor, Actor, ...)    \
  __OBJC3_ASYNC_C_LINKAGE void Name##_async_c(                                  \
      __VA_OPT__(__VA_ARGS__,) void * _Nullable __objc3_context,                \
      Name##_completion_t _Nonnull __objc3_completion) {                        \
    if (!__objc3_completion) __builtin_trap();                                 \
    objc3_async_frame __objc3_frame = (Frame);                                  \
    __objc3_async_c_check_result(&__objc3_frame, OBJC3_ASYNC_VOID,               \
        __builtin_objc3_type_identity((void *)0), 0, 1);                         \
    __objc3_async_c_submit(&__objc3_frame, (Executor), (Actor), (Checked),      \
        ^(const objc3_completion *__objc3_outcome) {                            \
          __objc3_completion(__objc3_context,                                  \
              (objc_async_completion_status_t)__objc3_outcome->kind,           \
              __objc3_async_c_completion_error(__objc3_outcome));               \
        });                                                                    \
  }

#endif
#endif
