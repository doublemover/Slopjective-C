# M271 Retainable C-Family Declaration Surface Core Feature Expansion Expectations (A003)

Contract ID: `objc3c-part8-retainable-c-family-source-completion/m271-a003-v1`

## Required behavior

- The compiler must admit callable `__attribute__((objc_family_retain(FamilyName)))`.
- The compiler must admit callable `__attribute__((objc_family_release(FamilyName)))`.
- The compiler must admit callable `__attribute__((objc_family_autorelease(FamilyName)))`.
- The compiler must admit canonical compatibility aliases on callables:
  - `os_returns_retained`, `os_returns_not_retained`, `os_consumed`
  - `cf_returns_retained`, `cf_returns_not_retained`, `cf_consumed`
  - `ns_returns_retained`, `ns_returns_not_retained`, `ns_consumed`
- The emitted frontend manifest must publish `frontend.pipeline.semantic_surface.objc_part8_retainable_c_family_source_completion`.

## Positive fixture proof

- `family_retain_sites = 1`
- `family_release_sites = 1`
- `family_autorelease_sites = 1`
- `compatibility_returns_retained_sites = 2`
- `compatibility_returns_not_retained_sites = 1`
- `compatibility_consumed_sites = 1`

## Non-goals

- This issue does not claim retainable-family legality.
- This issue does not claim ARC-family interop.
- This issue does not claim runtime retainable-family behavior.
