# Native upstream evidence terminal state

The Source 2.153 campaign completed every producer and consumer job, including
ASan/UBSan products, coverage, fuzzing and the repaired frontend consumer. Its
terminal union rejected the upstream selection receipt because the producer
recorded the selected suites and tool hashes without a terminal status, while
the independent evidence reader requires `PASS`.

The upstream owner now writes `RUNNING` before preparation, validates the actual
lit report, then writes `PASS`. Preparation, command and invalid-report failures
write `FAIL`; interrupted executions cannot retain a passing selection. The
reader independently checks the exact selected suite inventory and tool hashes.
Regression controls exercise the actual producer/reader boundary, failed native
commands, empty/unsupported-only reports, incomplete selections and tool sets.

The focused native assurance tests pass. This repairs the delivery mechanism;
it does not promote the older archive or qualify subsequent feature commits.
The next full campaign must build and seal the current implementation.
