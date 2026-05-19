# Runtime

Runtime behavior is implemented only where the capability matrix links a live
runtime test or source evidence. Reserved runtime concepts remain unavailable.

Strict runtime ownership is split as follows:

- `native/objc3c/src/runtime/public/objc3_runtime_api.h` owns exported C entry
  points for image registration, selector lookup, checked i32 dispatch, the
  plain i32 dispatch entrypoint, testing snapshots, and reset.
- `native/objc3c/src/runtime/public/objc3_runtime_result.h` owns registration
  and dispatch status codes plus the checked dispatch result payload.
- `native/objc3c/src/runtime/public/objc3_runtime_result_contract.h` owns the
  internal mapping from dispatch statuses to public diagnostic code/message
  payloads.
- `native/objc3c/src/runtime/errors/` owns Objective-C error bridging and catch
  filter helpers.
- `native/objc3c/src/runtime/images/` owns image descriptor validation and
  runtime-owned registration identity handling.
- `native/objc3c/src/runtime/strings/` owns borrowed runtime string storage used
  by the current public snapshot surfaces.

The public behavior claim remains the strict dispatch diagnostic row. The
broader runtime module split and public C API result surface keep ownership
clear without claiming more language support than the evidence map proves.
