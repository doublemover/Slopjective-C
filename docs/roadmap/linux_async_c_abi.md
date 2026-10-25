# Linux shared async frame and result ABI

The common lazy frame carrier and typed completion contract use task service
ABI 4 and frame ABI 1. The current native extension is ABI 21 with semantic
interface schema 2.1. Compilers, headers, objects, PCH files, semantic interfaces
and packages are delivered together; the task header requires the qualified
compiler's type-identity builtin.

Native [C async definitions](linux_native_c_definitions.md) now implement
functions, methods and Blocks with frame-resident values, owned results/errors,
cleanup and child awaits. [Task/group handle adapters](linux_async_handle_joins.md)
bridge existing retained handles into those definitions and C++ lazy tasks. The
current SDK passes all 33 installed gates, including shared-frame source-free
consumers, native C/C++ execution and debugger checks. The
[current receipt](../support/linux_async_handle_join_acceptance.json) records
that delivery and its fresh-distribution status. Public async/actor predicates
and full profile conformance remain reserved.

The snapshots and interrupted acceptance below describe their October 21 inputs.
They are historical evidence, not the current release checklist. The
[Linux roadmap](linux_forward_development.md) records remaining implementation.

## Historical SDK delivery — 2026-10-21

The October 21 local SDK was rebuilt from `3fe616aa2` in 1,431 seconds and activated
with seal `0098f5c514798351a5900755a85ba8508e96c6fe51e06d31af01bfec0a419703`.
It includes the actor fairness, autorelease boundary and package CLI fixes.
The [delivery receipt](../support/linux_sdk_refresh_acceptance.json) records the
archive path, SHA-256, source identity and installed checks.

All 8 GNU runtime tests, 24 frontend CTests, 37 installed driver/embedding
commands, 13 native integration nodes and 23 installed suites passed. After
archive extraction, 13 additional commands exercised the shipped compiler,
package graph builder and publish/install/consume CLI, including environment
and default SDK selection. The installed payloads match their source owners,
and the SDK seal verified before and after those checks.

Activate this installation with:

```sh
. out/toolchains/linux-sdk/activate.sh
```

The superseded SDK and archive were removed, reclaiming approximately 1.88 GiB.
The development build and compilation database now use the current installation.
This is a local SDK delivery; external release upload and fresh-distribution
acceptance remain separate from the host checks recorded here.

## Frame ownership

`objc3_async_frame` carries a context pointer, resume/destruction callbacks and
the declared result type. It exposes no C++ promise layout. A C provider can
return this carrier to a C++ caller, and a C++ coroutine can export the same
carrier to C.

```cpp
#include <objc3c/runtime/objc3_task.hpp>

objc3_async_frame frame = make_frame_in_c();
auto pending = objc3::task<int>::from_native(&frame);
auto running = std::move(pending).start(executor);
```

Import consumes the carrier only after validating its type. The inverse
`release_native()` transfers ownership to C. `task_handle<T>::adopt_native()`
and its `release_native()` transfer one owned running-task reference.

`objc3_async_frame_start` and `objc3_async_frame_start_child` clear the carrier
only on success. Failed starts preserve its owner. Disposing an unstarted frame
clears the carrier before invoking destruction, so reentrant disposal is safe.
An all-zero carrier is empty. C source must move these carriers explicitly;
copying their bits does not create a second owner.

The existing task scheduler owns successful starts. It serializes resumes,
preserves actor destinations, wakes cancellation and destroys frames exactly
once. A private child can inherit its parent's destination through the same C
API used by C++ awaits.

## Typed completions

`__builtin_objc3_type_identity((T *)0)` produces a canonical native type identity
without evaluating its operand. C and C++ aliases of the same native type agree.
Top-level qualifiers are omitted; owned object and Block representations supply
their own ownership. Pointer operands for Objective-C retainable types must
spell their ownership explicitly.

A result descriptor combines that identity with its size, alignment and value,
object, Block or void representation. Runtime validation compares identity bytes
and layout rather than descriptor addresses or copy-function addresses.
Private C++ types, including local records and closures, additionally carry a
per-type identity domain. Distinct private types remain distinct even if their
native mangled spellings coincide. External C/C++ identities have no domain.
Providers must agree on complete named-type definitions, as with a native C ABI;
this descriptor does not establish compatibility between conflicting definitions.

Task start copies the declared identity before scheduling work. Successful
completion must match it before any payload copy runs. Completion also owns its
identity independently of the producer's frame. Zero, false and nil successes
retain payload presence; void success has a typed descriptor and no payload.

`objc3_task_get_result_type` makes a declared type available before completion.
`objc3_task_copy_result` checks type and layout before invoking the consumer's
copy operation outside runtime locks. Failed copies leave no newly owned value.
Cancellation and failure return separate statuses and preserve the task's
inspectable completion and declared result type.

Objects and Blocks use a shared owned-pointer payload. Copying retains an object
or heap-copies a Block; destruction releases that independent owner. C copy-out
returns one owned pointer. C++ reads retain their existing autoreleased +0
contract. MRC code must retain a read value before a later suspension drains its
execution-slice pool.

Checked Objective-C errors and native C++ exceptions retain their separate
transport behavior. C can inspect a failed completion without interpreting a
private C++ exception payload. A C++ caller continues to receive native exception
rethrowing through the established task bridge.

## Source-free frame interfaces

Exported frame factories use the installed SDK declaration in canonical
interfaces. The frontend admits that declaration only after checking the exact
public header bytes, native record layout and callback signatures. The required
`objc3.sdk-runtime-frames.v4.<header-sha256>` semantic binds importers to that
header. Internal reparsing reconstructs the trusted declaration without retaining
the provider's filesystem paths. Modified headers, packed layouts and shortened
enums fail before publication; unrelated external types still need resolved
interface dependencies.

The immutable package gate covers C17 frame factories and C++ coroutine frame
exports after provider sources are removed. Both paths check relocation, complete
compile reuse and the exact SDK header contract.

## Qualification

Development qualification passes 20 mixed-language configurations and 114
commands, including ten resumed-frame debugger cases. C providers compile as
C17, Objective-C providers as Objective-C 3.0 (C17 base mode), and consumers as C++20/23 or
Objective-C++ (C++20/23 mode) at O0/O2 with all ARC/MRC provider-consumer pairs.

Execution covers moves, abandonment, failed starts, reentrant destruction,
typed handle adoption, zero results, records, complex values, 256-byte-aligned
values, void, objects, Blocks, nil, private child awaits, actor destinations and
worker cancellation. Mismatched types reject before copying. Mutable starting
descriptors do not change stored type identities; failed consumer copies and
reentrant copy callbacks preserve task ownership and avoid runtime locks.

The existing async regression suite passes all 16 configurations and 104
commands. ASan/LSan/UBSan and TSan pass 40 additional bridge configurations with
both services and clients instrumented. Sanitizer libraries come from the pinned
LLVM resource directory and are identified in the receipt; GNU libobjc is loaded
as shipped. Value-only C/C++ bridge clients are checked to link without libobjc.

## SDK snapshot at the requested pause — 2026-10-21

Development, builds, tests and acceptance were temporarily stopped at the
user's request. Validation has since been reauthorized for the four open P2/P3
issues. [PR #8274](https://github.com/doublemover/Slopjective-C/pull/8274)
contains the ongoing work. The SDK snapshot below was built from
`e6412ec2b776f2e0a83f63518a12f775df7bbf22`; later runtime and tooling fixes must
not be attributed to that installation.

The final host SDK build completed successfully before the stop. Its receipt
reports all 23 installed package gates and the installed driver/embedding gate
passing. That includes the new C frame package path, which was a failure in an
earlier candidate and is fixed in this implementation. That binary snapshot has
since been replaced by the SDK delivery above; its interrupted acceptance remains
historical evidence.

| Completed coverage                     | Recorded result                                          |
| -------------------------------------- | -------------------------------------------------------- |
| Compiler foundation                    | 219 lit tests passed; 3 unsupported; 57 AST tests passed |
| Frontend and runtime                   | 24 frontend CTests and 8 GNU runtime tests passed        |
| Native integration                     | 13 selected validation nodes passed                      |
| Installed driver/embedding             | 37 commands passed                                       |
| Installed package gates                | All 23 passed, including source-free C17 and C++ frames  |
| Shared C/C++ async bridge              | 20 configurations, 114 commands and 10 LLDB cases passed |
| Existing native async bridge           | 16 configurations and 104 commands passed                |
| Immutable async packages               | 12 cases and 36 executions passed                        |
| Tooling                                | 327 tests passed, 1 skipped; 33 subtests                 |
| GitHub checks on implementation commit | Policy, tooling and documentation passed                 |

The final SDK build took 1,458 seconds. Its exact identities are:

```text
implementation: e6412ec2b776f2e0a83f63518a12f775df7bbf22
LLVM source: llvmorg-23.1.3-0d261d1ca552-655bdb15865c-fd761312
SDK input: c39d60a34fb8ee21f7349101a09f631940b38ff098d069de6b6eaee49f810c70
SDK seal: 10417938178eb64c0b8908e399b102124342713f3773b76a02eb856b51991fda
archive SHA-256: b9e310e1267e37d4c8df4d8d87be1fa8555ddb2ca2d45f9d6226f49968a07582
archive bytes: 429953314
```

The archive is
`out/toolchains/sdk-cache/llvmorg-23.1.3-0d261d1ca552-655bdb15865c-fd761312-linux-x86_64-10417938178eb64c.tar.zst`.
The build and installed-gate results are in
`out/toolchains/linux-sdk-builds/<SDK input>/release.json`.
Native integration evidence is in
`out/native-validation/linux-sdk-20261021T160115587216Z/summary.json`.
These generated outputs stay outside Git; the identities above identify the
local results rather than claiming the archive has been uploaded to GitHub.

### Historical interrupted acceptance

The final acceptance run was stopped during `native-async-sanitizers`, before
that suite produced its final receipt. Partial logs are retained, but this
attempt has no sanitizer-suite PASS. Earlier development bridge sanitizer runs
passed 40 configurations, and the preceding installed SDK passed 16 native
async sanitizer configurations. Those results do not establish sanitizer
acceptance for the final archive above.

An earlier coroutine sanitizer run failed to link because its build driver
omitted `services/objc_task_error.cpp`, which now supplies shared object/Block
reference operations. Commit `62fcbbfac` corrected that driver; focused ARC
ASan/UBSan and MRC TSan checks passed. Full qualification for that historical
archive was still pending at the pause. Commit `e6412ec2b` then normalized the two C17 fixture JSON
files before the successful final SDK build.

That stopped run remains incomplete evidence for its original archive. Later
source/runtime/SDK acceptance is recorded separately by the current receipt; it
does not relabel this older artifact. At interruption, the remaining checklist
was:

1. Complete the final archive's 114 sanitizer configurations and the checked
   ABI, regression and post-hop debugger checks.
2. Run ordinary-user archive consumers on fresh Debian 13 and Ubuntu 26.04.
3. Record latency against the existing budgets, SDK-bound frontend CTests and
   a release-cache reuse witness.
4. Publish the acceptance and latency receipts, then refresh the current
   release documentation to ABI 20. Existing ABI 19 release receipts describe
   their earlier qualified snapshots and have not been relabeled.

Local interrupted-run status and logs remain under `out/async-c-abi/acceptance/`;
`out/async-c-abi/stopped-at-user-request.json` records the stopped process tree.
No acceptance finalizer or automatic closeout is running.

### Subsequent compiler implementation

The later native C definition milestone supplies AST, Sema and code generation
for functions, methods and Blocks, including suspension-safe ownership and
checked errors. Recognized spawn APIs, admitted isolation entry forms and native
task/group joins are implemented and covered by the current SDK gates. These
bounded surfaces do not promote public async/actor predicates or complete
profiles. Automatic structured-scope finalization, native C group-result
iteration and other remaining forms are tracked in the current Linux roadmap.
Native Linux AArch64 implementation and qualification also remain open; the
recorded native-host evidence is Linux x86-64.

### Workspace state at the historical pause

The 32 GiB filesystem is now approximately 29% used, with 22 GiB available.
Cleanup removed 33 superseded SDK archives, the obsolete Linux 22 checkpoint,
all 21 old checkout copies, legacy package/build folders, superseded test
outputs, crash reproducers and compiled probes. The last housekeeping pass
also removed 35 obsolete toolchain output folders and 33 merged or duplicate
local branches. Historical binary rollback artifacts are not retained.

The active checkout is the sole worktree. Five source heads outside its history
were preserved and pushed under `unintegrated-source/` branches named
`concurrency-development`, `naming-review`, `packages-development`, `spi-review`
and `task-bridge-review`. These preserve source commits without full checkout
copies. Other unmerged and platform references remain untouched.

The retained build state consists of the active LLVM source/build tree, current
SDK archive and installation, current frontend build, active runtime sources
and current receipts. Final build receipts and interrupted acceptance logs
remain available. Machine-local housekeeping records are in `out/session/`.

Loose authored experiments, scripts, patches and notes are under `scratch/`.
The committed `.gitignore` covers that directory, all generated `out/` contents,
pytest caches, local clangd configuration, compilation databases and CMake user
presets. The workspace root contains only the checkout, managed cloud tools
and the platform's `shared` and `library-files` directories.

Local development settings cap ccache at 1 GB, select the current patched
clangd with two workers and background indexing disabled, repair the editor's
retired-staging paths and provide a frontend/runtime-only `linux-dev` CMake
user preset. Four compile jobs and one link job match the four-CPU quota and
16 GiB memory limit. See the
[development setup](linux_forward_development.md#architecture-and-release-performance).
The frontend build has now been reconfigured against the current SDK with
testing enabled, and the runtime regression targets have been built and run.

Tests and validation have resumed; background indexing remains disabled.
At cleanup, thirty-three old compiler/test process entries were confirmed to be zombies,
with no live compiler/test processes and no resident memory. They are adopted
by PID 1 (`tail`); reaping those records requires proper init handling rather
than termination of an active project process. No instance restart was made.

Subsequent tooling now routes compiler temporaries through private RAM-backed
`TMPDIR` and scopes sanitizer executables, objects and service archives to
temporary directories cleaned after each case. Durable logs, hashes and receipts
remain on disk, as do the incremental compiler build, SDK and compilation cache.
Cleanup/report-preservation regression coverage now passes as part of the
current tooling suite. The delivered SDK now includes these changes and the
four-issue fixes; the paused snapshot above remains a record of the earlier build.
