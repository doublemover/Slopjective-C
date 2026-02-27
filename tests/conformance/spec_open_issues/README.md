# Spec Open Issues Seeded Conformance Fixtures (Issue #167)

This directory contains deterministic fixtures and traceability data for seeded
Part 0/3 closure obligations (`B-11`).

## Implemented fixture families

- `P0-P3-MANIFEST-*`
- `P0-P3-AMSYNC-*`
- `P0-P3-OPT-*`
- `P0-P3-MANGLE-*`
- `P0-P3-REIFY-*`

Each family has one `required-pass` and one `required-fail` fixture.

## Deterministic checks

```sh
rg -n "^S1-P0-AMSYNC|^S1-P0-MANIFEST|^S1-P3-MANGLE|^S1-P3-OPT|^S1-P3-REIFY" tests/conformance/spec_open_issues/P0-P3-seed_traceability.csv
rg -n "required-pass|required-fail" tests/conformance/spec_open_issues/P0-P3-seed_traceability.csv
rg -n "\"id\": \"P0-P3-.*\"|\"signal\":" tests/conformance/spec_open_issues
```
