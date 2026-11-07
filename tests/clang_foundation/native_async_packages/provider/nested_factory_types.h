#ifndef NATIVE_NESTED_FACTORY_TYPES_H
#define NATIVE_NESTED_FACTORY_TYPES_H
namespace NativeNestedFactoryTypes {
inline auto factory(int n){
  struct Root {
    struct Left {struct Value {int value;};};
    struct Right {struct Value {int value;};};
    enum class Code:int {good=42};
    union Choice {int number;double real;};
    Left::Value value;
  };
  return Root{{n}};
}
using Root=decltype(factory(0));using Left=Root::Left::Value;using Right=Root::Right::Value;
using Code=Root::Code;using Choice=Root::Choice;
template<class T> auto family(T n){struct Root {struct Leaf {T value;};Leaf value;};return Root{{n}};}
using Integer=decltype(family(0))::Leaf;using Wide=decltype(family(0L))::Leaf;
inline auto closure(int base){return [base](int n){struct Root{struct Leaf{int value;};Leaf value;};return Root{{base+n}};};}
using Closure=decltype(closure(0)(0))::Leaf;
inline auto owned(int n){struct Root{struct Leaf{NativeClosureFactories::State value;};Leaf value;};return Root{{NativeClosureFactories::State(n)}};}
using Owned=decltype(owned(0))::Leaf;
template<class T> struct Box{T value;};using Wrapped=Box<Left>;
struct Holder{Left left;Right right;};
inline int read(Left value){return value.value;}
inline int read(Right value){return value.value;}
inline Left shared{42};
int provider_read(Left value){return value.value;}
Left *provider_shared(){return &shared;}
Owned provider_owned(int n){return {NativeClosureFactories::State(n)};}
template<class A,class B> struct Pair {A first;B second;};
inline auto aggregate(int n){struct A{int value;};struct B{int value;};return Pair<A,B>{{n},{2}};}
using Aggregate=decltype(aggregate(0));
using AggregateFirst=decltype(aggregate(0).first);using AggregateSecond=decltype(aggregate(0).second);
template<class T> auto wrapped(T n){struct Local{T value;};return Box<Local>{{n}};}
using WrappedInteger=decltype(wrapped(0));using WrappedWide=decltype(wrapped(0L));
inline auto pointer(int n){struct Local{int value;};static Local value{42};value.value=n;return &value;}
inline decltype(auto) reference(){struct Local{int value;};static Local value{42};return (value);}
using Pointer=decltype(pointer(0));using Reference=decltype(reference());
inline auto callback(){struct Local{int value;};return Pair{+[](Local value){return value.value;},Local{42}};}
using Callback=decltype(callback());using CallbackValue=decltype(callback().second);
inline auto member_factory(int base){
  struct Root {
    int base;
    auto make(int n) const {struct Local{int value;};return Local{base+n};}
    auto make(long n) const {struct Local{long value;};return Local{base+n};}
    auto second(int n) const {struct Local{int value;};return Local{base+n};}
    auto nested(int n) const {struct Local{struct Leaf{int value;};Leaf value;};return Local{{base+n}};}
  };
  return Root{base};
}
using Member=decltype(member_factory(0).make(0));using MemberWide=decltype(member_factory(0).make(0L));
using MemberSecond=decltype(member_factory(0).second(0));using MemberLeaf=decltype(member_factory(0).nested(0))::Leaf;
inline int read(Member value){return value.value;}
inline int read(MemberSecond value){return value.value;}
Aggregate provider_aggregate(int n){return aggregate(n);}
Callback provider_callback(){return callback();}
Pointer provider_pointer(int n){return pointer(n);}
Reference provider_reference(){return reference();}
Member provider_member(int n){return member_factory(40).make(n);}

}
#endif
