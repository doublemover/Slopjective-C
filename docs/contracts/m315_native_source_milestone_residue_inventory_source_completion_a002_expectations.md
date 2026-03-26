# M315-A002 Expectations

Contract ID: `objc3c-cleanup-native-source-milestone-residue-inventory/m315-a002-v1`

`M315-A002` narrows the repo-wide residue baseline to the concrete native-source
surface that later `M315` cleanup issues must edit.

Expected outcomes:

- One machine-readable native-source inventory exists under
  `spec/planning/compiler/m315/`.
- The inventory breaks native residue down by subdirectory, file kind, hotspot
  file, and code-only hotspot file.
- The inventory makes it explicit that embedded docs inside
  `native/objc3c/src/` dominate the raw count but that several code files still
  carry triple-digit residue counts.
- The inventory hands the main removal work to `M315-B003` and `M315-B005`.
