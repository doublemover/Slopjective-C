#include "standard_library_test_support.h"
#include "clang_foundation/interface_module_resolver.h"
#include "llvm/ADT/ScopeExit.h"
#include <string_view>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
constexpr const char *External=R"blocks(
#include <objc3c/runtime/objc3_runtime_services.h>
@protocol Sendable @end
__attribute__((objc_root_class)) @interface BindingUnsafe @end

typedef void (^BindingWork)(void) __objc3_async_frame;
extern "C" objc3_task_t *binding_start(BindingWork,objc3_executor_t *) __attribute__((objc_task_detached));
extern "C" void binding_pause(void) __objc3_async_frame;
extern "C" unsigned binding_pause_count(void);
extern "C" void binding_release(void *);
extern "C" BindingWork binding_unresolved_work(void);
extern "C" void *_Block_copy(const void *);
extern "C" void _Block_release(const void *);
)blocks";
constexpr const char *Source=R"blocks(
__attribute__((objc_root_class)) @interface BindingOwner<Sendable> {
 Class isa;
@public
 unsigned *destroyed;
 unsigned value;
}
+ (id)new;
@end
namespace BindingModule {
inline BindingWork project_reference(BindingWork __strong &work){return work;}
inline BindingWork project(BindingWork work){return work;}
inline BindingWork choose_work(unsigned choose,BindingWork left,BindingWork right){if(choose)return left;return right;}
inline BindingWork nested_work(unsigned choose,BindingWork left,BindingWork right){const BindingWork work=choose_work(choose,left,right);return choose?project(work):right;}
inline BindingWork const global_work=^void(void) __objc3_async_frame {};
#if __has_feature(objc_arc)
#define FACTORY_RESULT(...) (__VA_ARGS__)
#else
#define FACTORY_RESULT(...) ((BindingWork)_Block_copy((const void *)(__VA_ARGS__)))
#endif
inline BindingWork factory_work(BindingOwner *owner,unsigned *ran,unsigned *deferred){
 return FACTORY_RESULT(^void(void) __objc3_async_frame {defer {++*deferred;}*ran+=owner->value;__objc3_await binding_pause();if(owner->value!=1 && owner->value!=2)__builtin_trap();});
}
inline BindingWork conditional_factory(unsigned choose,BindingOwner *left,BindingOwner *right,unsigned *ran,unsigned *deferred){
 if(choose)return FACTORY_RESULT(^void(void) __objc3_async_frame {defer {++*deferred;}*ran+=left->value;__objc3_await binding_pause();});
 switch(choose){case 1:break;default:return FACTORY_RESULT(^void(void) __objc3_async_frame {defer {++*deferred;}*ran+=right->value;__objc3_await binding_pause();});}
 return FACTORY_RESULT(^void(void) __objc3_async_frame {defer {++*deferred;}*ran+=left->value;__objc3_await binding_pause();});
}
struct Factory{static BindingWork make(BindingOwner *owner,unsigned *ran,unsigned *deferred){
 return FACTORY_RESULT(^void(void) __objc3_async_frame {defer {++*deferred;}*ran+=owner->value;__objc3_await binding_pause();});
}};
inline BindingWork callback_factory(BindingWork callback){return FACTORY_RESULT(^void(void) __objc3_async_frame {__objc3_await callback();});}
inline BindingWork scalar_factory(int value){return FACTORY_RESULT(^void(void) __objc3_async_frame {(void)value;});}
inline BindingWork alias_factory(BindingOwner *owner,unsigned *ran,unsigned *deferred){
 BindingOwner *const snapshot=owner;unsigned *const count=ran;unsigned *const cleanup=deferred;
 return FACTORY_RESULT(^void(void) __objc3_async_frame {defer {++*cleanup;}*count+=snapshot->value;__objc3_await binding_pause();});
}
template<class T>inline BindingWork template_factory(T owner,unsigned *ran,unsigned *deferred){
 const T snapshot=owner;return FACTORY_RESULT(^void(void) __objc3_async_frame {defer {++*deferred;}*ran+=snapshot->value;__objc3_await binding_pause();});
}
template<class T>struct TemplateFactory{static BindingWork make(T owner,unsigned *ran,unsigned *deferred){
 const T snapshot=owner;return FACTORY_RESULT(^void(void) __objc3_async_frame {defer {++*deferred;}*ran+=snapshot->value;__objc3_await binding_pause();});
}};
inline BindingWork callback_alias_factory(BindingWork callback){const BindingWork snapshot=callback;return FACTORY_RESULT(^void(void) __objc3_async_frame {__objc3_await snapshot();});}
inline BindingWork scalar_alias_factory(int value){const int first=value;const int second=first;return FACTORY_RESULT(^void(void) __objc3_async_frame {(void)first;(void)second;});}
inline BindingWork calculated_factory(BindingOwner *owner,unsigned value,unsigned *ran,unsigned *deferred){
 const unsigned tripled=value*3;const unsigned expected=tripled/3;
 return FACTORY_RESULT(^void(void) __objc3_async_frame {defer {++*deferred;}*ran+=expected;__objc3_await binding_pause();if(owner->value!=expected)__builtin_trap();});
}
inline BindingWork calculated_scalar_factory(int left,int right){const long wide=(long)left;const int result=(int)(wide*3+right);return FACTORY_RESULT(^void(void) __objc3_async_frame {(void)result;});}
inline BindingWork unsafe_factory(BindingUnsafe *owner){return FACTORY_RESULT(^void(void) __objc3_async_frame {(void)owner;});}
struct StaticWorkView {
 static BindingWork forward(BindingWork work){return work;}
 static BindingWork construct(BindingOwner *owner,unsigned *ran,unsigned *deferred){return forward(alias_factory(owner,ran,deferred));}
 static int scalar(int value){return value;}
 static int &reference(int &value){return value;}
};
inline objc3_task_t *static_start(objc3_executor_t *executor,unsigned choose,unsigned *destroyed,unsigned *ran,unsigned *deferred){
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 const BindingWork work=StaticWorkView::construct(choose?one:two,ran,deferred);
 objc3_task_t *task=binding_start(work,executor);
#if !__has_feature(objc_arc)
 _Block_release((const void *)work);binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
struct InstanceWorkView {
 BindingWork forward(BindingWork work){return work;}
 BindingWork construct(BindingOwner *owner,unsigned *ran,unsigned *deferred){return alias_factory(owner,ran,deferred);}
 int scalar(int value){return value;}
 int &reference(int &value){return value;}
};
inline objc3_task_t *instance_start(objc3_executor_t *executor,unsigned choose,unsigned *destroyed,unsigned *ran,unsigned *deferred){
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 InstanceWorkView source;const BindingWork work=source.forward(source.construct(choose?one:two,ran,deferred));
 objc3_task_t *task=binding_start(work,executor);
#if !__has_feature(objc_arc)
 _Block_release((const void *)work);binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
struct ChainedWorkView {
 BindingWork passthrough(BindingWork work)const{return work;}
 BindingWork forward(BindingWork work)const{return passthrough(work);}
 BindingWork base(BindingOwner *owner,unsigned *ran,unsigned *deferred)const{return alias_factory(owner,ran,deferred);}
 BindingWork construct(BindingOwner *owner,unsigned *ran,unsigned *deferred)const{return base(owner,ran,deferred);}
 int scalar(int value){return value;}
 int &reference(int &value){return value;}
};
inline objc3_task_t *chained_start(objc3_executor_t *executor,unsigned choose,unsigned *destroyed,unsigned *ran,unsigned *deferred){
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 ChainedWorkView source;const BindingWork work=source.forward(source.construct(choose?one:two,ran,deferred));
 objc3_task_t *task=binding_start(work,executor);
#if !__has_feature(objc_arc)
 _Block_release((const void *)work);binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
struct VirtualWorkView {
 virtual BindingWork forward(BindingWork work)const{return work;}
 virtual BindingWork construct(BindingOwner *owner,unsigned *ran,unsigned *deferred)const{return alias_factory(owner,ran,deferred);}
};
struct FinalWorkView final:VirtualWorkView {
 BindingWork forward(BindingWork work)const override{return VirtualWorkView::forward(work);}
 BindingWork construct(BindingOwner *owner,unsigned *ran,unsigned *deferred)const override{return VirtualWorkView::construct(owner,ran,deferred);}
};
inline objc3_task_t *virtual_start(objc3_executor_t *executor,unsigned choose,unsigned *destroyed,unsigned *ran,unsigned *deferred){
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 FinalWorkView source;const BindingWork work=source.forward(source.construct(choose?one:two,ran,deferred));
 objc3_task_t *task=binding_start(work,executor);
#if !__has_feature(objc_arc)
 _Block_release((const void *)work);binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
inline BindingWork nested_local_factory(BindingOwner *owner,unsigned *ran,unsigned *deferred){
 void (^const callback)(void)=^{*ran+=owner->value;};
 const BindingWork work=^void(void) __objc3_async_frame {defer {++*deferred;}__objc3_await binding_pause();callback();};
 return FACTORY_RESULT(work);
}
inline BindingWork local_producer_factory(BindingOwner *owner,unsigned *ran,unsigned *deferred){const BindingWork work=alias_factory(owner,ran,deferred);return work;}
inline BindingWork local_wrapped_factory(BindingOwner *owner,unsigned *ran,unsigned *deferred){
 const BindingWork base=^void(void) __objc3_async_frame {defer {++*deferred;}*ran+=owner->value;__objc3_await binding_pause();};
 const BindingWork work=callback_alias_factory(base);return work;
}
inline BindingWork nested_scalar_factory(int value){void (^const callback)(void)=^{(void)value;};const BindingWork work=^void(void) __objc3_async_frame {callback();};return FACTORY_RESULT(work);}
inline BindingWork local_unknown_callback_factory(BindingWork callback){const BindingWork work=callback_alias_factory(callback);return work;}
inline objc3_task_t *nested_start(objc3_executor_t *executor,unsigned choose,unsigned *destroyed,unsigned *ran,unsigned *deferred){
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 const BindingWork work=nested_local_factory(choose?one:two,ran,deferred);
 objc3_task_t *task=binding_start(work,executor);
#if !__has_feature(objc_arc)
 _Block_release((const void *)work);binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
inline objc3_task_t *producer_start(objc3_executor_t *executor,unsigned choose,unsigned *destroyed,unsigned *ran,unsigned *deferred){
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 const BindingWork work=local_producer_factory(choose?one:two,ran,deferred);
 objc3_task_t *task=binding_start(work,executor);
#if !__has_feature(objc_arc)
 _Block_release((const void *)work);binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
inline objc3_task_t *wrapped_start(objc3_executor_t *executor,unsigned choose,unsigned *destroyed,unsigned *ran,unsigned *deferred){
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 const BindingWork work=local_wrapped_factory(choose?one:two,ran,deferred);
 objc3_task_t *task=binding_start(work,executor);
#if !__has_feature(objc_arc)
 _Block_release((const void *)work);binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
inline BindingWork unowned_factory(BindingOwner *owner){return FACTORY_RESULT(^[unowned owner] void(void) __objc3_async_frame {(void)owner;});}
template<class T>inline BindingWork unowned_template_factory(T owner){return FACTORY_RESULT(^[unowned owner] void(void) __objc3_async_frame {(void)owner;});}
inline BindingWork local_literal_factory(BindingOwner *owner,unsigned *ran,unsigned *deferred){
 BindingOwner *const snapshot=owner;
 const BindingWork work=^void(void) __objc3_async_frame {defer {++*deferred;}*ran+=snapshot->value;__objc3_await binding_pause();};
 const BindingWork alias=work;return FACTORY_RESULT(alias);
}
inline BindingWork local_owned_factory(BindingOwner *owner,unsigned *ran,unsigned *deferred){
 const BindingWork work=FACTORY_RESULT(^void(void) __objc3_async_frame {defer {++*deferred;}*ran+=owner->value;__objc3_await binding_pause();});
 const BindingWork alias=work;return alias;
}
inline BindingWork local_scalar_factory(int value){const BindingWork work=^void(void) __objc3_async_frame {(void)value;};return FACTORY_RESULT(work);}
inline objc3_task_t *local_start(objc3_executor_t *executor,unsigned choose,bool copied,unsigned *destroyed,unsigned *ran,unsigned *deferred){
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 const BindingWork work=copied?local_owned_factory(choose?one:two,ran,deferred):local_literal_factory(choose?one:two,ran,deferred);
 objc3_task_t *task=binding_start(work,executor);
#if !__has_feature(objc_arc)
 _Block_release((const void *)work);binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
inline BindingWork factory_chain(BindingOwner *owner,unsigned value,unsigned *ran,unsigned *deferred){const unsigned expected=value+0;return calculated_factory(owner,expected,ran,deferred);}
inline BindingWork callback_chain(BindingWork callback){return callback_alias_factory(callback);}
inline BindingWork scalar_chain(int left,int right){return calculated_scalar_factory(left+1,right*2);}
inline BindingWork unsafe_chain(BindingUnsafe *owner){return unsafe_factory(owner);}
inline objc3_task_t *chain_start(objc3_executor_t *executor,unsigned choose,bool callback,unsigned *destroyed,unsigned *ran,unsigned *deferred){
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 const BindingWork base=factory_chain(choose?one:two,choose?1u:2u,ran,deferred);
 const BindingWork produced=callback?callback_chain(base):base;
 objc3_task_t *task=binding_start(produced,executor);
#if !__has_feature(objc_arc)
 if(callback)_Block_release((const void *)produced);_Block_release((const void *)base);
 binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
inline objc3_task_t *calculated_start(objc3_executor_t *executor,unsigned choose,unsigned *destroyed,unsigned *ran,unsigned *deferred){
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 BindingWork produced=calculated_factory(choose?one:two,choose?1u:2u,ran,deferred);
 objc3_task_t *task=binding_start(produced,executor);
#if !__has_feature(objc_arc)
 _Block_release((const void *)produced);binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
inline objc3_task_t *start(objc3_executor_t *executor,unsigned choose,unsigned shape,
                           unsigned *destroyed,unsigned *ran,unsigned *deferred) {
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 BindingWork first=^void(void) __objc3_async_frame {
  defer {++*deferred;}*ran+=one->value;__objc3_await binding_pause();
  if(one->value!=1)__builtin_trap();
 };
 BindingWork second=^void(void) __objc3_async_frame {
  defer {++*deferred;}*ran+=two->value;__objc3_await binding_pause();
  if(two->value!=2)__builtin_trap();
 };
 BindingWork selected=first;if(!choose)selected=second;
 BindingWork alias=selected;
 for(unsigned n=0;n!=2;++n)alias=choose?alias:selected;
 selected=alias;
 void (^callback)(void)=^{*ran+=one->value;};
 if(!choose)callback=^{*ran+=two->value;};
 BindingWork nested=^void(void) __objc3_async_frame {
  defer {++*deferred;}__objc3_await binding_pause();callback();
 };
 BindingWork produced=(BindingWork)0;
 BindingWork work=alias;if(shape==1)work=nested;
 if(shape==2)work=choose?first:second;
 if(shape==3){work=(BindingWork)0;work=selected;}
 if(shape==4)work=choose_work(choose,first,second);
 if(shape==5)work=nested_work(choose,first,second);
 if(shape==6){const BindingWork immutable=project(work);work=project(immutable);}
 if(shape==7)work=project_reference(selected);
 if(shape==8)work=global_work;
 if(shape==9){if(0)work=binding_unresolved_work();}
 if(shape==10){while(0)work=binding_unresolved_work();for(;0;work=binding_unresolved_work())work=binding_unresolved_work();}
 if(shape==11){(void)(0 && (work=binding_unresolved_work()));(void)(1 || (work=binding_unresolved_work()));}
 if(shape==12){(void)(1?work:(work=binding_unresolved_work()));}
 if(shape==13){if constexpr(false)work=binding_unresolved_work();}
 if(shape==14){produced=factory_work(choose?one:two,ran,deferred);work=produced;}
 if(shape==15){produced=conditional_factory(choose,one,two,ran,deferred);work=produced;}
 if(shape==16){produced=Factory::make(choose?one:two,ran,deferred);work=produced;}
 if(shape==17){produced=factory_work(choose?one:two,ran,deferred);work=^void(void) __objc3_async_frame {__objc3_await produced();};}
 if(shape==18){BindingWork base=factory_work(choose?one:two,ran,deferred);produced=callback_factory(base);
#if !__has_feature(objc_arc)
 _Block_release((const void *)base);
#endif
 work=produced;
 }
 if(shape==19){produced=alias_factory(choose?one:two,ran,deferred);work=produced;}
 if(shape==20){produced=template_factory(choose?one:two,ran,deferred);work=produced;}
 if(shape==21){produced=TemplateFactory<BindingOwner *>::make(choose?one:two,ran,deferred);work=produced;}
 if(shape==22){BindingWork base=alias_factory(choose?one:two,ran,deferred);produced=callback_alias_factory(base);
#if !__has_feature(objc_arc)
 _Block_release((const void *)base);
#endif
 work=produced;
 }
 objc3_task_t *task=binding_start(work,executor);
#if !__has_feature(objc_arc)
 if(produced)_Block_release((const void *)produced);
 binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
}
)blocks";
constexpr const char *Consumer=R"blocks(
#include <objc3c/runtime/objc3_runtime_services.h>
#include <assert.h>
#include <stdio.h>
static int execute(void) {
 objc3_executor_t *executor=nullptr;assert(objc3_executor_create(1,&executor)==OBJC3_SERVICE_OK);
 for(unsigned choose=0;choose!=2;++choose)for(unsigned shape=0;shape!=35;++shape)for(unsigned cancel=0;cancel!=4;++cancel){
  unsigned destroyed=0,ran=0,deferred=0;const unsigned before=binding_pause_count();
  auto require=[&](bool ok){if(!ok){fprintf(stderr,"block matrix shape=%u route=%u choose=%u destroyed=%u ran=%u deferred=%u pauses=%u\n",shape,cancel,choose,destroyed,ran,deferred,binding_pause_count()-before);__builtin_abort();}};
  objc3_task_t *task=shape==34?BindingModule::virtual_start(executor,choose,&destroyed,&ran,&deferred):shape==33?BindingModule::chained_start(executor,choose,&destroyed,&ran,&deferred):shape==32?BindingModule::instance_start(executor,choose,&destroyed,&ran,&deferred):shape==31?BindingModule::static_start(executor,choose,&destroyed,&ran,&deferred):shape==30?BindingModule::wrapped_start(executor,choose,&destroyed,&ran,&deferred):shape==29?BindingModule::producer_start(executor,choose,&destroyed,&ran,&deferred):shape==28?BindingModule::nested_start(executor,choose,&destroyed,&ran,&deferred):shape>=26?BindingModule::local_start(executor,choose,shape==27,&destroyed,&ran,&deferred):shape>=24?BindingModule::chain_start(executor,choose,shape==25,&destroyed,&ran,&deferred):shape==23?BindingModule::calculated_start(executor,choose,&destroyed,&ran,&deferred):BindingModule::start(executor,choose,shape,&destroyed,&ran,&deferred);
  require(task && destroyed==(shape==8?2u:1u) && ran==0 && deferred==0);
  uint32_t executed=0;
  if(cancel>=2){
   require(objc3_executor_pump(executor,1,&executed)==OBJC3_SERVICE_OK && executed==1);
   if(cancel==3 && shape!=8){
    unsigned turns=0;
    while(binding_pause_count()==before){require(++turns<=32);require(objc3_executor_pump(executor,1,&executed)==OBJC3_SERVICE_OK && executed==1);}
   }
   objc3_completion pending={sizeof(pending),OBJC3_SERVICE_ABI_VERSION};
   require(objc3_task_get_completion(task,&pending)==(shape==8?OBJC3_SERVICE_OK:OBJC3_SERVICE_NOT_READY));
   require(destroyed==(shape==8?2u:1u) && deferred==0);
   require(binding_pause_count()-before==(cancel==3 && shape!=8?1u:0u));
  }
  if(cancel)objc3_task_request_cancel(task);
  require(objc3_executor_pump(executor,UINT32_MAX,&executed)==OBJC3_SERVICE_OK);
  objc3_task_group_t *group=nullptr;assert(objc3_task_group_create(nullptr,&group)==OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(group,task)==OBJC3_SERVICE_OK);assert(objc3_task_group_close(group)==OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(group)==OBJC3_SERVICE_OK);
  objc3_completion completion={sizeof(completion),OBJC3_SERVICE_ABI_VERSION};assert(objc3_task_get_completion(task,&completion)==OBJC3_SERVICE_OK);
  const bool cancelled=cancel && !(shape==8 && cancel>=2);
  require(completion.kind==(cancelled?OBJC3_COMPLETION_CANCELLED:OBJC3_COMPLETION_SUCCESS));
  const bool wrapper=shape==17 || shape==18 || shape==22 || shape==25 || shape==30;
  const bool late_work=shape==1 || shape==28;
  const unsigned expected_ran=shape==8 || cancel==1 || (cancel==2 && (wrapper || late_work)) || (cancel==3 && late_work)?0u:choose?1u:2u;
  const unsigned expected_deferred=shape==8 || cancel==1 || (cancel==2 && wrapper)?0u:1u;
  require(destroyed==2 && ran==expected_ran && deferred==expected_deferred);
  require(binding_pause_count()-before==(shape!=8 && (cancel==0 || cancel==3)?1u:0u));
  objc3_task_release(task);objc3_task_group_release(group);
 }
 objc3_executor_release(executor);return 0;
}
)blocks";
constexpr const char *Main=R"blocks(
#include <assert.h>
extern "C" void objc_release(void *);
extern "C" void binding_release(void *object){objc_release(object);}
extern "C" id class_createInstance(Class,__SIZE_TYPE__) __attribute__((ns_returns_retained));
extern "C" id object_dispose(__unsafe_unretained id);
@implementation BindingOwner
+ (id)new{return class_createInstance(self,0);}
- (void)dealloc{++*destroyed;object_dispose(self);}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease{}
#endif
@end
static unsigned binding_pauses=0;
extern "C" unsigned binding_pause_count(void){return binding_pauses;}
extern "C" void binding_pause(void) __objc3_async_frame {++binding_pauses;}
extern "C" objc3_task_t *binding_start(BindingWork work,objc3_executor_t *executor) {
 objc3_async_frame frame=work();objc3_task_t *task=nullptr;
 if(objc3_async_frame_start(&frame,executor,nullptr,&task)!=OBJC3_SERVICE_OK)(void)objc3_async_frame_dispose(&frame);
 return task;
}
extern "C" int check_a();extern "C" int check_b();extern "C" int check_c();extern "C" int check_d();
int main(){return check_a()||check_b()||check_c()||check_d();}
)blocks";
void Run(const fs::path &root,const char *standard,int ownership) {
 fs::create_directories(root/"provider");fs::create_directories(root/"include/objc3c");
 fs::create_directory_symlink(OBJC3_BLOCK_BINDING_RUNTIME_HEADERS,root/"include/objc3c/runtime");
 auto request=[&](const fs::path &input,const fs::path &output){
  auto value=Request(input,output,standard,ownership,"-O2",{"-fblocks","-I"+(root/"include").string(),"-Wno-nullability-completeness"});
  value.profile=SemanticProfile::StrictConcurrency;return value;
 };
 const auto external_source=root/"external.mm";Write(external_source,External);auto external_request=request(external_source,root/"external.o");
 external_request.module_name="fixture.BlockBindingABI";external_request.package_identity="block-binding-abi-v1";external_request.emit_semantic_interface=true;
 auto external_built=Compile(external_request);Success(external_built);assert(external_built.interface_artifact && external_built.interface_artifact->success);
 const auto external_path=root/"BlockBindingABI.objc3i";Write(external_path,external_built.interface_artifact->source_bytes);Write(external_path.string()+".json",external_built.interface_artifact->sidecar_json);
 InterfaceImportInput external;external.source_path=external_path.string();external.locked_identity=external_built.interface_artifact->identity;
 const auto include=root/"provider"/BuildInterfaceIncludePath(external.locked_identity.canonical_name);fs::create_directories(include.parent_path());Write(include,external_built.interface_artifact->source_bytes);
 const auto source=root/"provider/module.mm";
 auto publish=[&](const std::string &text,bool owned=true){
  Write(source,text);auto value=request(source,root/"provider.o");value.imported_interfaces={external};value.module_name="fixture.BlockBindings";value.package_identity="block-bindings-v1";
  value.emit_semantic_interface=true;value.module_owned_names=owned;if(owned)value.module_source_root=(root/"provider").string();
  value.interface_exports={"BindingOwner","BindingModule"};
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;
 };
 const auto artifact=publish(Source),replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 publish(Source);assert(fs::remove(source));
 const auto interface=root/"BlockBindings.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
 for(const char *name:{"a","b","c","d"}){
  const bool directed=*name=='c' || *name=='d';
  const auto input=root/(std::string(name)+".mm");
  std::string prefix;
  if(directed)prefix="#include <objc3c/runtime/objc3_runtime_services.h>\n@import fixture.BlockBindingABI;\n#include \"provider/"+BuildInterfaceIncludePath(external.locked_identity.canonical_name)+"\"\n@import fixture.BlockBindings;\n";
  Write(input,prefix+Consumer+"extern \"C\" int check_"+name+"(){return execute();}");
  auto value=request(input,root/(std::string(name)+".o"));
  if(!directed)value.imported_interfaces={external,dependency};
  else {
   value.source_directed_imports=true;value.interface_search_paths={root.string()};
   for(const auto &identity:{external.locked_identity,dependency.locked_identity}){
    InterfaceModuleRequest module;module.canonical_name=identity.canonical_name;module.locked_identity=identity;module.initially_visible=false;value.module_imports.push_back(module);
   }
  }
  Success(Compile(value));
 }
 for(const char *body:{
  "void rejected(unsigned choose,BindingUnsafe *object){BindingWork bad=^void(void) __objc3_async_frame {(void)object;};BindingWork safe=^void(void) __objc3_async_frame {};binding_start(BindingModule::choose_work(choose,bad,safe),nullptr);}",
  "void rejected(unsigned choose,BindingUnsafe *object){BindingWork bad=^void(void) __objc3_async_frame {(void)object;};BindingWork safe=^void(void) __objc3_async_frame {};binding_start(BindingModule::nested_work(choose,safe,bad),nullptr);}",
  "void rejected(BindingWork unknown){binding_start(BindingModule::project(unknown),nullptr);}",
  "void rejected(BindingWork unknown){binding_start(BindingModule::callback_factory(unknown),nullptr);}",
  "void rejected(BindingWork unknown){binding_start(BindingModule::StaticWorkView::forward(unknown),nullptr);}",
  "void static_close(int);int static_open(void);void rejected(void){int descriptor __attribute__((objc_resource(close=static_close,invalid=-1)))=static_open();int copied=BindingModule::StaticWorkView::scalar(descriptor);binding_start(BindingModule::scalar_factory(copied),nullptr);}",
  "void static_close(int);int static_open(void);void rejected(void){int descriptor __attribute__((objc_resource(close=static_close,invalid=-1)))=static_open();int copied=42;BindingModule::StaticWorkView::reference(copied)=descriptor;binding_start(BindingModule::scalar_factory(copied),nullptr);}",
  "void rejected(BindingOwner *owner){binding_start(BindingModule::unowned_factory(owner),nullptr);}",
  "void rejected(BindingOwner *owner){binding_start(BindingModule::unowned_template_factory(owner),nullptr);}",
  "void rejected(BindingWork unknown){binding_start(BindingModule::local_unknown_callback_factory(unknown),nullptr);}",
  "void nested_close(int);int nested_open(void);void rejected(void){int descriptor __attribute__((objc_resource(close=nested_close,invalid=-1)))=nested_open();binding_start(BindingModule::nested_scalar_factory(descriptor),nullptr);}",
  "void local_close(int);int local_open(void);void rejected(void){int descriptor __attribute__((objc_resource(close=local_close,invalid=-1)))=local_open();binding_start(BindingModule::local_scalar_factory(descriptor),nullptr);}",
  "void rejected(BindingWork unknown){binding_start(BindingModule::callback_chain(unknown),nullptr);}",
  "void rejected(BindingUnsafe *owner){binding_start(BindingModule::unsafe_chain(owner),nullptr);}",
  "void chain_close(int);int chain_open(void);void rejected(void){int descriptor __attribute__((objc_resource(close=chain_close,invalid=-1)))=chain_open();binding_start(BindingModule::scalar_chain(descriptor,42),nullptr);}",
  "void chain_close(int);int chain_open(void);void rejected(void){int descriptor __attribute__((objc_resource(close=chain_close,invalid=-1)))=chain_open();binding_start(BindingModule::scalar_chain(41,descriptor),nullptr);}",
  "void calculation_close(int);int calculation_open(void);void rejected(void){int descriptor __attribute__((objc_resource(close=calculation_close,invalid=-1)))=calculation_open();binding_start(BindingModule::calculated_scalar_factory(descriptor,42),nullptr);}",
  "void calculation_close(int);int calculation_open(void);void rejected(void){int descriptor __attribute__((objc_resource(close=calculation_close,invalid=-1)))=calculation_open();binding_start(BindingModule::calculated_scalar_factory(41,descriptor),nullptr);}",
  "void rejected(BindingWork unknown){binding_start(BindingModule::callback_alias_factory(unknown),nullptr);}",
  "void factory_close(int);int factory_open(void);void rejected(void){int descriptor __attribute__((objc_resource(close=factory_close,invalid=-1)))=factory_open();binding_start(BindingModule::scalar_alias_factory(descriptor),nullptr);}",
  "void rejected(BindingUnsafe *owner){binding_start(BindingModule::unsafe_factory(owner),nullptr);}",
  "void factory_close(int);int factory_open(void);void rejected(void){int descriptor __attribute__((objc_resource(close=factory_close,invalid=-1)))=factory_open();binding_start(BindingModule::scalar_factory(descriptor),nullptr);}",
  "void factory_close(int);int factory_open(void);void rejected(void){int descriptor __attribute__((objc_resource(close=factory_close,invalid=-1)))=factory_open();int copied=descriptor;BindingWork inner=BindingModule::scalar_factory(copied);BindingWork outer=^void(void) __objc3_async_frame {__objc3_await inner();};binding_start(outer,nullptr);}"}) {
  const auto input=root/"bad.mm";Write(input,body);auto value=request(input,root/"bad.o");value.imported_interfaces={external,dependency};auto result=Compile(value);assert(!result.success);
  bool matched=false;for(const auto &diagnostic:result.diagnostics)matched|=diagnostic.message.find("non-Sendable type")!=std::string::npos || diagnostic.message.find("cannot prove Sendable captures")!=std::string::npos || diagnostic.message.find("cleanup-managed capture")!=std::string::npos;assert(matched);
 }
 const auto main=root/"main.mm";Write(main,Main);auto entry=request(main,root/"main.o");entry.imported_interfaces={external,dependency};Success(Compile(entry));
 LinkAndExecute(root,entry.output_path,true,{(root/"c.o").string(),(root/"d.o").string(),OBJC3_BLOCK_BINDING_RUNTIME_LIBRARY,"-pthread"});
}
}
int main(int argc,char **argv){
 if(argc!=1 && (argc!=3 || (std::string_view(argv[1])!="c++20" && std::string_view(argv[1])!="c++23") ||
                           (std::string_view(argv[2])!="1" && std::string_view(argv[2])!="2"))) {
  std::cerr<<"usage: "<<argv[0]<<" [c++20|c++23 1|2]\n";
  return 2;
 }
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-block-bindings",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const char *standard:{"c++20","c++23"})for(int ownership:{1,2}){
  if(argc==3 && (std::string_view(argv[1])!=standard || argv[2][0]!='0'+ownership))continue;
  const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}
 return cases.Finish();
}
