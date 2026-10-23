@import objc3.system;
#include <assert.h>
#include <errno.h>
#include <fcntl.h>

#ifdef INVALID_SYSTEM_LEAK
void missing_cleanup(void) {
  objc3_file_descriptor value = objc3_file_descriptor_unsafe_adopt(3);
  (void)value;
}
#elif defined(INVALID_SYSTEM_MOVE)
void moved_use(void) {
  @resource(objc3_file_descriptor_close, invalid: -1)
  objc3_file_descriptor value = objc3_file_descriptor_unsafe_adopt(3);
  objc3_file_descriptor owner = take(value);
  objc3_file_descriptor_close(owner);
  (void)dup(value);
}
#elif defined(INVALID_SYSTEM_FAMILY)
static void wrong_close(int value) __attribute__((nothrow));
static void wrong_close(int value) { (void)value; }
@requires_cleanup(wrong_close) @resource_family(Unrelated) @invalid(-1)
typedef int WrongHandle;
void wrong_family(void) {
  objc3_file_descriptor value = objc3_file_descriptor_unsafe_adopt(3);
  wrong_close(value);
}
#else
static void observe_descriptor(int value, int *output) __attribute__((nothrow));
static void observe_descriptor(int value, int *output) { *output = value; }
static objc3_file_descriptor transfer_descriptor(int raw) {
  @resource(objc3_file_descriptor_close, invalid: -1)
  objc3_file_descriptor value = objc3_file_descriptor_unsafe_adopt(raw);
  return take(value);
}
static void system_resources(void) {
  int descriptors[2];
  assert(pipe(descriptors) == 0);
  int copied = -1;
  {
    @resource(objc3_file_descriptor_close, invalid: -1)
    objc3_file_descriptor input = transfer_descriptor(descriptors[0]);
    @resource(objc3_file_descriptor_close, invalid: -1)
    objc3_file_descriptor output = objc3_file_descriptor_unsafe_adopt(descriptors[1]);
    @resource(objc3_file_descriptor_close, invalid: -1)
    objc3_file_descriptor copy = objc3_file_descriptor_duplicate(input);
    assert(copy >= 0);
    /* Observation passes by value; ownership stays in copy. */
    observe_descriptor(copy, &copied);
    assert(write(output, "x", 1) == 1);
    char byte = 0;
    assert(read(input, &byte, 1) == 1 && byte == 'x');
    @resource(objc3_heap_buffer_free, invalid: (void *)0)
    objc3_heap_buffer buffer = objc3_heap_buffer_allocate(32);
    assert(buffer != (void *)0);
    ((unsigned char *)buffer)[0] = 42;
    reset(buffer, objc3_heap_buffer_allocate(64));
    assert(buffer != (void *)0);
    discard(buffer);
    assert(buffer == (void *)0);
  }
  errno = 0;
  assert(fcntl(descriptors[0], F_GETFD) == -1 && errno == EBADF);
  errno = 0;
  assert(fcntl(descriptors[1], F_GETFD) == -1 && errno == EBADF);
  errno = 0;
  assert(fcntl(copied, F_GETFD) == -1 && errno == EBADF);
  @resource(objc3_file_descriptor_close, invalid: -1)
  objc3_file_descriptor missing = objc3_file_descriptor_duplicate(-1);
  assert(missing == -1);
}
int main(void) { system_resources(); return 0; }
#endif
