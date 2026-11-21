#ifndef NATIVE_UNNAMED_RECORD_PROJECTIONS_H
#define NATIVE_UNNAMED_RECORD_PROJECTIONS_H
namespace NativeUnnamedRecordProjections {
struct Family {
  struct {
    enum class Kind {answer=42,other=43};
    template<class U> struct Inner {struct {U value;} leaf;};
    template<class U> using Rebind=Inner<U>;
    int value;int get() const {return value;}
  } first;
  struct {enum class Kind {answer=42,other=43};int value;int get() const {return value;}} second;
  union {int integer;double floating;};
};
using First=decltype(Family{}.first);
using Second=decltype(Family{}.second);
using Nominal=First;
using Nested=First::Inner<long>;
using NestedLeaf=decltype(Nested{}.leaf);
using Rebound=First::Rebind<long>;
template<template<class>class Template>struct TemplateBox {typename Template<long>::Value value;};
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
template<class T=First>struct Defaulted {T value;};
template<auto Value=First{42}>struct Constant {static constexpr int number=Value.value;};
using Answer=Constant<>;
using Different=Constant<First{43}>;
using FirstArray=First[2];
struct Holder {First first;Second second;int read() const {return first.value+second.value;}};
struct Derived:First {int read() const {return value;}};
template<class... T>struct Pack {};
using Ordered=Pack<First,Second>;
using Reversed=Pack<Second,First>;
inline First shared{40};
inline Member member=&First::value;
inline int read(First a,Second b){return a.value+b.value;}
inline int read(Second a,First b){return a.value-b.value;}
inline int invoke(Reader reader,First a,Second b){return reader(a,b);}
inline const First &reference(){return shared;}
int provider_read(First a,Second b);
int provider_read(Second a,First b);
}
#endif
