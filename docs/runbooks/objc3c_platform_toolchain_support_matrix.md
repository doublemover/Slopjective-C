# objc3c Platform And Toolchain Support Matrix

This runbook is the operator-facing companion to the machine-readable contract
at `tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json`.
It defines what may be called supported, what remains unsupported or reserved,
and which evidence classes are mandatory before a row can move.

Issue evidence for the post-cutover roadmap index and #8177 lives in
`docs/issues/objc3_next_8153_8179_evidence.md`.

The platform expansion epic is #8206. Linux x64 is #8228, macOS arm64 is
#8229, the AddressSanitizer runtime package variant is #8230, and the
UndefinedBehaviorSanitizer runtime package variant is #8231. Deterministic
native object emission and the hosted-runner missing-`llc` contract are #8232.
#8206 is umbrella readiness over those source-owned rows. It may project only
the existing `windows-x64` supported row; Linux, macOS, sanitizer, and
missing-`llc` lanes remain unsupported, reserved, or fail-closed until their
source evidence rows are promoted.

## Source Of Truth

Checked-in source truth:

- `schemas/objc3c-platform-toolchain-support-evidence-v1.schema.json`
- `schemas/objc3c-platform-support-matrix-v1.schema.json`
- `schemas/objc3c-platform-support-source-truth-v1.schema.json`
- `schemas/objc3c-platform-hosted-runner-capability-summaries-v1.schema.json`
- `tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json`
- `tests/tooling/fixtures/platform_hardening/platform_host_promotion_evidence_contract.json`
- `tests/tooling/fixtures/platform_hardening/host_promotion_reviewed_source_inputs.json`
- `tests/tooling/fixtures/platform_hardening/hosted_runner_capability_summaries.json`
- `tests/tooling/fixtures/platform_support/source_truth_matrix.json`
- `tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json`
- `tests/tooling/fixtures/platform_hardening/boundary_inventory.json`
- `.github/workflows/platform-host-evidence.yml`
- `.github/workflows/conformance-minima.yml`
- `scripts/ingest_objc3c_platform_host_evidence.py`
- `native/objc3c/src/driver/objc3_llvm_capability_routing.cpp`
- `scripts/objc3c_llvm_capability_probe/reports.py`
- `scripts/check_objc3c_cross_lane_e2e.py`

Generated artifacts are replay outputs only:

- `tmp/artifacts/platform-hardening/objc3c-platform-matrix.json`
- `tmp/reports/platform-hardening/platform-matrix-summary.json`
- `tmp/reports/platform-hardening/host-matrix-summary.json`
- `tmp/reports/platform-host-evidence/<platform>/host-evidence-report.json`
- `tmp/reports/platform-host-evidence/<platform>/promotion-readiness-requirements.json`
- `tmp/reports/platform-host-evidence/<platform>/review-candidate-source-truth.json`
- `tmp/reports/platform-host-evidence/<platform>/ingestion-summary.json`

## Current Support State

`windows-x64` is the only supported host row. It is supported only because the
row has build, package, install, and native execution evidence and because its
toolchain rows are evidence-bound to current local probes.

`linux-x64` and `darwin-arm64` are unsupported rows. They publish fail-closed
diagnostics only and do not carry support evidence. Their source-owned package
variant rows are intentionally fail-closed: they record the Linux ELF/toolchain
and macOS arm64 Mach-O/load-path blockers that must be replaced with real
build, package, install, and native execution evidence before any capability
matrix promotion.

AddressSanitizer and UndefinedBehaviorSanitizer are Windows x64 sanitizer
package rows, not platform support rows. Their package rows are still named
`reserved`, but the current source truth binds them to Windows x64 package,
install, and execution evidence through the sanitizer runtime promotion gate.
That evidence does not promote Linux, macOS, or any hosted sanitizer summary.
The rows still fail closed for unsupported hosts, mixed sanitized/unsanitized
runtime libraries, default release-channel misuse, missing sanitizer runtime
libraries, missing expected detection records, stale package metadata, and
generated-only reports.

Promotion prerequisites are explicit source data, not prose-only policy. An
unsupported host row must list every missing supported-host evidence class
(`build`, `package`, `install`, and `execution`). A fail-closed release package
row must list the missing build/package/install/execution classes that block
publication. ASan and UBSan package rows must stay tied to their checked
sanitizer package/install/execution evidence ids and fail-closed negative
contracts; hosted sanitizer summaries still list missing package/install/
execution classes because they are summary-only and cannot publish support.
Rows that omit the source-owned evidence fields are not eligible for package,
install, native execution, or capability-matrix promotion.

## Issue Closure Criteria

#8228 can close as Linux x64 support only when `linux-x64` has checked source
rows showing build, package, install, and native execution evidence, the
support row is no longer `unsupported`, the package variant row no longer lists
fail-closed publication blockers, and every host-promotion reviewed-source
record has been replaced by promotion-ready source truth for host identity,
toolchain probe, package root, install receipt, native execution, object
identity, debug identity, package install identity, and runtime load/link proof.
Generated Ubuntu hosted reports are required review inputs, but their
`support_truth` remains false until those reviewed source rows land.

#8229 can close as macOS arm64 support only under the same source-review bar for
`darwin-arm64`, with Mach-O, DWARF/dSYM, `@rpath`/`install_name`/codesign loader
behavior, package install, and native execution proven by checked source truth.
Generated macOS hosted evidence, cross-compiled Mach-O artifacts, tool presence,
or prose status updates do not promote the row.

#8206 can close as a platform expansion umbrella only when the matrix row moves
from `internal` to an evidence-backed implemented row, #8228 and #8229 are both
promoted by the criteria above, #8230 and #8231 remain bounded to Windows x64
through the sanitizer runtime promotion gate, and #8232 proves coherent
`llc --filetype=obj` native object emission plus non-empty target-specific
object output with no clang-substitute success path. Until all child gates agree, the only supported platform projection is
`windows-x64`.

Every platform support row records explicit host triples. The current source
contract recognizes `x86_64-pc-windows-msvc` as the supported Windows x64
triple, `x86_64-unknown-linux-gnu` as the fail-closed Linux x64 triple, and
`aarch64-apple-darwin` as the fail-closed macOS arm64 triple. Package variant
rows also carry a `metadata_freshness_guard`; stale generated package metadata
blocks publication and cannot be used as source truth.

## Host Evidence Architecture

Platform promotion is host-evidence driven. The source contract now carries
first-class records for host identity, toolchain probes, package roots, install
receipts, native execution evidence, object identity, debug identity, package
install identity, runtime link/load proof, and negative host/toolchain cases. A
row can publish support only when all of those records are checked in for the
same platform, the review decision is approved for source truth, no blockers
remain, and the native execution record points at real execution evidence.

Linux x64 and macOS arm64 deliberately have source-owned identity and package
root records, but every promotion record in
`host_promotion_reviewed_source_inputs.json` remains non-ready today:
toolchain probe, package root, install receipt, native execution, object
identity, debug identity, package install identity, and runtime link/load proof
all have `promotion_allowed: false` for both hosts. The Linux package root
records ELF/DWARF identity, `libobjc3-runtime.so`, and package-root loader
expectations. The macOS arm64 package root records Mach-O plus DWARF/dSYM
identity, `libobjc3-runtime.dylib`, and `@rpath`/`install_name`/codesign loader
expectations. Those distinctions are not support claims; they define the
evidence shape a future real host run must satisfy.

Negative cases are also source-owned. Missing `llc --filetype=obj`, failed
target-specific object output from `llc`, mixed LLVM roots, mismatched LLVM tool
versions, unsupported LLVM versions, and absent Linux/macOS native execution all
block package, execution, and publication surfaces. Hosted-runner tool presence, source-only package metadata,
object-emission status, and generated hosted reports remain summary or
review-input information and cannot clear the promotion gate.

Package variant rows also carry source-owned artifact identity and promotion
gates. These fields are not support claims by themselves; they are the exact
contract a future host or sanitizer package must satisfy before publication:

- `windows-x64` release packages are COFF/PDB packages with
  `artifacts/bin/objc3c-native.exe`, `artifacts/lib/objc3_runtime.lib`, and
  the runnable CLI layout proved by package, install, and execution evidence.
- `linux-x64` release packages are fail-closed ELF/DWARF packages with
  `libobjc3-runtime.so`, package-root loader behavior, and symbol export policy
  blocked until Linux build, package, install, and native execution evidence
  exists.
- `darwin-arm64` release packages are fail-closed Mach-O/DWARF/dSYM packages
  with `libobjc3-runtime.dylib`, `@rpath`/`install_name`/codesign loader
  behavior, and native execution evidence required before support.
- ASan and UBSan packages are Windows x64 sanitizer runtime package rows. Their
  source-owned metadata must capture exact target platform, sanitizer runtime
  library identity, compile/link flags, runtime mode, package/install identity,
  expected detection records, and fail-closed negative cases before any
  sanitizer package claim can publish; none of that widens platform support.

`package_root_layout` uses package-channel payload vocabulary, not installed
tree vocabulary. Release rows must match the full artifact-scoped runnable
package payload entries:

- `windows-x64`: `artifacts/package/objc3c-runnable-toolchain-package.json`,
  `artifacts/bin/objc3c-native.exe`, `artifacts/lib/objc3_runtime.lib`,
  `stdlib/workspace.json`, `stdlib/modules/objc3.core/module.json`, and
  `docs/runbooks/objc3c_packaging_channels.md`
- `linux-x64`: `artifacts/package/objc3c-runnable-toolchain-package.json`,
  `artifacts/bin/objc3c-native`, `artifacts/lib/libobjc3-runtime.so`,
  `stdlib/workspace.json`, `stdlib/modules/objc3.core/module.json`, and
  `docs/runbooks/objc3c_packaging_channels.md`
- `darwin-arm64`: `artifacts/package/objc3c-runnable-toolchain-package.json`,
  `artifacts/bin/objc3c-native`, `artifacts/lib/libobjc3-runtime.dylib`,
  `stdlib/workspace.json`, `stdlib/modules/objc3.core/module.json`, and
  `docs/runbooks/objc3c_packaging_channels.md`

The older `bin/`, `lib/`, and `include/` package-root vocabulary is not valid
for release package-root layout contracts. Linux and macOS rows still remain
fail-closed until their build, package, install, and native execution evidence
is reviewed into source truth.

Sanitizer `package_root_layout` is a Windows x64 reserved overlay, not a full
release layout and not a Linux/macOS sanitizer claim. ASan lists
`artifacts/lib/objc3_runtime.lib`, `share/objc3c/sanitizer/asan-metadata.json`,
`share/objc3c/sanitizer/asan-runtime-libraries.json`, and the
`artifacts/runtime/sanitizer/address/*` runtime artifacts. UBSan lists
`artifacts/lib/objc3_runtime.lib`, `share/objc3c/sanitizer/ubsan-metadata.json`,
`share/objc3c/sanitizer/ubsan-runtime-libraries.json`, and the
`artifacts/runtime/sanitizer/undefined/*` runtime artifacts. Those overlays
remain fail-closed outside the checked Windows x64 sanitizer runtime package
evidence and never imply Linux/macOS sanitizer support.

A package row can move from fail-closed or reserved to support only by changing
checked-in source rows and evidence references together. Hosted-runner summaries
remain summary-only and cannot clear a promotion gate.

## Hosted Runner Evidence Collection

The canonical public evidence workflow is
`.github/workflows/platform-host-evidence.yml`. The default-branch
`conformance-minima` workflow is also modeled as an explicit dispatch gateway
for the same generated host evidence jobs, so maintainers can trigger hosted
Linux/macOS evidence from a registered workflow path without turning that run
into support truth. Both workflow paths are accepted ingestion origins, and both
remain non-promoting. The dedicated evidence workflow is manual by design and
collects generated runner evidence without making pull-request CI depend on
currently unsupported host rows.

- `linux-x64` runs on `ubuntu-24.04`.
- `darwin-arm64` runs on `macos-15`.

Each hosted job attempts the same promotion-relevant path:

- install host toolchain prerequisites
- install workflow dependencies
- run `scripts/probe_objc3c_llvm_capabilities.py`
- `npm run objc3c -- build-native-binaries`
- `npm run objc3c -- package-runnable-toolchain`
- `npm run objc3c -- validate-packaging-channels-end-to-end`
- `npm run objc3c -- validate-package-install-distribution --from-nothing`
- `npm run objc3c -- test-hosted-execution-smoke`
- `npm run objc3c -- ingest-platform-host-evidence`

The hosted execution command writes a stable hosted summary at
`tmp/reports/hosted-execution-smoke/summary.json` and a normalized native
execution summary at `tmp/reports/objc3c-native-execution-smoke/summary.json`.
The Linux and macOS evidence jobs set deterministic native execution run IDs so
the source artifact summary can also be traced back to
`tmp/artifacts/objc3c-native/execution-smoke/platform-host-evidence-<platform>/summary.json`.
The ingestion helper writes a generated host evidence report, a review-candidate
source-truth artifact, and an ingestion summary under
`tmp/reports/platform-host-evidence/<platform>/`. It also writes a
promotion-readiness requirements artifact naming the build, package, install,
installed-root execution, object-format/debug, runtime link/load, and native
execution fields that must be reviewed before source-truth promotion. The review
candidate is source-consumable
review material for all nine required host-promotion record classes; it is not a
reviewed source path, cannot update source truth, and cannot promote Linux or
macOS support by itself. The helper mirrors build, package, install,
hosted-smoke, and native-execution outputs into that same
platform-scoped root before upload, including `build/object-identity.json`,
`build/debug-identity.json`, `package/runtime-library-manifest.json`,
`install/install-receipt.json`,
`install/end-to-end-summary.json`,
`install/install-distribution-credibility-summary.json`,
`install/install-distribution-verification.json`,
`install/clean-install-distribution-receipt.json`, and
`execution/runtime-load-probe.json`. The
workflow uploads only
`tmp/reports/platform-host-evidence/<platform>/**` and fails closed if that root
is empty, so Linux x64 and macOS arm64 readback cannot accidentally consume
shared `tmp/` or `artifacts/` paths from another lane. The summary and review
candidate are source-consumable but not source truth. The required ingestion
result is
`GENERATED_ONLY_REFUSED_FOR_SOURCE_TRUTH`: generated workflow output may be
reviewed by a maintainer, but it cannot clear `required_missing_evidence_classes`
or publish a support row until the reviewed evidence is promoted into checked-in
source truth.

The checked-in source contract records the candidate evidence rows
`objc3c.evidence.hosted-ci.linux-x64.generated-host-run` and
`objc3c.evidence.hosted-ci.darwin-arm64.generated-host-run` as `policy`
evidence with empty `supports_platform_ids`. Those rows document where real
host artifacts will appear; they deliberately do not support Linux x64 or macOS
arm64.

## Capability Truth

Capability truth is source-owned and fail-closed. The public capability summary
fixture may report that a hosted runner saw native object-emission capability,
but that is not host support unless the same platform also has checked build,
package, install, execution, object/debug identity, runtime load/link, and
reviewed source-truth records.

`windows-x64` is the only row whose hosted capability summary may publish
support today. Linux x64 and macOS arm64 capability summaries have empty
`platform_ids` and `publication_allowed: false`; even a successful generated
host run stays a review input until checked source rows are promoted. Missing
`llc`, missing `llc --filetype=obj`, failed target-specific object output from
`llc`, mixed LLVM roots, mismatched or unresolved tool versions, unsupported LLVM
versions, missing `llvm-ar`, and missing LLVM header/library discovery fail
closed before native object, package, execution, or platform support can be claimed.

## Required Host Evidence

Every supported host row must provide all four evidence classes:

- `build`
- `package`
- `install`
- `execution`

The validator rejects a supported row if any class is missing, if the evidence
requires live network access, if a generated report is used as source truth, or
if the evidence points at a platform outside the checked-in boundary.

## Required Toolchain Evidence

Every supported host row must also list the required toolchain components:

- `llvm`
- `clang`
- `cmake`
- `ninja`
- `python`
- `node`
- `pwsh`

Toolchain range rows are current-probe rows, not broad version promises. The
matrix may describe only the executable and package-bridge shape that replayed
through the public workflow surface. Unsupported versions and missing tools
fail closed with no range claim.

The LLVM version support matrix is checked in under
`llvm_version_support_matrix` in the platform support evidence fixture. It is
not a broad LLVM compatibility promise. The minimum supported version family is
`19.1`, and the current CI-pinned Windows evidence entry is
`objc3c.llvm.windows-x64.current-probed-22` for LLVM `22.1.6`, which binds
Windows x64 support to the public LLVM capability probe, native build
resolution, package/archive evidence, clean-room install evidence, and native
execution smoke evidence.
`clang`, `clang++`, `llc`, `llvm-ar`, and runtime headers/libs discovered from
llvm-config or an installed LLVM root are required for supported claims. Missing archive tooling or header/library
discovery rejects package, native execution, and platform support before a claim
can be published.

Object emission is supported only when `llc` is resolved, the probe verifies
`--filetype=obj`, every required LLVM subtool publishes a coherent version
family, and required LLVM tools resolve from a coherent install root. Package
and native execution claims also require `llvm-ar`, clang++, and LLVM
header/library discovery from llvm-config or an installed LLVM root. Missing
`llc`, missing archive/header tools, mixed LLVM tool roots, mismatched LLVM
tool versions, unresolved tool versions, or versions outside the minimum
supported family fail before object, package, native execution, or platform
support claims are published. The source contract records
`native_object_emission_supported`, `native_object_emission_missing_llc`,
`native_object_emission_filetype_obj_unavailable`,
`native_object_emission_mixed_toolchain_root`,
`native_object_emission_mismatched_tool_versions`,
`native_object_emission_unsupported_tool_version`, and
`native_object_emission_unresolved_tool_version` as native object-emission
statuses. It also records `native_object_emission_target_object_unavailable`
when `llc` accepts `--filetype=obj` but cannot emit a non-empty object for the
target. Capability-routed object emission must select `llvm-direct` only after
the coherent `llc --filetype=obj` target-object probe succeeds; it must not fall
back to clang and then publish a native object success claim.

Hosted runner capability summaries are checked-in source fixtures, not
generated proof. `hosted_runner_capability_summaries.json` records the supported
Windows x64 summary, fail-closed Linux/macOS summaries, reserved ASan/UBSan
summaries, and missing-`llc`/mixed-root/mismatched-version toolchain summaries.
Those records can explain why a hosted runner failed closed, but only the
Windows x64 row may publish platform support.

Task-hygiene hosted smoke and source-parity gates are optional publication
guards: when `llc --filetype=obj` is absent or the LLVM tool identity is
incoherent, they skip with no native object, package, execution, or platform
success claim. `conformance-minima` is stricter. It runs
`check-hosted-llvm-capabilities` with
`OBJC3C_REQUIRE_HOSTED_NATIVE_OBJECT_EMISSION=1`, so missing `llc`, missing
`--filetype=obj`, mixed roots, mismatched versions, unsupported versions, or
unresolved tool versions fail closed before cross-lane runtime proof. LLVM
header/library discovery through `llvm-config` or an installed LLVM root remains
required for package and native execution claims, not for the narrower native
object-emission prerequisite by itself.

## Replay Surface

Use the public bridge for replay:

- `npm run objc3c -- build-platform-support-matrix`
- `npm run objc3c -- validate-platform-hardening`
- `npm run objc3c -- validate-platform-hardening-end-to-end`
- `npm run objc3c -- build-native-binaries`
- `npm run objc3c -- package-runnable-toolchain`
- `npm run objc3c -- test-hosted-execution-smoke`
- `npm run objc3c -- validate-package-install-distribution --from-nothing`
- `npm run objc3c -- ingest-platform-host-evidence -- --platform-id linux-x64 --runner-label ubuntu-24.04`
- `npm run objc3c -- ingest-platform-host-evidence -- --platform-id darwin-arm64 --runner-label macos-15`
- `npm run objc3c -- review-platform-host-evidence -- --platform-id linux-x64 --github-run-id <run-id> --expected-head-sha <sha>`
- `npm run objc3c -- review-platform-host-evidence -- --platform-id darwin-arm64 --github-run-id <run-id> --expected-head-sha <sha>`

The matrix can cite generated reports from those commands as evidence outputs,
but checked-in schemas, fixtures, validators, and runbooks remain the source of
truth.

## Fail-Closed Rules

A row must remain unsupported or reserved when any required host or toolchain
evidence is absent. Tool presence alone does not widen host support, source-only
build success does not imply package support, and report-only output cannot
publish a new support claim. Missing runtime libraries, missing sanitizer
runtime libraries, mixed sanitized/unsanitized runtime libraries, unsupported
host triples, and stale package metadata all fail closed before package
publication or native execution claims. A row with any non-empty
`required_missing_evidence_classes` list is a fail-closed prerequisite row, not
a support row; generated reports may echo that list, but they cannot clear it.
