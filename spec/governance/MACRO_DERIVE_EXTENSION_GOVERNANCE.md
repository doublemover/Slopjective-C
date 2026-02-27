# Macro/Derive Extension Governance {#macro-derive-extension-governance}

_Working draft v0.11 - policy version 1.0.0 - last updated 2026-02-23_

Issue tracking: #121 (`v0.11-A07`)

## 1. Purpose and scope {#mdx-gov-1}

This policy defines governance for experimental and vendor-specific extensions to Objective-C 3.0 derives and macros (Part 10), including:

- lifecycle stages and transition criteria,
- vendor extension compatibility rules,
- conformance-claim constraints for experimental features,
- required transition evidence,
- versioning and change-control rules.

This document is normative for metaprogramming extensions in the v0.11 spec train.

## 2. Lifecycle stages {#mdx-gov-2}

Every derive/macro extension shall be assigned exactly one stage at all times.

### 2.1 Experimental {#mdx-gov-2-1}

- Intended for early validation and rapid iteration.
- Must be explicitly opt-in (`off` by default).
- May change incompatibly between minor releases.
- Must use a vendor namespace (Section 3); portable unprefixed names are forbidden.
- Shall not be counted toward conformance profile requirements.

### 2.2 Provisional {#mdx-gov-2-2}

- Intended for cross-vendor convergence before standardization.
- Syntax and core semantics are frozen except additive changes and defect fixes.
- Must publish diagnostics, migration notes, and feature-test identifiers.
- Must remain opt-in unless/until promoted to Stable.
- May be referenced by ecosystem tooling, but still does not satisfy required portable feature minima on its own.

### 2.3 Stable {#mdx-gov-2-3}

- Normative, portable, and eligible for conformance claims.
- Backward compatibility is required (Section 7).
- Unprefixed canonical names are allowed only at this stage.
- Any incompatible change requires major-version change control and explicit migration policy.

### 2.4 Deprecated {#mdx-gov-2-4}

- Feature remains supported for compatibility but is scheduled for retirement.
- New functionality is forbidden; only fixes, diagnostics, and migration support are allowed.
- Deprecation diagnostics and fix-its are required.
- Removal is allowed only after the minimum deprecation window in Section 7.4.

## 3. Vendor extension compatibility rules {#mdx-gov-3}

### 3.1 Namespacing and identifiers {#mdx-gov-3-1}

- Vendor extensions shall use a globally unique, vendor-qualified ID:
  - `vendor.<vendor_id>.meta.<feature_id>`
- `<vendor_id>` and `<feature_id>` shall be lowercase ASCII `[a-z0-9_]+`.
- IDs in the `objc3.meta.*` namespace are reserved for Stable portable features and shall not be used by vendor-only extensions.

### 3.2 Macro and derive surface compatibility {#mdx-gov-3-2}

- A vendor extension shall not redefine the behavior of any Stable derive or macro name.
- If semantics differ from a Stable feature, a distinct vendor-qualified name is required.
- Vendor aliases to Stable names are allowed only during explicit migration windows and shall emit compatibility warnings.

### 3.3 Feature-test and metadata compatibility {#mdx-gov-3-3}

- Implementations shall expose a deterministic feature-test macro for each vendor extension by normalizing the ID to uppercase underscores:
  - Example: `vendor.acme.meta.fast_hash` -> `__OBJC3_EXT_VENDOR_ACME_META_FAST_HASH__`
- Module metadata and textual interfaces shall record:
  - extension ID,
  - lifecycle stage,
  - extension semantic version,
  - required enable flag(s), if any.
- Importing an interface that requires an unavailable vendor extension shall produce a deterministic diagnostic.

## 4. Conformance constraints for experimental features {#mdx-gov-4}

For any conformance claim (Core/Strict/Strict Concurrency/Strict System and optional profiles):

- Experimental extensions shall be disabled by default in conforming mode.
- Test results that require experimental extensions shall not be counted toward minimum conformance suite obligations.
- Conformance reports shall list experimental extensions under a non-conforming/extension section with explicit stage metadata.
- A toolchain shall not claim portable support for functionality implemented only via experimental vendor extensions.
- If an experimental extension affects interface shape or ABI, emitted metadata/interfaces shall be marked as non-portable unless the same extension is enabled downstream.

## 5. Required evidence for stage transitions {#mdx-gov-5}

A stage change request shall include a transition dossier with, at minimum:

- normative spec text: syntax, semantic rules, diagnostics, and metadata/interface requirements,
- test evidence: parser, semantic, diagnostics, module_roundtrip, and (if ABI-impacting) lowering_abi coverage,
- implementation evidence: shipping implementation(s) and reproducibility proof for macro/derive expansion,
- compatibility evidence: migration notes, breakage analysis, and fallback behavior for unsupported toolchains,
- issue hygiene: unresolved critical soundness/security issues must be zero at approval time.

Additional requirements by transition:

- Experimental -> Provisional: at least one production implementation and complete conformance-test mapping for the feature.
- Provisional -> Stable: at least two independent implementations plus cross-toolchain interoperability results.
- Stable -> Deprecated: replacement path, deprecation diagnostics/fix-its, and documented sunset timeline.

## 6. Stage transition decision table {#mdx-gov-6}

| Transition | Allowed | Minimum decision inputs | Effective in |
| --- | --- | --- | --- |
| Experimental -> Provisional | Yes | Full dossier; implementation evidence (1+); test mapping complete | Next minor or major release |
| Provisional -> Stable | Yes | Full dossier; independent implementations (2+); interop proof; migration guidance | Next major release by default (minor only with explicit waiver) |
| Stable -> Deprecated | Yes | Replacement and migration plan; deprecation diagnostics/fix-its; sunset schedule | Next minor or major release |
| Provisional -> Experimental (rollback) | Yes | Regression/soundness report and rollback note | Immediate patch/minor release |
| Deprecated -> Stable (reversal) | Exceptional | New evidence equivalent to Provisional -> Stable plus rationale for reversal | Next major release |

## 7. Versioning and change control {#mdx-gov-7}

### 7.1 Policy versioning {#mdx-gov-7-1}

This governance policy uses semantic versioning:

- MAJOR: normative rule changes that alter required behavior or allowed transitions.
- MINOR: additive governance requirements or new required evidence fields.
- PATCH: clarifications with no normative behavior change.

### 7.2 Extension versioning by stage {#mdx-gov-7-2}

- Experimental and Provisional extensions should use `0.y.z`.
- Promotion to Stable shall start at `1.0.0` or carry forward an equivalent stable-major mapping.
- Deprecated extensions may publish compatibility patches only; no new functionality.

### 7.3 Change-control process {#mdx-gov-7-3}

Any stage change or normative extension change shall include:

- a tracked issue and pull request,
- updated spec text and transition dossier links,
- updated conformance-test manifest references,
- release-note and migration-note entries,
- an entry in the decisions log with effective version/date.

No stage transition is valid until all items above are merged and published in the same spec release.

### 7.4 Deprecation/removal window {#mdx-gov-7-4}

- Deprecated status shall be maintained for at least two minor releases or 12 months, whichever is longer, before removal.
- Early removal requires a security/safety exception and explicit release-note waiver.
