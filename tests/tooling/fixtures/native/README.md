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
  command/library parity and baseline comparison. These may describe external
  interoperability, but they must not claim drop-in compatibility.

Use `fixture_family_catalog.json` for machine-readable boundary ownership.
