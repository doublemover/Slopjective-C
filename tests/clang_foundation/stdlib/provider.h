#ifndef OBJC3_STDLIB_TEST_PROVIDER_H
#define OBJC3_STDLIB_TEST_PROVIDER_H
@import objc3.core;
@import objc3.errors;
@import objc3.text;
@import objc3.collections;
@import objc3.concurrency;

#ifdef __cplusplus
extern "C" {
#endif
int stdlib_records(void);
int stdlib_tasks(void);
Result(int, int) stdlib_optional(Optional(int) value, int (^factory)(void));
Result(int, int) stdlib_status(int status, int value, int (^factory)(int));
void stdlib_status_throwing(int status, id<Error> (^factory)(int)) throws(id<Error>);
int stdlib_throwing(Optional(int) value, id<Error> (^factory)(void))
    throws(id<Error>);
#ifdef __cplusplus
}
#endif
#endif
