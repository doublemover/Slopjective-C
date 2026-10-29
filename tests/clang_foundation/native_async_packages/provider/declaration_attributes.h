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
template<class T> struct __attribute__((swift_bridge("Interop.NativeBox"),swift_attr("@Second"),swift_attr("@First"),swift_attr("@Second"))) ForeignBox {T value;};
template<class T> __attribute__((swift_name("Interop.project(_:)"))) inline T foreign(T value){return value;}
template<class T> __attribute__((always_inline,pure)) inline T project(T value){return value;}
template<class T> struct Family {
  alignas(T) inline static T shared{};
  T value;
  struct {
    template<class U> struct Inner {struct {U value{};U get() const {return value;}} leaf;};
    T value{};T get() const {return value;}
    template<int Offset> T shifted() const {return value+Offset;}
    template<int Offset> T shifted(int extra) const {return value+Offset+extra;}
  } first;
  struct {T value{};T get() const {return value;}} second;
  struct {T value{};T get() const {return value;}} third;
  T nested() const {return first.template shifted<0>()+second.get()+third.get();}
  [[nodiscard]] T get() const {return value;}
  template<class U> __attribute__((always_inline)) U call(U added) const {return U(value)+added;}
};
template<class T> struct Projected {struct {T value;T get() const {return value;}} storage;T get() const {return storage.get();}};
template<class T> struct Projected<T *> {struct {T *value;T get() const {return *value;}} storage;T get() const {return storage.get();}};
inline int nested_projection(int first,int second) {
  Family<int> value{0,{first},{second},{0}};
  using First=decltype(value.first);
  First::Inner<long> inner{{42}};
  Projected<int> direct{{first}};Projected<int *> indirect{{&second}};
  return inner.leaf.get()==42 && direct.get()==first && indirect.get()==second ? value.first.get()+value.second.get()+value.third.get() : -1;
}
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
__attribute__((malloc,alloc_size(1),alloc_align(2),returns_nonnull)) void *allocate(unsigned long size,unsigned long alignment);
void deallocate(void *value);
void deallocate_other(void *value);
__attribute__((malloc(deallocate,1),returns_nonnull)) int *allocated_value(unsigned long value);
__attribute__((assume_aligned(32,8))) void *aligned_offset(void *value);
__attribute__((nonnull(1))) int required(const int *value);
__attribute__((format(printf,1,2))) int formatted(const char *format,...);
__attribute__((format_arg(1))) const char *format_source(const char *value);
__attribute__((preserve_most,noinline)) int provider_preserved(int value);
template<class T> __attribute__((nonnull(1))) inline T required_template(const T *value){return *value;}
__attribute__((visibility("hidden"))) int provider_packet(Packet value);
Aligned provider_aligned(Aligned value);
Slot<int,32> provider_slot(Slot<int,32> value);
Compact provider_compact(int value);
int *provider_shared();
int *provider_storage();
int *provider_partial();
int provider_nested(Family<int> value);
Special<int> provider_special(Special<int> value);
FieldAligned provider_field(FieldAligned value);
// Native parameter contracts retain hidden size arguments and caller diagnostics.
unsigned long provider_extent(const void *value);
unsigned long provider_extent(const void *const value __attribute__((pass_object_size(0))));
unsigned long provider_object_size(const void *const value __attribute__((pass_object_size(0))));
unsigned long provider_dynamic_size(const void *const value __attribute__((pass_dynamic_object_size(0))));
inline unsigned long object_size(const void *const value __attribute__((pass_object_size(0)))) {return __builtin_object_size(value,0);}
inline unsigned long dynamic_size(const void *const value __attribute__((pass_dynamic_object_size(0)))) {return __builtin_dynamic_object_size(value,0);}
inline const int &lifetime(const int &value [[clang::lifetimebound]]) {return value;}
inline int noescape(int *value __attribute__((noescape))) {return *value;}
__attribute__((callback(1,2))) int provider_callback(int (*callback)(int),int value);
__attribute__((sentinel(0,0))) int provider_sentinel(const char *first,...);
inline constexpr int minimum=0;
inline int checked(int value) __attribute__((diagnose_if(value<minimum,"negative value","error"))) {return value;}
template<class T>inline T checked_template(T value) __attribute__((diagnose_if(value<0,"negative template value","error"))) {return value;}
__attribute__((returns_twice)) int returns_twice();
__attribute__((nothrow,leaf,noinline,disable_tail_calls,not_tail_called,nomerge,noduplicate)) int provider_controlled(int value);
__attribute__((optnone,noinline)) inline int unoptimized(int value) {return value+2;}
}
#endif
