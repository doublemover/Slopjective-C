#ifndef NATIVE_FACTORY_PACKAGE_CHECKS_H
#define NATIVE_FACTORY_PACKAGE_CHECKS_H

int factory_package_checks() {
  {
    namespace Standard=NativeStandardCoroutines;
    static_assert(sizeof(Standard::Handle<int>)==sizeof(void *));
    static_assert(__is_same(Standard::IntPromise,Standard::Promise<int>));
    static_assert(__is_same(Standard::WidePromise,Standard::Promise<long>));
    static_assert(__is_same(typename std::coroutine_traits<Standard::External<int>,int>::promise_type,Standard::Promise<int>));
    static_assert(__is_same(typename std::coroutine_traits<Standard::External<long>,long>::promise_type,Standard::Promise<long>));
    if(Standard::provider_frames() || Standard::Counts::values)return 181;
    {
      auto task=Standard::provider_task(40);
      if(!task.resume() || Standard::Counts::values!=1 || task.run()!=42)return 182;
      auto generic=Standard::generic(40L);
      if(!generic.resume() || generic.handle.promise().value!=40 || generic.run()!=42)return 183;
      auto external=Standard::provider_external(40);
      Standard::Bridge bridge{external.handle};auto erased=Standard::erase(bridge.handle);
      if(erased.address()!=Standard::provider_address(external.handle) || external.run()!=42)return 184;
      auto partial=Standard::external_generic(40);auto full=Standard::external_full(40L);
      if(partial.run()!=42 || full.run()!=42)return 185;
      Standard::Worker worker{40};auto member=worker.run(2);if(member.run()!=42)return 186;
      auto factory=Standard::closure(40);auto closure=factory(2);if(closure.run()!=42)return 187;
      auto throwing=Standard::throwing();throwing.run();if(throwing.error()!=73)return 188;
      auto no_op=Standard::noop();if(!no_op || no_op.done())return 189;no_op.resume();no_op.destroy();
      {auto cancelled=Standard::external(40);if(!cancelled.resume() || Standard::Counts::values!=1)return 190;}
      if(Standard::Counts::values)return 191;
    }
    if(Standard::provider_frames() || Standard::Counts::values || Standard::Counts::destroyed!=9)return 192;
  }
  {
    namespace Attr=NativeDeclarationAttributes;
    static_assert(sizeof(Attr::Packet)==5 && alignof(Attr::Packet)==1);
    static_assert(alignof(Attr::FieldAligned)==32 && sizeof(Attr::FieldAligned)==64);
    static_assert(alignof(Attr::Special<int>)==32);
    static_assert(alignof(Attr::Aligned)==32 && alignof(Attr::Slot<int,32>)==32);
    static_assert(alignof(Attr::TypeAligned<long>)==alignof(long));
    static_assert(sizeof(Attr::Compact)==sizeof(int));
    if(Attr::provider_packet({2,40})!=42 || Attr::provider_aligned({42}).value!=42 ||
       Attr::provider_slot({42}).value!=42 || Attr::provider_shared()!=&Attr::shared ||
       reinterpret_cast<__UINTPTR_TYPE__>(&Attr::shared)%64 ||
       Attr::provider_field({2,40}).value!=40 || Attr::provider_storage()!=&Attr::storage<int> ||
       reinterpret_cast<__UINTPTR_TYPE__>(&Attr::storage<int>)%64 ||
       Attr::provider_special({42}).value!=42 || Attr::provider_partial()!=&Attr::partial<int *> ||
       reinterpret_cast<__UINTPTR_TYPE__>(&Attr::partial<int *>)%64 || Attr::partial<int *> != 42 ||
       Attr::selected(42)!=42)return 211;
    if(Attr::slow(40)!=42 || Attr::fast(40)!=42 || Attr::calculate(40)!=42 ||
       Attr::read(&Attr::shared)!=40 || Attr::cold(42)!=42 || Attr::hot(42)!=42 || Attr::answer(40)!=42 || Attr::parameter(40)!=42 ||
       Attr::project(42)!=42 || Attr::foreign(42)!=42 || Attr::ForeignBox<int>{42}.value!=42 ||
       Attr::Family<int>{40}.get()!=40 || Attr::Family<int>{40}.call(2L)!=42)return 212;
    {
      auto compact=Attr::provider_compact(40);auto copy=compact;
      if(compact.value!=40 || copy.value!=40 || Attr::Counts::live!=2)return 213;
    }
    if(Attr::Counts::live || Attr::Counts::destroyed!=2)return 214;
    auto packet_frame=NativeCustomCoroutines::attribute_packet(40);
    auto packet=packet_frame.run();if(packet.code+packet.value!=42)return 216;
    Attr::shared=42;Attr::storage<int> = 42;
    if(*Attr::provider_shared()!=42 || *Attr::provider_storage()!=42)return 215;
  }
  {
    namespace Custom=NativeCustomCoroutines;
    if(Custom::Counts::frames || Custom::Counts::values)return 201;
    {
      auto transformed=Custom::provider_task(40);
      if(Custom::provider_frames()!=1 || transformed.run()!=42 || transformed.error())return 202;
      auto yielded=Custom::generic(40L);
      if(!yielded.resume() || yielded.value()!=40 || Custom::Counts::values!=1)return 203;
      if(yielded.run()!=42 || Custom::Counts::values)return 204;
      Custom::Worker worker{40};auto member=worker.run(2);
      auto closure=Custom::closure(40);auto called=closure(2);
      if(member.run()!=42 || called.run()!=42)return 205;
      auto throwing=Custom::throwing();throwing.run();
      if(throwing.error()!=73 || Custom::Counts::values)return 206;
      {auto cancelled=Custom::transformed(40);cancelled.resume();if(Custom::Counts::values!=1)return 207;}
      if(Custom::Counts::values)return 208;
      Custom::Counts::fail_allocation=true;
      auto failed=Custom::provider_task(40);Custom::Counts::fail_allocation=false;
      if(failed.frame)return 209;
    }
    if(Custom::provider_frames() || Custom::Counts::values || Custom::Counts::destroyed!=6)return 210;
  }
  {
    namespace Projection=NativeFactoryTypeProjections;
    auto a=Projection::first(40);auto b=Projection::second(2);auto c=Projection::chosen(0);
    Projection::Reader reader=static_cast<Projection::Reader>(&Projection::read);
    Projection::ReverseReader reverse=static_cast<Projection::ReverseReader>(&Projection::read);
    Projection::Holder holder{a,b,c};Projection::Derived derived{a};auto member=Projection::Owner{}.make(42);
    if(reader(a,b)!=42 || reverse(b,a)!=-38 || holder.get()!=42 || derived.get()!=40 || Projection::member(member)!=42 ||
       Projection::pointer(&a)!=40 || Projection::provider_read(a,b)!=42 || Projection::provider_chosen(42).value!=42 ||
       &Projection::reference()!=Projection::provider_shared() || Projection::shared.value!=40)return 247;
    Projection::reference().value=42;if(Projection::provider_shared()->value!=42)return 248;
    auto owned=Projection::provider_owned(40);auto copy=owned;auto moved=static_cast<Projection::Owned &&>(owned);
    int provider_number=Projection::provider_owned_value(copy);
    if(owned.get()!=0 || copy.get()!=40 || moved.get()!=40 || provider_number!=40 ||
       NativeClosureFactories::State::live!=3)return 249;
  }
  if(NativeClosureFactories::State::live)return 250;
  {
    namespace Template=NativeFactoryTemplateArguments;
    namespace Projection=NativeFactoryTypeProjections;
    Template::Closed value{Projection::first(40)};Template::Nested nested{value};
    Template::Other other{Projection::second(2)};Template::Aliased alias{Projection::first(42)};
    Template::PointerBox pointer{&value.value};Template::ArrayBox array{{Projection::first(40),Projection::first(2)}};
    Template::MemberBox member{&Template::First::value};
    if(value.get()!=40 || Template::provider_value(value)!=40 || nested.value.get()!=40 || other.value.value!=2 ||
       alias.get()!=42 || pointer.value->value!=40 || array.value[0].value+array.value[1].value!=42 ||
       value.value.*member.value!=40 || Template::Packed::count!=3 || Template::read(value.value)!=40 ||
       Template::deduced(Projection::first(42))!=42 || Template::closed_lambda(42)!=42 ||
       Template::provider_slot()!=&Template::slot<Template::First> || Template::slot<Template::First>!=42)return 251;
    Template::slot<Template::First> = 40;if(*Template::provider_slot()!=40)return 252;
    auto owned=Template::provider_owned(40);auto copy=owned;
    if(owned.value.get()!=40 || copy.value.get()!=40 || NativeClosureFactories::State::live!=2)return 253;
  }
  if(NativeClosureFactories::State::live)return 254;
  {
    namespace Generic=NativeGenericFactoryProjections;
    auto a=Generic::make(40);auto wide=Generic::make(42L);
    auto member=Generic::Family<int>{}.make(2);auto method=Generic::Concrete{}.make(0);
    Generic::Holder holder{a,member,method};Generic::Wrapped wrapped{a};
    if(Generic::read(a)!=40 || Generic::read(wide)!=42 || holder.first.value+holder.second.value!=42 ||
       wrapped.value.value!=40 || Generic::local()!=42 || Generic::provider_read(a)!=40 ||
       Generic::provider_shared()!=&Generic::shared)return 255;
    Generic::shared.value=42;if(Generic::provider_shared()->value!=42)return 225;
    auto owned=Generic::provider_owned(40);auto copy=owned;auto moved=static_cast<Generic::Owned &&>(owned);
    if(owned.get().value!=0 || copy.get().value!=40 || moved.get().value!=40 || NativeClosureFactories::State::live!=3)return 226;
  }
  if(NativeClosureFactories::State::live)return 227;
  {
    namespace Value=NativeFactoryValueArguments;
    if(Value::First::get()!=42 || Value::Other::get()!=42 || Value::Partial::get()!=40 ||
       Value::Generic::get()!=42 || Value::Packed::get()!=42 || Value::Aliased::get()!=42 ||
       Value::read<Value::first(42)>()!=42 || Value::read<Value::second(42)>()!=42 ||
       Value::local()!=42 || Value::provider_value({})!=42 ||
       Value::provider_slot()!=&Value::slot<Value::first(42)>)return 228;
    Value::slot<Value::first(42)> = 40;if(*Value::provider_slot()!=40)return 229;
  }
  {
    namespace Closure=NativeClosureResultProjections;
    auto first=Closure::provider_factory(40);auto second=Closure::second(2);
    Closure::Holder holder{first,second};Closure::Wrapped wrapped{first};
    Closure::Generic generic=Closure::generic(40);
    Closure::Integer integer=Closure::factory(40);Closure::Wide wide=Closure::factory(40L);
    if(Closure::read(first)!=42 || Closure::read(second)!=4 || holder.get()!=42 || wrapped.get()!=42 ||
       generic(2)!=42 || generic(2L)!=42 || integer(2)!=42 || wide(2L)!=42 ||
       Closure::local()!=42 || Closure::structural()!=42 || Closure::provider_read(first)!=42 ||
       Closure::provider_shared()!=&Closure::shared || Closure::shared(2)!=42)return 230;
    auto owned=Closure::provider_owned(40);auto copy=owned;auto moved=static_cast<Closure::Owned &&>(owned);
    if(owned(0)!=0 || copy(2)!=42 || moved(2)!=42 || NativeClosureFactories::State::live!=3)return 231;
  }
  if(NativeClosureFactories::State::live)return 232;
  {
    namespace Lambda=NativeLambdaFactoryResults;
    auto first=Lambda::provider_factory(40);auto second=Lambda::second(0)(2);
    Lambda::Holder holder{first,second};Lambda::Wrapped wrapped{first};
    if(Lambda::read(first)!=42 || Lambda::read(second)!=2 || holder.first.value!=42 || wrapped.value.value!=42 ||
       Lambda::generic(40)(2).value!=42 || Lambda::generic(40)(2L).value!=42 ||
       Lambda::outer(40)(2).value!=42 || Lambda::outer(40L)(2L).value!=42 ||
       Lambda::nested(38)(2)(2).value!=42 || Lambda::local()!=42 || Lambda::Structural::get()!=42 ||
       Lambda::provider_read(first)!=42 || Lambda::provider_shared()!=&Lambda::shared)return 233;
    auto owned=Lambda::provider_owned(40);auto copy=owned;auto moved=static_cast<Lambda::Owned &&>(owned);
    if(owned.value.value!=0 || copy.value.value!=42 || moved.value.value!=42 || NativeClosureFactories::State::live!=3)return 234;
  }
  if(NativeClosureFactories::State::live)return 235;
  {
    namespace Nested=NativeNestedFactoryTypes;
    Nested::Left left{40};Nested::Right right{2};Nested::Holder holder{left,right};Nested::Wrapped wrapped{left};
    Nested::Integer integer{40};Nested::Wide wide{2L};Nested::Closure closure{42};
    Nested::Choice choice{.number=42};
    if(Nested::read(left)+Nested::read(right)!=42 || holder.left.value+holder.right.value!=42 || wrapped.value.value!=40 ||
       integer.value+wide.value!=42 || closure.value!=42 || choice.number!=42 || static_cast<int>(Nested::Code::good)!=42 ||
       Nested::provider_read(left)!=40 || Nested::provider_shared()!=&Nested::shared)return 236;
    auto aggregate=Nested::provider_aggregate(40);auto callback=Nested::provider_callback();
    auto member=Nested::provider_member(2);Nested::MemberSecond other=Nested::member_factory(40).second(2);
    if(aggregate.first.value+aggregate.second.value!=42 || callback.first(callback.second)!=42 ||
       Nested::wrapped(42).value.value!=42 || Nested::wrapped(42L).value.value!=42 ||
       Nested::provider_pointer(42)!=Nested::pointer(42) || &Nested::provider_reference()!=&Nested::reference() ||
       Nested::read(member)!=42 || Nested::read(other)!=42 || Nested::member_factory(40).make(2L).value!=42 ||
       Nested::member_factory(40).nested(2).value.value!=42)return 239;
    auto owned=Nested::provider_owned(42);auto copy=owned;auto moved=static_cast<Nested::Owned &&>(owned);
    if(owned.value.value!=0 || copy.value.value!=42 || moved.value.value!=42 || NativeClosureFactories::State::live!=3)return 237;
  }
  if(NativeClosureFactories::State::live)return 238;
  if(NativeForeignAttributes::read({40})+NativeForeignAttributes::increment(1)!=42 ||
     NativeForeignAttributes::hidden(42)!=42)return 240;
  return 0;
}

#endif
