# Part 1 - Canonical Language Mode And Conformance {#part-1}

_Working draft v0.11 - hard-cutover public surface_

## 1.1 Purpose {#part-1-1}

This part defines:

- the single Objective-C 3.0 language mode,
- how source and behavior claims map to capability states,
- conformance levels as evidence-backed support claims,
- required feature-test and report mechanisms,
- canonical diagnostics for rejected source forms.

Public support ownership lives in:

- `docs/support/capability_matrix.md`
- `docs/support/capability_matrix.json`
- `docs/support/evidence_map.json`
- `docs/support/evidence_map.md`
- `docs/support/capability_claim_responsibility.md`
- `scripts/objc3c_shared/schema_registry.py`

Any claim in this part is subordinate to those capability and evidence
surfaces. The capability matrix and evidence map schemas are selected by shared
registry ids, not by schema fragments in this part.

Public command ownership lives in `package.json` and
`scripts/objc3c_workflow/action_catalog.py`. The supported command shape is
`npm run objc3c -- <action>`; direct helper commands and retired command
surfaces are implementation details unless a public workflow document generated
from the action catalog lists them through the npm bridge.

## 1.2 Language mode selection {#part-1-2}

### 1.2.1 Canonical compiler surface {#part-1-2-1}

A conforming implementation exposes Objective-C 3.0 as one canonical source
language. The current public command surface must not expose a second language
profile, source mode, or command switch that accepts retired spelling families
as supported input.

In this repository, the public command bridge is intentionally single-script:
`package.json` exposes `objc3c`, and `npm run objc3c -- <action>` dispatches to
the `scripts.objc3c_workflow` module. Action names, validation tiers, and
guarantee owners are action-catalog-owned, not duplicated in per-doc command
lists.

A command-line mechanism equivalent to the following may select Objective-C 3.0
for toolchains that also host other languages:

- `-fobjc-version=3`

Selecting Objective-C 3.0 shall:

- enable the canonical Objective-C 3.0 grammar additions,
- enable Objective-C 3.0 default rules such as nonnull-by-default regions where
  those rules are implemented,
- emit canonical diagnostics for rejected source forms,
- report support through the capability matrix rather than through chapter status
  prose.

### 1.2.2 Translation-unit granularity {#part-1-2-2}

For Objective-C 3.0 v1, language selection is translation-unit-only.

Mixing Objective-C 3.0 with another Objective-C source language inside a single
translation unit is not conforming.

A conforming implementation:

- treats the effective language as fixed for the entire translation unit,
- rejects per-region language switching forms such as `push`/`pop` pragmas,
- diagnoses any attempt to change the language selection after parsing starts.

### 1.2.3 Source directive {#part-1-2-3}

A source directive such as this may be supported:

- `#pragma objc_language_version(3)`

If supported in v1, the directive:

- applies to the entire translation unit,
- appears only at file scope before the first non-preprocessor declaration or
  definition token,
- rejects region-scoped variants such as `#pragma objc_language_version(push, 3)`
  and `pop`,
- diagnoses conflicting selections between the command line and source.

### 1.2.4 Required conformance tests for language selection {#part-1-2-4}

A conforming implementation includes tests for accepted and rejected forms,
including at minimum:

- TUV-01: accepted command-line translation-unit selection (`-fobjc-version=3`).
- TUV-02: accepted file-scope pragma form `#pragma objc_language_version(3)`
  before declarations, when that directive is implemented.
- TUV-03: rejected region switching forms (`push`/`pop`, begin/end, or
  equivalent).
- TUV-04: rejected in-function or post-declaration version-selection pragma.
- TUV-05: rejected conflicting command-line/source selections.

Test expectations and diagnostic portability requirements are defined in
[Part 12](#part-12).

## 1.3 Reserved keywords and rejected source forms {#part-1-3}

### 1.3.1 Reserved keywords {#part-1-3-1}

In Objective-C 3.0 mode, at minimum the following tokens are reserved as
keywords:

- Control flow: `defer`, `guard`, `match`, `case`
- Effects/concurrency: `async`, `await`, `actor`
- Errors: `try`, `throw`, `do`, `catch`, `throws`
- Bindings: `let`, `var`

### 1.3.2 Contextual keywords {#part-1-3-1-2}

Some tokens may be contextual keywords only within specific grammar positions
when that keeps the canonical syntax precise.

Examples include:

- `borrowed` (type qualifier; [Part 8](#part-8), also cataloged in [B.8.3](#b-8-3))
- `move`, `weak`, `unowned` (block capture lists; [Part 8](#part-8), also cataloged in [B.8.5](#b-8-5))

Contextual treatment is a grammar rule, not an alternate source mode.

### 1.3.3 Escaped identifiers {#part-1-3-2}

A conforming implementation may provide an explicit escaped-identifier spelling
for identifiers that collide with reserved keywords.

Acceptable designs include:

- a raw identifier syntax such as `@identifier(defer)`, or
- a backtick escape such as `` `defer` ``.

The toolchain may provide fix-its to rewrite collisions to the canonical escaped
form. The fix-it does not make the rejected unescaped form supported behavior.

### 1.3.4 Retired spelling rejection {#part-1-3-3}

Retired source spellings such as `YES`, `NO`, `NULL`, and `optional<T>` are
represented as rejected behavior unless the capability matrix states otherwise.
They are diagnostic inputs, not accepted Objective-C 3.0 syntax.

## 1.4 Feature and capability reporting {#part-1-4}

### 1.4.1 Required macros {#part-1-4-1}

A conforming implementation provides predefined macros for Objective-C 3.0
source:

- `__OBJC_VERSION__` (integer; at least `3`)
- `__OBJC3__` (defined to `1`)

### 1.4.2 Per-feature macros {#part-1-4-2}

A conforming implementation may provide per-feature macros:

- `__OBJC3_FEATURE_<NAME>__`

Examples:

- `__OBJC3_FEATURE_DEFER__`
- `__OBJC3_FEATURE_OPTIONALS__`
- `__OBJC3_FEATURE_THROWS__`
- `__OBJC3_FEATURE_ASYNC_AWAIT__`
- `__OBJC3_FEATURE_ACTORS__`

Feature macros report capability state. A recognized but unavailable feature is
reported as unavailable and remains rejected or reserved according to the
capability matrix.

### 1.4.3 Capability state reporting {#part-1-4-3}

Public reports use the capability states defined in
`docs/support/capability_matrix.md`:

- `implemented`
- `rejected`
- `reserved`
- `internal`

These states are the support vocabulary for Objective-C 3.0 public docs.
Internal rows may describe compiler decomposition, workflow bridges,
runtime-public-header ownership, or schema helpers. They do not claim public
language behavior without an implemented behavior row and evidence map entry.

### 1.4.4 `__has_feature` integration {#part-1-4-4}

A conforming implementation may expose Objective-C 3.0 features through a
`__has_feature`-style mechanism. If present, it aligns with per-feature macros
and capability states.

## 1.5 Conformance levels {#part-1-5}

### 1.5.1 Levels {#part-1-5-1}

Conformance is evidence-backed. A support claim is valid only when the
capability matrix points to the executable test, diagnostic, source, schema, or
documentation evidence that owns the claim.

Objective-C 3.0 v1 uses these public support states:

- **Implemented**: parser, semantic analysis, lowering, runtime, docs, and
  executable tests exist for the named behavior.
- **Rejected**: parser or semantic analysis emits a canonical diagnostic for the
  source form.
- **Reserved**: the syntax or concept remains unavailable and is documented as
  unavailable.
- **Internal**: the surface is an implementation helper, report shape, or
  workflow contract that is not public Objective-C 3.0 behavior.

### 1.5.2 Claiming support {#part-1-5-2}

A conforming implementation claims support through a machine-readable matrix and
human-readable evidence map. Local prose must not widen support beyond those
files.

Evidence map rows may cite source owners such as
`native/objc3c/src/runtime/public/objc3_runtime_api.h`,
`native/objc3c/src/runtime/public/objc3_runtime_result.h`,
`native/objc3c/src/io/json/`, or
`native/objc3c/src/artifacts/json/`. Those owner rows keep command, runtime, and
schema responsibility explicit without converting implementation surfaces into
new language features.

The machine-readable support contract is:

- capability data: `docs/support/capability_matrix.json`
- capability schema id: `objc3c-capability-matrix-v1`
- evidence data: `docs/support/evidence_map.json`
- evidence schema id: `objc3c-capability-evidence-map-v1`
- claim responsibility: `docs/support/capability_claim_responsibility.md`
- schema owner: `scripts/objc3c_shared/schema_registry.py`

Docs, spec prose, and site pages must not introduce a status state beyond
`implemented`, `rejected`, `reserved`, or `internal`.

### 1.5.3 Diagnostic escalation rule {#part-1-5-3}

Diagnostic severity can vary by toolchain profile, but severity changes must not
turn rejected behavior into accepted Objective-C 3.0 source.

## 1.6 Orthogonal checking modes {#part-1-6}

### 1.6.1 Strict concurrency checking {#part-1-6-1}

Concurrency checking may be an orthogonal analysis axis. The current public
support state for async and actor runtime closure is `reserved` unless the
capability matrix states otherwise.

A conforming implementation may provide an option equivalent to:

- `-fobjc3-concurrency=strict|off`

Enabling or disabling such checking must not weaken canonical diagnostics for
rejected non-concurrency source forms.

### 1.6.2 Required checking consistency rules {#part-1-6-2}

A conforming implementation applies checking modes as evidence-backed analysis
settings:

- no checking mode accepts rejected syntax,
- no checking mode advertises reserved behavior as implemented,
- profile claims map to capability states and evidence rows.

Conformance tests for checking consistency are defined in [Part 12](#part-12).

### 1.6.3 Performance checking {#part-1-6-3}

Implementations may provide additional diagnostics or assertions for [Part 9](#part-9)
features such as static regions and direct methods. If provided, those checks are
reported as capability-backed behavior.

## 1.7 Canonical source principles {#part-1-7}

### 1.7.1 No alternate Objective-C 3.0 source mode {#part-1-7-1}

Objective-C 3.0 source is canonical-only in this specification. Retired forms
are rejected or reserved according to the capability matrix.

Retired adapters, alternate language paths, retired-source lanes, old modes, and
success-without-evidence wording are not alternate Objective-C 3.0 support states.

### 1.7.2 Contained default changes {#part-1-7-2}

Default changes, such as nonnull-by-default regions, apply only inside
Objective-C 3.0 translation units and explicitly marked module boundaries.

### 1.7.3 Public header discipline {#part-1-7-3}

Headers that claim Objective-C 3.0 support use canonical spellings and published
capability gates. A header must not require a reader to infer support from
archived notes, private issue history, or file-level status comments.

## 1.8 Canonical diagnostics and fix-its {#part-1-8}

A conforming implementation provides diagnostics and optional fix-its for common
rejected forms.

Minimum diagnostic capabilities:

1. Reject retired null and boolean spellings when canonical spellings are
   required ([Part 3](#part-3)).
2. Reject unsupported ownership or capture forms and point at canonical spelling
   where one exists ([Part 8](#part-8)).
3. Reject unsupported cleanup forms and point at implemented `defer` or resource
   patterns where those are available ([Part 8](#part-8)).
4. Reject borrowed-pointer lifetime violations with a canonical lifetime
   diagnostic ([Part 8](#part-8)).
5. Report unavailable error and `Result` bridging surfaces as reserved until
   capability evidence marks them implemented ([Part 6](#part-6)).
6. Report unavailable async bridging overlays as reserved until capability
   evidence marks them implemented ([Part 11](#part-11)).

Fix-its are allowed only as canonicalization hints. They do not create another
accepted language surface.

## 1.9 Profiles {#part-1-9}

Objective-C 3.0 profiles are named bundles of additional restrictions, defaults,
and required library/runtime surfaces aimed at specific domains such as
`system`, `app`, or `freestanding`.

Profiles are selected by toolchain configuration, not by source-level language
switches. A conforming implementation may provide a mechanism equivalent to:

- `-fobjc-profile=<name>`

Profiles may:

- enable additional diagnostics as errors ([Part 12](#part-12)),
- require particular standard modules such as Concurrency ([Part 7](#part-7)),
- require additional module metadata preservation ([D](#d)),
- restrict unsafe constructs such as borrowed pointer escapes ([Part 8](#part-8)).

Profile contents are specified in `CONFORMANCE_PROFILE_CHECKLIST.md` and must
also resolve to capability states and evidence rows before they are public
support claims.

When emitting a machine-readable conformance report, implementations report the
selected profile set using the schema in [Part 12](#part-12).
