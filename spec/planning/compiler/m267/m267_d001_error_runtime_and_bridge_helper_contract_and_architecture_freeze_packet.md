# M267-D001 Error Runtime And Bridge-Helper Contract And Architecture Freeze Packet

Issue: `#7277`
Packet: `M267-D001`
Milestone: `M267`
Lane: `D`
Dependencies:
- `M267-C002`
- `M267-C003`

Goal:
Freeze the first truthful private runtime helper ABI for the runnable Part 6 slice and prove that lowering now routes through that helper surface.

Required implementation:

- add the private runtime helper cluster for thrown-error slot store/load, status bridge normalization, `NSError` bridge normalization, and catch-kind matching
- keep the helper ABI private to `objc3_runtime_bootstrap_internal.h`
- add a private testing snapshot for deterministic runtime probe evidence
- publish explicit IR boundary anchors for the helper contract
- keep generalized foreign exception ABI and public error-runtime headers deferred

Validation:

- static contract/checker coverage
- positive Part 6 fixture compile with helper call-site inspection
- runtime probe for helper entrypoints and snapshot state

Evidence path:

- `tmp/reports/m267/M267-D001/error_runtime_bridge_helper_contract_summary.json`

Next issue:

- `M267-D002` is the next issue after this freeze lands.
