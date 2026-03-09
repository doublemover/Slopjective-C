# M256 Inheritance, Override, and Realization Legality Core Feature Expansion Expectations (B004)

- Contract ID: `objc3c-inheritance-override-realization-legality/m256-b004-v1`
- Issue: `#7135`
- Milestone: `M256`
- Lane: `B`
- Summary path:
  `tmp/reports/m256/M256-B004/inheritance_override_realization_legality_summary.json`

## Required behavior

1. Realized classes fail closed when a declared superclass interface is missing.
2. Realized classes fail closed when the superclass chain contains a cycle.
3. Realized classes fail closed when a superclass interface exists but the
   superclass implementation needed for runtime realization is absent.
4. Realized-class overrides fail closed when an inherited method signature is
   incompatible.
5. Realized-class overrides fail closed when selector-kind drift changes
   method kind across the superclass chain.
6. Realized-class inherited properties fail closed when the subclass property
   signature is incompatible with the inherited property.
7. The legality pass emits deterministic diagnostics under `O3S220`.
8. Parser and IR remain source/proof only for this rule set. Sema owns the live
   legality decision.

## Proof corpus

The B004 proof corpus must include at least:

- one positive realized-class inheritance fixture
- one missing-superclass negative fixture
- one inheritance-cycle negative fixture
- one incompatible override-signature negative fixture
- one selector-kind-drift negative fixture
- one missing-realized-superclass negative fixture
- one incompatible inherited-property negative fixture
