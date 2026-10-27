#if CASE == 1
namespace Nested { inline int api() { return 42; } }
#elif CASE == 2
template<class T> struct Generic { T value; };
#elif CASE == 3
struct Defaulted { Defaulted(); int value; }; Defaulted::Defaulted() = default;
#elif CASE == 4
struct Base { int value; }; struct Derived : virtual Base { int second=1; int get() const { return second; } };
#elif CASE == 5
struct Methods { int value; int get() const { return value; } };
#elif CASE == 6
class Virtual { public: virtual int get() const { return 42; } };
#elif CASE == 7
struct Default { int value=[] { return 42; }(); };
#elif CASE == 8
int api(int value=[] { return 42; }());
#elif CASE == 9
inline int api() { return 42; }
#elif CASE == 10
extern constexpr int exported=42;
#elif CASE == 11
struct Nested { struct Inner { int value=[] { return 42; }(); }; Inner child; };
#elif CASE == 12
union Overlap { int one; long two; };
#elif CASE == 13
extern const int exported=42;
#elif CASE == 14
constexpr int evaluated() { return 42; }
#endif
