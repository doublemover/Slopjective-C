// RUN: %clang -fobjc-version=3 -fobjc-runtime=gnustep-2.2 -fblocks -fobjc-arc -fsyntax-only -Xclang -verify %s
void capture_use(int);
void capture_close_checked(int) __attribute__((nothrow));
void capture_close_throwing(int);
void capture_arbitrary_cleanup(int *);
void capture_semantics(id object) {
  int scalar = 4;
  const int immutable = 5;
  void (^weak_bad)(void) = ^[weak scalar] {}; // expected-error {{weak and unowned Block captures require}}
  void (^unowned_bad)(void) = ^[unowned scalar] {}; // expected-error {{weak and unowned Block captures require}}
  void (^duplicate)(void) = ^[weak object, strong object] {}; // expected-error {{a variable may appear only once}}
  void (^missing)(void) = ^[strong nonexistent] {}; // expected-error {{must name a local variable}}
  void (^const_move)(void) = ^[move immutable] {}; // expected-error {{requires a mutable, nonvolatile value}}
  int resource __attribute__((objc_resource(close=capture_close_checked, invalid=-1))) = 7;
  void (^resource_copy)(void) = ^[resource] {}; // expected-error {{resource Block capture must use the move modifier}}
  void (^resource_implicit)(void) = ^{ capture_use(resource); }; // expected-error {{requires an explicit move Block capture}}
  int throwing __attribute__((objc_resource(close=capture_close_throwing, invalid=-1))) = 8;
  void (^throwing_move)(void) = ^[move throwing] {}; // expected-error {{requires a statically nonthrowing cleanup}}
  int arbitrary __attribute__((cleanup(capture_arbitrary_cleanup))) = 0;
  void (^arbitrary_move)(void) = ^[move arbitrary] {}; // expected-error {{arbitrary cleanup storage cannot be transferred}}
  (void)weak_bad; (void)unowned_bad; (void)duplicate; (void)missing;
  (void)const_move; (void)resource_copy; (void)resource_implicit;
  (void)throwing_move; (void)arbitrary_move;
}
__attribute__((objc_root_class))
@interface CaptureHandler
- (void)setHandler:(void (^)(void))handler;
- (void)touch;
@end
@implementation CaptureHandler
- (void)setHandler:(void (^)(void))handler { (void)handler; }
- (void)touch {}
- (void)install {
  [self setHandler:^[weak self] {
    guard let self = self else { return; }
    [self touch];
  }];
  [self setHandler:^[unowned self] { [self touch]; }];
  [self setHandler:^[strong self] {}]; // expected-warning {{capturing 'self' strongly}} expected-note {{block will be retained}}
}
@end
