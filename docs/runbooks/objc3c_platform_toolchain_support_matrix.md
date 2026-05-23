# objc3c Platform And Toolchain Support Matrix

This runbook is the operator-facing companion to the machine-readable contract
at `tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json`.
It defines what may be called supported, what remains unsupported or reserved,
and which evidence classes are mandatory before a row can move.

Issue evidence for the post-cutover roadmap index and #8177 lives in
`docs/issues/objc3_next_8153_8179_evidence.md`.

## Source Of Truth

Checked-in source truth:

- `schemas/objc3c-platform-toolchain-support-evidence-v1.schema.json`
- `schemas/objc3c-platform-support-matrix-v1.schema.json`
- `tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json`
- `tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json`
- `tests/tooling/fixtures/platform_hardening/boundary_inventory.json`

Generated artifacts are replay outputs only:

- `tmp/artifacts/platform-hardening/objc3c-platform-matrix.json`
- `tmp/reports/platform-hardening/platform-matrix-summary.json`
- `tmp/reports/platform-hardening/host-matrix-summary.json`

## Current Support State

`windows-x64` is the only supported host row. It is supported only because the
row has build, package, install, and native execution evidence and because its
toolchain rows are evidence-bound to current local probes.

`linux-x64` and `darwin-arm64` are unsupported rows. They publish fail-closed
diagnostics only and do not carry support evidence.

AddressSanitizer and UndefinedBehaviorSanitizer variants are reserved. They
cannot list supported platform ids until sanitizer package, install, and native
execution evidence exists for a supported host.

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
the public LLVM capability probe, native build resolution, and native execution
smoke evidence. `clang`, `clang++`, `llc`, and runtime headers/libs are required
for supported claims. `llvm-ar` and `llvm-config` remain reserved until package
archive and header/library discovery lanes consume the same matrix directly.

Object emission is supported only when `llc` is resolved and the probe verifies
`--filetype=obj`. Missing `llc`, mixed LLVM tool roots, or versions outside
known-good evidence fail before package, native execution, or platform support
claims are published.

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
publish a new support claim.
