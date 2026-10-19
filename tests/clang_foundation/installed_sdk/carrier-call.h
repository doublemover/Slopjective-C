#pragma once
typedef __objc3_optional(id) CallStrong;
typedef __objc3_optional(id __weak) CallWeak;
typedef __objc3_optional(id __unsafe_unretained) CallUnsafe;
typedef __objc3_result(id, id) CallOutcome;
typedef __objc3_result(id, id __weak) CallMixed;
typedef void (^CallAction)(void);
typedef __objc3_optional(CallAction) CallBlock;

id call_object(int key) __attribute__((ns_returns_retained));
int call_destroyed(int key);
CallStrong call_make(int key);
CallStrong call_identity(CallStrong value);
CallStrong call_forward(int key);
void call_replace_parameter(CallStrong value, int key);
CallWeak call_weak(CallWeak value);
CallUnsafe call_unsafe(CallUnsafe value);
CallOutcome call_outcome(CallOutcome value, int reverse);
CallMixed call_mixed(CallMixed value);
CallBlock call_block(id object);
CallBlock call_block_identity(CallBlock value);
CallStrong call_many(int first, CallStrong a, CallStrong b, int second,
                     CallStrong c, CallStrong d, CallStrong e, CallStrong f,
                     CallStrong g, int last);
#ifdef __cplusplus
CallStrong call_throw(CallStrong value);
struct CallBase {
  virtual CallStrong apply(CallStrong value) = 0;
  virtual ~CallBase() = default;
};
struct CallDerived final : CallBase {
  CallStrong apply(CallStrong value) override;
  static CallStrong make(int key);
};
#endif
