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
- `tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json`
- `tests/tooling/fixtures/platform_support/source_truth_matrix.json`
- `tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json`
- `tests/tooling/fixtures/platform_hardening/boundary_inventory.json`
- `native/objc3c/src/driver/objc3_llvm_capability_routing.cpp`
- `scripts/objc3c_llvm_capability_probe/reports.py`
- `scripts/check_objc3c_cross_lane_e2e.py`

Generated artifacts are replay outputs only:

- `tmp/artifacts/platform-hardening/objc3c-platform-matrix.json`
- `tmp/reports/platform-hardening/platform-matrix-summary.json`
- `tmp/reports/platform-hardening/host-matrix-summary.json`

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

AddressSanitizer and UndefinedBehaviorSanitizer variants are reserved. They
cannot list supported platform ids until sanitizer package, install, and native
execution evidence exists for a supported host. Their runtime package variant
metadata is source-owned, but the current rows remain reserved and fail closed
for unsupported hosts, mixed sanitized/unsanitized runtime libraries, release
channel publication, missing sanitizer runtime libraries, and stale package
metadata.

Promotion prerequisites are explicit source data, not prose-only policy. An
unsupported host row must list every missing supported-host evidence class
(`build`, `package`, `install`, and `execution`). A fail-closed or reserved
package row must list the missing package/install/native-execution classes that
block publication. ASan and UBSan sanitizer rows must list `package`,
`install`, and `execution` as both their promotion prerequisites and their
current missing evidence classes. Rows that omit those fields are not eligible
for package, install, native execution, or capability-matrix promotion.

Every platform support row records explicit host triples. The current source
contract recognizes `x86_64-pc-windows-msvc` as the supported Windows x64
triple, `x86_64-unknown-linux-gnu` as the fail-closed Linux x64 triple, and
`aarch64-apple-darwin` as the fail-closed macOS arm64 triple. Package variant
rows also carry a `metadata_freshness_guard`; stale generated package metadata
blocks publication and cannot be used as source truth.

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
not a broad LLVM compatibility promise. The current supported entry is
`objc3c.llvm.windows-x64.current-probed-19`, which binds Windows x64 support to
the public LLVM capability probe, native build resolution, package/archive
evidence, clean-room install evidence, and native execution smoke evidence.
`clang`, `clang++`, `llc`, `llvm-ar`, and runtime headers/libs discovered from
llvm-config or an installed LLVM root are required for supported claims. Missing archive tooling or header/library
discovery rejects package, native execution, and platform support before a claim
can be published.

Object emission is supported only when `llc` is resolved and the probe verifies
`--filetype=obj`. Package/archive claims also require `llvm-ar`, and package or
native execution claims require LLVM header/library discovery from llvm-config or
an installed LLVM root. Missing `llc`, missing archive/header tools, mixed LLVM tool roots,
or versions outside known-good evidence fail before package, native execution,
or platform support claims are published. The source contract records
`native_object_emission_missing_llc`,
`native_object_emission_filetype_obj_unavailable`, and
`native_object_emission_supported` as the native object-emission statuses.
Capability-routed object emission must select `llvm-direct` only after the
`llc --filetype=obj` probe succeeds; it must not fall back to clang and then
publish a native object success claim. Hosted runner and conformance-minima
lanes may skip or fail closed when the status is unavailable, but they cannot
publish object, package, execution, or parity success from a missing-`llc`
summary.

## Replay Surface

Use the public bridge for replay:

- `npm run objc3c -- build-platform-support-matrix`
- `npm run objc3c -- validate-platform-hardening`
- `npm run objc3c -- validate-platform-hardening-end-to-end`
- `npm run objc3c -- build-native-binaries`
- `npm run objc3c -- package-runnable-toolchain`
- `npm run objc3c -- test-hosted-execution-smoke`
- `npm run objc3c -- validate-package-install-distribution`

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
