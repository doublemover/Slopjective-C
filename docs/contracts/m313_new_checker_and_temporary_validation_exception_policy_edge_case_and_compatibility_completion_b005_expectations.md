# M313 New Checker And Temporary Validation Exception Policy Edge Case And Compatibility Completion Expectations (B005)

Contract ID: `objc3c-cleanup-new-checker-temporary-validation-exception-policy/m313-b005-v1`

## Purpose

Freeze the exception policy that governs the rare cases where new checker-style or temporary validation surfaces may still be introduced during the cleanup program.

## Exception requirements

- Default to no new milestone-local checker, readiness, or pytest-wrapper surface without a recorded exception.
- Keep exceptions time-bounded, owned, and linked to a concrete replacement target.
- Require a machine-readable exception registry even when the registry is empty.
- Keep shared acceptance suites, shared harness flows, and retained static guards exception-free by default.
- Hand off a stable exception/waiver contract to `M313-C001` so later artifact and bridge work consumes one consistent policy.

## Required machine-readable outputs

- explicit allowed-without-exception classes
- prohibited-without-exception classes
- required exception record fields
- empty-by-default exception registry
- next-issue handoff to `M313-C001`
