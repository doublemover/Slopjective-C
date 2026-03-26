# M315-A001 Expectations

Contract ID: `objc3c-cleanup-repo-wide-milestone-residue-inventory/m315-a001-v1`

`M315-A001` freezes the repo-wide baseline for milestone-coded residue before the
source-hygiene cleanup begins in earnest.

Expected outcomes:

- One machine-readable inventory exists under `spec/planning/compiler/m315/`.
- The inventory captures repo-wide counts by scope, hotspot file, and milestone
  family rather than only the narrow `native/objc3c/src` slice.
- The inventory explicitly hands off the native-source slice to `M315-A002` and
  the artifact-authenticity slice to `M315-A003`.
- The inventory makes later scope boundaries legible so downstream issues can
  remove residue instead of arguing about where it lives.

Boundary notes:

- This issue freezes the baseline; it does not remove residue yet.
- Historical planning/spec/test/tooling surfaces remain in scope for inventory
  purposes even if later cleanup prioritizes product and generated surfaces.
