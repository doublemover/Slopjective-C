# Parser Draft Syntax Surface

- Contract: `objc3c.parser.draft-syntax-surface.v1`
- Status: `PASS`
- Issue: `#8011`
- Positive fixture: `tests/tooling/fixtures/native/recovery/dispatch/parser_draft_syntax_surfaces.objc3`
- Negative fixture: `tests/tooling/fixtures/native/recovery/negative/negative_parser_draft_syntax_macro_payload.objc3`
- Source truth avoids tmp: `True`

## Checks
- `ast_fields`: `PASS`
- `parser_anchors`: `PASS`
- `ast_builder`: `PASS`
- `parser_contract`: `PASS`
- `artifact_manifest`: `PASS`
- `positive_fixture`: `PASS`
- `negative_fixture`: `PASS`
- `readme`: `PASS`

## Validation Commands
- `npm run test:objc3c:fixture-matrix`
- `npm run test:objc3c:negative-expectations`
- `npm run test:objc3c:execution-replay-proof`
