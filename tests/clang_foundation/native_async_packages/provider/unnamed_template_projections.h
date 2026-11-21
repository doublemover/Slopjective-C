#ifndef NATIVE_UNNAMED_TEMPLATE_PROJECTIONS_H
#define NATIVE_UNNAMED_TEMPLATE_PROJECTIONS_H
namespace NativeUnnamedTemplateProjections {
struct Tracked {
  static inline int live=0;
  int value;
  Tracked(int value=0):value(value){++live;}
  Tracked(const Tracked &other):value(other.value){++live;}
  Tracked(Tracked &&other) noexcept:value(other.value){++live;}
  Tracked &operator=(const Tracked &other){value=other.value;return *this;}
  ~Tracked(){--live;}
};
template<class T> struct Family {
  struct {
    enum class Kind {answer=42,other=43};
    template<class U> struct Inner {struct {U value;} leaf;};
    template<class U> using Rebind=Inner<U>;
    T value;T get() const {return value;}
  } first;
  struct {enum class Kind {answer=42,other=43};T value;T get() const {return value;}} second;
};
template<class T> using FirstOf=decltype(Family<T>{}.first);
template<class T> using SecondOf=decltype(Family<T>{}.second);
using First=FirstOf<int>;
using Second=SecondOf<int>;
using Nominal=First;
using Wide=FirstOf<long>;
using Owned=FirstOf<Tracked>;
using Nested=First::Inner<long>;
using NestedLeaf=decltype(Nested{}.leaf);
using Rebound=First::Rebind<long>;
template<template<class>class Template>struct TemplateBox {using Value=Template<long>;Value value;};
using TemplateWrapped=TemplateBox<First::Inner>;
using AliasTemplateWrapped=TemplateBox<First::Rebind>;
template<class T=First>struct Defaulted {T value;};

using Reader=int (*)(First,Second);
using ReverseReader=int (*)(Second,First);
using Kind=First::Kind;
using OtherKind=Second::Kind;
template<auto Value>struct EnumConstant {static constexpr int number=static_cast<int>(Value);};
using EnumAnswer=EnumConstant<Kind::answer>;
using OtherEnumAnswer=EnumConstant<OtherKind::answer>;
using Member=int First::*;
template<Member Pointer=&First::value>struct MemberDefault {static int read(First value){return value.*Pointer;}};
using MemberExplicit=MemberDefault<&First::value>;
template<Member Pointer=nullptr>struct NullableMember {static constexpr bool empty=Pointer==nullptr;};
using EmptyMember=NullableMember<nullptr>;
template<template<class>class Template=First::Inner>struct TemplateDefault {Template<long> value;};
template<auto Value=First{42}>struct Constant {static constexpr int number=Value.value;};
using Answer=Constant<>;
using Different=Constant<First{43}>;
using FirstArray=First[2];
struct Holder {First first;Second second;int read() const {return first.value+second.value;}};
struct Derived:First {int read() const {return value;}};
template<class T> struct Box {T value;};
template<class T> struct Box<T *> {T *value;};
template<> struct Box<Second> {Second value;int tag() const {return 42;}};
template<class T> using BoxedFirst=Box<FirstOf<T>>;
using Wrapped=BoxedFirst<int>;
using OwnedBox=Box<Owned>;
template<class... T> struct Pack {static constexpr unsigned size=sizeof...(T);};
using Ordered=Pack<First,Second>;
using Reversed=Pack<Second,First>;
inline First shared{40};
inline Member member=&First::value;
inline int read(First a,Second b){return a.value+b.value;}
inline int read(Second a,First b){return a.value-b.value;}
inline int invoke(Reader reader,First a,Second b){return reader(a,b);}
template<class T> inline int generic(T value){return value.value;}
inline const First &reference(){return shared;}
Kind provider_kind(Kind value);
First provider_first(First value);
Second provider_second(Second value);
int provider_read(First a,Second b);
int provider_read(Second a,First b);
First *provider_shared();
Owned provider_owned(int value);
int provider_owned_value(Owned value);
Wrapped provider_wrapped(Wrapped value);
}
#endif
