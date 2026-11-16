#pragma once

inline constexpr const char *kNativeThreadSafetySource=R"thread(
namespace ThreadSafety {
struct __attribute__((capability("mutex"))) Mutex {
 int state=0;
 void lock() __attribute__((acquire_capability(),no_thread_safety_analysis)) {state=1;}
 void unlock() __attribute__((release_capability(),no_thread_safety_analysis)) {state=0;}
 void lock_shared() __attribute__((acquire_shared_capability(),no_thread_safety_analysis)) {state=1;}
 void unlock_shared() __attribute__((release_shared_capability(),no_thread_safety_analysis)) {state=0;}
 void unlock_any() __attribute__((release_generic_capability(),no_thread_safety_analysis)) {state=0;}
 bool try_lock() __attribute__((try_acquire_capability(true),no_thread_safety_analysis)) {state=1;return true;}
 bool try_shared() __attribute__((try_acquire_shared_capability(true),no_thread_safety_analysis)) {state=1;return true;}
 void assert_held() __attribute__((assert_capability(),no_thread_safety_analysis)) {}
 void assert_shared() __attribute__((assert_shared_capability(),no_thread_safety_analysis)) {}
};
struct __attribute__((capability("recursive"),reentrant_capability)) Reentrant {int state;};
struct Counter {
 Mutex mutex;
 int value __attribute__((guarded_by(mutex)))=0;
 int *pointer __attribute__((pt_guarded_by(mutex)))=nullptr;
 int any __attribute__((guarded_var))=0;
 int *any_pointer __attribute__((pt_guarded_var))=nullptr;
 int read() const __attribute__((requires_shared_capability(mutex))) {return value;}
 void bump() __attribute__((requires_capability(mutex))) {++value;}
 int unlocked() __attribute__((locks_excluded(mutex))) {return 0;}
 Mutex &get_mutex() __attribute__((lock_returned(mutex))) {return mutex;}
};
struct __attribute__((scoped_lockable)) Scope {
 Mutex &mutex;
 explicit Scope(Mutex &lock) __attribute__((acquire_capability(lock),no_thread_safety_analysis)):mutex(lock){mutex.lock();}
 ~Scope() __attribute__((release_capability(),no_thread_safety_analysis)){mutex.unlock();}
};
inline Mutex first;
inline Mutex second __attribute__((acquired_after(first)));
inline Mutex before __attribute__((acquired_before(second)));
void acquire(Mutex *original) __attribute__((acquire_capability(original)));
inline void acquire(Mutex *current) __attribute__((no_thread_safety_analysis)){current->lock();}
void release(Mutex *original) __attribute__((release_capability(original)));
inline void release(Mutex *current) __attribute__((no_thread_safety_analysis)){current->unlock();}
template<class T>struct Box {
 Mutex mutex;T value __attribute__((guarded_by(mutex))){};
 T read() const __attribute__((requires_capability(mutex))){return value;}
};
inline int run(Counter &counter){Scope scope(counter.mutex);counter.bump();return counter.read();}
}
)thread";
