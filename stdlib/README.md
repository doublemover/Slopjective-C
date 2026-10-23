# Objective-C 3.0 standard library

The native Linux library uses Clang header modules. Compile in Objective-C 3.0
mode with `-fobjc-version=3 -fobjc-runtime=gnustep-2.2 -fblocks -fmodules`, add the
SDK's `include` directory, and import the canonical module you need:

```objective-c
@import objc3.text;
@import objc3.errors;

int example(Optional(int) value) {
  Result(int, int) result = okOr(value, ^int { return 7; });
  return isOk(result);
}
```

Link runtime calls with `libobjc3_runtime_services.a`, the C++ standard library,
threads and the SDK's GNUstep runtime. The SDK contains the module map, public
headers, module inventory and a separately compiled runnable qualification.

[The inventory](module_inventory.json) records each canonical capability,
version, implemented surface and remaining obligations against the
[standard-library contract](../spec/STANDARD_LIBRARY_CONTRACT.md). Module version
2 is a hard cutover from the removed `module`/`fn` frontend. Those source files,
fake integer helpers and package metadata are no longer library inputs.

`objc3.core`, `objc3.errors`, `objc3.text`, `objc3.collections` and
`objc3.concurrency` are native imports. The development `objc3.system` module is
available under `-fobjc3-profile=strict-system` and supplies Linux resource
helpers. The partial native `objc3.keypath` module supplies typed paths and
immutable metadata; its development version is 2.1.0. Module presence does not claim a
complete Core or Strict profile.

Text uses validated UTF-8 and explicit byte/scalar lengths; embedded NUL is data.
Prefixes count Unicode scalars. Concrete and generic collections use opaque
runtime identities, explicit ownership and status-returning mutation/iteration.
Public declarations live in `native/objc3c/src/runtime/public`; the higher-level
module headers live in `stdlib/include/objc3`.

`okOr(value, factory)` and `orThrow(value, factory)` infer the Optional payload,
evaluate the input once, and evaluate the factory expression only for None.
`orThrow` is a throwing Block call: use `tryValue`, `tryOptional` or `tryForce` as
for any checked throwing call. The factory returns `id<Error>` for `orThrow`;
`okOr` preserves the factory's concrete error type and lowers directly through
native matching, so a success path does not copy an inactive C++ callable.
`orThrow` supports payloads admitted by the compiler's Block call ABI; unsupported
types are diagnosed.

Strict profiles also check that error-factory results statically conform to
`Error`, including inherited protocols, class adoption and categories. Native
checking preserves the factory expression and its value/ownership recipe; it
introduces no runtime wrapper. Compatibility helpers retain their concrete error
types without claiming the constrained Core library contract.

The development headers also provide `resultFromStatus(status, successValue,
makeError)` for zero-success status APIs. The status runs once; the success
expression runs only for zero, and the error factory is constructed/called only
for nonzero with the original status. `statusOrThrow(status, makeError)` bridges
void-success operations to checked `id<Error>` propagation. `resultFromErrorOut(success, successValue, errorValue, makeError)` bridges
BOOL/error-out APIs: it runs the operation before capturing outputs, ignores
error storage on success, and lets the domain factory normalize even a missing
error on failure. It accepts NSError-style object slots without requiring a
Foundation dependency in the library. Domain-specific factories retain the API's error meaning. These additions require the next SDK
build and installed validation; the current released archive predates them.

The concurrency module exposes the shared task/frame, group, cancellation,
executor and serial actor ABI. Include `<objc3c/runtime/objc3_task.hpp>` explicitly
for the C++ coroutine adapter. Public async/await and actor language capabilities
remain reserved until compiler lowering and isolation are complete.

The development `objc3.system` header exposes `objc3_file_descriptor` and
`objc3_heap_buffer` with native cleanup, family and sentinel contracts. Descriptor
helpers adopt raw ownership explicitly, duplicate a borrowed descriptor into a
new owner, and close once with POSIX status/errno behavior. Close is not retried
after EINTR and is not declared nonthrowing because it is a cancellation point.
Heap-buffer helpers allocate, adopt and free ordinary libc storage. Adoption is
explicitly unsafe: the caller promises ownership and the correct deallocator.

```objective-c
@import objc3.system;

void allocate(void) {
  @resource(objc3_heap_buffer_free, invalid: (void *)0)
  objc3_heap_buffer buffer = objc3_heap_buffer_allocate(64);
  if (buffer == (void *)0) return;
  /* Use buffer. Scope exit frees it. */
}
```

These headers and their source/PCH/runtime fixtures are authored but unvalidated.
The installed gate is wired for C17/C++20/C++23, ARC/MRC, O0/O2, profile rejection
and ownership diagnostics. The compiler source now also implements native
`withLifetime(owner) { ... }` and `keepAlive(owner);` with strong precise owners
under ARC and MRC. These statements need no library import; `objc3.system`
imports retain their Strict System gate. Borrowed-pointer contracts and escape
checking are authored in the native compiler; remaining alias/diagnostic boundaries
and complete Strict System conformance remain outstanding. The active SDK predates this implementation.

Run the installed integration gate with:

```sh
python3 -B SDK/share/objc3c/source-build/qualify_stdlib.py \
  --sdk-root SDK --output-dir out/native-validation/stdlib
```

Builds and tests use disposable directories under `TMPDIR`; receipts and logs
remain in the requested output directory.

The development `objc3.keypath` module now exposes native typed property literals,
get/set applications, readonly conversion and immutable metadata. Its compiler
and runtime slices are authored; remaining traversal/value boundaries are
recorded in the inventory. Structured runtime development now includes parent
cancellation propagation, canonical main/global destinations and retained named
executor lookup. The global destination has four workers; ordinary explicitly
created executors keep their serial contract. These new slices await the
coordinated validation batch.

The authored key-path 2.1 source adds record-field writeback, exact readonly
narrowing, opaque descriptor signatures, nested nullability identity and bounded
readonly traversal through nullable receivers when the final value is a nullable
object or Block. Native application snapshots keep owned pointer operands alive
through later operand evaluation and unwind. Scalar/aggregate or nonnull results
through nullable intermediates and writable nullable chains remain diagnosed.
The current installed SDK does not establish validation of these additions.
