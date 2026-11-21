#ifndef NATIVE_FACTORY_VALUE_ARGUMENTS_H
#define NATIVE_FACTORY_VALUE_ARGUMENTS_H
namespace NativeFactoryValueArguments {
constexpr auto first(int n){struct Local {int value;};return Local{n};}
constexpr auto second(int n){struct Local {int value;};return Local{n};}
template<class T> constexpr auto generic(T n){struct Local {T value;};return Local{n};}
template<auto V> struct Box {static constexpr int get(){return V.value;}};
template<> struct Box<first(42)> {static constexpr int get(){return 42;}};
template<auto V> requires(V.value==40) struct Box<V> {static constexpr int get(){return V.value;}};
template<auto... V> struct Pack {static constexpr int get(){return (0+...+V.value);}};
template<auto V> using Alias=Box<V>;
using First=Box<first(42)>;using Other=Box<second(42)>;
using Partial=Box<first(40)>;using Generic=Box<generic(42L)>;
using Packed=Pack<first(40),second(2)>;using Aliased=Alias<first(42)>;
template<auto V> inline int slot=V.value;
template<> inline int slot<first(42)> = 42;
template<auto V> int read(){return V.value;}
template<> int read<first(42)>(){return 42;}
inline int local(){return []<auto V>(){return V.value;}.template operator()<first(42)>();}
int provider_value(First){return First::get();}
int *provider_slot(){return &slot<first(42)>;}
}
#endif
