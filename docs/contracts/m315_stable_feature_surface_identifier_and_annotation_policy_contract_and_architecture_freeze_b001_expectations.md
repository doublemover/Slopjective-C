# M315-B001 Expectations

Contract ID: `objc3c-cleanup-stable-feature-surface-identifier-policy/m315-b001-v1`

`M315-B001` freezes the stable non-milestone identifier scheme that later `M315`
implementation work must use when removing milestone-coded residue from product and
generated source-of-truth surfaces.

The policy must:
- define one durable identifier grammar for product, runtime, IR, artifact, fixture,
  and report surfaces;
- prohibit milestone-coded identifiers such as `m315-b001`, `m228-d002`, lane tags,
  and issue-number fragments inside durable identifiers;
- distinguish durable identifiers from archival historical references so planning
  history can remain in planning/docs without leaking into product code or emitted
  evidence;
- define a bounded set of annotation families that later issues can apply consistently.

The frozen identifier grammar for durable surfaces is:
- `objc3c.<domain>.<subsystem>.<surface>.vN`
- lowercase ASCII dot-separated segments
- terminal version segment required
- no milestone, lane, or issue tokens in any segment

This issue must hand off product-code provenance placement to `M315-B002`, native
source removal work to `M315-B003`, and source-of-truth enforcement to `M315-C001`.
