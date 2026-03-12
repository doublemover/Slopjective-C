# M266 Defer, Guard, Match, And Pattern Source Closure Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-part5-control-flow-source-closure/m266-a001-v1`

Frontend semantic-surface path:

- `frontend.pipeline.semantic_surface.objc_part5_control_flow_source_closure`

Required truths:

- `guard let` / `guard var` remains admitted as parser-owned control-flow syntax
- `switch` / `case` remains the currently supported pattern carrier in the live parser slice
- `defer` is reserved as a frontend keyword and fails closed with `unsupported 'defer' statement [O3P154]`
- `match` is reserved as a frontend keyword and fails closed with `unsupported 'match' statement [O3P155]`
- the frontend emits one deterministic Part 5 source-closure packet instead of scattering this boundary across docs and parser heuristics

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m266-a001-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m266_part5_control_flow_source_closure_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m266/a001/positive --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m266_defer_statement_fail_closed_negative.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m266/a001/defer --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m266_match_statement_fail_closed_negative.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m266/a001/match --emit-prefix module --no-emit-ir --no-emit-object`

Expected positive summary facts:

- `guard_binding_sites = 1`
- `guard_binding_clause_sites = 1`
- `switch_case_pattern_sites = 2`
- `switch_default_pattern_sites = 1`
- `defer_keyword_sites = 0`
- `match_keyword_sites = 0`

Expected fail-closed diagnostics:

- defer row: `unsupported 'defer' statement [O3P154]`
- match row: `unsupported 'match' statement [O3P155]`
