# M315-E001 Planning Packet

Issue: `M315-E001`
Title: `Source-hygiene and proof-hygiene gate`
Contract ID: `objc3c.cleanup.source-hygiene.proof-hygiene.gate/m315-e001-v1`

## Problem
`M315-D002` makes the anti-noise policy live, but milestone closeout still needs one explicit gate that states what the repo is allowed to claim after the sweep:
- the two D001 zero-target classes are gone from live product code;
- only the quarantined residual classes remain in the B005 native-source sweep;
- the synthetic parity fixture stays explicit and fenced;
- the D002 workflow keeps the guard live.

## Decision
- freeze the gate over the live D001, D002, and B005 summaries;
- require the D002 workflow and package entrypoints to stay in place;
- fail closed if any zero-target residue returns, if the remaining residual-class set drifts, or if the retired compiler surface reappears.

## Acceptance proof
- the gate checker reruns the D001 and D002 checkers and consumes their summaries;
- the gate summary records the remaining residual class set and the zero current counts for the D001 historical targets;
- the gate only unlocks `M315-E002` once the live state matches the frozen closeout boundary.

Next issue: `M315-E002`.