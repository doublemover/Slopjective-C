#ifndef NATIVE_LOCAL_FACTORY_RESULTS_H
#define NATIVE_LOCAL_FACTORY_RESULTS_H

namespace NativeLocalFactoryResults {
inline int live=0;
inline auto named(int n) {
  struct Local {
    int value;
    explicit Local(int n):value(n) {++live;}
    Local(const Local &other):value(other.value) {++live;}
    Local(Local &&other):value(other.value) {other.value=0;++live;}
    ~Local() {--live;}
    int get() const {return value;}
  };
  return Local(n);
}
inline decltype(auto) unnamed(int n) {
  struct {int value;int get() const {return value;}} result{n};return result;
}
inline auto owned(int n) {
  struct {
    NativeClosureFactories::State state;
    int get() const {return state.value;}
  } result{NativeClosureFactories::State(n)};
  return result;
}
inline auto enumeration() {enum class Local {answer=42};return Local::answer;}
inline auto anonymous_enum() {enum {answer=42};return answer;}
inline auto union_value(int n) {union Local {int value;double other;};return Local{n};}
inline auto nested(int n) {
  struct Outer {struct Value {int value;int get() const {return value;}};};
  return Outer::Value{n};
}
inline auto closure() {return [](int n){struct Local {int value;};return Local{n};};}
template<class T> auto templated(T n) {
  struct Local {T value;T get() const {return value;}};return Local{n};
}
}

#endif
