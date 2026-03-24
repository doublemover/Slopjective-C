# M273 Synthesized AST and IR Emission for Derives, Macros, and Behaviors Expectations (C002)

Contract ID: `objc3c-part10-synthesized-ast-ir-emission/m273-c002-v1`

Surface key: `objc_part10_synthesized_ast_and_ir_emission`

Expected proof:
- supported derives lower into real synthesized method bodies in emitted IR
- supported macro callables lower into replay-visible emitted globals
- supported property behaviors lower into replay-visible emitted globals
- emitted object symbols prove those artifacts survive object emission
- evidence lands at `tmp/reports/m273/M273-C002/synthesized_ast_ir_emission_summary.json`
