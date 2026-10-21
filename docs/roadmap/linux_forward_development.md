# Objective-C 3.0 on Linux

The target is Objective-C 3.0 as defined by `spec/`, implemented by the patched
Clang toolchain. C17 and C++20/23 name the underlying Clang source modes; they
are not separate Objective-C product versions. Linux x86-64 with GNUstep 2.2 is
the currently qualified runtime target. Windows and compatibility with retired
frontends are outside the active development plan.

The [language capability authority](../../schemas/objc3-language-capabilities.json)
records public compiler claims. The [library inventory](../../stdlib/module_inventory.json)
records native library coverage against the
[standard-library contract](../../spec/STANDARD_LIBRARY_CONTRACT.md). Existing
historical milestone checklists do not establish current Clang conformance.

## Delivered foundations

The [native-library SDK receipt](../support/linux_native_stdlib_acceptance.json) identifies
the last activated archive and its actual installed tests. That release includes
extension ABI 20, service ABI 4, the shared C/C++ async frame/result contract, native standard-library modules,
per-callback and per-resume autorelease pools, fair actor scheduling and the
current SDK-independent package install command. New development must produce a
new archive and receipt before it is described as delivered in that SDK.

The public synchronous compiler supports native `defer`, `Optional`, `Result`
and checked `throws` in the bounded compatibility profile. Its ownership and
ABI tests cover strong/weak/unsafe objects, Blocks, nested carriers, arrays,
records, heap storage, direct dispatch, typed errors and native unwinding.

Native semantic interfaces preserve callable effects, ownership, availability,
module-owned names, explicit exports and SPI. Source graphs and immutable binary
packages support separately compiled and source-free providers. Installed editor
and LLDB tests exercise the same canonical identities.

The structured runtime supplies groups, cancellation, task outcomes, serial
actors, executor destinations and suspended dependency/hop scheduling. The
Objective-C++ coroutine compiler supports async functions, methods and Blocks
through reserved spellings. Public async/await and actors remain reserved.
The [shared ABI contract](linux_async_c_abi.md) no longer exposes C++ promise
layout at the C frame boundary.

## Active completion sequence

Each milestone includes implementation, diagnostics, imports, ownership,
execution and installed-archive tests before its capability is promoted.

1. **Native standard library.** Replace the retired `module`/`fn` sources and
   synthetic integer helpers with canonical Clang modules backed by the real
   runtime. Ship core discovery/versioning, lazy nil-to-error helpers, validated
   UTF-8, concrete/generic collections and structured runtime declarations.
   Exercise mixed Objective-C/Objective-C++ providers, ARC/MRC consumers, O0/O2,
   cached imports and expected failures. Implement typed key paths and strict
   system helpers as their compiler semantics become available.
2. **Core type and control-flow completion.** Reconcile every Part 3 and Part 5
   requirement with the current AST and diagnostics: generic constraints,
   optional binding/chaining, `guard`, exhaustive matching, typed key paths and
   flow-sensitive refinements. Carry these through PCH and semantic interfaces.
3. **Native Objective-C async.** Lower functions, methods and Blocks to the
   existing C frame ABI without requiring C++ source. Preserve live locals,
   cleanup, owned results/errors, native exceptions and cancellation across
   suspension. Recognize structured versus detached spawn and enforce awaited
   effectful calls before promoting public syntax.
4. **Isolation and concurrency safety.** Implement actor/global-actor callable
   metadata, awaited executor crossings, isolated state access and Sendable
   transfer checks. Preserve logical identity and mailbox fairness through
   nested calls, imports and source-free packages.
5. **Ownership and strict profiles.** Complete resource, borrowed/lifetime,
   capture and escape rules from Part 8. Enable Core, Strict, Strict Concurrency
   and Strict System only when the full relevant checklist and required library
   modules pass, including required diagnostics and fix-its.
6. **Remaining specified feature sets.** Complete the specified reflection,
   metaprogramming/property behavior, foreign/Swift interop and tooling surfaces.
   Optional feature-set status must remain explicit until its entire contract
   is implemented and tested.
7. **Release and architecture coverage.** Run the actual new archive on fresh
   supported Linux distributions, publish its exact content identity and
   installation evidence, then qualify AArch64 on a native host. Emulation and
   cross-compilation can find bugs but do not establish native support.

The standard-library cutover is version 2. Its inventory distinguishes completed
module contracts from partial and unimplemented modules. Importing a module
alone does not establish profile conformance. No placeholder key-path or system
module may turn a missing implementation into apparent coverage.

## Development and release operation

Use one hard cutover for source, objects, interfaces, packages and installed SDK
artifacts when an ABI changes. Retain the current archive/installation and active
compiler source/build tree. Remove superseded generated artifacts once a new
release is verified; Git and pinned source/patch inputs preserve history.
Preserve unique authored work before removing a worktree.

| Location                 | Purpose                                                    |
| ------------------------ | ---------------------------------------------------------- |
| `out/toolchains/`        | Active compiler/build and current SDK archive/installation |
| `out/native-validation/` | Current native execution receipts and diagnostics          |
| `out/session/`           | Local handoff records and PR drafts                        |
| `scratch/`               | Disposable experiments and temporary review worktrees      |

These output roots are gitignored. Maintained code belongs under `scripts/`,
`native/`, `stdlib/` or `tests/`; outputs do not belong beside source files.

This instance has a four-CPU quota and 16 GiB of RAM. Use four compile jobs and
one link job, a bounded 1 GB ccache with content-based compiler checks, and the
private mode-0700 `/tmp/objc3c-1000` directory exported as `TMPDIR` by managed
activation. `/tmp` already uses RAM; no extra ramdisk reservation is needed.
Keep persistent compiler objects, SDKs and ccache on disk. Disposable integration
executables and service archives use case-scoped temporary directories and are
reclaimed on success or failure; logs and receipts remain on disk.

The ignored local `linux-dev` CMake preset targets the current frontend build.
The SDK pipeline enables all required qualification independently. Frontend
CTest executables are built and executed serially, then passed binaries are
reclaimed to stay within the disk budget. Source, configuration and actual
foundation contents determine reuse; deleting a duplicate stage does not
invalidate an unchanged foundation.

`out/bin/clangd` uses the current patched SDK, two workers and in-memory
preambles. The ignored `.clangd` and root compilation database select current
build paths. After an SDK cutover, reconfigure the frontend against
`out/toolchains/linux-sdk/current` so no deleted candidate path remains in the
compilation database.
