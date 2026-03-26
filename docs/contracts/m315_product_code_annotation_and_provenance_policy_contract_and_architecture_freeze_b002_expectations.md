# M315-B002 Expectations

Contract ID: `objc3c-cleanup-product-code-annotation-provenance-policy/m315-b002-v1`

`M315-B002` freezes where stable annotations and provenance markers may live in
product code and generated source-of-truth surfaces.

The policy must:
- allow stable `surface_id` markers only where they materially help identify durable
  runtime, lowering, IR, or manifest surfaces;
- prohibit milestone, lane, and issue tokens in product-code comments, constexpr
  labels, and generated truth surfaces;
- define the allowed provenance classes for fixtures, samples, reports, and replay
  artifacts;
- distinguish product-code annotations from archival historical references.

The resulting boundary must be precise enough that `M315-B003` and `M315-B005` can
remove residue mechanically, while `M315-C002` and `M315-C003` can enforce provenance
semantics on tracked artifacts and regenerated replay outputs.
