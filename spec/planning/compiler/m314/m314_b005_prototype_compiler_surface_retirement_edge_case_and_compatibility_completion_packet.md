# M314-B005 Planning Packet

## Summary

Retire the dead Python prototype compiler surface now that the public command
surface, unified runner, and compatibility-window registry are in place.

## Implementation shape

- Move the last tracked prototype compiler source out of `compiler/` and into a
  clearly retired governance archive path.
- Make the archived copy non-operational by storing it as text rather than a
  live Python module.
- Publish one machine-readable retirement registry with the archive path, the
  supported compiler root, and the allowed historical-reference policy.
- Update operator-facing docs and package metadata so the repo presents one
  unambiguous compiler implementation path: `native/objc3c/`.

## Non-goals

- Do not erase historical planning evidence that froze the original path.
- Do not remove compatibility aliases yet.
- Do not rewrite the broader workflow/API contract; `M314-C001` owns that.

## Evidence

- `spec/planning/compiler/m314/m314_b005_prototype_compiler_surface_retirement_edge_case_and_compatibility_completion_registry.json`
- `spec/governance/retired_surfaces/compiler_objc3c/semantic.py.txt`
- `spec/governance/retired_surfaces/compiler_objc3c/README.md`

Next issue: `M314-C001`.
