# M315-B004 Expectations

Contract ID: `objc3c-cleanup-ir-fixture-compatibility-semantics/m315-b004-v1`

`M315-B004` freezes and enforces the current compatibility semantics for tracked
`.ll` fixtures.

The implementation must:
- classify the two `library_cli_parity` `.ll` files as explicit synthetic fixtures;
- classify the `tests/tooling/fixtures/objc3c/**/replay_run_*/module.ll` files as
  generated-replay candidates rather than fully proven genuine artifacts;
- require explicit synthetic labeling for the synthetic fixture class;
- record the legacy replay-header gap instead of pretending it does not exist.

The frozen baseline for this issue is:
- `78` tracked `.ll` files total;
- `2` explicit synthetic `.ll` fixtures;
- `76` replay-candidate `.ll` fixtures;
- `30` replay `.ll` files with the `objc3c native frontend IR` header;
- `46` replay `.ll` files still missing that header and therefore carrying a
  legacy compatibility debt owned by later provenance work.
