#ifndef OBJC3_CATCH_BINDING_OWNERSHIP_H
#define OBJC3_CATCH_BINDING_OWNERSHIP_H
#include <assert.h>
#pragma clang assume_nonnull begin
#ifdef __cplusplus
extern "C" {
#endif
id _Nullable class_createInstance(Class,__SIZE_TYPE__) __attribute__((ns_returns_retained));
id _Nullable object_dispose(__unsafe_unretained id);
#ifdef __cplusplus
}
#endif
static int handler_created,handler_destroyed,handler_body,handler_cleanup,handler_outer;
__attribute__((objc_root_class)) @interface HandlerProbe {
  Class isa;
@public
  char payload;
#if !__has_feature(objc_arc)
  int references;
#endif
}
+ (instancetype _Nonnull)new;
#if !__has_feature(objc_arc)
- (id)retain;
- (oneway void)release;
#endif
@end
@implementation HandlerProbe
+ (instancetype _Nonnull)new {
  HandlerProbe *value=class_createInstance(self,0);value->payload='q';
#if !__has_feature(objc_arc)
  value->references=1;
#endif
  ++handler_created;return value;
}
#if !__has_feature(objc_arc)
- (id)retain {++references;return self;}
- (oneway void)release {if(!--references)[self dealloc];}
#endif
- (void)dealloc {++handler_destroyed;object_dispose(self);}
@end
typedef borrowed const char *HandlerBytes;
static HandlerBytes handler_bytes(HandlerProbe *owner)
    __attribute__((objc_returns_borrowed(owner_index=0)));
__attribute__((objc_returns_borrowed(owner_index=0)))
static HandlerBytes handler_bytes(HandlerProbe *owner) {
  static const char bytes[]="q";assert(owner->payload=='q');return bytes;
}
static void handler_use(const char *bytes __attribute__((noescape))) {assert(*bytes=='q');}
static int handler_paths(int mode) {
  int before=handler_destroyed;
#if __has_feature(objc_arc)
  HandlerProbe *source=[HandlerProbe new];
#else
  HandlerProbe * _Nonnull source=[HandlerProbe new];
#endif
  @try {
    @try {@throw source;}
    @catch(HandlerProbe *owner) {
#if !__has_feature(objc_arc)
      [source release];
#else
      source=(id)0;
#endif
      HandlerBytes bytes=handler_bytes(owner);
      __objc3_defer {
        assert(handler_destroyed==before && owner->payload=='q');handler_use(bytes);++handler_cleanup;
      }
      handler_use(bytes);++handler_body;
      if(mode==0)return 10;
      if(mode==1)@throw;
      if(mode==2)goto finished;
    }
  } @catch(HandlerProbe *outer) {
    assert(mode==1 && handler_destroyed==before);
    handler_use(handler_bytes(outer));++handler_outer;
  }
finished:
  return 20;
}
static void catch_binding_suite(void) {
  for(int mode=0;mode!=4;++mode) {
    int before=handler_destroyed,body=handler_body,cleanup=handler_cleanup;
    @autoreleasepool {assert(handler_paths(mode)==(mode==0?10:20));}
    assert(handler_body==body+1 && handler_cleanup==cleanup+1 && handler_destroyed==before+1);
  }
  assert(handler_outer==1 && handler_created==4 && handler_destroyed==4);
}
#pragma clang assume_nonnull end
#endif
