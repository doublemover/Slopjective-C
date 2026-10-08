# Parser Conformance Corpus

This corpus is fixture metadata for canonical Objective-C 3.0 parser behavior.
Accepted cases must use canonical grammar. Rejected cases describe the strict
diagnostic contract for removed declaration spellings, malformed punctuation,
noncanonical type syntax, or reserved language-evolution syntax.

The manifest is intentionally behavior-first: a rejected legacy-looking spelling
is not a retired-source path or alternate parser acceptance path. It is a
hard-cutover diagnostic expectation.
