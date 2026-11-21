# Bounded borrowed-result proofs for static storage

Source 2.148 / System 2.74 extends borrowed-result precision to immutable native
union storage and statically bound helper chains. Union fields must all retain
the existing immutable, non-resource, non-borrowed, nonvolatile and trivial
lifetime requirements. Native active-member and pointer-access rules still apply.
No live or expired owner fact is erased by a static-storage proof.

When ordinary constant evaluation cannot resolve a call, the mandatory frontend
pass may inspect a statically bound definition containing exactly one return
expression. Internal functions and C++ inline definitions can compose known
static addresses, offsets and conditional alternatives. Every alternative must
prove a permanent immutable base. Parameter-derived or unknown addresses,
mutable/TLS/automatic bases and multi-statement bodies retain their diagnostics.
Nonstatic methods, variadic functions, weak/weakref/alias/ifunc entries and
replaceable external functions cannot supply this summary. C external-inline
bodies are excluded because their external definitions can differ.

A shared 4096-step budget bounds expression and storage-type traversal, with a
32-level depth limit. Recursive or exhausted summaries fail admission. The proof
runs during the final diagnostic pass instead of every CFG fixed-point iteration.
It describes returned storage; arbitrary mutation/callback effects remain a
separate analysis requirement.

Published owner-contracted borrowed result expressions require
`objc3.borrowed-result-proofs.v1`. Canonical identity, source-free capability
selection and both production validators agree. Missing/invented claims and
readers without the capability reject. Declaration-only signatures keep their
existing ownership contract without claiming implementation-body proofs.

All 56 borrowed compiler regressions pass, including eighteen expanded
C17/C++20/C++23 ARC/MRC source/PCH commands. Six publication profiles preserve
union/helper identity after relocation and reparse and retain valid/escaping
consumer diagnostics. Nine adjacent frontend groups and 660 exact production
decoder replays across 33 families pass. The maintained source-free native
witness removes its provider before compiling two consumers and verifies helper
results in four ASan/UBSan configurations; the three Objective-C++ configurations
run Strict System checks, alongside the ordinary C++ control.

The [receipt](../support/linux_static_borrowed_summaries_progress.json) records
focused results. Source 2.147 retains the complete 546-case compiler sweep.
Full matched analysis and one SDK follow remaining feature implementation.
