# Native reference and record captures

Source 2.236 / System 2.162 extends closed callable side-effect graphs to native
reference captures, trivial records and fixed arrays. Scalar fields, nested
records/arrays and native converting construction retain demonstrated borrowed
and resource inputs. Mutable operators preserve the union of known old/new
values. Unproved trivial-record factories retain known inputs conservatively;
this does not establish a factory ownership or side-effect summary.

Reference-captured writes reach original caller storage. Capture creation alone
does not escape that storage when the complete local callable graph is admitted.
The final scope pass checks every closure storage owner, including copies,
aggregate containers, nested closures and reference aliases. If any owner escapes
or changes, the original reference-captured storage escapes conservatively.
Escaping a copied enclosing aggregate cannot hide the shared reference.
Structured-binding cells resolve to their underlying decomposition storage.

Record shape admission requires trivial copyability and bounded recursive
by-value fields/bases. Reference subobjects, retained native objects/Blocks,
volatile/atomic fields, arbitrary special members and open receiver state remain
outside this proof. Shape discovery has 128 work items and depth 16; receiver,
call-graph, cell and storage-owner bounds remain independent. Discovery exhaustion
retains conservative provenance. This does not supply general MRC/foreign
reference counts, reference-result capture lifetimes or implicit union cleanup.

Maintained compiler controls cover ordinary/reference/record copies, converting
constructors, native record factories, nested records, fixed arrays, mutable
fields, reference bindings, source-free helpers, resource numbers, enclosing
aggregate/closure escapes and shape exhaustion. All 504 new/selected adjacent
source/PCH commands, six expanded source-free ARC/MRC configurations and 100
focused tooling checks pass. The preceding compiler misses five new transfer
diagnostics and rejects two ordinary closed-reference uses. The full isolation
regression is tracked separately until it finishes. Matching SDK, complete
profiles and native ARM acceptance remain pending.
