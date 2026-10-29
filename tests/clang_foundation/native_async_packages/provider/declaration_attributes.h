#ifndef NATIVE_DECLARATION_ATTRIBUTES_H
#define NATIVE_DECLARATION_ATTRIBUTES_H
namespace NativeDeclarationAttributes {
struct __attribute__((packed)) Packet {unsigned char code;int value;};
struct alignas(32) Aligned {int value;};
struct FieldAligned {char code;alignas(32) int value;};
template<class T,int Alignment=alignof(T)> struct alignas(Alignment) Slot {T value;};
template<class T> struct alignas(T) TypeAligned {T value;};
template<class T> struct Special {T value;};
template<> struct alignas(32) Special<int> {int value;};
template<class T> inline int partial=0;
template<class T> alignas(64) inline int partial<T *> = 42;
template<class T> inline T selected(T value){return value;}
template<> [[nodiscard]] __attribute__((noinline)) inline int selected<int>(int value){return value;}
template<class T> struct __attribute__((swift_bridge("Interop.NativeBox"))) ForeignBox {T value;};
template<class T> __attribute__((swift_name("Interop.project(_:)"))) inline T foreign(T value){return value;}
template<class T> __attribute__((always_inline,pure)) inline T project(T value){return value;}
template<class T> struct Family {
  alignas(T) inline static T shared{};
  T value;
  [[nodiscard]] T get() const {return value;}
  template<class U> __attribute__((always_inline)) U call(U added) const {return U(value)+added;}
};
struct Counts {static inline int live=0;static inline int destroyed=0;};
struct Empty {
  Empty(){++Counts::live;}
  Empty(const Empty &){++Counts::live;}
  ~Empty(){--Counts::live;++Counts::destroyed;}
};
struct Compact {[[no_unique_address]] Empty empty;int value;};
struct [[nodiscard("keep \"value\"\\result")]] Result {int value;};
struct __attribute__((visibility("hidden"),type_visibility("hidden"))) Hidden {int value;};
alignas(64) inline int shared=40;
template<class T> alignas(64) inline T storage{};
[[maybe_unused]] inline int quiet=0;
__attribute__((noinline)) int slow(int value);
__attribute__((noinline)) inline int slow(int value){return value+2;}
__attribute__((always_inline,flatten)) inline int fast(int value){return value+2;}
__attribute__((pure)) inline int read(const int *value){return *value;}
__attribute__((const)) inline int calculate(int value){return value+2;}
__attribute__((cold)) inline int cold(int value){return value;}
__attribute__((hot)) inline int hot(int value){return value;}
[[nodiscard("use \"answer\"\\value")]] inline int answer(int value){return value+2;}
inline int parameter([[maybe_unused]] int value){return value+2;}
[[noreturn]] inline void stop(){__builtin_trap();}
__attribute__((noreturn)) inline void terminate(){__builtin_trap();}
__attribute__((visibility("hidden"))) int provider_packet(Packet value);
Aligned provider_aligned(Aligned value);
Slot<int,32> provider_slot(Slot<int,32> value);
Compact provider_compact(int value);
int *provider_shared();
int *provider_storage();
int *provider_partial();
Special<int> provider_special(Special<int> value);
FieldAligned provider_field(FieldAligned value);
}
#endif
