#ifndef NATIVE_CUSTOM_COROUTINES_H
#define NATIVE_CUSTOM_COROUTINES_H
#include <coroutine>
namespace NativeCustomCoroutines {
struct Suspend {
  bool ready=false;
  bool await_ready() const noexcept {return ready;}
  template<class Handle> void await_suspend(Handle) const noexcept {}
  void await_resume() const noexcept {}
};
struct YieldToken {};
inline Suspend operator co_await(YieldToken){return {};}
struct Number {int value;};
struct NumberAwaiter {
  int value;
  bool await_ready() const noexcept {return false;}
  template<class Handle> void await_suspend(Handle) const noexcept {}
  int await_resume() const noexcept {return value;}
};
struct Token {int value;};
inline NumberAwaiter operator co_await(Token token) {return {token.value};}
struct Counts {
  static inline int frames=0;
  static inline int values=0;
  static inline int destroyed=0;
  static inline bool fail_allocation=false;
};
struct Lifetime {
  Lifetime(){++Counts::values;}
  Lifetime(const Lifetime &){++Counts::values;}
  ~Lifetime(){--Counts::values;++Counts::destroyed;}
};
template<class T> struct Task {
  struct promise_type;
  void *frame;
  explicit Task(void *frame):frame(frame){}
  Task(Task &&other) noexcept:frame(other.frame){other.frame=nullptr;}
  Task(const Task &)=delete;
  ~Task(){if(frame)__builtin_coro_destroy(frame);}
  bool resume(){if(!frame || __builtin_coro_done(frame))return false;__builtin_coro_resume(frame);return !__builtin_coro_done(frame);}
  int error() const {return static_cast<promise_type *>(__builtin_coro_promise(frame,alignof(promise_type),false))->error;}
  T value() const {return static_cast<promise_type *>(__builtin_coro_promise(frame,alignof(promise_type),false))->value;}
  T run(){while(resume()){}return value();}
  struct promise_type {
    T value{};
    int error=0;
    static void *operator new(decltype(sizeof(0)) bytes) noexcept {
      if(Counts::fail_allocation)return nullptr;
      void *storage=__builtin_malloc(bytes);if(storage)++Counts::frames;return storage;
    }
    static void operator delete(void *storage) noexcept {--Counts::frames;__builtin_free(storage);}
    static Task get_return_object_on_allocation_failure() noexcept {return Task{nullptr};}
    Task get_return_object(){return Task{__builtin_coro_promise(this,alignof(promise_type),true)};}
    Suspend initial_suspend() const noexcept {return {};}
    Suspend final_suspend() const noexcept {return {};}
    void return_value(T result){value=result;}
    void unhandled_exception(){error=73;}
    YieldToken yield_value(T result){value=result;return {};}
    NumberAwaiter await_transform(Number number){return {number.value};}
    Token await_transform(Token token){return token;}
    Suspend await_transform(Suspend suspend){return suspend;}
  };
};
inline Task<int> transformed(int value) {
  Lifetime lifetime;
  int first=co_await Number{value};
  int second=co_await Token{2};
  co_return first+second;
}
template<class T> Task<T> generic(T value) {
  Lifetime lifetime;
  co_yield value;
  co_await Suspend{};
  co_return value+T{2};
}
struct Worker {
  int value;
  Task<int> run(int added) const {Lifetime lifetime;co_await Suspend{};co_return value+added;}
};
inline Task<int> throwing() {Lifetime lifetime;co_await Suspend{};throw 73;co_return 0;}
inline auto closure(int value) {
  return [value](int added) -> Task<int> {Lifetime lifetime;co_await Suspend{};co_return value+added;};
}
Task<int> provider_task(int value);
int provider_frames();
}
#endif
