#ifndef NATIVE_FACTORY_TYPE_PROJECTIONS_H
#define NATIVE_FACTORY_TYPE_PROJECTIONS_H
namespace NativeFactoryTypeProjections {
inline auto first(int n) {struct {int value;} result{n};return result;}
inline auto second(int n) {struct {int value;} result{n};return result;}
inline auto chosen(int n) {struct {int value;} a{1};struct {int value;} b{n};return b;}
inline auto chosen_owned(int n) {
  struct {NativeClosureFactories::State state;} a{NativeClosureFactories::State(1)};
  struct {NativeClosureFactories::State state;int get() const {return state.value;}} b{NativeClosureFactories::State(n)};
  return b;
}
using Owned=decltype(chosen_owned(0));
using First=decltype(first(0));
using Second=decltype(second(0));
using Chosen=decltype(chosen(0));
using Reader=int (*)(First,Second);
using ReverseReader=int (*)(Second,First);
struct Holder {
  First first;
  Second second;
  Chosen chosen;
  int get() const {return first.value+second.value+chosen.value;}
};
struct Derived:First {int get() const {return value;}};
inline First shared=first(40);
inline int read(First a,Second b) {return a.value+b.value;}
inline int read(Second a,First b) {return a.value-b.value;}
inline int pointer(const First *value) {return value->value;}
inline First &reference() {return shared;}
struct Owner {
  auto make(int n) const {struct Local {int value;};return Local{n};}
};
using Member=decltype(Owner{}.make(0));
inline int member(Member value) {return value.value;}
int provider_read(First a,Second b) {return a.value+b.value;}
First *provider_shared() {return &shared;}
Chosen provider_chosen(int n) {return chosen(n);}
Owned provider_owned(int n) {return chosen_owned(n);}
int provider_owned_value(Owned value) {return value.get();}
}
#endif
