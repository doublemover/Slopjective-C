#ifndef NATIVE_CLOSURE_RESULT_PROJECTIONS_H
#define NATIVE_CLOSURE_RESULT_PROJECTIONS_H
namespace NativeClosureResultProjections {
inline auto first(int n){return [n](int delta){return n+delta;};}
inline auto second(int n){return [n](int delta){return n+delta;};}
inline auto generic(int n){return [n]<class T>(T delta){return n+delta;};}
template<class T> auto factory(T n){return [n](T delta){return n+delta;};}
inline auto owned_factory(NativeClosureFactories::State value){return [copy=static_cast<NativeClosureFactories::State &&>(value)](int delta){return copy.value+delta;};}
using First=decltype(first(0));using Second=decltype(second(0));
using Generic=decltype(generic(0));using Integer=decltype(factory(0));using Wide=decltype(factory(0L));
using Owned=decltype(NativeClosureResultProjections::owned_factory(NativeClosureFactories::State(0)));
template<class T> struct Box {T value;int get(){return value(2);}};
using Wrapped=Box<First>;
struct Holder {First first;Second second;int get(){return first(0)+second(0);}};
inline int read(First value){return value(2);}
inline int read(Second value){return value(2);}
inline First shared=first(40);
inline int local(){return []<class T>(T call){return call(2);}(first(40));}
constexpr auto stateless(){return [](int n){return n+2;};}
template<auto Call> int invoke(){return Call(40);}
inline int structural(){return invoke<stateless()>();}
int provider_read(First value){return value(2);}
First provider_factory(int n){return first(n);}
First *provider_shared(){return &shared;}
Owned provider_owned(int n){return NativeClosureResultProjections::owned_factory(NativeClosureFactories::State(n));}
}
#endif
