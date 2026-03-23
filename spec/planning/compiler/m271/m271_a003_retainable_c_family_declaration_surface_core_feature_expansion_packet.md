# M271-A003 Packet: Retainable C-Family Declaration Surface - Core Feature Expansion

## Goal

Complete the remaining Part 8 frontend/source-model callable surface for retainable C-family annotations and compatibility aliases.

## Scope

This issue is parser/frontend only.
It does not claim:
- retainable-family legality,
- ARC-family interop,
- runtime retainable-family behavior.

## Required packet

The emitted frontend manifest must publish:
- `frontend.pipeline.semantic_surface.objc_part8_retainable_c_family_source_completion`

## Positive fixture proof

The positive fixture must prove deterministic publication for:
- `family_retain_sites = 1`
- `family_release_sites = 1`
- `family_autorelease_sites = 1`
- `compatibility_returns_retained_sites = 2`
- `compatibility_returns_not_retained_sites = 1`
- `compatibility_consumed_sites = 1`
