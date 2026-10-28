#include <objc3c/runtime/objc3_task.hpp>
#include <objc/runtime.h>
#include <pthread.h>
@import fixture.NativeAsync;
struct native_thread_observation { bool passed; const void *ordinary; const void *shared; };
static void *native_thread_worker(void *context) {
  auto *observation=static_cast<native_thread_observation *>(context);
  observation->ordinary=&native_thread_value;
  observation->shared=&native_inline_thread_object;
  observation->passed=native_thread_sum()==240 && native_local_thread_value()==41 &&
      native_thread_local_bridge()==42;
  ++native_thread_value; ++native_inline_thread_value;
  ++NativeThreadState::value; ++NativeThreadState::shared;
  ++native_thread_object.value; ++native_inline_thread_object.value;
  observation->passed &= native_thread_sum()==246;
  return nullptr;
}
static void *native_variable_template_worker(void *context) {
  auto *passed=static_cast<bool *>(context);
  *passed=native_template_tls<> ==40 && native_template_shared<> ==42;
  *passed &= NativeNestedFamily<int>::local<> ==40 && NativeNestedFamily<int>::shared<> ==42;
  *passed &= NativeConcreteTemplates::local<> ==40 && NativeConcreteTemplates::shared<> ==42;
  ++NativeConcreteTemplates::local<>;
  *passed &= NativeConcreteTemplates::local<> ==41;
  ++NativeNestedFamily<int>::local<>;
  *passed &= NativeNestedFamily<int>::local<> ==41;
  ++native_template_tls<>;
  *passed &= native_variable_template_producer()==85 && native_template_tls<> ==42;
  return nullptr;
}
template<class T> concept native_has_number = requires(const T &value) { value.number(); };
static int imported_dynamic_order=NativeInline::ordered;
static_assert(!native_has_number<NativeDeletedMember>);
static_assert(sizeof(NativeDeletedMember) == sizeof(int));
template<class T> concept native_public_first = requires(T *p) { static_cast<NativeFirstBase *>(p); };
static_assert(native_public_first<NativeDerivedValue>);
static_assert(!native_public_first<NativeAccessValue>);
static_assert(std::is_trivially_copyable_v<NativeDefaultedValue>);
static_assert(std::is_aggregate_v<NativeAggregate>);
static_assert(std::is_abstract_v<NativeVirtualRoot>);
static_assert(std::is_polymorphic_v<NativeVirtualDiamond>);
static_assert(std::is_final_v<NativeVirtualDiamond>);
static_assert(std::has_virtual_destructor_v<NativeVirtualRoot>);
static_assert(std::is_same_v<NativePublished::Scalar, long>);
static_assert(std::is_same_v<NativeOther::Scalar, int>);
template<class T> concept native_has_hidden_type = requires { typename T::Hidden; };
template<class T> concept native_has_counter_type = requires { typename T::Counter; };
static_assert(!native_has_hidden_type<NativeFamily> && !native_has_counter_type<NativeFamily>);
static_assert(std::is_same_v<NativeFamily::Result,NativeFamily::Value>);
static_assert(static_cast<unsigned>(NativeFamily::Kind::Answer)==42);

static_assert(std::is_same_v<decltype(native_module_object(1)),
                             objc3::task<NativeModuleResult *>>);
static_assert(std::is_same_v<decltype(native_module_block(1)),
                             objc3::task<native_module_reader>>);

#if !__has_feature(objc_arc)
#include <objc/objc-arc.h>
struct consumer_result_owner {
  id value;
  ~consumer_result_owner() { objc_release(value); }
};
#endif

int checked_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  __objc3_do {
    int success = __objc3_try(__objc3_await native_module_checked(42, false));
    if (success != 42) return 0;
    return __objc3_try(__objc3_await native_module_checked(96, true));
  } __objc3_catch(id<NativeModuleError> error) {
    __objc3_await objc3::task_yield;
    objc3_current_actor_facts facts{};
    facts.struct_size = sizeof(facts);
    facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    if (objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK ||
        facts.actor_identity != objc3_actor_identity(actor)) return 0;
    return [(NativeModuleResult *)error number] == 96 ? 42 : 0;
  }
}

int actor_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  auto expected = objc3_actor_identity(actor);
  int result = __objc3_await native_module_value(41);
  NativeModuleResult *object = __objc3_await native_module_object(21);
#if !__has_feature(objc_arc)
  consumer_result_owner owner{objc_retain(object)};
#endif
  native_module_reader reader = __objc3_await native_module_block(21);
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  if (objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK ||
      facts.actor_identity != expected)
    return 0;
  return result == 42 ? [object number] + reader(0) : 0;
}

int frame_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  auto frame = native_module_frame(41);
  int value = __objc3_await objc3::task<int>::from_native(&frame);
  if (frame.context || value != 42) return 0;
  frame = native_module_object_frame(21);
  NativeModuleResult *object =
      __objc3_await objc3::task<NativeModuleResult *>::from_native(&frame);
#if !__has_feature(objc_arc)
  consumer_result_owner owner{objc_retain(object)};
#endif
  if (frame.context) return 0;
  frame = native_module_block_frame(21);
  native_module_reader reader =
      __objc3_await objc3::task<native_module_reader>::from_native(&frame);
  return !frame.context ? [object number] + reader(0) : 0;
}


int member_application(NativeModuleWorker &worker, NativeModuleWorker &base,
                       objc3_actor_t *actor) __objc3_async {
  static_assert(std::is_same_v<decltype(worker(2)), objc3::task<int>>);
  using Member = int (NativeModuleWorker::*)(int) const __objc3_async;
  using Checked = int (NativeModuleWorker::*)(int, bool) const
      __objc3_async __objc3_throws(id<NativeModuleError>);
  Member member = &NativeModuleWorker::value;
  Checked checked = &NativeModuleWorker::checked;
  __objc3_await objc3::hop_to(actor);
  if (__objc3_await (base.*member)(2) != 42 || __objc3_await worker(3) != 43)
    return 0;
  __objc3_do {
    if (__objc3_try(__objc3_await base.checked(42, false)) != 42) return 0;
    return __objc3_try(__objc3_await (base.*checked)(96, true));
  } __objc3_catch(id<NativeModuleError> error) {
    __objc3_await objc3::task_yield;
    return [(NativeModuleResult *)error number] == 96 ? 42 : 0;
  }
}

int checked_values_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  using Record = NativeModuleValue (*)(int, bool)
      __objc3_async __objc3_throws(id<NativeModuleError>);
  Record function = native_module_record;
  auto value = __objc3_try_force(__objc3_await function(42, false));
  auto number = __objc3_try_force(__objc3_await native_module_complex_value(40, false));
  auto data = __objc3_try_force(__objc3_await native_module_data_value(false));
  auto some = __objc3_try_optional(__objc3_await function(42, false));
  auto none = __objc3_try_optional(__objc3_await function(96, true));
  if (!__objc3_has_value(some) || __objc3_has_value(none) || value.*data != 42 ||
      __real__ number + __imag__ number != 42 || value.padding[4] != 5) return 0;
  __objc3_do {
    (void)__objc3_try(__objc3_await function(96, true));
  } __objc3_catch(id<NativeModuleError> error) {
    __objc3_await objc3::task_yield;
    objc3_current_actor_facts facts{};
    facts.struct_size = sizeof(facts); facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    return objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK &&
        facts.actor_identity == objc3_actor_identity(actor) &&
        [(NativeModuleResult *)error number] == 96 ? 42 : 0;
  }
  return 0;
}

int objc_method_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  NativeModuleMethods *receiver = [NativeModuleMethods new];
#if !__has_feature(objc_arc)
  consumer_result_owner owner{receiver};
#endif
  static_assert(std::is_same_v<decltype([receiver direct:42]), objc3::task<int>>);
  id<NativeModuleMethodProtocol> protocol = receiver;
  __objc3_do {
    int scalar = __objc3_try(__objc3_await [protocol checked:42 fail:false]);
    auto record = __objc3_try(__objc3_await [receiver record:42 fail:false]);
    auto present = __objc3_try_optional(__objc3_await [receiver record:42 fail:false]);
    auto absent = __objc3_try_optional(__objc3_await [receiver record:42 fail:true]);
    NativeModuleResult *object = __objc3_await [receiver object:42];
#if !__has_feature(objc_arc)
    consumer_result_owner result_owner{objc_retain(object)};
#endif
    int direct = __objc3_await [receiver direct:42];
    if (scalar != 42 || record.number != 42 || record.padding[4] != 5 ||
        !__objc3_has_value(present) || __objc3_has_value(absent) ||
        [object number] != 42 || direct != 42) return 0;
    return __objc3_try(__objc3_await [protocol checked:96 fail:true]);
  } __objc3_catch(id<NativeModuleError> error) {
    __objc3_await objc3::task_yield;
    objc3_current_actor_facts facts{};
    facts.struct_size = sizeof(facts); facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    if (objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK ||
        facts.actor_identity != objc3_actor_identity(actor)) return 0;
    return [(NativeModuleResult *)error number] == 96 ? 42 : 0;
  }
}

int block_application(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  native_module_async_block block = __objc3_await native_module_async_factory(40);
#if !__has_feature(objc_arc)
  consumer_result_owner block_owner{objc_retain((id)block)};
#endif
  native_module_checked_block checked = __objc3_await native_module_checked_factory();
#if !__has_feature(objc_arc)
  consumer_result_owner checked_owner{objc_retain((id)checked)};
#endif
  static_assert(std::is_same_v<decltype(block(2)), objc3::task<int>>);
  if (__objc3_await block(2) != 42) return 0;
  __objc3_do {
    auto value = __objc3_try(__objc3_await checked(42, false));
    auto absent = __objc3_try_optional(__objc3_await checked(96, true));
    if (value.number != 42 || value.padding[4] != 5 || __objc3_has_value(absent)) return 0;
    (void)__objc3_try(__objc3_await checked(96, true));
  } __objc3_catch(id<NativeModuleError> error) {
    __objc3_await objc3::task_yield;
    objc3_current_actor_facts facts{};
    facts.struct_size = sizeof(facts); facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    return objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK &&
        facts.actor_identity == objc3_actor_identity(actor) &&
        [(NativeModuleResult *)error number] == 96 ? 42 : 0;
  }
  return 0;
}

int main() {
  {
    char small=2;float promoted=2.0f;
    if(native_variadic_provider()!=42 || native_variadic_sum<long>(2,40,small)!=42 ||
       native_variadic_sum<>(0)!=0 || NativeVariadic<long>{}.sum(2,40,small)!=42 ||
       NativeVariadic<int>{}.add<long>(2,40.0,promoted)!=42)return 160;
    int (*indirect)(unsigned,...)=&native_variadic_sum<int>;
    if(indirect(2,40,2)!=42)return 161;
  }
  {
    static_assert(NativeConcreteTemplates::answer<40> ==42);
    if(native_concrete_state()!=123 || NativeConcreteTemplates::next<>()!=42 ||
       ++NativeConcreteTemplates::shared<> !=42 || ++NativeConcreteTemplates::local<> !=42)return 162;
    NativeConcreteTemplates::Array<2> parts={40,2};
    auto *value=native_concrete_make();
    NativeConcreteTemplates::Pointer<> empty=nullptr;
    if(empty || value->number()!=40 || value->add<long>()!=42 || (*value)(2L)!=42 ||
       NativeConcreteTemplates::reader(parts[0]+parts[1])()!=42)return 163;
    auto copy=*value;delete value;
    if(copy.add(2.0)!=42.0)return 164;
    static_assert(native_concrete_nested_probe()==42);
    auto *nested=native_concrete_nested_make();
    if(nested->number()!=42 || NativeConcreteTemplates::Box<int>::live!=1)return 165;
    {
      auto other=*nested;delete nested;
      if(other.add(0L)!=42 || NativeConcreteTemplates::Box<int>::live!=1)return 166;
    }
    if(NativeConcreteTemplates::Box<int>::live!=0)return 167;
    NativeConcreteTemplates::Box<long,3> different(39);
    NativeConcreteTemplates::Box<long,3>::Pointer<> pointer=nullptr;
    if(different.add(0)!=42 || pointer)return 168;
  }
  static_assert(NativeNestedFamily<long>::answer<40> ==42 && native_nested_argument_probe()==42);
  if(native_nested_state()!=82 || ++NativeNestedFamily<int>::shared<> !=42 ||
     ++NativeNestedFamily<int>::local<> !=42 || NativeNestedFamily<long>::shared<> !=40)return 154;
  {
    auto *value=native_make_nested();
    if(value->number()!=42 || NativeNestedFamily<int>::Box<int>::live!=1)return 155;
    auto copy=*value;
    if(copy.add(0L)!=42 || NativeNestedFamily<int>::Box<int>::live!=2)return 156;
    delete value;
    if(NativeNestedFamily<int>::Box<int>::live!=1)return 157;
    NativeNestedFamily<long>::Box<long,3> local(39);
    NativeNestedFamily<long>::Detail detail(42);
    NativeNestedFamily<long>::Box<long>::Pointer<> pointer=nullptr;
    if(local.add(0)!=42 || detail.number()!=42 || pointer)return 158;
  }
  if(NativeNestedFamily<int>::Box<int>::live!=0 || NativeNestedFamily<long>::Box<long,3>::live!=0)return 159;

  {
    NativeMemberAliases<long>::Pointer<> pointer=nullptr;
    NativeMemberAliases<long>::Array<2> values={40,2};
    NativeMemberAliases<long>::Element<NativeTemplateBox<int>> result=42;
    NativeMemberAliases<long>::Callback<short,int> callback=[](short a,int b)->long{return a+b;};
    if(pointer || callback(values[0],values[1])!=42 || result!=42 || native_member_alias_probe()!=42)return 151;
    NativeMemberAliases<int>::Rebind<long> value(42);auto copy=value;
    if(copy.number()!=42 || NativeTemplateValue<long>::live!=2)return 152;
  }
  if(NativeTemplateValue<long>::live!=0 || NativeTemplateValue<int>::live!=0)return 153;

  static_assert(native_member_probe()==42);
  static_assert(NativeMemberBox<long>{40}.add(0L)==42);
  static_assert(NativeMemberBox<long>{40}.sum(1,1L)==42);
  static_assert(NativeMemberBox<long>{40}.reader(1L)(1)==42);
  static_assert(NativeMemberBox<long>{42}(0)==42);
  static_assert(static_cast<long>(NativeMemberBox<int>{42})==42);
  if(native_member_producer()!=1 || NativeMemberBox<int>::next<>()!=2 ||
     NativeMemberBox<int>::next<long>()!=1 || NativeMemberBox<long>::next<>()!=1)return 147;
  {
    NativeMemberBox<long> box(40);++box.reference<>();
    if(box.add(0)!=43 || box.add()!=45)return 148;
  }
  {
    NativeTemplateValue<int> value(42);
    NativeMemberBox<NativeTemplateValue<int>> box(value);
    auto reader=box.map([](const NativeTemplateValue<int> &n){return n.number();});
    auto copy=reader;
    if(reader()!=42 || copy()!=42 || NativeTemplateValue<int>::live!=4)return 149;
  }
  if(NativeTemplateValue<int>::live!=0)return 150;

  {
    NativeForeignCounts counts{};
    NativeForeignRef owner=native_foreign_create(&counts,42);
    if(*native_foreign_borrow(owner)!=42) return 101;
    NativeForeignConsumer callback=native_foreign_callback();
    if(native_foreign_select(callback)!=42 ||
       native_foreign_select(+[](NativeForeignRef){})!=43) return 106;
    native_foreign_forward(callback,owner);
    if(counts.consumed!=1 || counts.destroyed!=1) return 102;
    owner=native_foreign_create(&counts,43);
    callback(owner);
    if(counts.consumed!=2 || counts.destroyed!=2) return 103;
    owner=native_foreign_create(&counts,44);
    if(!native_foreign_scrubbed(owner)) return 109;
    native_foreign_captured_release(owner);
    if(counts.consumed!=3 || counts.destroyed!=3) return 110;
  }
  {
    const int before=native_result_destructions();
    {
      NativeRetainedFactory factory=native_retained_callback();
      id result=factory(42);
#if !__has_feature(objc_arc)
      consumer_result_owner cleanup{result};
#endif
      if([(NativeModuleResult *)result number]!=42) return 104;
    }
    if(native_result_destructions()!=before+1) return 105;
  }
  {
    const int before=native_result_destructions();
    {
      id result=native_retained_create(42);
#if __has_feature(objc_arc)
      NativeIntegratedAlias _Nonnull view=(__bridge NativeIntegratedAlias _Nonnull)result;
      result=nil;
      if(native_integrated_value(view)!=42) return 107;
      view=nullptr;
#else
      consumer_result_owner cleanup{result};
      NativeIntegratedAlias _Nonnull view=(NativeIntegratedAlias _Nonnull)result;
      if(native_integrated_value(view)!=42) return 107;
#endif
    }
    if(native_result_destructions()!=before+1) return 108;
  }
  if (NativeHiddenScope::get({40})!=42 || NativeHiddenScope::later(40)!=42 ||
      NativeVersioned::get({40})!=42) return 85;

  {
    static_assert(NativeImmediate{}.get()==42);
    static_assert(NativeImmediate{}==NativeImmediate{});
    static_assert(NativeImmediateOut{}.value==42);
    static_assert(NativeConstexprOut{}.value==42 && native_constexpr_out.value==42);
    NativeConstexprOut ordinary;
    if (ordinary.value!=42) return 84;
    static_assert(native_immediate.get()==42 && native_immediate_out.value==42);
    NativeImmediate value{};
    if (value.get()!=42 || (&native_immediate)->get()!=42) return 83;
  }

  {
    static_assert(NativeComparable{42,{1,2}}==NativeComparable{42,{1,2}});
    static_assert(NativeComparable{42,{1,2}}!=NativeComparable{42,{2,1}});
    static_assert(NativeFriendComparable{{42,{1,2}}}!=NativeFriendComparable{{43,{1,2}}});
    NativeFriendComparable first{{42,{1,2}}}, second{{42,{1,2}}};
    if (!(first==second) || !native_module_compare(first,second)) return 77;
    second.value.values[1]=3;
    if (!(first!=second) || native_module_compare(first,second)) return 78;
    NativeDerivedComparable left{{42,{1,2}},2}, right{{42,{1,2}},2};
    if (!(left==right)) return 79;
    right.extra=3;
    if (!(left!=right)) return 80;
  }

  {
    if (imported_dynamic_order!=42 || NativeInline::dynamic!=40 || NativeInline::ordered!=42) return 76;
    volatile int forty=40;
    if (NativeInline::client_only(forty)!=42) return 74;
    int *client_total=&NativeInline::client_total;
    ++*client_total;
    if (*client_total!=41) return 75;
    static_assert(NativeInline::compute(21)==42);
    static_assert(NativeInline::immediate(40)==42);
    constexpr NativeInline::Value constant(40);
    static_assert(constant.get()==42);
    NativeInline::total=40;
    if (NativeInline::step()!=1 || NativeInline::provider_step(2)!=2 ||
        NativeInline::provider_total()!=42 || NativeInline::total!=42) return 72;
    NativeInline::Value::updates=0;
    NativeInline::Value value(40);
    if (value.get()!=42 || value.update()!=43 || NativeInline::provider_total()!=43 ||
        NativeInline::provider_updates()!=1) return 73;
  }

  {
    NativeDefaults::evaluations=0;
    if (NativeDefaults::accept()!=42 || NativeDefaults::evaluations!=1) return 65;
    NativeDefaults::Aggregate aggregate{};
    if (aggregate.value+aggregate.extra!=42 || NativeDefaults::evaluations!=2) return 66;
    NativeDefaults::Object implicit;
    if (implicit.value!=40 || NativeDefaults::evaluations!=3) return 67;
    NativeDefaults::Object explicit_value(2);
    if (explicit_value.value!=42 || NativeDefaults::evaluations!=4) return 68;
    if (implicit.get()!=80 || NativeDefaults::evaluations!=5) return 69;

  }

  objc3_executor_t *executor = nullptr;
  if (objc3_executor_create(1, &executor) != OBJC3_SERVICE_OK)
    return 1;
  {
    objc3_executor_t *main_destination=nullptr;
    if (objc3_executor_get_main(&main_destination)!=OBJC3_SERVICE_OK) return 96;
    auto frame=__objc3_async_from_frame<int>(native_published_isolation_frame(40)).start(executor);
    auto task=native_published_isolation_task(40).start(executor);
    auto task_entry_frame=__objc3_async_from_frame<int>(native_published_task_entry_frame(40)).start(executor);
    auto task_entry=native_published_task_entry(40).start(executor);
    for (unsigned step=0;step!=1000;++step) {
      uint32_t count=0;
      if (objc3_executor_pump(main_destination,32,&count)!=OBJC3_SERVICE_OK ||
          objc3_executor_pump(executor,32,&count)!=OBJC3_SERVICE_OK) return 97;
      objc3_completion first{sizeof(first),OBJC3_SERVICE_ABI_VERSION};
      objc3_completion second{sizeof(second),OBJC3_SERVICE_ABI_VERSION};
      objc3_completion third{sizeof(third),OBJC3_SERVICE_ABI_VERSION};
      objc3_completion fourth{sizeof(fourth),OBJC3_SERVICE_ABI_VERSION};
      if (objc3_task_get_completion(frame.native_handle(),&first)==OBJC3_SERVICE_OK &&
          objc3_task_get_completion(task.native_handle(),&second)==OBJC3_SERVICE_OK &&
          objc3_task_get_completion(task_entry_frame.native_handle(),&third)==OBJC3_SERVICE_OK &&
          objc3_task_get_completion(task_entry.native_handle(),&fourth)==OBJC3_SERVICE_OK) break;
    }
    if (frame.result()!=42 || task.result()!=42 ||
        task_entry_frame.result()!=42 || task_entry.result()!=42) return 98;
    objc3_executor_release(main_destination);
  }

  {
    auto frame=__objc3_async_from_frame<int>(NativeDefaults::frame()).start(executor);
    if (NativeDefaults::evaluations!=6) return 70;
    uint32_t completed=0;
    if (objc3_executor_pump(executor,100,&completed)!=OBJC3_SERVICE_OK ||
        frame.result()!=42 || NativeDefaults::evaluations!=6) return 71;
  }
  {
    static_assert(!std::is_trivially_copyable_v<NativeOutOfLineDefaulted>);
    static_assert(!std::is_trivially_default_constructible_v<NativeOutOfLineDefaulted>);
    NativeOutOfLineDefaulted value;
    NativeOutOfLineDefaulted copy(value);
    NativeOutOfLineDefaulted moved(std::move(copy));
    copy=value;
    copy=std::move(moved);
    if (value.get()!=42 || copy.get()!=42) return 81;
    auto returned=__objc3_async_from_frame<NativeOutOfLineDefaulted>(native_module_defaulted(std::move(value))).start(executor);
    uint32_t done=0;
    if (objc3_executor_pump(executor,100,&done)!=OBJC3_SERVICE_OK || returned.result().get()!=42) return 82;
  }
  {
    NativeClosureDefaults defaults;
    if (defaults.get()!=42 || native_closure(40)!=42) return 86;
    auto frame=__objc3_async_from_frame<int>(native_closure_frame()).start(executor);
    uint32_t done=0;
    if (objc3_executor_pump(executor,100,&done)!=OBJC3_SERVICE_OK || frame.result()!=42) return 87;
  }
  {
    if (native_thread_sum()!=240 || native_local_thread_value()!=41 ||
        native_thread_local_bridge()!=42 || native_thread_lifetimes(0)!=2 ||
        native_thread_lifetimes(1)!=0) return 88;
    native_thread_value+=2;
    native_thread_observation observation{};
    pthread_t worker;
    if (pthread_create(&worker,nullptr,native_thread_worker,&observation)!=0) return 89;
    if (pthread_join(worker,nullptr)!=0 || !observation.passed ||
        observation.ordinary==&native_thread_value || observation.shared==&native_inline_thread_object ||
        native_thread_sum()!=242 || native_thread_lifetimes(0)!=4 ||
        native_thread_lifetimes(1)!=2) return 90;
  }
  {
    constexpr NativeFriendValue value(40);
    static_assert(native_friend_reveal(value)==40);
    if (!(value==NativeFriendValue(40)) || value==NativeFriendValue(41) ||
        NativeFriendReader::read(value)!=40 || native_friend_external(value)!=42 ||
        native_friend_delayed(value)!=42 || probe(NativeFriendScope::Value{40})!=42) return 91;
  }
  {
    if (native_statement_live()!=0) return 93;
    auto value=native_statement_value();
    NativeBlockDefaults defaults;
    if (native_published_block(40)!=42 || native_published_statement(40)!=42 ||
        value.value!=42 || native_statement_live()!=1 || defaults.get()!=42) return 92;
  }
  if (native_statement_live()!=0) return 94;
  if (native_published_carrier_capture(2)!=44 || native_statement_live()!=0) return 95;
  if (native_published_mixed_capture(42)!=42 || native_statement_live()!=0) return 96;
  if (native_published_plain_native_capture(40)!=42) return 97;
  static_assert(native_local_constant()==42);
  static_assert(native_unnamed_constant()==42);
  if (native_generic_probe(42)!=42 || native_generic_probe(42L)!=42L) return 116;
  if (native_generic_captured_local(1)!=42) return 117;
  if (native_union_throwing(40)!=42) return 119;
  static_assert(NativeNumber<int> && NativeWide<int> && !NativeWide<char> &&
                NativePositive<2> && !NativePositive<-1> && NativeNumbers<int,long>);
  static_assert(native_concept_value()==42);
  if (native_concept_bridge()!=42) return 120;
  static_assert(native_forwarded()==42 && native_forwarded<long,3>(39)==42);
  static_assert(native_late_default(40)==42 && native_tail_decl()==42);
  static_assert(native_constrained_default()==42 && native_template_argument_default()==42);
  if(native_template_defaults_bridge()!=210) return 128;
  static_assert(native_template_deduced_add(40L)==42 && native_template_make_reader(40L)(2)==42);
  static_assert(native_template_deduced_answer<long> ==42 && NativeTemplateHolder<long>{40}.number()==42);
  static_assert(NativeTemplateHolder<long>{40}.reader()(2)==42);
  {
    int value=40;++native_template_deduced_borrow(value);
    NativeTemplateHolder<int> holder{40};++holder.reference();
    if(value!=41 || holder.number()!=43 || native_template_deduced_reader<long>(2)!=42 ||
       native_template_deduction_bridge()!=84)return 144;
  }
  {
    NativeTemplateValue<int> value(42);
    auto reader=native_template_object_reader(value);auto copy=reader;
    if(reader()!=42 || copy()!=42 || NativeTemplateValue<int>::live!=3)return 145;
  }
  if(NativeTemplateValue<int>::live!=0)return 146;
  if(native_template_forward_value<> !=sizeof(int) || native_template_forward_value<long> !=sizeof(long))return 139;
  {
    auto *root=native_template_hierarchy_new();
    auto *left=dynamic_cast<NativeTemplateLeft<int> *>(root);
    auto *right=dynamic_cast<NativeTemplateRight<int> *>(root);
    if(!left || !right || static_cast<NativeTemplateRoot<int> *>(left)!=static_cast<NativeTemplateRoot<int> *>(right) ||
       root->number()!=42 || NativeTemplateRoot<int>::live!=1)return 140;
    delete root;
  }
  if(NativeTemplateRoot<int>::live!=0)return 141;
  {
    NativeTemplateDiamond<long> value;
    NativeTemplateRoot<long> *root=&value;
    if(!dynamic_cast<NativeTemplateLeft<long> *>(root) || !dynamic_cast<NativeTemplateRight<long> *>(root) ||
       root->number()!=42 || NativeTemplateRoot<long>::live!=1)return 142;
  }
  if(NativeTemplateRoot<long>::live!=0)return 143;
  static_assert(NativeTemplateBox<>{}.number()==42 && NativeTemplateBox<long,3>(39).number()==42);
  static_assert(__is_same(NativeTemplateBox<long>::element,long));
  static_assert(NativeTemplateArity<int,long,double>::count==3);
  if(native_class_template_producer()!=1 || NativeTemplateBox<>::next()!=2 ||
     NativeTemplateBox<long>::next()!=1) return 131;
  {
    NativeTemplateValue<long> first(42);auto second=first;
    if(second.number()!=42 || NativeTemplateValue<long>::live!=2) return 132;
  }
  if(NativeTemplateValue<long>::live!=0 || NativeTemplateValue<int>::live!=0) return 133;
  {
    NativeTemplateValue<int> value(42);
    auto pending=native_class_template_task(value);
    if(NativeTemplateValue<int>::live!=2) return 134;
    auto started=std::move(pending).start(executor);
    uint32_t completed=0;
    if(objc3_executor_pump(executor,100,&completed)!=OBJC3_SERVICE_OK ||
       started.result().number()!=42) return 135;
  }
  if(NativeTemplateValue<int>::live!=0) return 136;
  {
    NativeTemplateValue<int> value(42);
    auto returned=__objc3_async_from_frame<NativeTemplateValue<int>>(native_class_template_frame(value)).start(executor);
    uint32_t completed=0;
    if(objc3_executor_pump(executor,100,&completed)!=OBJC3_SERVICE_OK ||
       returned.result().number()!=42) return 137;
  }
  if(NativeTemplateValue<int>::live!=0) return 138;
  static_assert(native_template_answer<> ==42 && native_template_answer<long,39> ==41);
  static_assert(native_template_arity<int,long,double> ==3 && native_template_width<char> ==1);
  if(native_variable_template_producer()!=82 || ++native_template_shared<> !=42 ||
     ++native_template_tls<> !=42 || native_template_zero<long> !=0) return 129;
  bool variable_thread_passed=false;pthread_t variable_thread;
  if(pthread_create(&variable_thread,nullptr,native_variable_template_worker,&variable_thread_passed) ||
     pthread_join(variable_thread,nullptr) || !variable_thread_passed ||
     NativeNestedFamily<int>::local<> !=42 || NativeNestedFamily<int>::shared<> !=42 ||
     NativeConcreteTemplates::local<> !=42 || NativeConcreteTemplates::shared<> !=42 ||
     native_template_tls<> !=42 || native_template_shared<> !=43) return 130;

  static_assert(__is_same(NativeTemplatePointer<>,int*));
  static_assert(__is_same(NativeTemplateElement<NativeAliasBox>,int));
  static_assert(__is_same(NativeNumericPointer<long>,long*));
  NativeTemplateCallback<int,int> alias_callback=+[](int a,int b){return a+b;};
  if(alias_callback(20,22)!=42) return 127;
  static_assert(native_template_add(40)==42 && native_template_add<long,3>(39)==42);
  static_assert(native_template_sum(20,22)==42 && native_template_rank(1)==42);
  if (native_template_producer()!=40 || native_template_count<40>()!=41 ||
      native_template_count<1>()!=1 || native_template_bridge()!=126) return 121;
  {
    NativeClassStats stats{};
    {
      NativeClassValue source(&stats,40);
      auto result=native_template_copy(source);
      if(result.number()!=40 || stats.live!=2 || stats.copied!=1 || stats.moved!=1) return 122;
    }
    if(stats.live || stats.destroyed!=3) return 123;
  }
  {
    NativeClassStats stats{};
    {
      NativeClassValue source(&stats,40);
      try {auto result=native_template_fail_copy(source);(void)result;return 124;}
      catch(int code) {if(code!=7 || stats.live!=1 || stats.copied!=1 || stats.destroyed!=1) return 125;}
    }
    if(stats.live || stats.destroyed!=2) return 126;
  }
  static_assert(native_union_choices()==84);
  static_assert(__builtin_offsetof(NativePlainUnion,integer)==0 &&
                __builtin_offsetof(NativePlainUnion,floating)==0);
  auto union_value=native_union_make(42);
  if (native_union_read(union_value)!=42 || native_union_lifecycle(40)!=42 ||
      native_union_local(40)!=42 || native_union_bridge(40)!=84) return 118;
  static_assert(native_generic_float_bits()==6);
  if (native_generic_float_bits()!=6) return 115;
  static_assert(native_generic_contracts()==210);
  if (native_generic_contracts_bridge()!=210) return 114;
  static_assert(native_generic_constant()==42);
  static_assert(native_generic_immediate()==42 && native_generic_default()==42);
  if (native_generic_conversion(40)!=42 || native_generic_mutable(40)!=42 ||
      native_generic_record(40)!=42 || native_generic_static(40)!=42) return 113;
  if (native_generic_local(40)!=42 || native_generic_nested(40)!=42 ||
      native_generic_bridge(40)!=84) return 112;
  if (native_unnamed_nested(40)!=42 || native_unnamed_copies(40)!=42 ||
      native_unnamed_virtual(40)!=42 || native_unnamed_bridge(40)!=126) return 111;
  if (native_published_local_type(40)!=42 || native_local_type_bridge(40)!=42 ||
      native_local_lambda(40)!=42) return 98;
  auto pending = native_module_value(41).start(executor);
  uint32_t executed = 0;
  if (objc3_executor_pump(executor, 2, &executed) != OBJC3_SERVICE_OK ||
      executed != 2 || pending.result() != 42)
    return 2;
  pending = {};
  auto published_task=native_published_task_await(41).start(executor);
  if (objc3_executor_pump(executor, UINT32_MAX, &executed)!=OBJC3_SERVICE_OK ||
      published_task.result()!=42) return 101;
  published_task={};
  int frame_borrow=40;
  auto frame_await=native_published_frame_await(frame_borrow).start(executor);
  frame_borrow=42;
  if (objc3_executor_pump(executor,UINT32_MAX,&executed)!=OBJC3_SERVICE_OK ||
      frame_await.result()!=42) return 102;
  frame_await={};
  objc3_executor_t *runtime_destination=nullptr;
  if (objc3_executor_create(1,&runtime_destination)!=OBJC3_SERVICE_OK) return 105;
  {
    NativePublishedRuntime publisher{2};
    auto member=publisher.run(40,runtime_destination).start(executor);
    auto runtime=native_published_runtime(40,runtime_destination).start(executor);
    auto group=native_published_group(40).start(executor);
    auto leaf=native_module_value(41).start(executor);
    auto joined=native_published_raw_task(leaf.native_handle()).start(executor);
    if (objc3_executor_pump(executor,UINT32_MAX,&executed)!=OBJC3_SERVICE_OK ||
        objc3_executor_pump(runtime_destination,UINT32_MAX,&executed)!=OBJC3_SERVICE_OK ||
        runtime.result()!=42 || group.result()!=42 || joined.result()!=42 ||
        member.result()!=42) return 103;
  }
  objc3_executor_release(runtime_destination);
  auto scoped = native_module_scoped(41).start(executor);
  if (objc3_executor_pump(executor, UINT32_MAX, &executed) != OBJC3_SERVICE_OK ||
      scoped.result() != 42)
    return 39;
  scoped = {};
  auto object = native_module_object(40).start(executor);
  auto block = native_module_block(40).start(executor);
  if (objc3_executor_pump(executor, 100, &executed) != OBJC3_SERVICE_OK)
    return 6;
  @autoreleasepool {
    NativeModuleResult *value = object.result();
    native_module_reader reader = block.result();
    object = {};
    block = {};
    if ([value number] != 40 || reader(2) != 42)
      return 7;
  }
  objc3_actor_t *actor = nullptr;
  if (objc3_actor_create(executor, &actor) != OBJC3_SERVICE_OK)
    return 3;
  {
    auto runtime=native_published_actor(42,actor).start(executor);
    if (objc3_executor_pump(executor,UINT32_MAX,&executed)!=OBJC3_SERVICE_OK ||
        runtime.result()!=42) return 104;
  }
  pending = actor_application(actor).start(executor);
  if (objc3_executor_pump(executor, 100, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42)
    return 4;
  pending = {};
  pending = checked_application(actor).start(executor);
  if (objc3_executor_pump(executor, 100, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42)
    return 8;
  pending = {};
  NativeModuleWorker worker{40};
  auto *base = static_cast<NativeModuleWorker *>(&worker);
  {
    int extra = 1;
    auto member = &NativeModuleWorker::frame_borrow;
    auto frame = (worker.*member)(extra);
    static_assert(std::is_same_v<decltype(frame), objc3_async_frame>);
    auto borrowed = __objc3_async_from_frame<int>(frame).start(executor);
    int answer = 1;
    auto referenced = __objc3_async_from_frame<int>(native_module_reference(answer)).start(executor);
    extra = 2; answer = 42;
    if (objc3_executor_pump(executor, UINT32_MAX, &executed) != OBJC3_SERVICE_OK ||
        borrowed.result() != 42 || referenced.result() != 42) return 40;
  }
  pending = member_application(worker, *base, actor).start(executor);
  if (objc3_executor_pump(executor, 100, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42) return 10;
  pending = {};
  pending = checked_values_application(actor).start(executor);
  if (objc3_executor_pump(executor, 200, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42) return 11;
  pending = {};
  pending = objc_method_application(actor).start(executor);
  if (objc3_executor_pump(executor, 300, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42) return 12;
  pending = {};
  pending = block_application(actor).start(executor);
  if (objc3_executor_pump(executor, 300, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42) return 13;
  pending = {};
  pending = frame_application(actor).start(executor);
  if (objc3_executor_pump(executor, 300, &executed) != OBJC3_SERVICE_OK ||
      pending.result() != 42) return 14;
  pending = {};
  {
    NativeClassStats stats{};
    {
      NativeClassValue value(&stats, 42);
      NativeClassValue copy(value);
      NativeClassValue moved(std::move(copy));
      copy = value;
      moved = std::move(copy);
      if (!static_cast<bool>(moved) || moved.number() != 42) return 41;
      auto member = __objc3_async_from_frame<int>(value.asyncNumber()).start(executor);
      auto task = __objc3_async_from_frame<NativeClassValue>(
          native_module_class(std::move(moved))).start(executor);
      auto unique = __objc3_async_from_frame<int>(
          native_module_move_only(NativeMoveOnly(42))).start(executor);
      if (objc3_executor_pump(executor, UINT32_MAX, &executed) != OBJC3_SERVICE_OK ||
          member.result() != 42 || task.result().number() != 42 ||
          unique.result() != 42) return 42;
      NativeClassValue *heap = new NativeClassValue(value);
      if (heap->number() != 42) return 43;
      delete heap;
    }
    if (stats.live || stats.copied < 2 || stats.moved < 2 || !stats.destroyed) return 44;
  }
  {
    NativeClassStats stats{};
    {
      NativeDerivedValue value(&stats, 20);
      NativeDerivedValue copy(value);
      NativeDerivedValue moved(std::move(copy));
      copy = value; moved = std::move(copy);
      NativeClassValue *base = &value;
      if ((void *)base == (void *)&value || base->number() != 20 || value.sum() != 42) return 45;
      int extra = 0;
      auto frame = __objc3_async_from_frame<int>(value.asyncTotal(extra)).start(executor);
      auto member = &NativeClassValue::asyncNumber;
      auto inherited = __objc3_async_from_frame<int>((value.*member)()).start(executor);
      auto result = __objc3_async_from_frame<NativeDerivedValue>(native_module_derived(std::move(moved))).start(executor);
      NativeAggregate aggregate{{}, {30}, 12};
      auto plain = __objc3_async_from_frame<int>(native_module_aggregate(aggregate)).start(executor);
      if (objc3_executor_pump(executor, UINT32_MAX, &executed) != OBJC3_SERVICE_OK ||
          frame.result() != 42 || inherited.result() != 20 || result.result().sum() != 42 || plain.result() != 42) return 46;
      NativeDerivedValue *heap = new NativeDerivedValue(value);
      if (heap->sum() != 42) return 47;
      delete heap;
      NativeDefaultedValue pod{}; pod.first = 30; pod.value = 12;
      NativeDefaultedValue copied(pod); NativeDefaultedValue movedPod(std::move(copied));
      pod = movedPod; copied = std::move(pod);
      if (copied.first + copied.value != 42 || NativeAccessValue(42).total() != 42) return 48;
    }
    if (stats.live || !stats.copied || !stats.moved || !stats.destroyed) return 49;
  }
  {
    NativeDispatchStats stats{};
    {
      NativeVirtualDiamond value(&stats, 20);
      NativeVirtualRoot *root = &value;
      NativeVirtualRight *right = &value;
      NativeVirtualLeft *left = dynamic_cast<NativeVirtualLeft *>(right);
      if (root->score() != 42 || right->right() != 20 || !left ||
          static_cast<NativeVirtualRoot *>(left) != root ||
          root->self() != root || dynamic_cast<void *>(root) != &value ||
          !native_module_virtual_is(root)) return 50;
      int extra = 0;
      auto frame = __objc3_async_from_frame<int>(root->frame(extra)).start(executor);
      auto member = &NativeVirtualRoot::score;
      if ((value.*member)() != 42) return 51;
      NativeVirtualDiamond copied(value);
      auto result = __objc3_async_from_frame<NativeVirtualDiamond>(
          native_module_virtual(std::move(copied))).start(executor);
      if (objc3_executor_pump(executor, UINT32_MAX, &executed) != OBJC3_SERVICE_OK ||
          frame.result() != 42 || result.result().score() != 42) return 52;
      int freed=native_module_virtual_deallocations();
      NativeVirtualRoot *heap = new NativeVirtualDiamond(value);
      if (heap->score() != 42) return 53;
      delete heap;
      if (native_module_virtual_deallocations()!=freed+1) return 95;
    }
    if (stats.live || !stats.destroyed) return 54;
  }
  {
    NativePublished::Value value{40};
    if (NativePublished::add(value) != 42 || NativeAlias::add(value) != 42 ||
        NativePublished::bias != 2) return 55;
    auto frame = __objc3_async_from_frame<int>(
        NativePublished::frame(value)).start(executor);
    if (objc3_executor_pump(executor, UINT32_MAX, &executed) != OBJC3_SERVICE_OK ||
        frame.result() != 42) return 56;
  }
  {
    NativeClassStats stats{};
    {
      NativeFamily family(&stats,40);
      NativeFamily::Result value(family.stored);
      NativeFamily::PublicHidden hidden{42};
      if(value.sum()!=42 || hidden.token!=42) return 57;
      auto member=__objc3_async_from_frame<int>(family.stored.frame()).start(executor);
      auto result=__objc3_async_from_frame<NativeFamily::Result>(
          native_module_nested(std::move(value))).start(executor);
      if(objc3_executor_pump(executor,UINT32_MAX,&executed)!=OBJC3_SERVICE_OK ||
         member.result()!=42 || result.result().sum()!=42) return 58;
    }
    if(stats.live || !stats.destroyed) return 59;
  }
  {
    if(static_cast<unsigned>(Answer)!=42) return 64;
    NativeUsingItem value{40};
    NativeUsing::NativeUsingItem alias{40};
    if(using_calculate(value)!=42 || using_calculate(40)!=42 ||
       NativeUsing::using_calculate(alias)!=42) return 60;
    NativeClassStats stats{};
    {
      NativeInherited inherited(&stats,42);
      if(inherited.number()!=42) return 61;
      auto frame=__objc3_async_from_frame<int>(inherited.asyncNumber()).start(executor);
      if(objc3_executor_pump(executor,UINT32_MAX,&executed)!=OBJC3_SERVICE_OK || frame.result()!=42) return 62;
    }
    if(stats.live || !stats.destroyed) return 63;
  }
  if (objc3_actor_close(actor) != OBJC3_SERVICE_OK ||
      objc3_actor_join(actor) != OBJC3_SERVICE_OK)
    return 5;
  objc3_actor_release(actor);
  objc3_executor_release(executor);
}
