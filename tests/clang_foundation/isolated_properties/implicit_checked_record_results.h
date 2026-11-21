#ifndef OBJC3_NATIVE_IMPLICIT_CHECKED_RECORD_RESULTS_H
#define OBJC3_NATIVE_IMPLICIT_CHECKED_RECORD_RESULTS_H
#ifdef __cplusplus
struct ImplicitCheckedHolder {CarrierRecordLeaf value;};
static ImplicitCheckedHolder implicit_checked_holder(int mode) __objc3_throws(id<Error>);
static ImplicitCheckedHolder implicit_checked_holder(int mode) __objc3_throws(id<Error>) {
 if(mode==1)__objc3_throw(checked_property_error);
 ImplicitCheckedHolder value{CarrierRecordLeaf(157,0)};
 if(mode==2)__objc3_throw(checked_property_error);
 if(mode==3)throw 753;
#if __has_feature(objc_exceptions)
 @try{return value;}@finally {if(mode==4)__objc3_throw(checked_property_error);}
#else
 __objc3_defer {if(mode==4)throw 754;}
 return value;
#endif
}
#if __has_feature(objc_arc)
struct ImplicitCheckedARCRecord {MixedCallStrong object;MixedCallWeak observer;AlignedBlockProbe callback;};
static ImplicitCheckedARCRecord implicit_checked_arc_record(int mode) __objc3_throws(id<Error>);
static ImplicitCheckedARCRecord implicit_checked_arc_record(int mode) __objc3_throws(id<Error>) {
 if(mode==1)__objc3_throw(checked_property_error);
 MixedCallObject *object=[MixedCallObject new];
 ImplicitCheckedARCRecord value{object,object,^int{return 159;}};
 if(mode==2)__objc3_throw(checked_property_error);
 if(mode==3)throw 753;
#if __has_feature(objc_exceptions)
 @try{return value;}@finally {if(mode==4)__objc3_throw(checked_property_error);}
#else
 __objc3_defer {if(mode==4)throw 754;}
 return value;
#endif
}
@interface PropertyMain (ImplicitCheckedARCRecords)
- (ImplicitCheckedARCRecord)implicitRecord:(int)mode __objc3_throws(id<Error>);
@end
@implementation PropertyMain (ImplicitCheckedARCRecords)
- (ImplicitCheckedARCRecord)implicitRecord:(int)mode __objc3_throws(id<Error>){return __objc3_try(implicit_checked_arc_record(mode));}
@end
#endif
static void implicit_checked_record_result_suite(PropertyMain *object) {
 @autoreleasepool {
  checked_property_error=object;
  for(int mode=0;mode!=5;++mode)for(int route=0;route!=5;++route) {
   @autoreleasepool {
    bool typed=false,native=false;
    try {
     __objc3_do {
      if(route==0){auto value=__objc3_try(implicit_checked_holder(mode));assert(value.value.value==157);}
      else if(route==1){auto callback=implicit_checked_holder;auto value=__objc3_try(callback(mode));assert(value.value.value==157);}
      else if(route==2){auto callback=^ImplicitCheckedHolder(int input) __objc3_throws(id<Error>){return __objc3_try(implicit_checked_holder(input));};auto value=__objc3_try(callback(mode));assert(value.value.value==157);}
      else if(route==3){auto value=__objc3_try_optional(implicit_checked_holder(mode));assert(__objc3_has_value(value)==(mode==0));}
      else __objc3_try(implicit_checked_holder(mode));
     }__objc3_catch(id<Error> error){assert(error==checked_property_error);typed=true;}
    }catch(int error){assert(error==753 || error==754);native=true;}
    bool nativeExpected=mode==3;
#if !__has_feature(objc_exceptions)
    nativeExpected|=mode==4;
#endif
    assert(native==nativeExpected);assert(typed==(mode!=0 && !nativeExpected && route!=3));
   }
#if __has_feature(objc_arc)
   mixed_storage_assert_clean(10,0,route,mode);
#else
   carrier_record_assert_clean();
#endif
  }
#if __has_feature(objc_arc)
  for(int mode=0;mode!=5;++mode)for(int route=0;route!=6;++route) {
   @autoreleasepool {
    bool typed=false,native=false;
    try {
     __objc3_do {
      if(route==0){auto value=__objc3_try(implicit_checked_arc_record(mode));assert(value.callback()==159 && value.object==value.observer);}
      else if(route==1){auto callback=implicit_checked_arc_record;auto value=__objc3_try(callback(mode));assert(value.callback()==159);}
      else if(route==2){auto callback=^ImplicitCheckedARCRecord(int input) __objc3_throws(id<Error>){return __objc3_try(implicit_checked_arc_record(input));};auto value=__objc3_try(callback(mode));assert(value.callback()==159);}
      else if(route==3){auto value=__objc3_try_optional(implicit_checked_arc_record(mode));assert(__objc3_has_value(value)==(mode==0));}
      else if(route==4){auto value=__objc3_try([object implicitRecord:mode]);assert(value.callback()==159);}
      else __objc3_try(implicit_checked_arc_record(mode));
     }__objc3_catch(id<Error> error){assert(error==checked_property_error);typed=true;}
    }catch(int error){assert(error==753 || error==754);native=true;}
    bool nativeExpected=mode==3;
#if !__has_feature(objc_exceptions)
    nativeExpected|=mode==4;
#endif
    assert(native==nativeExpected);assert(typed==(mode!=0 && !nativeExpected && route!=3));
   }
   mixed_storage_assert_clean(11,0,route,mode);
  }
#endif
  checked_property_error=(id<Error>)0;
 }
}
#else
static void implicit_checked_record_result_suite(PropertyMain *object){(void)object;}
#endif
#endif
