#include "service_actor.h"
#include "service_group.h"

#include <new>

namespace {
namespace services = objc3::services;
template <class T, class U> T *handle(U *pointer) noexcept {
  return reinterpret_cast<T *>(pointer);
}
template <class F> objc3_service_status status(F &&call) noexcept {
  try {
    return call();
  } catch (const std::bad_alloc &) {
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  } catch (...) {
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  }
}
} // namespace

extern "C" {
objc3_service_status objc3_task_group_create(objc3_task_group_t *parent,
                                             objc3_task_group_t **out_group) {
  if (!out_group)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_group = nullptr;
  return status([&] {
    services::TaskGroup *group = nullptr;
    const auto result = services::task_group_create(
        handle<services::TaskGroup>(parent), &group);
    if (result == OBJC3_SERVICE_OK)
      *out_group = handle<objc3_task_group_t>(group);
    return result;
  });
}
objc3_task_group_t *objc3_task_group_retain(objc3_task_group_t *group) {
  try {
    return handle<objc3_task_group_t>(
        services::task_group_retain(handle<services::TaskGroup>(group)));
  } catch (...) {
    return nullptr;
  }
}
void objc3_task_group_release(objc3_task_group_t *group) {
  (void)status([&] {
    services::task_group_release(handle<services::TaskGroup>(group));
    return OBJC3_SERVICE_OK;
  });
}
objc3_service_status objc3_task_group_add(objc3_task_group_t *group,
                                          objc3_task_t *task) {
  return status([&] {
    return services::task_group_add_task(handle<services::TaskGroup>(group),
                                         task);
  });
}
objc3_service_status objc3_task_group_close(objc3_task_group_t *group) {
  return status([&] {
    return services::task_group_close(handle<services::TaskGroup>(group));
  });
}
objc3_service_status objc3_task_group_cancel(objc3_task_group_t *group) {
  return status([&] {
    return services::task_group_cancel(handle<services::TaskGroup>(group));
  });
}
objc3_service_status objc3_task_group_next(objc3_task_group_t *group,
                                           objc3_task_t **out_task) {
  if (out_task)
    *out_task = nullptr;
  return status([&] {
    return services::task_group_try_next_completed(
        handle<services::TaskGroup>(group), out_task);
  });
}
objc3_service_status objc3_task_group_join(objc3_task_group_t *group) {
  return status([&] {
    return services::task_group_join(handle<services::TaskGroup>(group));
  });
}
objc3_service_status objc3_task_group_await(objc3_task_group_t *group,
                                           objc3_task_t *parent,
                                           objc3_service_status *out_status) {
  return status([&] {
    return services::task_group_await(handle<services::TaskGroup>(group), parent,
                                      out_status);
  });
}
objc3_service_status objc3_task_group_await_next(
    objc3_task_group_t *group, objc3_task_t *parent,
    const objc3_async_result_type *expected_type,
    objc3_service_status *out_status, objc3_task_t **out_task) {
  return status([&] {
    return services::task_group_await_next(handle<services::TaskGroup>(group),
                                           parent, expected_type, out_status,
                                           out_task);
  });
}
objc3_service_status objc3_task_group_await_cleanup(
    objc3_task_group_t *group, objc3_task_t *parent,
    objc3_service_status *out_status) {
  return status([&] {
    return services::task_group_await_cleanup(handle<services::TaskGroup>(group),
                                              parent, out_status);
  });
}
objc3_service_status objc3_actor_create(objc3_executor_t *executor,
                                        objc3_actor_t **out_actor) {
  if (!out_actor)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_actor = nullptr;
  return status([&] {
    services::Actor *actor = nullptr;
    const auto result = services::actor_create(executor, &actor);
    if (result == OBJC3_SERVICE_OK)
      *out_actor = handle<objc3_actor_t>(actor);
    return result;
  });
}
objc3_actor_t *objc3_actor_retain(objc3_actor_t *actor) {
  try {
    return handle<objc3_actor_t>(
        services::actor_retain(handle<services::Actor>(actor)));
  } catch (...) {
    return nullptr;
  }
}
void objc3_actor_release(objc3_actor_t *actor) {
  (void)status([&] {
    services::actor_release(handle<services::Actor>(actor));
    return OBJC3_SERVICE_OK;
  });
}
uint64_t objc3_actor_identity(const objc3_actor_t *actor) {
  return services::actor_identity(handle<const services::Actor>(actor));
}
uint64_t objc3_actor_executor_identity(const objc3_actor_t *actor) {
  return services::actor_executor_identity(
      handle<const services::Actor>(actor));
}
objc3_service_status objc3_actor_enqueue(objc3_actor_t *actor,
                                         objc3_actor_job job, void *context,
                                         objc3_actor_context_destroy destroy,
                                         objc3_actor_hop_facts *out_facts) {
  return status([&] {
    return services::actor_enqueue(handle<services::Actor>(actor), job, context,
                                   destroy, out_facts);
  });
}
objc3_service_status objc3_actor_close(objc3_actor_t *actor) {
  return status(
      [&] { return services::actor_close(handle<services::Actor>(actor)); });
}
objc3_service_status objc3_actor_cancel(objc3_actor_t *actor) {
  return status(
      [&] { return services::actor_cancel(handle<services::Actor>(actor)); });
}
objc3_service_status objc3_actor_join(objc3_actor_t *actor) {
  return status(
      [&] { return services::actor_join(handle<services::Actor>(actor)); });
}
objc3_service_status
objc3_actor_current_facts(objc3_current_actor_facts *out_facts) {
  return services::actor_current_facts(out_facts);
}
}
