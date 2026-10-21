#if CASE == 1
namespace Nested { int api(); }
#elif CASE == 2
template<class T> struct Generic { T value; };
#elif CASE == 3
struct Special { Special(); int value; };
#elif CASE == 4
struct Base { int value; }; struct Derived : Base { int second; };
#elif CASE == 5
struct Methods { int value; int get() const { return value; } };
#elif CASE == 6
class Hidden { int value; };
#elif CASE == 7
struct Default { int value=42; };
#elif CASE == 8
int api(int value=42);
#elif CASE == 9
inline int api() { return 42; }
#elif CASE == 10
extern constexpr int exported=42;
#elif CASE == 11
struct Nested { struct Inner { int value; }; Inner child; };
#elif CASE == 12
union Overlap { int one; long two; };
#elif CASE == 13
extern const int exported=42;
#elif CASE == 14
constexpr int evaluated() { return 42; }
#endif
