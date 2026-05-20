# Objective-C 3 Language And Runtime Threat Model

This runbook is the human-readable companion to the checked-in language/runtime
threat model and mitigation backlog contract.

Canonical source truth:

- `tests/tooling/fixtures/security_hardening/language_runtime_threat_model_backlog.json`
- `tests/tooling/fixtures/security_hardening/macro_package_provenance_trust_policy.json`
- `tests/tooling/fixtures/security_hardening/macro_supply_chain_trust_registry.json`
- `tests/tooling/fixtures/security_hardening/runtime_hardening_contract.json`
- `tests/tooling/fixtures/security_hardening/sanitizer_validation_contract.json`

The public validation action is:

- `npm run objc3c -- check-security-language-runtime-threat-model`

The validation action publishes:

- `tmp/reports/security-hardening/language-runtime-threat-model-summary.json`

## Scope

This model covers language-level macro/provenance risk, compiler frontend
security risk, runtime object/dispatch/memory risk, and the mitigation backlog
needed to keep those claims tied to checked-in evidence.

It does not claim hostile macro execution safety, hosted package signing,
remote key custody, signed installer trust, or native sanitizer execution on
every platform. Those remain explicit non-claims until separately implemented
and validated by their owning tracks.

## Closure Rule

The threat model is coherent only when every threat row maps to checked-in macro
supply-chain, runtime-hardening, compiler/sanitizer, or release evidence, and
every backlog row maps back to at least one threat. Generated summaries are
evidence indexes only; they do not create support on their own.
