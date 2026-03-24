# M272-C002 Packet: Direct, Sealed, And Final Dispatch Lowering - Core Feature Implementation

Objective:
- implement the first truthful live Part 9 lowering slice rather than carrying only the `M272-C001` contract packet

Implementation requirements:
- lower effective `objc_direct` sends on concrete `self` and known class receivers as exact LLVM direct calls
- preserve `objc_dynamic` opt-out behavior on the runtime dispatch entrypoint
- widen emitted method-list payloads so direct/final intent survives into metadata
- widen emitted class/metaclass descriptor payloads so final/sealed intent survives into metadata

Validation requirements:
- one deterministic positive fixture
- issue-local checker
- issue-local pytest
- lane-C readiness replay
- stable evidence under `tmp/reports/m272/M272-C002/`
