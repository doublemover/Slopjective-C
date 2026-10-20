# Linux forward development

The active product is the Clang-only Linux x86-64 SDK. The
[acceptance receipt](../support/linux_public_surface_acceptance.json) identifies
its exact archive, implementation, installed execution and fresh-distribution
evidence. Tool versions come from [the manifest](../../toolchains/llvm/manifest.json);
public language claims come from [the capability authority](../../schemas/objc3-language-capabilities.json).

## Public synchronous core

The [public Optional/Result contract](linux_public_carriers.md) and
[synchronous error contract](linux_public_errors.md) now have native compiler
syntax in Objective-C17 and Objective-C++20/23. Extension ABI 13 requires a full
rebuild of objects, PCH files, semantic interfaces and packages. The public
feature predicates describe the bounded Linux x86-64 GNUstep 2.2 compatibility
profile; async/await and actors remain reserved.

The release qualification carries public syntax through the installed ownership,
call, runtime, typed-error, direct-dispatch, recursive-value, capture, array,
record, heap and C++ interface suites. Explicit checked calls are required for
evaluated throwing calls, including nested arguments and receivers. Local ARC
values preserve native strong, weak, unsafe and Block ownership; managed MRC
transfer and unsupported storage are diagnosed. Native exceptions retain native
cleanup and unwinding behavior.

The implemented foundations have separate bounded contracts:

- [Managed calls](linux_managed_call_abi.md),
  [runtime calls](linux_runtime_call_abi.md),
  [typed errors](linux_managed_typed_errors.md) and
  [GNUstep 2.2 direct dispatch](linux_direct_dispatch.md).
- [Recursive values](linux_recursive_carriers.md),
  [owned captures](linux_owned_block_captures.md),
  [arrays](linux_managed_arrays.md), [records](linux_managed_records.md) and
  [heap storage](linux_heap_storage.md).
- [C++ semantic interfaces](linux_cxx_interfaces.md),
  [explicit source packages](linux_source_packages.md) and
  [immutable packages](linux_immutable_packages.md).
- [Explicit ownership projections](linux_ownership_projections.md) connect
  closed ARC and MRC C APIs, including source-free packages and transitive
  consumer-mode re-exports.
- [Public module surfaces](linux_public_module_surfaces.md) select native root
  declarations, reject private type leaks and preserve dependent build reuse
  across private implementation edits. Source manifests v4 and binary packages
  v3 require explicit export policies. Semantic interfaces remain schema 2.

- [Public API availability](linux_api_availability.md) preserves native
  deprecation, unavailability and platform metadata through imports, ownership
  projections and source-free packages. Metadata edits invalidate dependents.

## Next implementation sequence

1. Extend the bounded public module API with privileged SPI and module-owned
   linker/runtime naming contracts. Native availability metadata is implemented. Native imports,
   qualified lookup, explicit export selection and immutable dependency
   identities are implemented. Registry transport is a separate concern.
2. Build structured task groups, cancellation and actor isolation on that
   ownership and module foundation. Require deterministic error/cleanup tests,
   native threaded execution and sanitizer evidence before feature promotion.

## Architecture and release performance

AArch64 requires a separate native host/runtime/ABI milestone. Cross-compilation
and emulation can probe implementation choices; support requires a real native
host running the installed archive, ownership, interfaces, editor and debugger
checks. The current qualified architecture remains x86-64.

Keep retained build timings, compiler concurrency limits, content-addressed
reuse and archive identities as release inputs. Publication records a verified
qualified archive before reclaiming its staging prefix; interrupted installation
resumes from that archive without recompiling or repeating qualification.
