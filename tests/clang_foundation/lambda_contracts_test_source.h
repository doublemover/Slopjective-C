#pragma once
inline constexpr const char *kNativeLambdaContractsSource=R"lambda(
namespace LambdaContracts {
class __attribute__((capability("mutex"))) Mutex {
public:
  void lock() __attribute__((acquire_capability())) {}
  void unlock() __attribute__((release_capability())) {}
};
struct State {
  Mutex mutex;
  int value __attribute__((guarded_by(mutex)))=42;
  int read() __attribute__((requires_capability(mutex))) {
    auto predicate=[this]() __attribute__((requires_capability(mutex))) {return value;};
    return predicate();
  }
};
inline int checked(){State state;state.mutex.lock();int result=state.read();state.mutex.unlock();return result;}
inline constexpr int defaults(){
  auto add=[](int value=40) constexpr noexcept -> int {return value+2;};
  return add();
}
inline int constrained(){
  auto add=[]<class T>(T value) requires(sizeof(T)==sizeof(int)){return value+2;};
  return add(40);
}
inline int attributed(){
  auto identity=[](int value __attribute__((unused))) __attribute__((noinline,no_sanitize("address"))) {return value;};
  return identity(42);
}
inline int swift_contract(){
  auto call=[](void *context __attribute__((swift_context)),int value) __attribute__((swiftcall,noinline)){return value;};
  return call(nullptr,42);
}
inline int vector_contract(){
  auto call=[](int value) __attribute__((vectorcall,noinline)){return value;};return call(42);
}
inline int diagnosed(){
  auto call=[](int value) __attribute__((diagnose_if(value<0,"negative","error"))){return value;};return call(42);
}
inline constexpr int immediate(){return []() consteval {return 42;}();}
static_assert(defaults()==42 && immediate()==42);
}
)lambda";
