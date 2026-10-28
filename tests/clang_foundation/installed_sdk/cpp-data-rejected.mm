#if CASE == 1
namespace Nested { inline auto open = [](auto value) { return value; }; }
#elif CASE == 2
template<class T> struct Generic;
#elif CASE == 3
struct Defaulted { template<class T> Defaulted(T); };
#elif CASE == 4
template<class T> struct Generic { T value; };
template<class T> struct Generic<T *> { T *value; };
#elif CASE == 5
struct Methods { template<class T> int get(T value) { return int(value); } };
#elif CASE == 6
template<class T> struct Methods { template<class U> int get(U value) { return int(value); } };
template<> template<> int Methods<int>::get<int>(int value) { return value; }
#elif CASE == 7
static int hidden() { return 42; }
struct Default { int value = hidden(); };
#elif CASE == 8
template<class T> struct Storage { template<class U> static U value; };
#elif CASE == 9
inline auto api() { return [](auto value) { return value; }; }
#elif CASE == 10
template<class T> extern T exported;
#elif CASE == 11
struct Nested { struct { int value; } child; };
#elif CASE == 12
template<class T> struct Overlap { union Inner { T one; long two; }; };
#elif CASE == 13
template<class T> T api(T value, ...) { return value; }
#elif CASE == 14
template<template<class> class C, class T> int api(C<T> value) { return 42; }
#endif
