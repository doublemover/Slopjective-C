#ifndef NATIVE_EXPLICIT_CXX_STORAGE_PACKAGE_H
#define NATIVE_EXPLICIT_CXX_STORAGE_PACKAGE_H
#include <objc3c/runtime/objc3_task.hpp>
namespace NativeExplicitStorage {
struct Counts {int live=0;int copies=0;int destroyed=0;};
struct Payload {
  Counts *counts;int number;
  Payload(Counts *counts,int n):counts(counts),number(n){++counts->live;}
  Payload(const Payload &other):counts(other.counts),number(other.number) {
    ++counts->live;++counts->copies;
  }
  ~Payload() noexcept {--counts->live;++counts->destroyed;}
};
union __attribute__((objc_unsafe_sendable)) Value {
  Payload payload;long other;
  Value(Counts *counts,int number):payload(counts,number){}
  Value(const Value &other):payload(other.payload){}
  ~Value() noexcept {payload.~Payload();}
  int number() const {return payload.number;}
};
template<class T> struct Entries {
  static T run(T value) noexcept __attribute__((objc_executor(main))) {return value;}
};
inline int member_pause() __objc3_async {__objc3_await objc3::task_yield;return 0;}
template<class T> struct __attribute__((objc_unsafe_sendable)) MemberEntries {
  Counts *counts;
  explicit MemberEntries(Counts *counts):counts(counts){++counts->live;}
  MemberEntries(const MemberEntries &)=delete;
  MemberEntries(MemberEntries &&)=delete;
  virtual ~MemberEntries() noexcept {--counts->live;++counts->destroyed;}
  T run(T value) const & noexcept __attribute__((objc_executor(main))) {return value;}
  T frame(T value) const & noexcept __objc3_async_frame __attribute__((objc_executor(main))) {
    __objc3_await member_pause();return value;
  }
  T task(T value) && noexcept __objc3_async __attribute__((objc_executor(main))) {
    __objc3_await objc3::task_yield;return value;
  }
  virtual T virtual_entry(T value) noexcept __attribute__((objc_executor(main))) {return value;}
};
template<class T> struct __attribute__((objc_unsafe_sendable)) DerivedEntries : MemberEntries<T> {
  explicit DerivedEntries(Counts *counts):MemberEntries<T>(counts){}
  T virtual_entry(T value) noexcept override {return value;}
};
template<class T> inline int member_generic(T value) __objc3_async_frame {
  Counts receivers;int number=0;
  {
    MemberEntries<T> receiver(&receivers);
    T first=__objc3_await receiver.run(static_cast<T&&>(value));
    T second=__objc3_await receiver.frame(static_cast<T&&>(first));
    T third=__objc3_await static_cast<MemberEntries<T>&&>(receiver).task(static_cast<T&&>(second));
    DerivedEntries<T> derived(&receivers);
    MemberEntries<T> *base=&derived;
    T result=__objc3_await base->virtual_entry(static_cast<T&&>(third));
    number=result.number();
  }
  return receivers.live==0 && receivers.destroyed==2?number:0;
}
template<class T> inline int generic(T value) __objc3_async_frame {
  T result=__objc3_await Entries<T>::run(static_cast<T &&>(value));
  return result.number();
}
inline int roundtrip(int number) __objc3_async_frame {
  Counts counts;int result=0;
  {
    Value source(&counts,number);
    Value value=__objc3_await Entries<Value>::run(static_cast<Value &&>(source));
    result=value.number();
  }
  return counts.live==0 && counts.copies>0 && counts.destroyed>0?result:0;
}
}
#endif
