# Linux forward development

The active product is the Clang-only Linux x86-64 SDK. The
[acceptance receipt](../support/linux_async_blocks_acceptance.json) identifies
its exact archive, implementation, installed execution and fresh-distribution
evidence. Tool versions come from [the manifest](../../toolchains/llvm/manifest.json);
public language claims come from [the capability authority](../../schemas/objc3-language-capabilities.json).

## Public synchronous core

The [public Optional/Result contract](linux_public_carriers.md) and
[synchronous error contract](linux_public_errors.md) now have native compiler
syntax in Objective-C17 and Objective-C++20/23. Extension ABI 19 requires a full
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

- [Named module SPI](linux_module_spi.md) binds native partitions and exact-provider
  grants through qualified lookup, source recipes and immutable consumers.
  Re-exports do not forward privileges.

- [Module-owned native names](linux_module_owned_names.md) isolate functions,
  globals, classes, protocols, categories and concrete private template bindings
  across providers. Canonical imports and Linux LLDB preserve the native binding.
- [Structured runtime services](linux_structured_runtime.md) expose task groups,
  nested cancellation, completion polling and serial actor mailboxes through the
  installed C ABI. Threaded and sanitizer qualification covers runtime ownership
  and cleanup.
- [Resumable task bridge](linux_task_coroutines.md) connects native C++20/23 and
  Objective-C++ coroutine frames to the public task ABI, with queued awaits,
  cancellation wakeup, retained results and native frame cleanup.
- [Native async function development](linux_native_async.md) connects logical
  results and canonical async effects to Objective-C++20/23 task frames through
  reserved compiler spellings. [Owned async results](linux_async_owned_results.md)
  retain objects and copy Blocks through nested awaits, canonical ARC/MRC task
  carriers and source-free providers. [Checked async errors](linux_async_checked_errors.md)
  add owned deferred Error objects, checked awaits and suspending handlers.
  [C++ async callables](linux_async_cxx_callables.md) extend this compiler work to
  member functions, virtual calls, member pointers and operators, with bounded
  source-free aggregate method interfaces. [Checked async value results](linux_async_value_results.md)
  add records, complex values and member pointers without constructing an absent
  result; direct automatic Optional initialization preserves C++ payload lifetimes.
  [Async Objective-C methods](linux_async_objc_methods.md) add dynamic, protocol,
  superclass and direct messages, owned lazy receivers, checked errors and
  source-free method interfaces. [Async Objective-C++ Blocks](linux_async_blocks.md)
  add owned suspended captures, checked Block calls, PCH bodies and source-free
  Block aliases and factories. Public async/await remains reserved.

## Next compiler milestones

The [shared C/C++ async frame and result ABI](linux_async_c_abi.md) introduces
the extension ABI 20 development cutover and service ABI 4. It removes C++
promise layout from the C frame boundary and copy-function identity from result
matching. Its Objective-C17 providers use explicit resume callbacks; native
Objective-C async syntax remains the next compiler step.

The [task destination bridge](linux_task_hops.md) supplies actor-bound resumes,
explicit executor/actor hops and inherited child destinations. Compiler isolation
and the public language contract still require the following work.

1. Implement a native Objective-C async milestone with a shared C/C++ frame and
   typed completion contract. Keep C++ promise storage private and preserve
   validated result ownership, checked errors, native exceptions and cancellation
   across language boundaries. Use one hard cutover for that ABI and the native
   Objective-C lowering of functions, methods and Blocks. Preserve cleanup and
   owned values across execution-slice autorelease pools.
   Align public await-required calls and library-recognized task spawning with
   the language contract before promoting the capability. The qualified
   Objective-C++ stage already includes checked errors, methods and async Blocks.
   Require Objective-C17 and Objective-C++20/23 canonical imports, source-free
   packages, O0/O2 execution, debugger values and instrumented services/clients.
2. Add compiler actor isolation and awaited executor hops. Preserve isolation in
   callable types and canonical interfaces, diagnose unsynchronized access, and
   retain mailbox serialization, logical identity and cleanup through nested
   calls. Promote the language capability after native compiler qualification.

## Architecture and release performance

AArch64 requires a separate native host/runtime/ABI milestone. Cross-compilation
and emulation can probe implementation choices; support requires a real native
host running the installed archive, ownership, interfaces, editor and debugger
checks. The current qualified architecture remains x86-64.

Keep retained build timings, compiler concurrency limits, content-addressed
reuse and archive identities as release inputs. Publication records a verified
qualified archive before reclaiming its staging prefix; interrupted installation
resumes from that archive without recompiling or repeating qualification.

Release reuse hashes foundation contents and immutable configure inputs. Mutable
staging inventories do not participate: reclaiming a duplicate foundation stage
preserves reuse. Source, bootstrap and build configuration changes still
invalidate the release identity.

Frontend CTests are built and executed one target at a time. Reclaim only the
passed generated test executables; preserve failures, compiler binaries, objects
and libraries. This keeps the complete test inventory affordable as archives
accumulate in the 32 GiB workspace.

Archive installation verifies a complete staged extraction even when the target
seal already exists. Reserve space for that extraction.

Development uses hard cutovers. Retain the current SDK archive, its installed
prefix and the active compiler source/build tree. Remove superseded SDK archives
and obsolete source/build checkpoints after a cutover; do not accumulate local
rollback snapshots. Git and the pinned source/patch inputs preserve development
history. Historical receipts describe completed runs without requiring their
binary artifacts to remain on the development instance.

Keep checks proportional to the changed behavior and reuse completed results
for unchanged inputs. Do not repeat broad qualification for rollback history or
make it the default next milestone. The current user-requested pause stops all
tests and qualification; resuming those requires a subsequent instruction.
