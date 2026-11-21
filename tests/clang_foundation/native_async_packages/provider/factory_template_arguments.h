#ifndef NATIVE_FACTORY_TEMPLATE_ARGUMENTS_H
#define NATIVE_FACTORY_TEMPLATE_ARGUMENTS_H
namespace NativeFactoryTemplateArguments {
using First=NativeFactoryTypeProjections::First;
using Second=NativeFactoryTypeProjections::Second;
using Owned=NativeFactoryTypeProjections::Owned;
template<class T> struct Box {T value;};
template<> struct Box<First> {First value;int get() const{return value.value;}};
template<class T> struct Box<T *> {T *value;};
template<class... T> struct Pack {static constexpr int count=sizeof...(T);};
template<class T> using Alias=Box<T>;
using Closed=Box<First>;
using Other=Box<Second>;
using Nested=Box<Box<First>>;
using Packed=Pack<First,Second,First *>;
using Aliased=Alias<First>;
using Pointer=First *;
using PointerBox=Box<Pointer>;
using Array=First[2];
using ArrayBox=Box<Array>;
using Member=decltype(&First::value);
using MemberBox=Box<Member>;
using OwnedBox=Box<Owned>;
template<class T> inline int slot=0;
template<> inline int slot<First> = 42;
template<class T> int read(T value){return value.value;}
template<> int read<First>(First value){return value.value;}
inline int deduced(First value){return read(value);}
inline int closed_lambda(int n) {
  auto call=[]<class T>(T value){return value.value;};
  return call(NativeFactoryTypeProjections::first(n))+call(NativeFactoryTypeProjections::second(0));
}
int provider_value(Closed value) {return value.get();}
int *provider_slot() {return &slot<First>;}
OwnedBox provider_owned(int n) {return {NativeFactoryTypeProjections::provider_owned(n)};}
}
#endif
