#if CASE == 1
namespace Nested { inline int api() { return [](auto value) { return value; }(42); } }
#elif CASE == 2
template<class T> struct Generic { T value; };
#elif CASE == 3
struct Defaulted { Defaulted() = default; int value = 42; };
#elif CASE == 4
struct Base { int value; }; struct Derived : virtual Base { int second=1; int get() const { return [](auto value) { return value; }(42); } };
#elif CASE == 5
struct Methods { template<class T> int get(T value) { return int(value); } };
#elif CASE == 6
class Virtual { public: virtual int get() const { return [](auto value) { return value; }(42); } };
#elif CASE == 7
struct Default { int value=[](auto value) { return value; }(42); };
#elif CASE == 8
int api(int value=[](auto value) { return value; }(42));
#elif CASE == 9
inline int api() { return [](auto value) { return value; }(42); }
#elif CASE == 10
extern constexpr int exported=[](auto value) { return value; }(42);
#elif CASE == 11
struct Nested { template<class T> struct Inner { T value; }; };
#elif CASE == 12
union Overlap { int one; long two; };
#elif CASE == 13
template<class T> T api(T value, ...) { return value; }
#elif CASE == 14
template<template<class> class C, class T> int api(C<T> value) { return 42; }
#endif
