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
`objc3.concurrency` are native imports. `objc3.keypath` and `objc3.system` remain
unimplemented and their imports fail. Module presence does not claim a complete
Core or Strict profile.

Text uses validated UTF-8 and explicit byte/scalar lengths; embedded NUL is data.
Prefixes count Unicode scalars. Concrete and generic collections use opaque
runtime identities, explicit ownership and status-returning mutation/iteration.
Public declarations live in `native/objc3c/src/runtime/public`; the higher-level
module headers live in `stdlib/include/objc3`.

`okOr(value, factory)` and `orThrow(value, factory)` infer the Optional payload,
evaluate the input once, and evaluate the factory expression only for None.
`orThrow` is a throwing Block call: use `tryValue`, `tryOptional` or `tryForce` as
for any checked throwing call. The factory returns `id<Error>` for `orThrow`;
`okOr` preserves the factory's concrete error type. The helpers support payloads
admitted by the compiler's Block call ABI; unsupported types are diagnosed.

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

Run the installed integration gate with:

```sh
python3 -B SDK/share/objc3c/source-build/qualify_stdlib.py \
  --sdk-root SDK --output-dir out/native-validation/stdlib
```

Builds and tests use disposable directories under `TMPDIR`; receipts and logs
remain in the requested output directory.
