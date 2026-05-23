# Semantic Analysis

Semantic support claims require canonical diagnostics or executable tests.
Rejected behavior is represented by diagnostics, not alternate acceptance paths.

Typed throws, value optionals, match expressions, and guarded match patterns are
not semantic support claims in the current slice. Parser/source-closure records
must keep those features fail-closed until ABI, interface preservation,
lowering, and runtime semantics exist for the specific feature family.

The semantic-analysis owner boundary is:

- `native/objc3c/src/sema/` owns semantic phase implementation,
- `tests/native/sema/types/typed_i32_bool_flow.objc3` owns the current typed
  flow support claim,
- `tests/tooling/test_objc3c_parser_contract_sema_integration.py` and
  `tests/conformance/diagnostics/manifest.json` own canonical rejection
  evidence.

Semantic analysis may feed lowering, runtime metadata, and artifact summaries,
but those summaries are not public support claims unless the capability matrix
links them to evidence. A diagnostic row is still a support boundary row; it says
the implementation rejects the source form, not that another source spelling is
accepted.
