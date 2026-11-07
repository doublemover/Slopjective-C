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
  char inline_bytes[4];
  struct {char bytes[4];int value;} nested;
#if !__has_feature(objc_arc)
  int references;
#endif
}
+ (instancetype _Nonnull)new;
@property(nonatomic,readonly) const char *view __attribute__((objc_returns_inner_pointer));
- (const char *)bytes __attribute__((objc_returns_inner_pointer));
#ifdef __cplusplus
- (char &)reference __attribute__((objc_returns_inner_pointer));
#endif
#if !__has_feature(objc_arc)
- (id)retain;
- (oneway void)release;
#endif
@end
@implementation HandlerProbe
+ (instancetype _Nonnull)new {
  HandlerProbe *value=class_createInstance(self,0);value->payload='q';
  value->inline_bytes[0]='q';value->nested.bytes[0]='q';value->nested.value=17;
#if !__has_feature(objc_arc)
  value->references=1;
#endif
  ++handler_created;return value;
}
#if !__has_feature(objc_arc)
- (id)retain {++references;return self;}
- (oneway void)release {if(!--references)[self dealloc];}
#endif
- (const char *)bytes {return &payload;}
- (const char *)view {return &payload;}
#ifdef __cplusplus
- (char &)reference {return payload;}
#endif
- (void)dealloc {++handler_destroyed;object_dispose(self);}
@end
typedef borrowed const char *HandlerBytes;
static HandlerBytes handler_bytes(HandlerProbe *owner)
    __attribute__((objc_returns_borrowed(owner_index=0)));
__attribute__((objc_returns_borrowed(owner_index=0)))
static HandlerBytes handler_bytes(HandlerProbe *owner) {
  assert(owner->payload=='q');return &owner->payload;
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
    @catch(HandlerProbe *_Nonnull owner) {
#if !__has_feature(objc_arc)
      [source release];
#else
      source=(id)0;
#endif
      HandlerBytes bytes=handler_bytes(owner);
      handler_use([owner bytes]);
      __objc3_defer {
        assert(handler_destroyed==before && owner->payload=='q');handler_use(bytes);++handler_cleanup;
      }
      handler_use(bytes);++handler_body;
      if(mode==0)return 10;
      if(mode==1)@throw;
      if(mode==2)goto finished;
    }
  } @catch(HandlerProbe *_Nonnull outer) {
    assert(mode==1 && handler_destroyed==before);
    handler_use(handler_bytes(outer));++handler_outer;
  }
finished:
  return 20;
}
static int interior_storage_case(void) {
  int before_destroyed=handler_destroyed,scalar=0;
  HandlerProbe *_Nonnull owner=[HandlerProbe new];
    withLifetime(owner) {
      const char *bytes=owner.view;
      const char *array=owner->inline_bytes;
      const char *nested=owner->nested.bytes;
      scalar=owner->nested.value;
#ifdef __cplusplus
      char &reference=[owner reference];
      struct Reference {char &byte;};
      Reference first{reference},second=first;
      char copy=reference;
      const char *address=__builtin_addressof(reference);
#endif
#if __has_feature(objc_arc)
      owner=(id)0;
#else
      [owner release];
#endif
      assert(handler_destroyed==before_destroyed);
      handler_use(bytes);handler_use(array);handler_use(nested);
#ifdef __cplusplus
      handler_use(&second.byte);handler_use(address);assert(copy=='q');
#endif
    }
  return scalar;
}
static void interior_storage_suite(void) {
  int before_created=handler_created,before_destroyed=handler_destroyed;
  @autoreleasepool {assert(interior_storage_case()==17);}
  assert(handler_created==before_created+1 && handler_destroyed==before_destroyed+1);
}
static void catch_binding_suite(void) {
  for(int mode=0;mode!=4;++mode) {
    int before=handler_destroyed,body=handler_body,cleanup=handler_cleanup;
    @autoreleasepool {assert(handler_paths(mode)==(mode==0?10:20));}
    assert(handler_body==body+1 && handler_cleanup==cleanup+1 && handler_destroyed==before+1);
  }
  assert(handler_outer==1 && handler_created==4 && handler_destroyed==4);
  interior_storage_suite();
}
#pragma clang assume_nonnull end
#endif
