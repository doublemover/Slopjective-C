#ifndef OBJC3_TASK_HPP
#define OBJC3_TASK_HPP

#include "objc3_runtime_services.h"

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

inline void rethrow_completion(const objc3_completion &completion) {
  if (completion.kind == OBJC3_COMPLETION_CANCELLED)
    throw task_cancelled();
  if (completion.kind != OBJC3_COMPLETION_ERROR)
    return;
  const auto *ops = objc3_error_payload_ops(completion.error);
  if (ops && ops->copy_init == value_ops<std::exception_ptr>::copy)
    std::rethrow_exception(*static_cast<const std::exception_ptr *>(
        objc3_error_payload(completion.error)));
  throw task_error(completion.error);
}

template <class T> struct result_storage {
  std::optional<T> value;
  template <class U> void return_value(U &&result) {
    value.emplace(std::forward<U>(result));
  }
  void set_completion(objc3_completion &completion) {
    completion.has_payload = 1;
    completion.payload = &*value;
    completion.payload_ops = &value_ops<T>::descriptor;
  }
};
template <> struct result_storage<void> {
  void return_void() noexcept {}
  void set_completion(objc3_completion &) noexcept {}
};
} // namespace task_detail

template <class T = void> class task;

/* A retained terminal result or running task. Releasing the last handle while
 * suspended cancels and destroys its frame. Explicit cancellation wakes await;
 * a shared dependency continues independently. Destruction does not wait for
 * the dependency's completion. */
template <class T = void> class task_handle {
  static_assert(std::is_void_v<T> ||
                (std::is_object_v<T> && !std::is_const_v<T> &&
                 !std::is_volatile_v<T>));
  objc3_task_t *task_ = nullptr;
  explicit task_handle(objc3_task_t *value) noexcept : task_(value) {}
  template <class> friend class task;

public:
  task_handle() noexcept = default;
  task_handle(const task_handle &other) noexcept
      : task_(objc3_task_retain(other.task_)) {}
  task_handle(task_handle &&other) noexcept
      : task_(std::exchange(other.task_, nullptr)) {}
  task_handle &operator=(task_handle other) noexcept {
    std::swap(task_, other.task_);
    return *this;
  }
  ~task_handle() { objc3_task_release(task_); }
  objc3_task_t *native_handle() const noexcept { return task_; }
  explicit operator bool() const noexcept { return task_ != nullptr; }
  void cancel() const noexcept { objc3_task_request_cancel(task_); }
  void wait() const { task_detail::check(objc3_task_wait(task_)); }
  T result() const {
    objc3_completion completion{};
    task_detail::check(objc3_task_get_completion(task_, &completion));
    task_detail::rethrow_completion(completion);
    if constexpr (!std::is_void_v<T>) {
      // Handles are created only by the corresponding native promise. Check
      // the descriptor as well so accidental C-side completion fails closed.
      const auto *ops = completion.payload_ops;
      if (!completion.has_payload || !ops ||
          ops->copy_init != task_detail::value_ops<T>::copy)
        throw service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
      return *static_cast<const T *>(completion.payload);
    } else if (completion.has_payload) {
      throw service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
    }
  }
};

struct task_checkpoint_t {};
struct task_yield_t {};
inline constexpr task_checkpoint_t task_checkpoint{};
inline constexpr task_yield_t task_yield{};

/* Lazy, move-only compiler-owned coroutine frame. start transfers the frame
 * to the C task service and schedules its first resume on an explicit executor.
 * Arguments follow standard C++ coroutine lifetime rules: pass owned values. */
template <class T> class [[nodiscard]] task {
public:
  struct promise_type : task_detail::result_storage<T> {
    objc3_task_t *owner = nullptr; // Borrowed only during a runtime resume.
    std::exception_ptr failure;
    bool failed = false;
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

    task get_return_object() noexcept {
      return task(coroutine::from_promise(*this));
    }
    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void unhandled_exception() noexcept {
      failed = true;
      failure = std::current_exception();
    }

    template <class U> struct dependency_awaiter {
      objc3_task_t *parent;
      task_handle<U> dependency;
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
    template <class U> auto await_transform(task_handle<U> dependency) {
      return dependency_awaiter<U>{owner, std::move(dependency)};
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
      if (failed && !failure) {
        // A foreign exception may have no C++ exception_ptr representation.
        // It must never publish an uninitialized successful result.
        completion.kind = OBJC3_COMPLETION_CANCELLED;
      } else if (failed) {
        try {
          std::rethrow_exception(failure);
        } catch (const task_cancelled &) {
          completion.kind = OBJC3_COMPLETION_CANCELLED;
        } catch (const task_error &foreign) {
          error = objc3_error_retain(foreign.native_handle());
          completion.kind = OBJC3_COMPLETION_ERROR;
        } catch (...) {
          completion.kind = OBJC3_COMPLETION_ERROR;
          if (objc3_error_create(
                  &failure,
                  &task_detail::value_ops<std::exception_ptr>::descriptor,
                  &error) != OBJC3_SERVICE_OK)
            completion.kind = OBJC3_COMPLETION_CANCELLED;
        }
        completion.error = error;
      } else {
        this->set_completion(completion);
      }
      const auto status = objc3_task_complete(owner, &completion);
      objc3_error_release(error);
      if (status != OBJC3_SERVICE_OK &&
          status != OBJC3_SERVICE_ALREADY_COMPLETED) {
        // A failed result copy must still terminalize and reclaim the frame.
        completion = {};
        completion.struct_size = sizeof(completion);
        completion.version = OBJC3_SERVICE_ABI_VERSION;
        completion.kind = OBJC3_COMPLETION_CANCELLED;
        (void)objc3_task_complete(owner, &completion);
      }
    }
    static void resume(objc3_task_t *owner, void *frame) noexcept {
      auto handle = coroutine::from_address(frame);
      handle.promise().owner = owner;
      handle.resume();
      if (handle.done())
        handle.promise().publish();
    }
    static void destroy(void *frame) noexcept {
      coroutine::from_address(frame).destroy();
    }
  };

private:
  using coroutine = std::coroutine_handle<promise_type>;
  coroutine frame_;
  explicit task(coroutine frame) noexcept : frame_(frame) {}

public:
  task(const task &) = delete;
  task &operator=(const task &) = delete;
  task(task &&other) noexcept : frame_(std::exchange(other.frame_, {})) {}
  task &operator=(task &&other) noexcept {
    if (this != &other) {
      if (frame_)
        frame_.destroy();
      frame_ = std::exchange(other.frame_, {});
    }
    return *this;
  }
  ~task() {
    if (frame_)
      frame_.destroy();
  }
  task_handle<T> start(objc3_executor_t *executor) && {
    if (!frame_)
      throw service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
    objc3_task_start_options options{};
    options.struct_size = sizeof(options);
    options.version = OBJC3_SERVICE_ABI_VERSION;
    options.executor = executor;
    options.frame = frame_.address();
    options.resume = promise_type::resume;
    options.destroy = promise_type::destroy;
    objc3_task_t *started = nullptr;
    task_detail::check(objc3_task_start(&options, &started));
    frame_ = {};
    return task_handle<T>(started);
  }
};

} // namespace objc3
#endif
