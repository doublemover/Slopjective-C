typedef int Answer;

Answer answer(void) {
  return 42;
}

@protocol Error @end
typedef int CheckedFunction(int) __objc3_throws(id<Error>);
typedef CheckedFunction *CheckedCallback;

int checked_value(int value) __objc3_throws(id<Error>) { return value; }

CheckedCallback checked_callback(void) { return checked_value; }

int call_checked(CheckedCallback callback, int value) __objc3_throws(id<Error>) {
  return __objc3_try(callback(value));
}

typedef int (^CheckedBlock)(int) __objc3_throws(id<Error>);
CheckedBlock checked_block(void) {
  return ^int(int value) __objc3_throws(id<Error>) { return value; };
}

@protocol CheckedMessageProtocol
+ (int)messageValue:(int)value __objc3_throws(id<Error>);
@end

__attribute__((objc_root_class))
@interface CheckedMessages <CheckedMessageProtocol>
+ (Class<CheckedMessageProtocol>)messageClass;
@end

@implementation CheckedMessages
+ (Class<CheckedMessageProtocol>)messageClass { return self; }
+ (int)messageValue:(int)value __objc3_throws(id<Error>) { return value; }
@end

Class<CheckedMessageProtocol> checked_message_class(void) {
  return [CheckedMessages messageClass];
}
