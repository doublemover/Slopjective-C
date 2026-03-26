# M315-B005 Expectations

Contract ID: `objc3c-cleanup-native-source-edge-sweep/m315-b005-v1`

`M315-B005` completes the native-source edge sweep after the broad B003 marker
removal and the B004 IR-fixture classification work.

The implementation must:
- rewrite milestone-coded contract identifiers in native product code to the
  stable dotted identifier form defined by `M315-B001`;
- strip milestone-era prose from native-source comments and descriptive string
  models where the string is product-local rather than a later C-owned
  source-of-truth field;
- reduce the milestone-token scan over `native/objc3c/src` to the explicit
  residual classes that `M315-C001` and `M315-C004` still own;
- fail if any milestone-coded residue remains outside those named residual
  classes.

The frozen post-sweep baseline for this issue is:
- milestone-token lines after `M315-B004`: `289`;
- milestone-token lines after the B005 sweep: `103`;
- removed milestone-token lines: `186`;
- disallowed residual lines: `0`.

Allowed residual classes at closeout:
- `legacy_fixture_path_reference`: `6` lines, owned by `M315-C004`;
- `legacy_m248_surface_identifier`: `34` lines, owned by `M315-C001`;
- `dependency_issue_array`: `3` lines, owned by `M315-C001`;
- `next_issue_schema_field`: `40` lines, owned by `M315-C001`;
- `issue_key_schema_field`: `8` lines, owned by `M315-C001`;
- `transitional_source_model`: `12` lines, owned by `M315-C001`.
