# Native host CI and installed helper contracts

The checks at Source 2.187 exposed three stale CI assumptions after the native
Linux AArch64 cutover. The release policy searched for the retired architecture-
independent archive name; installed-helper tests copied an incomplete dependency
list; runtime/generator fixtures omitted required native architecture context.

Release validation now checks the same architecture-bound archive names used by
the producer and fresh-host installer, with the selected host forwarded to both
the native build and implementation-analysis owner. Mutation controls reject
foreign or unbound archives and host substitutions. All 128 workflow-policy
checks pass, including the repaired mandatory archive-publication controls.

SDK assembly and isolated helper checks share one installation routine and
inventory, including the auxiliary source validators. Every shipped helper is
imported without a source checkout, both with and without Python's optional
compression module. The semantic-oracle input inventory also binds its native
host helper. Runtime fixtures use the actual native host's ELF machine identity
and complete bootstrap tool inventory; generator checks execute the production
CMake host-selection include. Separate controls reject foreign ELF images and
missing/foreign runtime profile identities. These are configuration and import
controls, not replacement compiler/runtime qualification.

The focused affected tooling passes 83 tests with nine optional native-product
skips. This repair does not promote SDK 2.62 / ABI 21 or replace the remaining
current-source SDK/native-host acceptance gates.

The complete local native-tooling CI command also passes: 1,689 tests,
155 optional skips and 33 subtests. Remote CI must run again on the committed
repair; local unit/configuration checks are not an SDK acceptance receipt.

## Subsequent CI audit

The Source 2.238 automatic documentation, native-tooling and workflow-policy jobs
all pass. The earlier three failing jobs at `d51e62900349` predate the repairs
above; their missing-helper, native loader fixture, generator, release-policy
and formatting failures are covered by the maintained controls. A renewed local
check of the affected helpers, loader/runtime fixtures and native producer/reader
boundary passes 208 tests, with nine optional native-product skips.

The separate Source 2.153 extended campaign failed in its terminal upstream
receipt merge after its producers and consumers passed. The
[terminal-state repair](linux_upstream_evidence_terminal_state.md) is committed;
that historical run does not qualify the current compiler.

The retained coverage diagnostic now returns the aggregate command's actual exit
status after printing its annotation. Previously its final `echo` made a failed
aggregate appear successful. Four executable shell controls preserve success,
ordinary failures and timeout exit codes; workflow-policy mutations reject a
missing or overwritten exit status. No archive is promoted by a diagnostic run.

The shared analysis foundation now transports its configured
`bin/llvm-tblgen` with the staged compiler and Ninja dependency inputs. The
fork-matched frontend invokes that executable to generate the scanner's option
table. The producer rejects a missing, non-executable or wrong-architecture
tool, and the consumer validates the restored member before frontend
configuration. The earlier AArch64 partitions all stopped at this boundary.
