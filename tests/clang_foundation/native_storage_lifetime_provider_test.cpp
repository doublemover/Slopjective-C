#include "standard_library_test_support.h"
#include "llvm/ADT/ScopeExit.h"

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
void Run(const fs::path &root,const char *standard,int ownership,
         const char *optimization,bool suppress_implicit_destruction) {
  fs::create_directories(root/"provider");
  std::vector<std::string> flags={"-pthread"};
  if(suppress_implicit_destruction)flags.push_back("-fno-c++-static-destructors");
  auto request=[&](const fs::path &input,const fs::path &output) {
    auto value=Request(input,output,standard,ownership,optimization,flags);
    if(ownership)value.arguments.push_back("-fobjc3-profile=strict-system");
    return value;
  };
  const auto source=root/"provider/module.mm";
  Write(source,R"storage(
namespace StorageLifetime {
enum {FirstPriority=201,LastPriority=203};
enum {OtherPriority=201,OtherLast=203};
using PriorityType=decltype(FirstPriority);using OtherPriorityType=decltype(OtherPriority);
inline PriorityType priority_identity(PriorityType value){return value;}
inline int priority_overload(PriorityType){return 1;}
inline int priority_overload(OtherPriorityType){return 2;}
template<class T>struct PriorityBox {T value;};
inline PriorityBox<PriorityType> priority_box(){return {FirstPriority};}
#ifdef __OBJC__
typedef borrowed const char *StaticLoan;
#define STATIC_BORROWED __attribute__((objc_returns_borrowed(owner_index=0)))
#else
typedef const char *StaticLoan;
#define STATIC_BORROWED
#endif
union StaticUnion {char bytes[8];long other;};
extern const StaticUnion static_union;
const StaticUnion static_union={{'s','t','a','t','i','c',0}};
inline int cleanup_order=0;
struct CleanupValue {
 int value;
 explicit CleanupValue(int value):value(value){if(value<0)throw 73;}
 ~CleanupValue()noexcept{cleanup_order=cleanup_order*10+2;}
 explicit operator int()const noexcept{return value;}
};
inline void cleanup_record(CleanupValue *)noexcept{cleanup_order=cleanup_order*10+1;}
inline void cleanup_throw(CleanupValue *){cleanup_order=cleanup_order*10+3;throw 91;}
inline void cleanup_int(int *value)noexcept{cleanup_order+=*value;}
inline int cleaned_record(int value,bool fail){CleanupValue object __attribute__((cleanup(cleanup_record)))(value);if(fail)throw 73;return object.value;}
inline void cleaned_throw(){CleanupValue object __attribute__((cleanup(cleanup_throw)))(42);}
template<class T>inline T cleaned_template(T initial){T value __attribute__((cleanup(cleanup_int)))=initial;return value;}
#ifdef __OBJC__
template<class T>struct GenericCleanup {
 static void close(T *value)noexcept{cleanup_order=cleanup_order*10+1;}
};
template<class T>inline int cleaned_dependent(int initial,bool fail){
 T value __attribute__((objc_cleanup(GenericCleanup<T>::close)))(initial);
 if(fail)throw 73;return static_cast<int>(value);
}
template<class T>inline void cleanup_function(T *)noexcept{cleanup_order=cleanup_order*10+1;}
template<class T>inline int cleaned_function(int initial){
 T value __attribute__((objc_cleanup(cleanup_function<T>)))(initial);
 return static_cast<int>(value);
}
inline int cleaned_auto(){auto value __attribute__((objc_cleanup(cleanup_int)))=45;return value;}
template<class T>inline int cleaned_auto_dependent(T initial){
 auto value __attribute__((objc_cleanup(GenericCleanup<T>::close)))=initial;
 return static_cast<int>(value);
}
inline void cleanup_value(int value)noexcept{cleanup_order+=value;}
inline void cleanup_reference(int &value)noexcept{cleanup_order+=value;value=0;}
inline int cleaned_value(){int value __attribute__((objc_cleanup(cleanup_value)))=43;return value;}
inline int cleaned_reference(){int value __attribute__((objc_cleanup(cleanup_reference)))=44;return value;}
inline int array_closed=0,array_destroyed=0,array_live=0;
struct ArrayCleanupValue {
 int value;
 explicit ArrayCleanupValue(int initial):value(initial){if(initial<0)throw 71;++array_live;}
 ~ArrayCleanupValue()noexcept{--array_live;array_destroyed=array_destroyed*10+value;}
};
inline void close_array_elements(int *value)noexcept{array_closed=value[0]+value[1];}
inline void close_array_rows(int (*value)[3])noexcept{array_closed=value[0][0]+value[1][2];}
inline void close_array_address(int (*value)[2])noexcept{array_closed=(*value)[0]+(*value)[1];}
inline void close_array_reference(int (&value)[2])noexcept{array_closed=value[0]+value[1];}
inline void close_array_objects(ArrayCleanupValue *value){array_closed=value[0].value+value[1].value;}
inline void close_array_throw(ArrayCleanupValue *value){close_array_objects(value);throw 81;}
inline void cleaned_array(){@cleanup(close_array_elements) int values[2]={1,2};(void)values;}
inline void cleaned_array_rows(){@cleanup(close_array_rows) int values[2][3]={{1,2,3},{4,5,6}};(void)values;}
inline void cleaned_array_address(){@cleanup(close_array_address) int values[2]={3,4};(void)values;}
inline void cleaned_array_reference(){@cleanup(close_array_reference) int values[2]={5,6};(void)values;}
template<class T>inline void close_array_generic(T *values)noexcept{array_closed=static_cast<int>(values[0]+values[1]);}
template<class T,int N>inline void cleaned_array_generic(T initial){@cleanup(close_array_generic<T>) T values[N]={initial,T(2)};(void)values;}
inline void cleaned_array_objects(bool unwind,bool fail){@cleanup(close_array_objects) ArrayCleanupValue values[2]={ArrayCleanupValue(1),ArrayCleanupValue(fail?-1:2)};(void)values;if(unwind)throw 79;}
inline void cleaned_array_throw(){@cleanup(close_array_throw) ArrayCleanupValue values[2]={ArrayCleanupValue(1),ArrayCleanupValue(2)};(void)values;}

inline int overload_closed=0;
inline void overload_close(int value)noexcept{overload_closed=value;}
inline void overload_close(long value)noexcept{overload_closed=static_cast<int>(value)+1;}
inline void overload_close(int *value)noexcept{overload_closed=*value+2;}
template<class T>inline void deduced_close(T *value)noexcept{overload_closed=static_cast<int>(*value);}
template<class T>inline void deduced_resource(T value)noexcept{overload_closed=static_cast<int>(value);}
template<class T,int N>inline void deduced_array(T (&value)[N])noexcept{overload_closed=static_cast<int>(value[0])+N;}
template<class T>requires(__is_integral(T)) inline void constrained_close(T *value)noexcept{overload_closed=static_cast<int>(*value);}
inline void constrained_close(void *)noexcept{overload_closed=-2;}
struct OverloadedStatic {static void close(int value)noexcept{overload_closed=value;}
 static void close(long value)noexcept{overload_closed=static_cast<int>(value)+1;}};
inline void cleaned_overload(){@cleanup(overload_close) int value=51;(void)value;}
inline void cleaned_overload_address(){@cleanup(deduced_close) long value=52;(void)value;}
inline void cleaned_overload_array(){@cleanup(deduced_array) int values[3]={53};(void)values;}
inline void cleaned_overload_constraint(){@cleanup(constrained_close) int value=57;(void)value;}
inline void cleaned_overload_static(){@cleanup(OverloadedStatic::close) long value=58;(void)value;}
template<class T>inline void cleaned_overload_dependent(T initial){@cleanup(deduced_close) auto value=initial;(void)value;}
inline void resource_overload(){@resource(overload_close,invalid:-1) int value=59;(void)value;}
template<class T>inline void resource_overload_dependent(T initial){@resource(deduced_resource,invalid:-1) auto value=initial;(void)value;}
@requires_cleanup(overload_close) typedef int OverloadedHandle;
inline void resource_overload_contract(){@resource(overload_close,invalid:-1) OverloadedHandle value=61;(void)value;}

inline int resource_closed=0;
template<class T>void resource_close(T value)noexcept{resource_closed+=static_cast<int>(value);}
template<class T>void resource_address(T *value)noexcept{resource_closed+=static_cast<int>(*value);*value=-1;}
template<class T>struct ResourceTraits {static void close(T value)noexcept{resource_close(value);}static constexpr T invalid=static_cast<T>(-1);};
inline void resource_auto(int initial){@resource(resource_close<int>, invalid: -1) auto value=initial;(void)value;}
template<class T>inline void resource_dependent(T initial){@resource(ResourceTraits<T>::close, invalid: ResourceTraits<T>::invalid) T value=initial;(void)value;}
template<class T>inline void resource_auto_dependent(T initial){@resource(resource_address<T>, invalid: -1) auto value=initial;(void)value;}
inline void resource_operations(){@resource(resource_close<int>, invalid: -1) auto value=3;__builtin_objc3_resource_reset(value,5);__builtin_objc3_resource_discard(value);}
inline int resource_take(){@resource(resource_close<int>, invalid: -1) auto value=11;return __builtin_objc3_resource_take(value);}
inline void resource_throw(int value){resource_closed+=value;throw 83;}
inline void resource_throw_discard(){@resource(resource_throw, invalid: -1) auto value=7;__builtin_objc3_resource_discard(value);}
inline int resource_acquire(){throw 79;}
inline void resource_init_failure(){@resource(resource_close<int>, invalid: -1) auto value=resource_acquire();(void)value;}
inline void resource_unwind(){@resource(resource_close<int>, invalid: -1) auto value=13;(void)value;throw 89;}

#endif
inline const char *static_source(){return static_union.bytes;}
inline const char *static_offset(int offset){return static_source()+offset;}
inline const char *static_branch_source(int choose){if(choose)return static_source();return "alternate";}
inline const char *static_switch_source(int choose){switch(choose){case 0:return static_source();case 1:return "alternate";default:return nullptr;}}
inline const char *static_loop_source(int count){while(count--){if(count==1)return "loop";}return static_source();}
inline const char *static_exception_source(int choose){try{if(choose)throw 37;return static_source();}catch(int){return "caught";}}
inline const char *static_retarget(const char *value,int choose){if(choose)value="second";value+=1;return value;}
inline const char *static_retarget_local(int choose){const char *value=static_union.bytes;if(choose)value="second";return ++value;}
inline const char *static_readonly(const char *value){const char *alias=value;const char *other=alias;return other;}
inline const char *static_readonly_callback(const char *(*callback)(const char *),const char *value){const char *(*alias)(const char *)=callback;return alias(value);}
STATIC_BORROWED
inline StaticLoan static_readonly_view(void *owner){return static_readonly_callback(static_readonly,static_union.bytes);}
inline const char *static_forward(const char *const value){return value;}
inline const char *static_forward_local(const char *const value){const char *const alias=value;return alias;}
inline const char *static_forward_chain(const char *const value){return static_forward_local(static_forward(value));}
inline const char *static_forward_default(const char *const value="default"){return static_forward_chain(value);}
inline const char *static_forward_choice(int choose,const char *const first,const char *const second){if(choose)return first;return second;}
using StaticCallback=const char *(*)(const char *);
inline const char *static_retarget_callback(StaticCallback callback,const char *value,int choose){if(choose)callback=static_readonly;return callback(value);}
inline StaticCallback static_retarget_factory(int choose){StaticCallback callback=static_readonly;if(choose)callback=static_forward_chain;return callback;}
STATIC_BORROWED
inline StaticLoan static_retarget_view(void *owner,int choose){return static_retarget_callback(static_retarget_factory(choose),static_retarget_local(choose),choose);}

inline const char *static_reference(const char *const &value){return value;}
inline const char *static_reference_chain(const char *const &value){const char *const &one=value;const char *const &two=one;return static_reference(two);}
inline const char *static_reference_callback(StaticCallback const &callback,const char *value){return callback(value);}
inline const char *static_function_reference(const char *(&callback)(const char *),const char *value){return callback(value);}
STATIC_BORROWED
inline StaticLoan static_reference_view(void *owner,int choose){
  const char *value=static_union.bytes;const char *const &one=value;const char *const &two=one;
  if(choose)value="reference";return static_reference_chain(two);
}
STATIC_BORROWED
inline StaticLoan static_reference_callback_view(void *owner,int choose){
  StaticCallback const &callback=choose?static_readonly:static_forward;
  return static_reference_callback(callback,static_union.bytes);
}
STATIC_BORROWED
inline StaticLoan static_function_reference_view(void *owner){
  auto &callback=static_forward;return static_function_reference(callback,static_union.bytes);
}
struct StaticMethodProducer {
 const char *read()const{return static_union.bytes;}
 const char *forward(const char *const &value)const{return static_reference_chain(value);}
 StaticCallback callback(int choose)const{return choose?static_readonly:static_forward;}
 const char *operator()(const char *const &value)const{return forward(value);}
};
struct StaticFinalMethodProducer final {virtual const char *read(){return static_union.bytes;}};
struct StaticDispatchBase {virtual const char *read(){return "base";}};
struct StaticDispatchDerived:StaticDispatchBase {const char *read()override{return "derived";}};
inline const char *static_dynamic_dispatch(StaticDispatchBase &value){return value.read();}
inline const char *static_qualified_dispatch(StaticDispatchBase &value){return value.StaticDispatchBase::read();}
STATIC_BORROWED
inline StaticLoan static_qualified_method_view(void *owner,StaticDispatchBase &value){return value.StaticDispatchBase::read();}
STATIC_BORROWED
inline StaticLoan static_method_view(void *owner,int choose){StaticMethodProducer producer;return choose?producer.read():producer("method");}
STATIC_BORROWED
inline StaticLoan static_method_callback_view(void *owner,int choose){return StaticMethodProducer{}.callback(choose)(static_union.bytes);}
STATIC_BORROWED
inline StaticLoan static_final_method_view(void *owner,StaticFinalMethodProducer &producer){return producer.read();}
STATIC_BORROWED
inline StaticLoan static_lambda_view(void *owner){return [](auto value){return value;}("lambda");}
struct StaticMemberTargets {
  const char *nothrow_read(const char *value)const noexcept{return value;}
  int shift=0;
  const char *read(const char *value)const{return value+shift;}
  const char *next(const char *value)const{return value+shift+1;}
  template<class T>const char *generic(T value)const{return value+shift;}
};
using StaticMemberPointer=const char *(StaticMemberTargets::*)(const char *)const;
inline constexpr StaticMemberPointer static_member_pointer=&StaticMemberTargets::read;
        struct StaticMemberCell {StaticMemberPointer callback;};
        inline constexpr StaticMemberCell static_member_cell{&StaticMemberTargets::read};

inline StaticMemberPointer static_member_factory(int choose){return choose?&StaticMemberTargets::read:&StaticMemberTargets::next;}
inline StaticMemberPointer const &static_member_reference(){return static_member_pointer;}
inline const char *static_member_apply(StaticMemberTargets &object,StaticMemberPointer callback,const char *value){return (object.*callback)(value);}
STATIC_BORROWED
inline StaticLoan static_member_view(void *owner,int choose) {
  StaticMemberTargets object;StaticMemberPointer callback=&StaticMemberTargets::read;
  StaticMemberPointer const &one=callback;StaticMemberPointer const &two=one;
  if(choose)callback=&StaticMemberTargets::next;return (object.*two)(static_union.bytes);
}
STATIC_BORROWED
inline StaticLoan static_member_factory_view(void *owner,int choose){StaticMemberTargets object;return static_member_apply(object,static_member_factory(choose),static_union.bytes);}
STATIC_BORROWED
inline StaticLoan static_member_reference_view(void *owner){StaticMemberTargets object;return (object.*static_member_reference())(static_union.bytes);}
STATIC_BORROWED
inline StaticLoan static_member_generic_view(void *owner){StaticMemberTargets object;return (object.*&StaticMemberTargets::generic<const char *>)(static_union.bytes);}
        STATIC_BORROWED
        inline StaticLoan static_member_nothrow_view(void *owner){StaticMemberTargets object;StaticMemberPointer target=&StaticMemberTargets::nothrow_read;return (object.*target)(static_union.bytes);}
        STATIC_BORROWED
        inline StaticLoan static_member_constant_field_view(void *owner){StaticMemberTargets object;return (object.*static_member_cell.callback)(static_union.bytes);}

        struct StaticMemberPrefix {long prefix[4]{};};
        struct StaticMemberDerived:StaticMemberPrefix,StaticMemberTargets {};
        STATIC_BORROWED
        inline StaticLoan static_member_derived_view(void *owner) {
          StaticMemberDerived object;object.shift=1;
          const char *(StaticMemberDerived::*callback)(const char *)const=&StaticMemberTargets::read;
          return (object.*callback)(static_union.bytes);
        }

inline const char *const static_pointer_object=static_union.bytes;
inline StaticCallback const static_callback_object=static_forward;
inline const char *const &static_permanent_pointer_reference(){return static_pointer_object;}
inline StaticCallback const &static_permanent_callback_reference(){return static_callback_object;}
inline const char *(&static_permanent_function_reference())(const char *){return static_forward;}
STATIC_BORROWED
inline StaticLoan static_permanent_pointer_reference_view(void *owner){return static_permanent_pointer_reference();}
STATIC_BORROWED
inline StaticLoan static_permanent_callback_reference_view(void *owner){return static_permanent_callback_reference()(static_union.bytes);}
STATIC_BORROWED
inline StaticLoan static_permanent_function_reference_view(void *owner){return static_permanent_function_reference()(static_union.bytes);}
inline constexpr StaticCallback static_callback=static_forward_chain;
inline const char *static_callback_source(StaticCallback const callback,const char *const value){StaticCallback const alias=callback;return alias(value);}
STATIC_BORROWED
inline StaticLoan static_callback_view(void *owner){return static_callback_source(static_callback,static_union.bytes);}
inline StaticCallback static_callback_factory(int choose){switch(choose){case 0:return static_forward;case 1:return static_forward_chain;default:return static_forward_local;}}
inline StaticCallback static_callback_alias(int choose){StaticCallback const callback=choose?static_forward:static_forward_chain;return callback;}
STATIC_BORROWED
inline StaticLoan static_callback_choice_view(void *owner,int choose){return static_callback_source(choose?static_forward:static_forward_chain,static_union.bytes);}
STATIC_BORROWED
inline StaticLoan static_callback_factory_view(void *owner,int choose){return (choose?static_callback_factory:static_callback_alias)(choose)(static_union.bytes);}

STATIC_BORROWED
inline StaticLoan static_forward_view(void *owner,int choose){return static_forward_choice(choose,static_forward_chain(static_union.bytes),static_forward_default());}
STATIC_BORROWED
inline StaticLoan static_graph_view(void *owner,int choose){const char *const value=static_branch_source(choose);return value;}
STATIC_BORROWED
inline StaticLoan static_graph_switch(void *owner,int choose){return static_switch_source(choose);}
STATIC_BORROWED
inline StaticLoan static_graph_loop(void *owner,int count){return static_loop_source(count);}
STATIC_BORROWED
inline StaticLoan static_graph_exception(void *owner,int choose){return static_exception_source(choose);}
STATIC_BORROWED
inline StaticLoan static_view(void *owner,int offset){return static_offset(offset);}
inline int static_read(void *owner){return static_view(owner,2)[0];}
union PointerCell {const char *first;const char *second;};
#ifdef __OBJC__
__attribute__((objc_returns_borrowed(owner_index=1)))
#endif
inline StaticLoan rebound_union(void *previous,void *current) {
  PointerCell value={static_view(previous,0)};
  value.second=static_view(current,1);return value.second;
}
inline int overwritten_union(void *owner) {
  PointerCell value={static_view(owner,0)};
  value.second="permanent";
  const char *pointer=value.second;return pointer[0];
}
STATIC_BORROWED
inline StaticLoan byte_union(void *owner) {
  PointerCell value={nullptr};char *bytes=(char *)&value;
  *(const char **)bytes=static_view(owner,2);return value.first;
}

extern int created[10],destroyed[10];
void create(int);void destroy(int);void startup(int);void shutdown(int);
struct Owner {
  int value;
  explicit Owner(int value):value(value){create(value);}
  ~Owner() noexcept {destroy(value);}
};
inline constinit int ready=42;
__attribute__((require_constant_initialization)) inline int attribute_ready=43;
inline __attribute__((init_priority(202),always_destroy)) Owner ordered(2);
inline __attribute__((no_destroy)) Owner retained(4);
inline __attribute__((always_destroy)) Owner finalized(5);
inline Owner &local_retained(){static __attribute__((no_destroy)) Owner value(6);return value;}
inline Owner &local_finalized(){static __attribute__((always_destroy)) Owner value(7);return value;}
inline Owner &thread_finalized(){static thread_local __attribute__((always_destroy)) Owner value(8);return value;}
Owner &provider_local(){return local_retained();}
void first() __attribute__((constructor(FirstPriority)));
void last() __attribute__((constructor(LastPriority)));
void late_shutdown() __attribute__((destructor(LastPriority)));
void early_shutdown() __attribute__((destructor(FirstPriority)));
void verify_exit() __attribute__((destructor(101)));
int created[10],destroyed[10];
int startup_count,startup_trace[3],shutdown_count,shutdown_trace[2];
void create(int value){++created[value];if(value==2)startup(2);}
void destroy(int value){++destroyed[value];}
void startup(int value){if(startup_count>=3)__builtin_trap();startup_trace[startup_count++]=value;}
void shutdown(int value){if(shutdown_count>=2)__builtin_trap();shutdown_trace[shutdown_count++]=value;}
void first(){startup(1);}void last(){startup(3);}
void late_shutdown(){shutdown(3);}void early_shutdown(){shutdown(1);}
void verify_exit(){
  if(startup_count!=3 || startup_trace[0]!=1 || startup_trace[1]!=2 || startup_trace[2]!=3 ||
     shutdown_count!=2 || shutdown_trace[0]!=3 || shutdown_trace[1]!=1)__builtin_trap();
  for(int value=2;value!=9;++value) {
    if(value==3)continue;
    if(created[value]!=(value==8?2:1))__builtin_trap();
    if(destroyed[value]!=(value==4 || value==6?0:value==8?2:1))__builtin_trap();
  }
}
}
)storage");
  auto provider=request(source,root/"provider.o");
  provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
  provider.module_name="fixture.StorageLifetime";provider.package_identity="storage-lifetime-v1";
  provider.interface_exports={"StorageLifetime"};provider.emit_semantic_interface=true;
  auto built=Compile(provider);Success(built);
  assert(built.interface_artifact && built.interface_artifact->success);
  const auto artifact=*built.interface_artifact;
  const auto interface=root/"StorageLifetime.objc3i";
  Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  assert(fs::remove(source));
  for(const char *name:{"a","b"}) {
    const auto consumer_source=root/(std::string("consumer-")+name+".mm");
    Write(consumer_source,std::string(R"storage(
#include <pthread.h>
static void *worker(void *){return StorageLifetime::thread_finalized().value==8?nullptr:(void *)1;}
)storage")+"extern \"C\" int check_"+name+R"storage(() {
  using namespace StorageLifetime;
  static_assert(!__is_same(PriorityType,OtherPriorityType));
  if(priority_identity(FirstPriority)!=FirstPriority || priority_overload(FirstPriority)!=1 || priority_overload(OtherPriority)!=2 || priority_box().value!=FirstPriority)return 4;
  if(static_read(&ready)!='a' || static_view(&ready,1)[0]!='t')return 5;
  if(rebound_union(&attribute_ready,&ready)[0]!='t' || overwritten_union(&ready)!='p' || byte_union(&ready)[0]!='a')return 6;
  if(static_graph_view(&ready,1)[0]!='s' || static_graph_view(&ready,0)[0]!='a' || static_graph_switch(&ready,0)[0]!='s' || static_graph_switch(&ready,1)[0]!='a' || static_graph_switch(&ready,2)!=nullptr)return 7;
  if(static_graph_loop(&ready,3)[0]!='l' || static_graph_loop(&ready,0)[0]!='s' || static_graph_exception(&ready,0)[0]!='s' || static_graph_exception(&ready,1)[0]!='c')return 8;
  if(static_forward_view(&ready,1)[0]!='s' || static_forward_view(&ready,0)[0]!='d')return 9;
  if(static_callback_view(&ready)[0]!='s')return 15;
  for(int choose=0;choose<3;++choose)if(static_callback_choice_view(&ready,choose)[0]!='s' || static_callback_factory_view(&ready,choose)[0]!='s')return 22;

  cleanup_order=0;if(cleaned_record(42,false)!=42 || cleanup_order!=12)return 10;
  cleanup_order=0;bool caught=false;try{(void)cleaned_record(42,true);}catch(int value){caught=value==73;}
  if(!caught || cleanup_order!=12)return 11;
  cleanup_order=0;caught=false;try{(void)cleaned_record(-1,false);}catch(int value){caught=value==73;}
  if(!caught || cleanup_order)return 12;
  cleanup_order=0;caught=false;try{cleaned_throw();}catch(int value){caught=value==91;}
  if(!caught || cleanup_order!=32)return 13;
  cleanup_order=0;if(cleaned_template(42)!=42 || cleanup_order!=42)return 14;
#ifdef __OBJC__
  cleanup_order=0;if(cleaned_value()!=43 || cleanup_order!=43)return 16;
  cleanup_order=0;if(cleaned_reference()!=44 || cleanup_order!=44)return 17;
  cleanup_order=0;if(cleaned_dependent<int>(42,false)!=42 || cleanup_order!=1)return 18;
  cleanup_order=0;if(cleaned_dependent<CleanupValue>(42,false)!=42 || cleanup_order!=12)return 19;
  cleanup_order=0;caught=false;try{(void)cleaned_dependent<CleanupValue>(42,true);}catch(int value){caught=value==73;}
  if(!caught || cleanup_order!=12)return 20;
  cleanup_order=0;caught=false;try{(void)cleaned_dependent<CleanupValue>(-1,false);}catch(int value){caught=value==73;}
  if(!caught || cleanup_order)return 21;
  cleanup_order=0;if(cleaned_auto()!=45 || cleanup_order!=45)return 22;
  cleanup_order=0;if(cleaned_auto_dependent(46)!=46 || cleanup_order!=1)return 23;
  cleanup_order=0;if(cleaned_function<int>(47)!=47 || cleanup_order!=1)return 24;
  cleanup_order=0;if(cleaned_function<CleanupValue>(48)!=48 || cleanup_order!=12)return 25;
  if(static_readonly_view(nullptr)[0]!='s')return 51;
  if(static_reference_view(nullptr,0)[0]!='s' || static_reference_view(nullptr,1)[0]!='r')return 53;
  if(static_reference_callback_view(nullptr,0)[0]!='s' || static_reference_callback_view(nullptr,1)[0]!='s')return 54;
  if(static_function_reference_view(nullptr)[0]!='s')return 55;
  if(static_method_view(nullptr,0)[0]!='m' || static_method_view(nullptr,1)[0]!='s')return 56;
  if(static_method_callback_view(nullptr,0)[0]!='s' || static_method_callback_view(nullptr,1)[0]!='s')return 57;
  StaticFinalMethodProducer final_method;
  if(static_final_method_view(nullptr,final_method)[0]!='s' || static_lambda_view(nullptr)[0]!='l')return 58;
  StaticDispatchDerived derived;
  if(static_dynamic_dispatch(derived)[0]!='d' || static_qualified_dispatch(derived)[0]!='b' || static_qualified_method_view(nullptr,derived)[0]!='b')return 59;
  if(static_member_view(nullptr,0)[0]!='s' || static_member_view(nullptr,1)[0]!='t')return 61;
  if(static_member_factory_view(nullptr,0)[0]!='t' || static_member_factory_view(nullptr,1)[0]!='s')return 62;
  if(static_member_nothrow_view(nullptr)[0]!='s' || static_member_constant_field_view(nullptr)[0]!='s')return 64;
  if(static_member_reference_view(nullptr)[0]!='s' || static_member_generic_view(nullptr)[0]!='s' || static_member_derived_view(nullptr)[0]!='t')return 63;
  if(static_permanent_pointer_reference_view(nullptr)[0]!='s' || static_permanent_callback_reference_view(nullptr)[0]!='s' || static_permanent_function_reference_view(nullptr)[0]!='s')return 60;
  if(static_retarget_view(nullptr,0)[0]!='t' || static_retarget_view(nullptr,1)[0]!='e' || static_retarget(static_union.bytes,1)[0]!='e')return 52;
  overload_closed=0;cleaned_overload();if(overload_closed!=51)return 42;
  overload_closed=0;cleaned_overload_address();if(overload_closed!=52)return 43;
  overload_closed=0;cleaned_overload_array();if(overload_closed!=56)return 44;
  overload_closed=0;cleaned_overload_constraint();if(overload_closed!=57)return 45;
  overload_closed=0;cleaned_overload_static();if(overload_closed!=59)return 46;
  overload_closed=0;cleaned_overload_dependent(62L);if(overload_closed!=62)return 47;
  overload_closed=0;resource_overload();if(overload_closed!=59)return 48;
  overload_closed=0;resource_overload_dependent(60L);if(overload_closed!=60)return 49;
  overload_closed=0;resource_overload_contract();if(overload_closed!=61)return 50;
  array_closed=0;cleaned_array();if(array_closed!=3)return 33;
  array_closed=0;cleaned_array_rows();if(array_closed!=7)return 34;
  array_closed=0;cleaned_array_address();if(array_closed!=7)return 35;
  array_closed=0;cleaned_array_reference();if(array_closed!=11)return 36;
  array_closed=0;cleaned_array_generic<long,3>(40);if(array_closed!=42)return 37;
  array_closed=array_destroyed=array_live=0;cleaned_array_objects(false,false);if(array_closed!=3 || array_destroyed!=21 || array_live)return 38;
  array_closed=array_destroyed=array_live=0;caught=false;try{cleaned_array_objects(true,false);}catch(int value){caught=value==79;}if(!caught || array_closed!=3 || array_destroyed!=21 || array_live)return 39;
  array_closed=array_destroyed=array_live=0;caught=false;try{cleaned_array_objects(false,true);}catch(int value){caught=value==71;}if(!caught || array_closed || array_destroyed!=1 || array_live)return 40;
  array_closed=array_destroyed=array_live=0;caught=false;try{cleaned_array_throw();}catch(int value){caught=value==81;}if(!caught || array_closed!=3 || array_destroyed!=21 || array_live)return 41;
  resource_closed=0;resource_auto(2);resource_dependent(3L);resource_auto_dependent(4);if(resource_closed!=9)return 26;
  resource_closed=0;resource_auto(-1);resource_dependent(-1L);resource_auto_dependent(-1);if(resource_closed)return 27;
  resource_closed=0;resource_operations();if(resource_closed!=8)return 28;
  resource_closed=0;if(resource_take()!=11 || resource_closed)return 29;
  resource_closed=0;caught=false;try{resource_throw_discard();}catch(int value){caught=value==83;}if(!caught || resource_closed!=7)return 30;
  resource_closed=0;caught=false;try{resource_init_failure();}catch(int value){caught=value==79;}if(!caught || resource_closed)return 31;
  resource_closed=0;caught=false;try{resource_unwind();}catch(int value){caught=value==89;}if(!caught || resource_closed!=13)return 32;

#endif
  if(ready!=42 || attribute_ready!=43 || ordered.value!=2 || retained.value!=4 || finalized.value!=5)return 1;
  if(&local_retained()!=&provider_local() || local_finalized().value!=7)return 2;
  pthread_t thread;void *result=(void *)1;
  if(pthread_create(&thread,nullptr,worker,nullptr) || pthread_join(thread,&result) || result)return 3;
  return 0;
}
)storage"+"extern \"C\" void *address_"+name+"(){return &StorageLifetime::local_retained();}\n");
    auto consumer=request(consumer_source,root/(std::string(name)+".o"));
    InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported};Success(Compile(consumer));
  }
  const auto main=root/"main.cpp";
  Write(main,R"(#include <cstdio>
extern "C" int check_a();extern "C" int check_b();
extern "C" void *address_a();extern "C" void *address_b();
int main(){
 const int a=check_a();if(a){std::fprintf(stderr,"storage consumer a: %d\n",a);return 1;}
 const int b=check_b();if(b){std::fprintf(stderr,"storage consumer b: %d\n",b);return 1;}
 return address_a()!=address_b();
}
)");
  auto entry=request(main,root/"main.o");Success(Compile(entry));
  LinkAndExecute(root,entry.output_path,ownership!=0,flags);
}
}

int main() {
  llvm::SmallString<128> temporary;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-native-storage-lifetime",temporary));
  const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
  struct Profile {const char *standard;int ownership;const char *optimization;bool suppress;};
  for(const auto &profile:std::vector<Profile>{{"c++20",0,"-O0",false},{"c++23",1,"-O2",false},
       {"c++23",2,"-O2",false},{"c++23",2,"-O2",true}}) {
    std::cerr<<profile.standard<<" ownership="<<profile.ownership<<" suppress="<<profile.suppress<<'\n';
    Run(root/(std::string(profile.standard)+"-"+std::to_string(profile.ownership)+(profile.suppress?"-explicit":"")),
        profile.standard,profile.ownership,profile.optimization,profile.suppress);
  }
}
