# Native parameter and callable contracts on Linux

Source **2.93** adds native parameter and callable attribute publication on top
of the [2.92 public template projections](linux_cxx_unnamed_template_projections.md).
The [source receipt](../support/linux_native_parameter_contracts_progress.json)
records the completed checks; SDK **2.62** remains unchanged.

Static and dynamic `pass_object_size` parameters retain their size mode and
hidden native ABI arguments and distinct overload USRs. `lifetimebound` and `noescape` retain Clang's native
parameter metadata. Callback encodings preserve ordered signed argument indices;
sentinel contracts preserve the sentinel and null positions. Diagnostic predicates
bind their typed expression, local/template parameter identities, referenced
export owners, message, severity and warning group. Private referenced owners
remain rejected. Canonical printing converts callback indices back to source
numbering, omits empty optional warning groups, places parameter-dependent
predicates before assembly labels, and deduplicates inherited predicates while
rebinding renamed parameters. Import copies predicate expressions and parent
declarations into the consumer AST.

Callable control attributes retain returns-twice, nothrow, leaf, tail-call,
merge/duplication and optimization contracts. Publication uses the existing
`objc3.native-declaration-attributes.v1` capability and typed attribute identities.
No new service ABI or runtime implementation is introduced.

All 33 frontend suites, 238 native unit tests, 412 compiler regressions and 27
focused tooling tests pass. Three platform compiler cases remain unsupported.
Source-free packages pass 32 configurations / 224 commands across normal, eager,
ASan/UBSan and TSan. The complete async fixture adds three smoke configurations /
21 commands. Reports bind final compiler, checker, fixture and unchanged runtime
inputs and retain earlier failures. The complete five-family source/PCH runtime
matrix remains recorded at source 2.91; it was not repeated in this batch.

The maintained source-free package witnesses call provider functions with hidden
size arguments, callbacks and sentinel arguments. Consumers retain diagnostic
predicate rejection, sentinel diagnostics and the native ban on taking an
object-size entry's address after provider source deletion. Native lifetime
metadata is preserved without claiming broader Objective-C borrowed-flow precision.

Further unmodeled attributes, general standard-library publication, remaining
ownership/concurrency/isolation/key-path/language/profile work and native Linux
AArch64 qualification remain open. Enable-if overload identity is outside this
batch.
