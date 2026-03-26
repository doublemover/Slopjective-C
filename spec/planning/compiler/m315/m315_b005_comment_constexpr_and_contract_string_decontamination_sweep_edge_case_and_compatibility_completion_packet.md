# M315-B005 Planning Packet

Issue: `#7800`  
Title: `Comment constexpr and contract-string decontamination sweep`

## Objective

Complete the native-source edge sweep so milestone-era residue no longer leaks
through ordinary comments, local constexpr labels, or product-local contract
strings, while leaving only the explicit source-of-truth and fixture debts that
the later C-lane issues already own.

## Implemented semantics

- rewrite milestone-coded native contract identifiers from the historical
  `objc3c-.../mNNN-LNNN-vN` shape to the stable dotted
  `objc3c....vN` shape;
- remove milestone-era lead-in prose from native comments where the text is
  explanatory rather than archival;
- normalize residual deferred-model strings so they no longer encode milestone
  numbers when the string is only a local implementation model;
- preserve only the residual classes that are still truthful handoff debt for:
  - `M315-C001`: generated/source-of-truth issue-schema rewrite and stable
    residual identifier completion
  - `M315-C004`: legacy fixture-path relocation and relabeling

## Frozen baseline

- milestone-token scan scope:
  - `native/objc3c/src/**`
  - excluding `**/*.md`
  - excluding `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h`
- milestone-token lines after `M315-B004`: `289`
- milestone-token lines after `M315-B005`: `103`
- disallowed residual lines after `M315-B005`: `0`

## Residual classes handed to later issues

- `legacy_fixture_path_reference` -> `M315-C004`
- `legacy_m248_surface_identifier` -> `M315-C001`
- `dependency_issue_array` -> `M315-C001`
- `next_issue_schema_field` -> `M315-C001`
- `issue_key_schema_field` -> `M315-C001`
- `transitional_source_model` -> `M315-C001`

## Validation posture

Static verification is justified because this issue freezes one narrow native
source-hygiene boundary and proves that every remaining milestone token in the
scan is an explicitly named residual class rather than accidental noise.

Next issue: `M315-C001`.
