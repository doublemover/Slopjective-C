#pragma once
#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#endif
static int access_failures, access_created, access_destroyed;
static int access_receivers, access_arguments, access_getters, access_messages;
#define ACCESS_CHECK(value) do { if (!(value)) ++access_failures; } while (0)
#ifdef __cplusplus
extern "C" {
#endif
id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
id object_dispose(__unsafe_unretained id);
#ifdef __cplusplus
}
#endif
__attribute__((objc_root_class))
@interface AccessObject { Class isa; @public int number; }
+ (instancetype)make:(int)value __attribute__((ns_returns_retained));
@property(readonly) AccessObject * _Nonnull next;
@property(readonly) id _Nullable missing;
@property(readonly) int (^ _Nonnull callback)(int);
- (id _Nonnull)take:(int)value other:(int)other;
- (void)effect:(int)value;
@end
@implementation AccessObject
+ (instancetype)make:(int)value {
  AccessObject *object = class_createInstance(self, 0);
  object->number = value;
  ++access_created;
  return object;
}
- (AccessObject *)next { ++access_getters; return self; }
- (id)missing { ++access_getters; return (id)0; }
- (int (^)(int))callback { ++access_getters; return ^(int value) { return value + 3; }; }
- (id)take:(int)value other:(int)other {
  ++access_messages;
  ACCESS_CHECK(number == 42 && value > 0 && other > 0);
  return self;
}
- (void)effect:(int)value {
  ++access_messages;
  ACCESS_CHECK(number == 42 && value > 0);
}
- (void)dealloc { ++access_destroyed; object_dispose(self); }
@end
static AccessObject *access_source(AccessObject *object) {
  ++access_receivers;
  return object;
}
static int access_argument(int value) { ++access_arguments; return value; }
#ifdef __cplusplus
template<class T> static auto access_template_member(T object) { return object?.next; }
template<class T> static auto access_template_send(T object) {
  return [object? take:access_argument(1) other:access_argument(2)];
}
#endif
#if __has_feature(objc_arc)
static int access_clear(AccessObject * __strong *owner) { *owner = (id)0; return 1; }
#endif
static void optional_access_suite(void) {
  AccessObject *owner = [AccessObject make:42];
  AccessObject *absent = (id)0;
  access_receivers = access_arguments = access_getters = access_messages = 0;
  id result = [access_source(absent)? take:access_argument(1) other:access_argument(2)];
  ACCESS_CHECK(result == (id)0 && access_receivers == 1 && access_arguments == 0 && access_messages == 0);
  result = [access_source(owner)? take:access_argument(1) other:access_argument(2)];
  ACCESS_CHECK(result == owner && access_receivers == 2 && access_arguments == 2 && access_messages == 1);
  [access_source(absent)? effect:access_argument(1)];
  ACCESS_CHECK(access_receivers == 3 && access_arguments == 2 && access_messages == 1);
  [access_source(owner)? effect:access_argument(1)];
  ACCESS_CHECK(access_receivers == 4 && access_arguments == 3 && access_messages == 2);
  result = access_source(absent)?.next?.next;
  ACCESS_CHECK(result == (id)0 && access_receivers == 5 && access_getters == 0);
  result = access_source(owner)?.next?.next;
  ACCESS_CHECK(result == owner && access_receivers == 6 && access_getters == 2);
  result = owner?.missing ?? owner;
  ACCESS_CHECK(result == owner && access_getters == 3);
  int (^block)(int) = owner?.callback;
  ACCESS_CHECK(block != (id)0 && block(4) == 7 && access_getters == 4);
  block = absent?.callback;
  ACCESS_CHECK(block == (id)0 && access_getters == 4);
  block = [absent? callback];
  ACCESS_CHECK(block == (id)0 && access_getters == 4);
  block = [owner? callback];
  ACCESS_CHECK(block != (id)0 && block(5) == 8 && access_getters == 5);
  // Ordinary nil sends evaluate arguments; optional sends intentionally skip.
  [(AccessObject *)0 effect:access_argument(1)];
  ACCESS_CHECK(access_arguments == 4 && access_messages == 2);
  bool condition = false;
  [condition ? owner : absent effect:access_argument(1)];
  ACCESS_CHECK(access_arguments == 5 && access_messages == 2);
#ifdef __cplusplus
  result = access_template_member(owner);
  ACCESS_CHECK(result == owner);
  result = access_template_send(absent);
  ACCESS_CHECK(result == (id)0 && access_arguments == 5);
  result = access_template_send(owner);
  ACCESS_CHECK(result == owner && access_arguments == 7);
#endif
#if __has_feature(objc_arc)
  AccessObject * __weak witness;
  {
    AccessObject *temporary_owner = [AccessObject make:42];
    witness = temporary_owner;
    // One weak receiver snapshot survives the argument releasing its source.
    [witness? effect:access_clear(&temporary_owner)];
    ACCESS_CHECK(temporary_owner == (id)0 && witness == (id)0);
  }
#else
  object_dispose(owner);
  ++access_destroyed;
#endif
}
