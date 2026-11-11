#ifndef NATIVE_STANDARD_COROUTINES_H
#define NATIVE_STANDARD_COROUTINES_H
#include <coroutine>
namespace NativeStandardCoroutines {
struct Counts {static inline int frames=0;static inline int values=0;static inline int destroyed=0;};
struct Lifetime {Lifetime(){++Counts::values;}~Lifetime(){--Counts::values;++Counts::destroyed;}};
template<class T> struct Task {
  struct promise_type;
  std::coroutine_handle<promise_type> handle;
  explicit Task(std::coroutine_handle<promise_type> handle):handle(handle){}
  Task(Task &&other) noexcept:handle(other.handle){other.handle=nullptr;}
  Task(const Task &)=delete;
  ~Task(){if(handle)handle.destroy();}
  bool resume(){if(!handle || handle.done())return false;handle.resume();return !handle.done();}
  T run(){while(resume()){}return handle.promise().value;}
  int error() const {return handle.promise().error;}
  struct promise_type {
    T value{};int error=0;
    static void *operator new(decltype(sizeof(0)) bytes) noexcept {void *storage=__builtin_malloc(bytes);if(storage)++Counts::frames;return storage;}
    static void operator delete(void *storage) noexcept {--Counts::frames;__builtin_free(storage);}
    static Task get_return_object_on_allocation_failure() noexcept {return Task{nullptr};}
    Task get_return_object(){return Task{std::coroutine_handle<promise_type>::from_promise(*this)};}
    std::suspend_always initial_suspend() const noexcept {return {};}
    std::suspend_always final_suspend() const noexcept {return {};}
    void return_value(T result){value=result;}
    std::suspend_always yield_value(T result){value=result;return {};}
    void unhandled_exception(){error=73;}
  };
};
template<class T> struct Promise;
template<class T> struct External {
  std::coroutine_handle<Promise<T>> handle;
  explicit External(std::coroutine_handle<Promise<T>> handle):handle(handle){}
  External(External &&other) noexcept:handle(other.handle){other.handle=nullptr;}
  External(const External &)=delete;
  ~External(){if(handle)handle.destroy();}
  bool resume(){if(!handle || handle.done())return false;handle.resume();return !handle.done();}
  T run(){while(resume()){}return handle.promise().value;}
  int error() const {return handle.promise().error;}
};
template<class T> struct Promise {
  T value{};int error=0;
  static void *operator new(decltype(sizeof(0)) bytes) noexcept {void *storage=__builtin_malloc(bytes);if(storage)++Counts::frames;return storage;}
  static void operator delete(void *storage) noexcept {--Counts::frames;__builtin_free(storage);}
  static External<T> get_return_object_on_allocation_failure() noexcept {return External<T>{nullptr};}
  External<T> get_return_object(){return External<T>{std::coroutine_handle<Promise>::from_promise(*this)};}
  std::suspend_always initial_suspend() const noexcept {return {};}
  std::suspend_always final_suspend() const noexcept {return {};}
  void return_value(T result){value=result;}
  std::suspend_always yield_value(T result){value=result;return {};}
  void unhandled_exception(){error=73;}
};
template<class T> using Handle=std::coroutine_handle<Promise<T>>;
struct Bridge {Handle<int> handle;};
inline std::coroutine_handle<> erase(Handle<int> handle){return handle;}
inline std::noop_coroutine_handle noop(){return std::noop_coroutine();}
Task<int> provider_task(int value);
External<int> provider_external(int value);
void *provider_address(std::coroutine_handle<> handle);
int provider_frames();
}
namespace std {
template<class T,class... Args>
struct coroutine_traits<NativeStandardCoroutines::External<T>,Args...> {
  using promise_type=NativeStandardCoroutines::Promise<T>;
};
template<>
struct coroutine_traits<NativeStandardCoroutines::External<long>,long> {
  using promise_type=NativeStandardCoroutines::Promise<long>;
};
}
namespace NativeStandardCoroutines {
using IntPromise=std::coroutine_traits<External<int>,int>::promise_type;
using WidePromise=std::coroutine_traits<External<long>,long>::promise_type;
inline Task<int> ordinary(int value){Lifetime lifetime;co_await std::suspend_always{};co_return value+2;}
template<class T> Task<T> generic(T value){Lifetime lifetime;co_yield value;co_return value+T{2};}
inline External<int> external(int value){Lifetime lifetime;co_await std::suspend_always{};co_return value+2;}
template<class T> External<T> external_generic(T value){Lifetime lifetime;co_yield value;co_return value+T{2};}
inline External<long> external_full(long value){Lifetime lifetime;co_return value+2;}
struct Worker {int value;External<int> run(int added) const {Lifetime lifetime;co_return value+added;}};
inline auto closure(int value){return [value](int added)->External<int>{Lifetime lifetime;co_await std::suspend_never{};co_return value+added;};}
inline External<int> throwing(){Lifetime lifetime;throw 73;co_return 0;}
}
#endif
