#ifndef NATIVE_CLOSURE_FACTORIES_H
#define NATIVE_CLOSURE_FACTORIES_H

namespace NativeClosureFactories {
struct State {
  static inline int live=0;
  int value;
  explicit State(int n):value(n) {++live;}
  State(const State &other):value(other.value) {++live;}
  State(State &&other):value(other.value) {other.value=0;++live;}
  ~State() {--live;}
  auto factory() const {return [copy=*this](int delta) {return copy.value+delta;};}
  auto generic() const {return [copy=*this]<class T>(T delta) {return copy.value+delta;};}
};
inline auto concrete(int base) {return [base](int n) {return base+n;};}
inline auto generic(int base) {return [base]<class T>(T n) {return base+n;};}
inline decltype(auto) mutable_factory(int base) {
  return [base](int n) mutable {base+=n;return base;};
}
inline auto reference_factory(int &base) {
  return [&base]<class T>(T n) {base+=n;return base;};
}
inline auto nested_factory(int base) {
  return [base](int n) {return [value=base+n](int delta) {return value+delta;};};
}
inline auto owned_factory(State state) {
  return [copy=static_cast<State &&>(state)]<class T>(T delta) {return copy.value+delta;};
}
inline auto task_factory(int bias) {
  return [bias]<class T>(T value)->T __objc3_async {
    __objc3_await objc3::task_yield;return value+bias;
  };
}
inline auto frame_factory(int bias) {
  return [bias]<class T>(T value)->T __objc3_async_frame {
    (void)__objc3_await NativeEffectLambdas::task(0);return value+bias;
  };
}
inline auto checked_factory(int bias) {
  return [bias]<class E>(int value,E error)->int __objc3_throws(E) {
    if(error)__objc3_throw(error);return value+bias;
  };
}
}

#endif
