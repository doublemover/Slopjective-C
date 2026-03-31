# Validation CI Topology Integration

- issue: `M313-D002`
- status: `PASS`
- failure_count: `0`

## Aggregate entrypoints
- `test:fast` -> `test-fast`
  - validation_tier: `fast`
  - family_count: `5`
  - guarantee_owner: `runtime acceptance, canonical replay, and a bounded smoke slice`
- `test:objc3c:full` -> `test-full`
  - validation_tier: `full`
  - family_count: `12`
  - guarantee_owner: `smoke, runtime acceptance, and replay without full recovery fan-out`
- `test:objc3c:nightly` -> `test-nightly`
  - validation_tier: `nightly`
  - family_count: `20`
  - guarantee_owner: `full validation plus performance governance reporting, release-foundation publication, conformance corpus indexing, recovery, and broad corpus sweeps`

Next issues: `M313-D003`, `M313-E001`
