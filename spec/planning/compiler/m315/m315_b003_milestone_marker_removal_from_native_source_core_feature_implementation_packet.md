# M315-B003 Planning Packet

Issue: `#7798`  
Title: `Milestone-marker removal from native source`

## Objective

Execute the first native-source cleanup sweep by removing the leading milestone
anchor markers from code comments while deliberately deferring runtime-visible
contract strings and generated-truth payload content to later `M315` issues.

## Implemented scope

- Remove leading `// MNNN-LNNN ...` comment markers from native code files.
- Keep the rest of the comment text intact.
- Do not rewrite durable emitted contract ids in this issue.
- Do not rewrite generated JSON snippets or deferred edge strings in this issue.

## Frozen post-state

- Leading comment markers remaining in native code files: `0`
- Remaining native-code `mNNN-LNNN` matches after the sweep: `495`
- Rewritten native code files: `24`

## Why this boundary is correct

Changing runtime-visible contract ids here would create avoidable regression risk
before `M315-C001` and `M315-C002` freeze the new source-of-truth and authenticity
schema boundaries. This issue therefore removes only the purely comment-anchor layer
and leaves the remaining stable-id migration to the later scoped issues.

## Downstream ownership

- `M315-B004`: IR fixture compatibility semantics
- `M315-B005`: remaining comment / constexpr / contract-string edge sweep
- `M315-C001`: source-of-truth and generated-artifact contract
- `M315-C002`: artifact authenticity schema and evidence contract

## Validation posture

Static verification is justified because this issue is a bounded source rewrite.
The checker must prove that leading native-code comment markers are zero and that
all remaining milestone matches belong to the later-owner slice.

Next issue: `M315-B004`.
