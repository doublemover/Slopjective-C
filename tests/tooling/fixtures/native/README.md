# Native Fixture Catalog

Native fixtures are behavior-first test inputs for the Objective-C 3.0 hard
cutover. Positive fixtures prove canonical behavior only. Retired adapters,
alternate acceptance paths, retired-source lanes, unsupported-feature claims,
and runtime-dispatch behavior belongs in negative metadata with strict
diagnostic expectations.

## Fixture Families

- `execution/positive/`: runnable e2e fixtures that compile, link, and return a
  deterministic exit code through canonical behavior only.
- `execution/negative/`: compile, link, or run failures with sidecar
  `*.meta.json` diagnostic metadata.
- `execution/negative/unsupported_feature_claim_*.objc3`: parsed source
  surfaces that must fail closed as compile-stage runnable-claim rejections.
- `recovery/positive/`: canonical parser/sema/lowering recovery fixtures and IR
  expectations for accepted source forms.
- `recovery/negative/`: rejected recovery fixtures for parser and semantic
  diagnostics.
- `parser_split/`, `lexer_split/`, and `driver_split/`: fixture-only source
  surfaces for compiler boundary validation.
- `library_cli_parity/` and `parity_baseline/`: fixture data for native
  command/library parity and baseline comparison. These may record external
  observations only as provenance for canonical fixtures; they must not define
  interoperability, drop-in compatibility, fallback acceptance, or alternate
  success paths.

## Owner Split Boundaries

Large fixture surfaces are split by behavior owner before they can be cited as
coverage:

- parser owns canonical syntax acceptance and grammar rejections.
- sema owns typed diagnostics, unsupported-feature runnable-claim rejections,
  and conservative semantic analysis boundaries.
- lowering owns canonical lowering, ABI handoff, IR-shape, and strict link
  errors for required runtime symbols.
- runtime owns live dispatch/status failures and runtime-backed behavior.
- e2e owns deterministic compile-link-run positives.
- canonical_rejection owns legacy-looking, fallback-looking, shim-looking,
  unsupported, or compatibility-looking surfaces that must remain non-positive.

The `recovery/positive`, `execution/positive`, `execution/negative`, and
root-level `*.objc3` surfaces are intentionally mixed. Their owners are listed
in `fixture_family_catalog.json` so a positive-looking basename cannot create a
support claim outside the parser/sema/lowering/runtime/e2e or
canonical-rejection boundary.

Use `fixture_family_catalog.json` for machine-readable boundary ownership.
