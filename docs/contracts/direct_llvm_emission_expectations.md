# Direct LLVM Emission Expectations (M135)

## Contract Metadata

- Contract ID: `objc3c-direct-llvm-emission/m135-v1`
- Owner issue: [#4268](https://github.com/doublemover/Slopjective-C/issues/4268)
- Related implementation issue: [#4264](https://github.com/doublemover/Slopjective-C/issues/4264)
- Scope: `.objc3` native compilation path in `objc3c-native`

## Expectations

| ID | Expectation |
| --- | --- |
| `M135-LLVM-01` | `.objc3` compilation emits deterministic `module.ll`, `module.obj`, `module.manifest.json`, and `module.diagnostics.json` artifacts from the native pipeline. |
| `M135-LLVM-02` | `.objc3` object emission uses direct LLVM-native object generation for production output. |
| `M135-LLVM-03` | Fallback path `RunIRCompile(...clang -x ir...)` is forbidden for `.objc3` production emission. |
| `M135-LLVM-04` | `--clang` remains scoped to non-`.objc3` compatibility (`.m`) and downstream link/test harness usage; it is not a `.objc3` object-emission dependency. |
| `M135-LLVM-05` | Emission failures are fail-closed with deterministic diagnostics and a non-zero exit code. |

## CI And Tooling Enforcement

The following checks are required to stay wired and fail-closed:

- `python scripts/check_m135_direct_llvm_contract.py`
- `npm run check:compiler-closeout:m135`
- `npm run check:task-hygiene`

The checker validates packet/docs presence and CI wiring so contract drift fails before milestone closeout.

## Validation Workflow

1. `npm run lint:md:all`
2. `npm run check:compiler-closeout:m135`
3. `npm run check:task-hygiene`
4. Execute the M135 native command set from `spec/planning/compiler/m135/m135_parallel_dispatch_plan_20260228.md`.
