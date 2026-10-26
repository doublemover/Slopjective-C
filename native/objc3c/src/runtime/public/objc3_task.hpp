#ifndef OBJC3_TASK_HPP
#define OBJC3_TASK_HPP

#if defined(__OBJC3_EXTENSION_ABI_REVISION__) &&                               \
    __OBJC3_EXTENSION_ABI_REVISION__ != 21
#error                                                                         \
    "The task bridge requires Objective-C 3 extension ABI 21; rebuild the compiler and all providers."
#endif

#include "objc3_runtime_services.h"

#if !__has_builtin(__builtin_objc3_type_identity)
#error "The task bridge requires the qualified compiler type-identity builtin."
#endif

#include <coroutine>
#include <exception>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

namespace objc3 {

class service_failure : public std::exception {
  objc3_service_status status_;

public:
  explicit service_failure(objc3_service_status status) noexcept
      : status_(status) {}
  const char *what() const noexcept override {
    return "Objective-C 3 task service failure";
  }
  objc3_service_status status() const noexcept { return status_; }
};

class task_cancelled : public std::exception {
public:
  const char *what() const noexcept override {
    return "Objective-C 3 task cancelled";
  }
};

class task_error : public std::exception {
  objc3_error_t *error_;

public:
  explicit task_error(objc3_error_t *error) noexcept
      : error_(objc3_error_retain(error)) {}
  task_error(const task_error &other) noexcept : task_error(other.error_) {}
  task_error &operator=(const task_error &other) noexcept {
    auto *replacement = objc3_error_retain(other.error_);
    objc3_error_release(std::exchange(error_, replacement));
    return *this;
  }
  ~task_error() override { objc3_error_release(error_); }
  objc3_error_t *native_handle() const noexcept { return error_; }
  const char *what() const noexcept override {
    return "Objective-C 3 task error";
  }
};

namespace task_detail {
inline void check(objc3_service_status status) {
  if (status != OBJC3_SERVICE_OK)
    throw service_failure(status);
}

inline objc3_error_t *service_error(objc3_service_status status) noexcept {
  objc3_error_t *error = nullptr;
  if (objc3_async_error_create_service(status, &error) != OBJC3_SERVICE_OK || !error)
    std::terminate();
  return error;
}

template <class T> struct value_ops;

template <class T, class = void> struct result_traits {
  using abi_type = T;
  using storage = T;
  static constexpr auto kind = OBJC3_ASYNC_VALUE;
  static constexpr size_t storage_size = sizeof(storage);
  static constexpr size_t storage_alignment = alignof(storage);
  static const void *address(const storage &value) { return &value; }
  static T read(const void *value) {
    return *static_cast<const storage *>(value);
  }
};
template <> struct result_traits<void> {
  using abi_type = void;
  static constexpr auto kind = OBJC3_ASYNC_VOID;
  static constexpr size_t storage_size = 0;
  static constexpr size_t storage_alignment = 1;
};

#ifdef __OBJC__
// Use opaque runtime calls so ARC and MRC instantiate the same owned payload
// representation. In particular, an MRC provider must copy a stack Block before
// its coroutine frame is reclaimed. Keep these declarations independent of the
// runtime headers' ownership-qualified Objective-C declarations.
extern "C" void *objc3_task_result_retain(void *) __asm__("objc_retain");
extern "C" void objc3_task_result_release(void *) __asm__("objc_release");
extern "C" void *
objc3_task_result_autorelease(void *) __asm__("objc_autorelease");
extern "C" void *
objc3_task_result_block_copy(const void *) __asm__("_Block_copy");
extern "C" void
objc3_task_result_block_release(const void *) __asm__("_Block_release");
extern "C" objc3_error_t *__objc3_task_error_create(void *) noexcept;
extern "C" void *__objc3_task_error_object(const objc3_error_t *) noexcept;

template <class T>
inline constexpr bool retained_result =
    __objc3_is_object_pointer(T) || __objc3_is_block_pointer(T);

template <class T> class retained_result_value {
  void *value_;
  static void *copy(void *value) {
    if (!value)
      return nullptr;
    if constexpr (__objc3_is_object_pointer(T))
      return objc3_task_result_retain(value);
    else {
      auto *owned = objc3_task_result_block_copy(value);
      if (!owned)
        throw std::bad_alloc();
      return owned;
    }
  }

public:
  explicit retained_result_value(T value)
      : value_(copy((__bridge void *)value)) {}
  retained_result_value(const retained_result_value &other)
      : value_(copy(other.value_)) {}
  retained_result_value &operator=(const retained_result_value &) = delete;
  ~retained_result_value() {
    if constexpr (__objc3_is_object_pointer(T))
      objc3_task_result_release(value_);
    else
      objc3_task_result_block_release(value_);
  }
  const void *payload_address() const noexcept { return &value_; }
  static T read_payload(const void *payload) {
    void *value;
    __builtin_memcpy(&value, payload, sizeof(value));
    return (__bridge T)objc3_task_result_autorelease(copy(value));
  }
  T get() const {
    // Both ownership modes return a +0 value valid through the current pool.
    // A nested await can therefore release its private child immediately after
    // extracting the result. MRC callers retain values across later suspension.
    return (__bridge T)objc3_task_result_autorelease(copy(value_));
  }
};

template <class T>
struct result_traits<T, std::enable_if_t<retained_result<T>>> {
  // Native C++ mangling distinguishes implicit ARC strong template arguments
  // from MRC arguments. The opaque payload supplies ownership itself, so its
  // type and the public handle use an explicit borrowed pointer identity.
  using abi_type = __unsafe_unretained T;
  using storage = retained_result_value<abi_type>;
  static constexpr auto kind =
      __objc3_is_object_pointer(T) ? OBJC3_ASYNC_OBJECT : OBJC3_ASYNC_BLOCK;
  static constexpr size_t storage_size = sizeof(void *);
  static constexpr size_t storage_alignment = alignof(void *);
  static const void *address(const storage &value) {
    return value.payload_address();
  }
  static T read(const void *value) { return storage::read_payload(value); }
};
#endif

template <class T> struct value_ops {
  static_assert(std::is_copy_constructible_v<T> &&
                std::is_nothrow_destructible_v<T>);
  static objc3_service_status copy(void *destination,
                                   const void *source) noexcept {
    try {
      ::new (destination) T(*static_cast<const T *>(source));
      return OBJC3_SERVICE_OK;
    } catch (const std::bad_alloc &) {
      return OBJC3_SERVICE_OUT_OF_MEMORY;
    } catch (...) {
      return OBJC3_SERVICE_INVALID_ARGUMENT;
    }
  }
  static void destroy(void *value) noexcept { static_cast<T *>(value)->~T(); }
  static objc3_service_status move(void *destination, void *source) noexcept {
    const auto status = copy(destination, source);
    if (status == OBJC3_SERVICE_OK)
      destroy(source);
    return status;
  }
  inline static const objc3_value_ops descriptor{sizeof(objc3_value_ops),
                                                 OBJC3_VALUE_OPS_VERSION,
                                                 sizeof(T),
                                                 alignof(T),
                                                 copy,
                                                 move,
                                                 destroy};
};

template <class T> const objc3_async_result_type *result_type() {
  using traits = result_traits<T>;
  static constexpr const char *identity =
      __builtin_objc3_type_identity((typename traits::abi_type *)nullptr);
  // A mutable per-instantiation object cannot be folded with another type's
  // token. External types remain interoperable with C using their native key;
  // local and anonymous types preserve nominal identity without callback IDs.
  static unsigned char domain;
  static constexpr bool local =
      __builtin_strncmp(identity, "objc3.async.local.v1:",
                        sizeof("objc3.async.local.v1:") - 1) == 0;
  static const objc3_async_result_type type{sizeof(objc3_async_result_type),
                                            OBJC3_ASYNC_ABI_VERSION,
                                            traits::storage_size,
                                            traits::storage_alignment,
                                            traits::kind,
                                            0,
                                            identity,
                                            __builtin_strlen(identity),
                                            local ? &domain : nullptr};
  return &type;
}

template <class T> const objc3_value_ops *result_ops() {
  if constexpr (std::is_void_v<T>)
    return nullptr;
  else if constexpr (result_traits<T>::kind == OBJC3_ASYNC_OBJECT ||
                     result_traits<T>::kind == OBJC3_ASYNC_BLOCK)
    return objc3_async_reference_ops(result_traits<T>::kind);
  else
    return &value_ops<typename result_traits<T>::storage>::descriptor;
}

inline void rethrow_completion(const objc3_completion &completion) {
  if (completion.kind == OBJC3_COMPLETION_CANCELLED)
    throw task_cancelled();
  if (completion.kind != OBJC3_COMPLETION_ERROR)
    return;
  objc3_service_status service_status;
  if (objc3_async_error_get_service(completion.error, &service_status) ==
      OBJC3_SERVICE_OK)
    throw service_failure(service_status);
  const auto *ops = objc3_error_payload_ops(completion.error);
  if (ops && ops->copy_init == value_ops<std::exception_ptr>::copy)
    std::rethrow_exception(*static_cast<const std::exception_ptr *>(
        objc3_error_payload(completion.error)));
  throw task_error(completion.error);
}

template <class T, class Promise> struct result_storage {
  using stored_type = typename result_traits<T>::storage;
  std::optional<stored_type> value;
  template <class U> void return_value(U &&result) {
    value.emplace(std::forward<U>(result));
    static_cast<Promise *>(this)->__objc3_clear_error();
  }
  void set_completion(objc3_completion &completion) {
    completion.has_payload = 1;
    completion.payload = result_traits<T>::address(*value);
    completion.payload_ops = result_ops<T>();
    completion.result_type = result_type<T>();
  }
};
template <class Promise> struct result_storage<void, Promise> {
  void return_void() noexcept {
    static_cast<Promise *>(this)->__objc3_clear_error();
  }
  void set_completion(objc3_completion &completion) noexcept {
    completion.result_type = result_type<void>();
  }
};
} // namespace task_detail

template <class T> class basic_task;
template <class T = void>
using task = basic_task<typename task_detail::result_traits<T>::abi_type>;
template <class T> class basic_task_handle;
template <class T = void>
using task_handle = basic_task_handle<typename task_detail::result_traits<T>::abi_type>;

namespace task_detail {
// A compiler-generated checked await supplies a frame-resident error slot.
// The compiler checks the error slot before evaluating await_resume.
template <class T> struct checked_dependency {
  basic_task<T> dependency;
  void **error;
};
} // namespace task_detail

/* A retained terminal result or running task. Releasing the last handle while
 * suspended cancels and destroys its frame after owned scope cleanup.
 * Explicit cancellation wakes await;
 * a shared dependency continues independently. Destruction does not wait for
 * the dependency's completion. */
template <class T> class basic_task_handle {
  static_assert(std::is_void_v<T> ||
                (std::is_object_v<T> && !std::is_volatile_v<T>));
  objc3_task_t *task_ = nullptr;
  explicit basic_task_handle(objc3_task_t *value) noexcept : task_(value) {}
  template <class> friend class basic_task;

public:
  basic_task_handle() noexcept = default;
  basic_task_handle(const basic_task_handle &other) noexcept
      : task_(objc3_task_retain(other.task_)) {}
  basic_task_handle(basic_task_handle &&other) noexcept
      : task_(std::exchange(other.task_, nullptr)) {}
  basic_task_handle &operator=(basic_task_handle other) noexcept {
    std::swap(task_, other.task_);
    return *this;
  }
  ~basic_task_handle() { objc3_task_release(task_); }
  objc3_task_t *native_handle() const noexcept { return task_; }
  // Consume one owned C reference only after validating the declared type.
  static basic_task_handle adopt_native(objc3_task_t **owned) {
    if (!owned || !*owned)
      throw service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
    const objc3_async_result_type *type = nullptr;
    task_detail::check(objc3_task_get_result_type(*owned, &type));
    if (!objc3_async_result_type_equal(type, task_detail::result_type<T>()))
      throw service_failure(OBJC3_SERVICE_TYPE_MISMATCH);
    return basic_task_handle(std::exchange(*owned, nullptr));
  }
  objc3_task_t *release_native() && noexcept {
    return std::exchange(task_, nullptr);
  }
  explicit operator bool() const noexcept { return task_ != nullptr; }
  void cancel() const noexcept { objc3_task_request_cancel(task_); }
  void wait() const { task_detail::check(objc3_task_wait(task_)); }
  T result() const {
    objc3_completion completion{};
    task_detail::check(objc3_task_get_completion(task_, &completion));
    task_detail::rethrow_completion(completion);
    if (!objc3_async_result_type_equal(completion.result_type,
                                       task_detail::result_type<T>()))
      throw service_failure(OBJC3_SERVICE_TYPE_MISMATCH);
    if constexpr (!std::is_void_v<T>) {
      using traits = task_detail::result_traits<T>;
      if (!completion.has_payload || !completion.payload_ops)
        throw service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
      return traits::read(completion.payload);
    } else if (completion.has_payload) {
      throw service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
    }
  }
};

template <class T> struct task_group_next_request;
struct task_group_cleanup_request;

/* A retained shared group, with explicit close and cancellation. Awaiting a
 * closed handle joins its complete task tree without blocking a worker. This
 * handle alone does not create a compiler-owned structured task-group scope.
 * Destruction releases ownership, and abandoning one await does not cancel the
 * shared tasks. Close and join before releasing the last caller owner, unless
 * this is a current_scope() view whose compiler guard owns finalization. */
class [[nodiscard]] task_group_handle {
  objc3_task_group_t *group_ = nullptr;
  explicit task_group_handle(objc3_task_group_t *owned) noexcept
      : group_(owned) {}

public:
  task_group_handle() noexcept = default;
  static task_group_handle create() {
    objc3_task_group_t *group = nullptr;
    task_detail::check(objc3_task_group_create(nullptr, &group));
    return task_group_handle(group);
  }
  // Independent retained view of this callback's compiler-owned scope. This
  // accessor neither creates a scope nor transfers its finalization obligation.
  static task_group_handle current_scope() {
    auto *group = objc3_task_group_scope_current();
    if (!group) throw service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
    return task_group_handle(objc3_task_group_retain(group));
  }
  task_group_handle child() const {
    if (!group_) throw service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
    objc3_task_group_t *group = nullptr;
    task_detail::check(objc3_task_group_create(group_, &group));
    return task_group_handle(group);
  }
  task_group_handle(const task_group_handle &other) noexcept
      : group_(objc3_task_group_retain(other.group_)) {}
  task_group_handle(task_group_handle &&other) noexcept
      : group_(std::exchange(other.group_, nullptr)) {}
  task_group_handle &operator=(task_group_handle other) noexcept {
    std::swap(group_, other.group_);
    return *this;
  }
  ~task_group_handle() { objc3_task_group_release(group_); }
  objc3_task_group_t *native_handle() const noexcept { return group_; }
  explicit operator bool() const noexcept { return group_ != nullptr; }
  template <class U> void add(const basic_task_handle<U> &task) const {
    task_detail::check(objc3_task_group_add(group_, task.native_handle()));
  }
  void close() const { task_detail::check(objc3_task_group_close(group_)); }
  void cancel() const { task_detail::check(objc3_task_group_cancel(group_)); }
  void wait() const { task_detail::check(objc3_task_group_join(group_)); }
  task_group_cleanup_request cancel_and_join() const;
  template <class U = void>
  task_group_next_request<typename task_detail::result_traits<U>::abi_type>
  next() const;
};

template <class T> struct task_group_next_request {
  task_group_handle group;
};

struct task_group_cleanup_request {
  task_group_handle group;
};
inline task_group_cleanup_request task_group_handle::cancel_and_join() const {
  return {*this};
}
template <class U>
task_group_next_request<typename task_detail::result_traits<U>::abi_type>
task_group_handle::next() const {
  return {*this};
}

struct task_checkpoint_t {};
struct task_yield_t {};
inline constexpr task_checkpoint_t task_checkpoint{};
inline constexpr task_yield_t task_yield{};

/* A retained destination can safely be passed by value into a lazy frame.
 * Hops always queue and preserve the destination through later suspension. */
class [[nodiscard]] task_destination {
  objc3_executor_t *executor_ = nullptr;
  objc3_actor_t *actor_ = nullptr;

public:
  explicit task_destination(objc3_executor_t *executor)
      : executor_(objc3_executor_retain(executor)) {
    if (!executor_)
      throw service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  }
  explicit task_destination(objc3_actor_t *actor)
      : actor_(objc3_actor_retain(actor)) {
    if (!actor_)
      throw service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
  }
  task_destination(const task_destination &) = delete;
  task_destination(task_destination &&other) noexcept
      : executor_(std::exchange(other.executor_, nullptr)),
        actor_(std::exchange(other.actor_, nullptr)) {}
  ~task_destination() {
    objc3_actor_release(actor_);
    objc3_executor_release(executor_);
  }
  objc3_executor_t *executor() const noexcept { return executor_; }
  objc3_actor_t *actor() const noexcept { return actor_; }
};
inline task_destination hop_to(objc3_executor_t *executor) {
  return task_destination(executor);
}
inline task_destination hop_to(objc3_actor_t *actor) {
  return task_destination(actor);
}

/* Lazy, move-only compiler-owned coroutine frame. start transfers the frame
 * to the C task service and schedules its first resume on an explicit executor.
 * Arguments follow standard C++ coroutine lifetime rules: pass owned values. */
template <class T> class [[nodiscard]] basic_task {
public:
  struct promise_type : task_detail::result_storage<T, promise_type> {
    promise_type() noexcept = default;
    objc3_task_t *owner = nullptr; // Borrowed only during a runtime resume.
    std::exception_ptr failure;
    bool failed = false;
    // Reserved compiler ABI: a checked error bypasses native exception handlers
    // and reaches final suspension through the ordinary coroutine cleanups.
    objc3_error_t *__objc3_error = nullptr;
    bool __objc3_error_raised = false;
    // Reserved compiler protocol selected only for objc_task_group bodies.
    // A completed coroutine stays at final suspend while the runtime joins its
    // group. The final notification publishes without resuming that coroutine.
    bool __objc3_group_entered = false;
    bool __objc3_group_finishing = false;
    objc3_service_status __objc3_group_status = OBJC3_SERVICE_NOT_READY;
    ~promise_type() { objc3_error_release(__objc3_error); }
    void __objc3_clear_error() noexcept {
      auto *previous = std::exchange(__objc3_error, nullptr);
      __objc3_error_raised = false;
      objc3_error_release(previous);
    }
    using coroutine = std::coroutine_handle<promise_type>;

    // Require Clang's -fcoro-aligned-allocation: the full frame can have more
    // alignment than either the promise or the returned value.
    static void *operator new(std::size_t size, std::align_val_t alignment) {
      return ::operator new(size, alignment);
    }
    static void operator delete(void *frame, std::size_t,
                                std::align_val_t alignment) noexcept {
      ::operator delete(frame, alignment);
    }

    basic_task get_return_object() noexcept {
      return basic_task(coroutine::from_promise(*this));
    }
    struct initial_awaiter {
      promise_type *promise;
      bool group_scope = false;
      bool await_ready() const noexcept { return false; }
      void await_suspend(coroutine) const noexcept {}
      void await_resume() const {
        if (group_scope) {
          task_detail::check(objc3_task_group_scope_enter(promise->owner));
          promise->__objc3_group_entered = true;
        }
        if (objc3_task_is_cancel_requested(promise->owner))
          throw task_cancelled();
      }
    };
    initial_awaiter initial_suspend() noexcept { return {this}; }
    initial_awaiter __objc3_group_initial_suspend() noexcept {
      return {this, true};
    }
    std::suspend_always final_suspend() noexcept { return {}; }
    void unhandled_exception() noexcept {
      failed = true;
      failure = std::current_exception();
    }

    template <class U> struct dependency_awaiter {
      objc3_task_t *parent;
      task_handle<U> dependency;
      bool owned = false;
      dependency_awaiter(objc3_task_t *parent, task_handle<U> dependency,
                         bool owned = false)
          : parent(parent), dependency(std::move(dependency)), owned(owned) {}
      dependency_awaiter(const dependency_awaiter &) = delete;
      dependency_awaiter(dependency_awaiter &&) = default;
      ~dependency_awaiter() {
        if (owned) {
          objc3_completion completion{};
          if (objc3_task_get_completion(dependency.native_handle(),
                                        &completion) == OBJC3_SERVICE_NOT_READY)
            dependency.cancel();
        }
      }
      bool await_ready() const noexcept { return false; }
      void await_suspend(std::coroutine_handle<>) {
        task_detail::check(
            objc3_task_await(parent, dependency.native_handle()));
      }
      U await_resume() {
        if (objc3_task_is_cancel_requested(parent))
          throw task_cancelled();
        return dependency.result();
      }
    };
    template <class U> auto await_transform(basic_task_handle<U> dependency) {
      return dependency_awaiter<U>{owner, std::move(dependency)};
    }

    template <class U> auto await_transform(basic_task<U> dependency) {
      if (objc3_task_is_cancel_requested(owner))
        throw task_cancelled();
      return dependency_awaiter<U>{
          owner, std::move(dependency).start_impl(nullptr, nullptr, owner),
          true};
    }

    struct group_awaiter {
      objc3_task_t *parent;
      task_group_handle group;
      objc3_service_status status = OBJC3_SERVICE_NOT_READY;
      bool await_ready() const noexcept { return false; }
      void await_suspend(std::coroutine_handle<>) {
        task_detail::check(objc3_task_group_await(group.native_handle(), parent,
                                                  &status));
      }
      void await_resume() {
        if (objc3_task_is_cancel_requested(parent)) throw task_cancelled();
        task_detail::check(status);
      }
    };
    auto await_transform(task_group_handle group) {
      if (objc3_task_is_cancel_requested(owner)) throw task_cancelled();
      return group_awaiter{owner, std::move(group)};
    }

    struct cleanup_group_awaiter {
      objc3_task_t *parent;
      task_group_handle group;
      objc3_service_status status = OBJC3_SERVICE_NOT_READY;
      bool await_ready() const noexcept { return false; }
      void await_suspend(std::coroutine_handle<>) {
        group.close();
        group.cancel();
        task_detail::check(objc3_task_group_await_cleanup(
            group.native_handle(), parent, &status));
      }
      void await_resume() { task_detail::check(status); }
    };
    auto await_transform(task_group_cleanup_request request) {
      // Scope unwind must finish even when owner cancellation already holds.
      // It remains observable by the next ordinary cancellation checkpoint.
      return cleanup_group_awaiter{owner, std::move(request.group)};
    }

    template <class U> struct group_next_awaiter {
      objc3_task_t *parent;
      task_group_handle group;
      objc3_service_status status = OBJC3_SERVICE_NOT_READY;
      objc3_task_t *result = nullptr;
      group_next_awaiter(objc3_task_t *parent, task_group_handle group)
          : parent(parent), group(std::move(group)) {}
      group_next_awaiter(const group_next_awaiter &) = delete;
      group_next_awaiter(group_next_awaiter &&other) noexcept
          : parent(other.parent), group(std::move(other.group)),
            status(other.status), result(std::exchange(other.result, nullptr)) {}
      ~group_next_awaiter() { objc3_task_release(result); }
      bool await_ready() const noexcept { return false; }
      void await_suspend(std::coroutine_handle<>) {
        task_detail::check(objc3_task_group_await_next(
            group.native_handle(), parent, task_detail::result_type<U>(),
            &status, &result));
      }
      task_handle<U> await_resume() {
        if (objc3_task_is_cancel_requested(parent)) throw task_cancelled();
        if (status == OBJC3_SERVICE_CLOSED) return {};
        task_detail::check(status);
        return task_handle<U>::adopt_native(&result);
      }
    };
    template <class U> auto await_transform(task_group_next_request<U> request) {
      if (objc3_task_is_cancel_requested(owner)) throw task_cancelled();
      return group_next_awaiter<U>{owner, std::move(request.group)};
    }

#ifdef __OBJC__
    template <class U>
    struct checked_dependency_awaiter : dependency_awaiter<U> {
      void **error;
      checked_dependency_awaiter(dependency_awaiter<U> dependency, void **error)
          : dependency_awaiter<U>(std::move(dependency)), error(error) {}
      void __objc3_check_error() {
        if (objc3_task_is_cancel_requested(this->parent))
          throw task_cancelled();
        objc3_completion completion{};
        task_detail::check(objc3_task_get_completion(
            this->dependency.native_handle(), &completion));
        if (completion.kind == OBJC3_COMPLETION_ERROR) {
          if (void *object =
                  task_detail::__objc3_task_error_object(completion.error)) {
            // The compiler's autoreleasing slot is consumed before its pool
            // drains, independently of private-child frame reclamation.
            *error = task_detail::objc3_task_result_autorelease(
                task_detail::objc3_task_result_retain(object));
            return;
          }
        }
        task_detail::rethrow_completion(completion);
      }
    };
    template <class U>
    auto await_transform(task_detail::checked_dependency<U> checked) {
      auto dependency = await_transform(std::move(checked.dependency));
      return checked_dependency_awaiter<U>{std::move(dependency), checked.error};
    }
#endif

    struct hop_awaiter {
      objc3_task_t *parent;
      task_destination destination;
      bool await_ready() const noexcept { return false; }
      void await_suspend(std::coroutine_handle<>) {
        task_detail::check(objc3_task_hop(parent, destination.executor(),
                                          destination.actor()));
      }
      void await_resume() {
        if (objc3_task_is_cancel_requested(parent))
          throw task_cancelled();
      }
    };
    auto await_transform(task_destination destination) {
      if (objc3_task_is_cancel_requested(owner))
        throw task_cancelled();
      return hop_awaiter{owner, std::move(destination)};
    }

    struct checkpoint_awaiter {
      objc3_task_t *parent;
      bool yield;
      bool await_ready() const noexcept { return !yield; }
      void await_suspend(std::coroutine_handle<>) {
        task_detail::check(objc3_task_schedule_resume(parent));
      }
      void await_resume() {
        if (objc3_task_is_cancel_requested(parent))
          throw task_cancelled();
      }
    };
    auto await_transform(task_checkpoint_t) {
      return checkpoint_awaiter{owner, false};
    }
    auto await_transform(task_yield_t) {
      return checkpoint_awaiter{owner, true};
    }

    void publish() noexcept {
      objc3_completion completion{};
      completion.struct_size = sizeof(completion);
      completion.version = OBJC3_SERVICE_ABI_VERSION;
      completion.kind = OBJC3_COMPLETION_SUCCESS;
      objc3_error_t *error = nullptr;
      if (__objc3_group_entered && objc3_task_is_cancel_requested(owner)) {
        completion.kind = OBJC3_COMPLETION_CANCELLED;
      } else if (__objc3_group_entered &&
                 __objc3_group_status != OBJC3_SERVICE_OK) {
        error = task_detail::service_error(__objc3_group_status);
        completion.kind = OBJC3_COMPLETION_ERROR;
        completion.error = error;
      } else if (__objc3_error_raised && !failed) {
        error = objc3_error_retain(__objc3_error);
        if (!error)
          error = task_detail::service_error(__objc3_error
                      ? OBJC3_SERVICE_NOT_READY : OBJC3_SERVICE_INVALID_ARGUMENT);
        completion.kind = OBJC3_COMPLETION_ERROR;
        completion.error = error;
      } else if (failed && !failure) {
        // A foreign exception may have no C++ exception_ptr representation.
        // Preserve failure when no native exception carrier is available;
        // this is neither a successful result nor a cancellation request.
        error = task_detail::service_error(OBJC3_SERVICE_INVALID_ARGUMENT);
        completion.kind = OBJC3_COMPLETION_ERROR;
        completion.error = error;
      } else if (failed) {
        try {
          std::rethrow_exception(failure);
        } catch (const task_cancelled &) {
          completion.kind = OBJC3_COMPLETION_CANCELLED;
        } catch (const task_error &foreign) {
          error = objc3_error_retain(foreign.native_handle());
          if (!error)
            error = task_detail::service_error(foreign.native_handle()
                        ? OBJC3_SERVICE_NOT_READY : OBJC3_SERVICE_INVALID_ARGUMENT);
          completion.kind = OBJC3_COMPLETION_ERROR;
        } catch (...) {
          completion.kind = OBJC3_COMPLETION_ERROR;
          const auto status = objc3_error_create(
                  &failure,
                  &task_detail::value_ops<std::exception_ptr>::descriptor,
                  &error);
          if (status != OBJC3_SERVICE_OK)
            error = task_detail::service_error(status);
        }
        completion.error = error;
      } else {
        this->set_completion(completion);
      }
      const auto status = objc3_task_complete(owner, &completion);
      objc3_error_release(error);
      if (status != OBJC3_SERVICE_OK &&
          status != OBJC3_SERVICE_ALREADY_COMPLETED) {
        // A failed result copy is a service failure, including OOM. Preserve
        // its status instead of making it indistinguishable from cancellation.
        completion = {};
        completion.struct_size = sizeof(completion);
        completion.version = OBJC3_SERVICE_ABI_VERSION;
        if (status == OBJC3_SERVICE_CANCELLED) {
          completion.kind = OBJC3_COMPLETION_CANCELLED;
        } else {
          completion.kind = OBJC3_COMPLETION_ERROR;
          error = task_detail::service_error(status);
          completion.error = error;
        }
        const auto published = objc3_task_complete(owner, &completion);
        if (completion.error) objc3_error_release(completion.error);
        if (published != OBJC3_SERVICE_OK &&
            published != OBJC3_SERVICE_ALREADY_COMPLETED)
          std::terminate();
      }
    }
    bool __objc3_finish_group_scope() noexcept {
      if (!__objc3_group_entered || __objc3_group_finishing)
        return true;
      __objc3_group_finishing = true;
      const auto status = objc3_task_group_scope_finish(
          owner, failed || __objc3_error_raised ||
                     objc3_task_is_cancel_requested(owner),
          &__objc3_group_status);
      if (status == OBJC3_SERVICE_OK ||
          status == OBJC3_SERVICE_ALREADY_COMPLETED)
        return false;
      // Entry reserved finalization storage. A failed registration here must
      // never publish a result or release a frame still owning live children.
      std::terminate();
    }
    static void resume(objc3_task_t *owner, void *frame) noexcept {
      auto handle = coroutine::from_address(frame);
      handle.promise().owner = owner;
      if (!handle.done())
        handle.resume();
      if (handle.done() && handle.promise().__objc3_finish_group_scope())
        handle.promise().publish();
    }
    static void destroy(void *frame) noexcept {
      coroutine::from_address(frame).destroy();
    }
  };

private:
  using coroutine = std::coroutine_handle<promise_type>;
  objc3_async_frame frame_{};
  explicit basic_task(coroutine frame) noexcept
      : frame_{sizeof(objc3_async_frame), OBJC3_ASYNC_ABI_VERSION,
               frame.address(),           promise_type::resume,
               promise_type::destroy,     task_detail::result_type<T>()} {}
  template <class> friend class basic_task;

  task_handle<T> start_impl(objc3_executor_t *executor, objc3_actor_t *actor,
                            objc3_task_t *parent = nullptr) {
    if (!frame_.context)
      throw service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
    objc3_task_t *started = nullptr;
    task_detail::check(
        parent ? objc3_async_frame_start_child(&frame_, parent, &started)
               : objc3_async_frame_start(&frame_, executor, actor, &started));
    return task_handle<T>(started);
  }

public:
  basic_task() noexcept = default;
  static basic_task from_native(objc3_async_frame *owned) {
    task_detail::check(objc3_async_frame_validate(owned));
    if (!objc3_async_result_type_equal(owned->result_type,
                                       task_detail::result_type<T>()))
      throw service_failure(OBJC3_SERVICE_TYPE_MISMATCH);
    basic_task result;
    result.frame_ = std::exchange(*owned, {});
    return result;
  }
  objc3_async_frame release_native() && noexcept {
    return std::exchange(frame_, {});
  }
  basic_task(const basic_task &) = delete;
  basic_task &operator=(const basic_task &) = delete;
  basic_task(basic_task &&other) noexcept : frame_(std::exchange(other.frame_, {})) {}
  basic_task &operator=(basic_task &&other) noexcept {
    if (this != &other) {
      (void)objc3_async_frame_dispose(&frame_);
      frame_ = std::exchange(other.frame_, {});
    }
    return *this;
  }
  ~basic_task() { (void)objc3_async_frame_dispose(&frame_); }
  task_handle<T> start(objc3_executor_t *executor) && {
    return start_impl(executor, nullptr);
  }
  task_handle<T> start_on(objc3_actor_t *actor) && {
    return start_impl(nullptr, actor);
  }
  // Library entrypoints use the active runtime owner rather than guessing a
  // parent from a destination or physical thread. Failure keeps frame_ owned.
  task_handle<T> start_child() && {
    objc3_task_t *started = nullptr;
    task_detail::check(objc3_async_frame_spawn(&frame_, &started));
    return task_handle<T>(started);
  }
  // Register before dispatch: a scope can join this frame even when its
  // completion is published on another worker before destruction finishes.
  task_handle<T> start_child(const task_group_handle &group) && {
    objc3_task_t *started = nullptr;
    task_detail::check(objc3_async_frame_spawn_in_group(
        &frame_, group.native_handle(), &started));
    return task_handle<T>(started);
  }
  task_handle<T> __objc3_start_group_body(const task_group_handle &scope) && {
    objc3_task_t *started = nullptr;
    task_detail::check(objc3_async_frame_spawn_scope_body(
        &frame_, scope.native_handle(), &started));
    return task_handle<T>(started);
  }
  task_handle<T> detach() && {
    objc3_task_t *started = nullptr;
    task_detail::check(objc3_async_frame_detach(&frame_, &started));
    return task_handle<T>(started);
  }
};

/* Borrow an existing raw C handle into a lazy typed await. The adapter takes
 * its own lease immediately. Failure/type rejection releases only that lease;
 * the caller's original handle remains owned by the caller. */
template <class T> task<T> join_task(objc3_task_t *dependency) {
  objc3_async_frame frame{};
  task_detail::check(objc3_async_frame_from_task(dependency, &frame));
  try {
    return task<T>::from_native(&frame);
  } catch (...) {
    (void)objc3_async_frame_dispose(&frame);
    throw;
  }
}

/* Close on first resume and join the retained group tree without blocking.
 * This explicit library operation does not establish an implicit scope. */
inline task<void> finish_task_group(task_group_handle group) {
  objc3_async_frame frame{};
  task_detail::check(objc3_async_frame_from_group(group.native_handle(), &frame));
  try {
    return task<void>::from_native(&frame);
  } catch (...) {
    (void)objc3_async_frame_dispose(&frame);
    throw;
  }
}

} // namespace objc3

// Reserved compiler hook. The canonical source function retains its logical
// result type; evaluated native async calls return this lazy runtime carrier.
template <class T> using __objc3_async_task = objc3::task<T>;

// Reserved explicit C-frame entry bridge. The by-value argument owns the lazy
// frame immediately, including when descriptor validation rejects adoption.
// Successful from_native consumes it; every failed adoption disposes it before
// the original service/native exception continues through the C++ coroutine.
template <class T>
__objc3_async_task<T> __objc3_async_from_frame(objc3_async_frame frame) {
  try {
    return __objc3_async_task<T>::from_native(&frame);
  } catch (...) {
    (void)objc3_async_frame_dispose(&frame);
    throw;
  }
}


#ifdef __OBJC__
template <class T>
auto __objc3_async_checked(objc3::basic_task<T> dependency, void **error) {
  return objc3::task_detail::checked_dependency<T>{std::move(dependency), error};
}
#endif

#endif
