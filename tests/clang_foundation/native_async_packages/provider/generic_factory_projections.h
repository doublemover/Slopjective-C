#ifndef NATIVE_GENERIC_FACTORY_PROJECTIONS_H
#define NATIVE_GENERIC_FACTORY_PROJECTIONS_H
namespace NativeGenericFactoryProjections {
template<class T> auto make(T n) {struct Local {T value;};return Local{n};}
template<class T> struct Family {auto make(T n) const {struct Local {T value;};return Local{n};}};
struct Concrete {template<class T> auto make(T n) const {struct Local {T value;};return Local{n};}};
template<class T> auto owned_factory(T value) {
  struct Local {T value;T &get(){return value;}};
  return Local{static_cast<T &&>(value)};
}
using Integer=decltype(make(0));
using Wide=decltype(make(0L));
using Member=decltype(Family<int>{}.make(0));
using MemberWide=decltype(Family<long>{}.make(0L));
using Method=decltype(Concrete{}.make(0));
using Owned=decltype(NativeGenericFactoryProjections::owned_factory(NativeClosureFactories::State(0)));
template<class T> struct Box {T value;};
using Wrapped=Box<Integer>;
struct Holder {Integer first;Member second;Method third;};
inline int read(Integer value){return value.value;}
inline int read(Wide value){return value.value;}
inline Integer shared=make(40);
inline int local(){return []<class T>(T value){return value.value;}(make(42));}
int provider_read(Integer value){return value.value;}
Integer *provider_shared(){return &shared;}
Owned provider_owned(int n){return NativeGenericFactoryProjections::owned_factory(NativeClosureFactories::State(n));}
}
#endif
