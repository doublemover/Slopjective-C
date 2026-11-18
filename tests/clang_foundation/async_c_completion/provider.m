#include "api.h"
#include "objc3_async_c_export.h"

@protocol Error
@end

#ifdef __cplusplus
extern "C" {
#endif
extern id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
extern id object_dispose(__unsafe_unretained id);
extern Class object_getClass(id);
extern unsigned char class_conformsToProtocol(Class, Protocol *);
#ifdef __cplusplus
}
#endif

static int live_objects;
static int cancel_entries;

__attribute__((objc_root_class))
@interface CompletionProbe <Error> {
  Class isa;
}
+ (id)new;
- (void)dealloc;
@end

@implementation CompletionProbe
+ (id)new {
  id object = class_createInstance(self, 0);
  if (object) __atomic_add_fetch(&live_objects, 1, __ATOMIC_RELAXED);
  return object;
}
- (void)dealloc {
  __atomic_sub_fetch(&live_objects, 1, __ATOMIC_RELAXED);
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end

CompletionObject completion_make_object(void) { return [CompletionProbe new]; }
int completion_object_count(void) { return __atomic_load_n(&live_objects, __ATOMIC_RELAXED); }
int completion_cancel_entries(void) { return cancel_entries; }
int completion_error_conforms(id error) {
  Class type = objc3_async_c_error_is_cancellation((id<Error>)error)
      ? (Class)error : object_getClass(error);
  return class_conformsToProtocol(type, @protocol(Error));
}

static int next(int value) async { return value + 1; }
static CompletionRecord value_result(long first, int second) async {
  int adjusted = await next(second);
  CompletionRecord result = {{first, adjusted, 73, 91}};
  return result;
}
static void void_result(void) async { await next(1); }
static id object_result(id object) async { await next(1); return object; }
static CompletionBlock block_result(int value) async {
  int adjusted = await next(value);
  return ^(int extra) { return adjusted + extra; };
}
static int error_result(id<Error> error) async throws(id<Error>) {
  await next(1);
  __objc3_throw(error);
}
static int cancel_result(objc3_actor_t *actor) async { ++cancel_entries; return actor != 0; }

static objc3_executor_t *main_executor(void) {
  // The shared process-wide destination outlives every submitted callback.
  objc3_executor_t *executor = 0;
  if (objc3_executor_get_main(&executor) != OBJC3_SERVICE_OK) __builtin_trap();
  objc3_executor_release(executor);
  return executor;
}

OBJC3_ASYNC_C_DEFINE_VALUE(completion_value, CompletionRecord,
    value_result(first, second), 0, main_executor(), 0, long first, int second)
OBJC3_ASYNC_C_DEFINE_VOID(completion_void, void_result(), 0, main_executor(), 0)
OBJC3_ASYNC_C_DEFINE_OBJECT(completion_object, CompletionObject,
    object_result(object), 0, main_executor(), 0, CompletionObject _Nullable object)
OBJC3_ASYNC_C_DEFINE_BLOCK(completion_block, CompletionBlock,
    block_result(value), 0, main_executor(), 0, int value)
OBJC3_ASYNC_C_DEFINE_VALUE(completion_error, int,
    error_result((id<Error>)error), 1, main_executor(), 0, CompletionObject _Nonnull error)
OBJC3_ASYNC_C_DEFINE_VALUE(completion_cancel, int,
    cancel_result(actor), 0, 0, actor, objc3_actor_t * _Nonnull actor)
OBJC3_ASYNC_C_DEFINE_VALUE(completion_worker, int, next(value), 0, 0, 0, int value)
