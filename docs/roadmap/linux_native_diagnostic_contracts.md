# Native allocator and diagnostic publication contracts

Current execution checkpoint: the maintained native frontend replay, identity and
consumer controls pass in the ABI 24 development embedding. See the
[2.110 consolidation](linux_publication_and_byte_ranges.md). Source-free package
execution, wider matrices and installed SDK qualification remain pending.

The current source span extends published native declaration contracts to
`ownership_returns`, `ownership_takes`, `ownership_holds`,
`argument_with_type_tag`, `pointer_with_type_tag`, `type_tag_for_datatype`,
`format_matches` and `diagnose_as_builtin`.

Interface identity binds each ownership operation, family and parameter set;
the argument/tag indices and pointer/value policy; the tag's matching C type and
layout/null flags; the expected format expression; and the builtin plus its
ordered source-argument mapping. Native printing retains these attributes on
ordinary declarations, inherited redeclarations and template members. Matching
types and format expressions undergo publication dependency checks, so a public
tag cannot silently depend on an excluded private type.

`objc3.native-declaration-attributes.v2` is a hard capability cutover. Existing
native declaration attributes use the same current contract. Active readers,
package qualification and paired codec fuzzing use v2; historical v1 acceptance
receipts retain their original identity.

The new fixture parses in all six C17/C++20/C++23 ARC/MRC modes. Compiler/frontend
syntax checks pass. Maintained frontend controls cover source replay and
relocation, payload changes, dependent tag types, inherited allocator contracts,
older-reader rejection, private type dependencies and independent valid/invalid
source-free consumers. Type-tag and format mismatches must still diagnose after
publication. Frontend execution, matched package verification and final SDK
acceptance remain pending. SDK 2.62 is unchanged.
