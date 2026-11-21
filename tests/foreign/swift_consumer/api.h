#pragma once

typedef int ObjC3SwiftAmount __attribute__((swift_newtype(struct), swift_name("Amount")));
typedef struct ObjC3SwiftToken ObjC3SwiftToken;

int objc3_swift_value(int value) __attribute__((swift_name("projectedValue(_:)")));
int objc3_swift_read(const int * _Nonnull value) __attribute__((swift_name("projectedRead(_:)")));
ObjC3SwiftAmount objc3_swift_amount(int value) __attribute__((swift_name("makeAmount(_:)")));
ObjC3SwiftToken * _Nonnull objc3_swift_create(int value);
int objc3_swift_token_value(const ObjC3SwiftToken * _Nonnull token);
void objc3_swift_destroy(ObjC3SwiftToken * _Nonnull token);
int objc3_swift_live(void);
int objc3_swift_unavailable(void) __attribute__((unavailable("consumer lane control"), swift_name("unavailableValue()")));
