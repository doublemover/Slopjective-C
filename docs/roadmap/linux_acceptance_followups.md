# Linux acceptance follow-ups

The acceptance review at `08e5b64bd` identified seven concrete requirements in
the previously claimed fixes. This continuation repairs six, with the focused
evidence below. The earlier review's complete acceptance/qualification boundaries
remain distinct from these source repairs. Issue #8282's actual-product evidence contract has since been implemented
and source-qualified. The published preview records the later hosted runs.

## Repaired requirements

- **#8324 — compiler rejection termination:** the compiler witnesses, standard
  library, source modules and installed package graph require an ordinary
  diagnostic exit. The package graph checks the wrapper and exact inner
  command result. Signals, translated driver crashes and fatal diagnostics
  accompanying exit 1 are rejected. The managed-module runner invalidates an
  earlier success receipt before starting. Runtime trap controls keep their own
  termination expectations. The retained deterministic malformed-corpus gate
  now uses report v2: only exit 1 with an ordinary source diagnostic passes;
  repeatable exits 0/2/86, silent rejection, sanitizer or loader/CLI diagnostics,
  signals and timeouts retain their exact termination class and fail.
- **#8286 — clean-host tool ordering:** admit the complete foundation before
  resolving optional development tools, build `opt`/`llvm-dis`/`llvm-reduce`, then
  perform strict tool admission. Removing `opt` before the real campaign proves
  that its owner rebuilds it and completes all 17 IR and four debugger routes.
- **#8284 — standalone staged DSO provenance:** compare staged LLVM/Clang libraries
  with canonical Ninja link outputs and their verified object/archive inputs.
  Reproduce only LLVM's CMake install runpath change on a disposable copy; all
  other bytes must match. The actual foundation passes with 3,368 compile
  products, 94 staged archives and 35 additional private link archives. Both DSOs
  bind their real link commands and complete input inventories. Six native
  controls cover unchanged/installed libraries, foreign bytes, an ordinary
  substitute, unverified objects and weakened link instrumentation.
- **#8302 — rejected text metadata:** the runtime validates the declared scalar
  count during the authoritative UTF-8 decode, before publishing any token. The
  public text adapter calls this operation directly. An instrumented regression
  rejects 4,096 mismatched records with unchanged token/live counts, accepts and
  destroys a correct record, and checks malformed/empty/null inputs.
- **#8312 — carrier payload identity:** native contract identity recursively
  visits Optional payloads and both Result arms before canonical nullability is
  lost. C17/C++20/C++23 cases cover ordinary signatures, aliases, nested carriers
  and callbacks. Four changed-nullability interfaces reject stale locks with
  `O3IFC134` before declaration import; the corresponding fresh locks succeed.
- **#8325 — managed-example freshness:** source graphs and the installed managed
  example share fresh preprocessing/dependency discovery. Provider, ARC projection
  and consumer stages bind positive/negative header lookup before reuse and check
  it again before publication. The installed example passes 32 builds and six
  rejected imports across O0/O2, including external content edits with preserved
  timestamps, new/removed earlier headers and consumer-only negative lookups.
- **#8326 — post-activation status:** a cleanup error after the atomic activation
  commit returns successful activation with an explicit cleanup warning/path.
  Precommit errors still fail. Full-install tests cover failed cleanup, retry,
  newly sourced shells selecting the new generation and an existing shell
  retaining the previous one.

The [inventory](../support/linux_open_issue_review.json) retains receipt hashes.
The carrier identity/import and text targets pass with the instrumented facade.
The updated compiler also passes the complete **33-case / seven-control** semantic
seed bank. The maintained selection passes **652 tests**, one Windows-only skip
and 33 subtests; **47** workflow/cadence checks pass.
The managed-example witness uses a sealed development-only diagnostic prefix;
no SDK archive is produced. The source-graph cold-host VFS repair additionally
passes its 112-build / 32-rejection matrix at its recorded checkpoint.

## Published preview

**#8282:** actual-product records now bind source/command identity, log digests,
duration and loaded routes across sanitizer owners. Campaign-relative shared
blobs replay after ZIP relocation without private symlinks. Complete blob
metadata is checked before verification is shared. Focused native replay and
corruption controls passed at this checkpoint. The sanitizer and terminal
gates later passed on both hosts for the published ABI 24 preview.

The [release status](../releases/status.md) identifies the exact preview
archives and hosted qualification. SDK 2.62 / ABI 21 was an earlier
development checkpoint. The [preview issue review](linux_preview_issue_review.md)
tracks the remaining acceptance work.
