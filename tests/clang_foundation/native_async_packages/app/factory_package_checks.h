#ifndef NATIVE_FACTORY_PACKAGE_CHECKS_H
#define NATIVE_FACTORY_PACKAGE_CHECKS_H

int factory_package_checks() {
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
  return 0;
}

#endif
