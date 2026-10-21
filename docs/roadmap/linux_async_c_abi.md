# Linux shared async frame and result ABI

Extension ABI 20 and task service ABI 4 introduce a common lazy frame carrier
and typed completion contract for C17, Objective-C17 and C++20/23. Rebuild
compilers, headers, objects, PCH files, semantic interfaces and packages together.
The task header requires the qualified compiler's type-identity builtin.

This milestone supplies the runtime boundary for native Objective-C async
lowering. Objective-C17 providers in this milestone implement resume callbacks
in C; the reserved async syntax still requires Objective-C++20/23. Public
async/await and actor capability predicates remain reserved.

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
C17, Objective-C providers as Objective-C17, and consumers as C++20/23 or
Objective-C++20/23 at O0/O2 with all ARC/MRC provider-consumer pairs.

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

## Progress at the requested pause — 2026-10-21

Development, builds, tests, acceptance and automatic closeout are stopped at the
user's request. [PR #8274](https://github.com/doublemover/Slopjective-C/pull/8274)
remains a draft. The implementation is committed through
`e6412ec2b776f2e0a83f63518a12f775df7bbf22`; this documentation update records the
handoff without changing compiler or SDK inputs.

The final host SDK build completed successfully before the stop. Its receipt
reports all 23 installed package gates and the installed driver/embedding gate
passing. That includes the new C frame package path, which was a failure in an
earlier candidate and is fixed in this implementation. The installed archive is
available locally; acceptance of that archive is incomplete.

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

### Interrupted and pending acceptance

The final acceptance run was stopped during `native-async-sanitizers`, before
that suite produced its final receipt. Partial logs are retained, but this
attempt has no sanitizer-suite PASS. Earlier development bridge sanitizer runs
passed 40 configurations, and the preceding installed SDK passed 16 native
async sanitizer configurations. Those results do not establish sanitizer
acceptance for the final archive above.

An earlier coroutine sanitizer run failed to link because its build driver
omitted `services/objc_task_error.cpp`, which now supplies shared object/Block
reference operations. Commit `62fcbbfac` corrected that driver; focused ARC
ASan/UBSan and MRC TSan checks passed. Full final-archive qualification of this
fix remains pending. Commit `e6412ec2b` then normalized the two C17 fixture JSON
files before the successful final SDK build.

The previously planned acceptance work is listed below as incomplete evidence,
not as an instruction to resume it. The user's latest direction prioritizes
hard cutovers and implementation over rollback retention and repeated broad
qualification:

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

### Subsequent compiler work

Native Objective-C17 async syntax lowering still needs its own AST/Sema/CodeGen
implementation for functions, methods and Blocks, with frame-resident values,
cleanup, checked errors and ownership across suspension. Removing the C++
language guard alone would not supply that implementation. Public await-required
calls, recognized child/detached/group spawning and compiler actor isolation
remain later milestones; public async/actor predicates stay reserved.

One unverified follow-up is to check type identities for same-named module-owned
Objective-C classes and protocols. Read-only inspection found that native type
mangling uses source names while module ownership supplies runtime names. No
reproducer was run, so this is an investigation item rather than a confirmed
defect. Native AArch64 qualification also requires a separate native host; this
milestone's host evidence is Linux x86-64.

### Workspace storage

At the pause the 32 GiB filesystem was 99% full, with approximately 472 MiB
available. The main consumers were 34 retained SDK archives (14 GiB), LLVM
source and build outputs (5.8 GiB), a preserved source checkpoint (2.7 GiB), the
installed SDK (1.5 GiB), earlier Linux packages (698 MiB) and native validation
evidence (544 MiB). These sizes are rounded and are not a complete filesystem
inventory.

The user then reaffirmed the hard-cutover policy. Superseded SDK archives and
the obsolete Linux 22 source/build checkpoint were removed: 33 archives plus
the checkpoint reclaimed approximately 16 GiB. Filesystem use fell to 46%,
with 17 GiB available; the remaining SDK cache occupies 412 MiB. The current SDK
archive and installed prefix, active compiler source/build tree and small
diagnostic receipts remain. Historical binary artifacts are no longer retained
for rollback. The temporary foundation RAM filesystem was released after the
final SDK completed. Future work follows the
[current-artifact retention policy](linux_forward_development.md#architecture-and-release-performance).

A further cleanup removed obsolete Linux packages, older validation runs,
superseded milestone outputs, failed-build bundles, old runtime build caches,
temporary test directories, 16 clean merged worktrees and compiled scratch
probes. It reclaimed another 3.75 GiB, leaving the filesystem 33% used with
approximately 21 GiB free. Authored probe source files, current SDK/build state,
final build receipts and interrupted acceptance logs remain. Five clean
worktrees with commits outside the current branch's history were retained.
No tests or qualification were resumed.

The final organization pass removed the obsolete ignored `tmp/` and `artifacts/`
build folders, more scattered binaries/logs and all five remaining checkout
copies. Their commits are preserved and pushed under `unintegrated-source/`
branches named `concurrency-development`, `naming-review`,
`packages-development`, `spi-review` and `task-bridge-review`. The active
checkout is now the only worktree.

Loose experiment sources, scripts and notes were consolidated under `scratch/`;
local handoff and cleanup records moved to `out/session/`. Both directories,
all generated `out/` contents and pytest caches are excluded by the committed
`.gitignore`, including on fresh clones. The organization pass reclaimed another
1.26 GiB, leaving filesystem usage at 29% with approximately 22 GiB available.
The workspace root now contains only the project checkout, managed cloud tools
and the platform's `shared` and `library-files` directories. Current SDK/build
state and authored experiment sources remain; tests and validation stay stopped.
