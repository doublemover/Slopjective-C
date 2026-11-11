#ifndef OBJC3_NATIVE_CPP_CARRIER_VALUES_H
#define OBJC3_NATIVE_CPP_CARRIER_VALUES_H
#ifdef __cplusplus
static int cpp_value_copies,cpp_value_moves,cpp_value_attempt,cpp_value_fail;
struct CppCarrierCopyValue {
  int value;
  explicit CppCarrierCopyValue(int v):value(v) {}
  CppCarrierCopyValue(const CppCarrierCopyValue &other):value(other.value) {
    if(++cpp_value_attempt==cpp_value_fail)throw 747;
    ++cpp_value_copies;
  }
  CppCarrierCopyValue(CppCarrierCopyValue &&other):value(other.value) {
    if(++cpp_value_attempt==cpp_value_fail)throw 748;
    other.value=-1;++cpp_value_moves;
  }
};
static int cpp_twin_live,cpp_twin_created,cpp_twin_destroyed;
struct CppCarrierTwinOwner {
 int value;explicit CppCarrierTwinOwner(int v):value(v){++cpp_twin_live;++cpp_twin_created;}
 CppCarrierTwinOwner(const CppCarrierTwinOwner &other):value(other.value){++cpp_twin_live;++cpp_twin_created;}
 ~CppCarrierTwinOwner() noexcept{--cpp_twin_live;++cpp_twin_destroyed;assert(cpp_twin_live>=0);}
};
struct CppCarrierPlainValue {int value;};
struct CppCarrierEmptyValue {};
struct alignas(64) CppCarrierWideValue {int value;};
union CppCarrierUnionValue {int value;double other;};
static_assert(__is_trivially_destructible(CppCarrierCopyValue));
static_assert(!__is_trivially_copyable(CppCarrierCopyValue));
using CppValueMaybe=__objc3_optional(CppCarrierCopyValue);
using CppPlainMaybe=__objc3_optional(CppCarrierPlainValue);
using CppEmptyMaybe=__objc3_optional(CppCarrierEmptyValue);
using CppUnionMaybe=__objc3_optional(CppCarrierUnionValue);
using CppWideMaybe=__objc3_optional(CppCarrierWideValue);
static int cpp_wide_live,cpp_wide_created,cpp_wide_destroyed;
struct alignas(128) CppCarrierWideOwner {
 int value;explicit CppCarrierWideOwner(int v):value(v){++cpp_wide_live;++cpp_wide_created;}
 CppCarrierWideOwner(const CppCarrierWideOwner &other):value(other.value){++cpp_wide_live;++cpp_wide_created;}
 ~CppCarrierWideOwner() noexcept{--cpp_wide_live;++cpp_wide_destroyed;assert(cpp_wide_live>=0);}
};
using CppWideOwnedMaybe=__objc3_optional(CppCarrierWideOwner);
static CppWideMaybe cpp_value_wide(CppWideMaybe value){return __objc3_copy(value);}
static int cpp_value_many(CppPlainMaybe a,CppPlainMaybe b,CppPlainMaybe c,CppPlainMaybe d,
 CppPlainMaybe e,CppPlainMaybe f,CppPlainMaybe g,CppPlainMaybe h,CppPlainMaybe i) {
 return __objc3_has_value(a)+__objc3_has_value(b)+__objc3_has_value(c)+__objc3_has_value(d)+
     __objc3_has_value(e)+__objc3_has_value(f)+__objc3_has_value(g)+__objc3_has_value(h)+__objc3_has_value(i);
}
static CppValueMaybe cpp_value_source(bool some) {
 if(some)return __objc3_some(CppCarrierCopyValue,CppCarrierCopyValue(31));
 return __objc3_none(CppCarrierCopyValue);
}
static int cpp_value_payload(const CppValueMaybe &value) {
 return __objc3_match(value){case __objc3_case_some(let payload)=>payload.value;case __objc3_case_none=>-1;};
}
static CppValueMaybe cpp_value_echo(CppValueMaybe value){return __objc3_copy(value);}
static CppValueMaybe cpp_value_checked(bool some,int mode) __objc3_throws(id<Error>) {
 if(mode==1)__objc3_throw(checked_property_error);
 if(mode==2)throw 749;
 return cpp_value_source(some);
}
@interface PropertyMain (CppCarrierValues)
- (CppWideMaybe)cppWide:(CppWideMaybe)value;
- (CppWideOwnedMaybe)cppWideOwned:(CppWideOwnedMaybe)value;
- (CppValueMaybe)cppValue:(CppValueMaybe)value;
- (CppPlainMaybe)cppPlain:(CppPlainMaybe)value;
- (CppEmptyMaybe)cppEmpty:(CppEmptyMaybe)value;
- (CppUnionMaybe)cppUnion:(CppUnionMaybe)value;
@end
@implementation PropertyMain (CppCarrierValues)
- (CppWideMaybe)cppWide:(CppWideMaybe)value{return __objc3_copy(value);}
- (CppWideOwnedMaybe)cppWideOwned:(CppWideOwnedMaybe)value{return __objc3_copy(value);}
- (CppValueMaybe)cppValue:(CppValueMaybe)value{return __objc3_copy(value);}
- (CppPlainMaybe)cppPlain:(CppPlainMaybe)value{return __objc3_copy(value);}
- (CppEmptyMaybe)cppEmpty:(CppEmptyMaybe)value{return __objc3_copy(value);}
- (CppUnionMaybe)cppUnion:(CppUnionMaybe)value{return __objc3_copy(value);}
@end
static void cpp_carrier_value_suite(PropertyMain *object) {
 @autoreleasepool {
 checked_property_error=object;
 for(int some=0;some!=2;++some)for(int route=0;route!=5;++route) {
  auto original=cpp_value_source(some);
  auto invoke=^CppValueMaybe(CppValueMaybe value){return __objc3_copy(value);};
  const int copies_before=cpp_value_copies,moves_before=cpp_value_moves;
  auto value=route==0?cpp_value_echo(original):route==1?[object cppValue:original]:
      route==2?invoke(original):route==3?cpp_value_echo(__objc3_move(original)):cpp_value_source(some);
  assert(__objc3_has_value(value)==some);
  assert(cpp_value_copies-copies_before==(some?(route<3?2:route==3?1:0):0));
  assert(cpp_value_moves-moves_before==(some && route==3?1:0));
  assert(cpp_value_payload(value)==(some?31:-1));
  cpp_value_echo(value);invoke(value);[object cppValue:value];
  for(int mode=0;mode!=3;++mode) {
   bool native=false,typed=false;
   try {
    __objc3_do {auto checked=__objc3_try(cpp_value_checked(some,mode));assert(__objc3_has_value(checked)==some);}
    __objc3_catch(id<Error> error){assert(error==checked_property_error);typed=true;}
   }catch(int error){assert(mode==2 && error==749);native=true;}
   assert(typed==(mode==1) && native==(mode==2));
   if(mode<2){auto optional=__objc3_try_optional(cpp_value_checked(some,mode));assert(__objc3_has_value(optional)==(mode==0));}
  }
 }
 for(int fault=0;fault!=2;++fault) {
  auto original=cpp_value_source(true);cpp_value_fail=cpp_value_attempt+1;
  bool caught=false;try{if(fault)cpp_value_echo(__objc3_move(original));else [object cppValue:original];}
  catch(int error){assert(error==(fault?748:747));caught=true;}
  cpp_value_fail=0;assert(caught && __objc3_has_value(original));
 }
 auto plain=[object cppPlain:__objc3_some(CppCarrierPlainValue,CppCarrierPlainValue{42})];
 auto empty=[object cppEmpty:__objc3_some(CppCarrierEmptyValue,CppCarrierEmptyValue{})];
 auto active=[object cppUnion:__objc3_some(CppCarrierUnionValue,CppCarrierUnionValue{73})];
 assert(__objc3_has_value(plain) && __objc3_has_value(empty) && __objc3_has_value(active));
 auto wide=cpp_value_wide(__objc3_some(CppCarrierWideValue,CppCarrierWideValue{97}));
 auto (*indirect)(CppWideMaybe)=cpp_value_wide;auto wide_copy=indirect(wide);
 assert(__objc3_has_value(wide_copy));
 auto wide_message=[object cppWide:wide_copy];assert(__objc3_has_value(wide_message));
 auto wide_block=^CppWideMaybe(CppWideMaybe value){return __objc3_copy(value);};
 auto wide_result=wide_block(wide_copy);assert(__objc3_has_value(wide_result));
 for(int some=0;some!=2;++some) {
  {
   auto owned=some?__objc3_some(CppCarrierWideOwner,CppCarrierWideOwner(101)):__objc3_none(CppCarrierWideOwner);
   auto copied=[object cppWideOwned:owned];assert(__objc3_has_value(copied)==some);
   [object cppWideOwned:owned];
  }
  assert(!cpp_wide_live && cpp_wide_created==cpp_wide_destroyed);
 }
 auto plain_none=__objc3_none(CppCarrierPlainValue);
 assert(cpp_value_many(plain,plain_none,plain,plain_none,plain,plain_none,plain,plain_none,plain)==5);
 auto reflect=[&](SEL selector,__SIZE_TYPE__ size,__SIZE_TYPE__ alignment){
  auto method=class_getInstanceMethod(object_getClass(object),selector);assert(method);
  char *type=method_copyReturnType(method);assert(type && std::strstr(type,"__objc3_cxx_storage_v"));
  assert(objc_sizeof_type(type)==size && objc_alignof_type(type)==alignment);std::free(type);
 };
 reflect(@selector(cppWide:),sizeof(CppWideMaybe),alignof(CppWideMaybe));
 reflect(@selector(cppWideOwned:),sizeof(CppWideOwnedMaybe),alignof(CppWideOwnedMaybe));
 reflect(@selector(cppValue:),sizeof(CppValueMaybe),alignof(CppValueMaybe));
 reflect(@selector(cppPlain:),sizeof(CppPlainMaybe),alignof(CppPlainMaybe));
 reflect(@selector(cppEmpty:),sizeof(CppEmptyMaybe),alignof(CppEmptyMaybe));
 reflect(@selector(cppUnion:),sizeof(CppUnionMaybe),alignof(CppUnionMaybe));
 for(int fault=0;fault!=3;++fault) {
  bool caught=false;
  try {
   auto captured=cpp_value_source(true);
   if(fault==1)cpp_value_fail=cpp_value_attempt+1;
   typedef CppValueMaybe (^ValueBlock)(void);
#if __has_feature(objc_arc)
   ValueBlock __unsafe_unretained literal=^CppValueMaybe{return __objc3_copy(captured);};
#else
   ValueBlock literal=^CppValueMaybe{return __objc3_copy(captured);};
#endif
   if(fault==2)cpp_value_fail=cpp_value_attempt+1;
#if __has_feature(objc_arc)
   ValueBlock copied=(__bridge_transfer ValueBlock)_Block_copy((__bridge const void *)literal);
#else
   ValueBlock copied=(ValueBlock)_Block_copy((const void *)literal);
#endif
   auto value=copied();assert(cpp_value_payload(value)==31);
#if __has_feature(objc_arc)
   copied=(ValueBlock)0;
#else
   _Block_release((const void *)copied);
#endif
  }catch(int error){assert(fault && error==747);caught=true;}
  cpp_value_fail=0;assert(caught==bool(fault));
 }
 {
  auto first_value=cpp_value_source(true);
  auto second_value=__objc3_some(CppCarrierTwinOwner,CppCarrierTwinOwner(55));
  typedef int (^ProbeBlock)(void);
  __unsafe_unretained ProbeBlock first=^int{return cpp_value_payload(first_value);};
  __unsafe_unretained ProbeBlock second=^int{return __objc3_has_value(second_value);};
#if __has_feature(objc_arc)
  ProbeBlock first_copy=(__bridge_transfer ProbeBlock)_Block_copy((__bridge const void *)first);
  ProbeBlock second_copy=(__bridge_transfer ProbeBlock)_Block_copy((__bridge const void *)second);
#else
  ProbeBlock first_copy=(ProbeBlock)_Block_copy((const void *)first);
  ProbeBlock second_copy=(ProbeBlock)_Block_copy((const void *)second);
#endif
  assert(first_copy()==31 && second_copy()==1 && cpp_twin_live==3);
#if __has_feature(objc_arc)
  first_copy=(ProbeBlock)0;second_copy=(ProbeBlock)0;
#else
  _Block_release((const void *)first_copy);_Block_release((const void *)second_copy);
#endif
  assert(cpp_twin_live==2);
 }
 assert(!cpp_twin_live && cpp_twin_created==cpp_twin_destroyed);
 assert(cpp_value_copies>0 && cpp_value_moves>0);checked_property_error=(id)0;
 }
}
#else
static void cpp_carrier_value_suite(PropertyMain *object){(void)object;}
#endif
#endif
