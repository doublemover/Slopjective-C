#ifndef NATIVE_LAMBDA_FACTORY_RESULTS_H
#define NATIVE_LAMBDA_FACTORY_RESULTS_H
namespace NativeLambdaFactoryResults {
inline auto first(int base){return [base](int n){struct Local{int value;};return Local{base+n};};}
inline auto second(int base){return [base](int n){struct Local{int value;};return Local{base+n};};}
inline auto generic(int base){return [base]<class T>(T n){struct Local{T value;};return Local{base+n};};}
template<class T> auto outer(T base){return [base](T n){struct Local{T value;};return Local{base+n};};}
inline auto nested(int base){return [base](int n){return [value=base+n](int delta){struct Local{int value;};return Local{value+delta};};};}
inline auto owned(int n){return [state=NativeClosureFactories::State(n)](int delta){struct Local{NativeClosureFactories::State value;};return Local{NativeClosureFactories::State(state.value+delta)};};}
using First=decltype(first(0)(0));using Second=decltype(second(0)(0));
using Integer=decltype(generic(0)(0));using Wide=decltype(generic(0)(0L));
using Outer=decltype(outer(0)(0));using OuterWide=decltype(outer(0L)(0L));
using Nested=decltype(nested(0)(0)(0));using Owned=decltype(owned(0)(0));
template<class T> struct Box{T value;};using Wrapped=Box<First>;
struct Holder{First first;Second second;};
inline int read(First value){return value.value;}
inline int read(Second value){return value.value;}
inline First shared=first(40)(2);
inline int local(){return []<class T>(T value){return value.value;}(first(40)(2));}
constexpr auto constant(){return [](int n){struct Local{int value;};return Local{n};};}
template<auto V> struct Value{static constexpr int get(){return V.value;}};
using Structural=Value<constant()(42)>;
int provider_read(First value){return value.value;}
First provider_factory(int n){return first(n)(2);}
First *provider_shared(){return &shared;}
Owned provider_owned(int n){return owned(n)(2);}
}
#endif
