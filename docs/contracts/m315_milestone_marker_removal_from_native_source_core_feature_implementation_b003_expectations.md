# M315-B003 Expectations

Contract ID: `objc3c-cleanup-native-source-marker-removal/m315-b003-v1`

`M315-B003` performs the low-risk core sweep in `native/objc3c/src`:
- remove leading milestone-anchor comment markers from code files;
- leave runtime-visible contract strings and generated-source truth identifiers
  untouched for later `M315-C*` and `M315-B005` work;
- prove the native code slice no longer carries `// MNNN-LNNN ...` anchors.

This issue is intentionally narrower than full decontamination. It must not claim
that all milestone-coded strings are gone from native source. It must only prove
that the leading comment-marker layer has been removed and that the remaining
residue is now the smaller contract-string / edge-string slice owned by later
issues.

The frozen post-state for this issue is:
- `0` leading comment markers in native code files;
- `495` remaining `mNNN-LNNN` matches across native code files;
- `24` native code files rewritten by the sweep.
