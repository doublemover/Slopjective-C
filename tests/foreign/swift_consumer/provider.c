#include "api.h"
#include <stdlib.h>

struct ObjC3SwiftToken { int value; };
static int Live;
int objc3_swift_value(int value) { return value + 1; }
int objc3_swift_read(const int *value) { return *value; }
ObjC3SwiftAmount objc3_swift_amount(int value) { return value; }
ObjC3SwiftToken *objc3_swift_create(int value) {
  ObjC3SwiftToken *token = malloc(sizeof(*token));
  if (!token) abort();
  token->value = value;
  ++Live;
  return token;
}
int objc3_swift_token_value(const ObjC3SwiftToken *token) { return token->value; }
void objc3_swift_destroy(ObjC3SwiftToken *token) { --Live; free(token); }
int objc3_swift_live(void) { return Live; }
int objc3_swift_unavailable(void) { return 99; }
