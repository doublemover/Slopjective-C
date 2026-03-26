# M313 Testing Pyramid And Static Guard Retention Model Contract And Architecture Freeze Expectations (B001)

Contract ID: `objc3c-cleanup-testing-pyramid-static-guard-model/m313-b001-v1`

## Purpose

Freeze the validation pyramid that decides which surfaces remain as retained static guards, which surfaces must move into executable acceptance suites, and which patterns are prohibited going forward.

## Pyramid requirements

- Prefer executable subsystem suites as the primary truth surface.
- Treat runtime probes and native `.objc3` fixture corpora as executable inputs, not reduction targets.
- Allow retained static guards only for schema, inventory, policy, and publication checks that cannot be expressed truthfully through executable suites.
- Treat `scripts/check_*.py`, `scripts/run_*_readiness.py`, and `tests/tooling/test_check_*.py` as migration-only surfaces unless explicitly retained.
- Prohibit new milestone-local validation namespaces and one-off validation flows that bypass the shared acceptance model.

## Required machine-readable outputs

- pyramid layers and allowed evidence types
- retained static-guard classes
- migration-only validation classes
- prohibited patterns
- next-issue handoff to `M313-B002`
