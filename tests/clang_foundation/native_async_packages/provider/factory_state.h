#ifndef NATIVE_FACTORY_STATE_H
#define NATIVE_FACTORY_STATE_H

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
}

#endif
