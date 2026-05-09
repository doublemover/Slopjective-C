# Native Recovery Fixture Boundaries

`recovery/positive` and `recovery/negative` are mixed parser, sema, lowering,
IR, runtime, and canonical-rejection fixture surfaces. The directory name does
not create a generic support owner.

## Positive Surface

Positive recovery fixtures are phase provenance only:

- parser-owned positives cover canonical syntax, literal spelling, selector
  shape, and statement-shape recovery.
- sema-owned positives cover type flow, alias signatures, assignment,
  increment/decrement, compound assignment, and return-path behavior.
- lowering-owned positives cover canonical lowering, entrypoint ABI handoff,
  nil elision, short-circuit lowering, and unary-plus lowering.
- IR-owned expectations cover paired `*-ir.expect.txt` files and
  accepted-source IR-shape provenance.
- runtime-owned positives cover live-dispatch and fast-path dispatch evidence
  only when the canonical runtime entrypoint remains required.

The e2e owner does not get a positive claim from this tree; executable success
belongs in `execution/positive`.

## Negative Surface

Negative recovery fixtures are canonical rejection or strict diagnostic
evidence. Parser rejections own `O3P*`, sema rejections own `O3S*`, and
runtime-dispatch residues stay non-positive unless the canonical runtime owner
indexes them as strict errors.

Legacy-looking, fallback-looking, shim-looking, unsupported, or
compatibility-looking cases must remain rejection metadata or be absent from the
positive surface.
