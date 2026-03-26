# M315-E002 Planning Packet

Issue: `M315-E002`
Title: `De-scaffolding and authenticity closeout matrix`
Contract ID: `objc3c.cleanup.de-scaffolding.authenticity.closeout.matrix/m315-e002-v1`

## Problem
`M315` has accumulated the full cleanup chain, but the milestone still needs one final matrix that proves the cleanup is coherent across all lanes instead of only within issue-local pockets.

## Decision
- rerun every predecessor `M315` checker from lane A through lane E;
- publish one closeout summary that records:
  - predecessor return codes;
  - the final B005 residual-class set and match count;
  - the D001 historical zero-target contract and current zero live counts;
  - the D002 live anti-noise guard state;
- fail closed if any predecessor drift reappears or if the final closeout state widens beyond the frozen residual boundary.

## Acceptance proof
- all predecessor checkers pass in one E002 closeout run;
- the closeout summary preserves the exact final residual boundary;
- the closeout matrix leaves `M316` as the next cleanup-first milestone.

Next milestone: `M318`.
Next issue outside this milestone: `M318-A001`.