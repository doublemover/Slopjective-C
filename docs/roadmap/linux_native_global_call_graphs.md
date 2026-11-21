# Closed native helper graphs and global provenance

Source 2.229 / System 2.155 follows known global writes and scalar results
through bounded closed native free/static helpers. Actual scalar arguments map
to canonical callee parameters; return expressions and global write sources
remain connected through helper chains, aliases and recursive calls. Repeated
calls contribute a conservative union of possible sources rather than replacing
an earlier value. Constant dead branches do not introduce writes.

The admitted graph uses scalar by-value parameters and void/scalar results,
internal or inline definitions, and ordinary synchronous native calls. Open,
weak, alias/ifunc, multiversion, actor/executor, ownership-taking and async/throws
entries do not establish these summaries. Nonstatic receiver state, reference
parameters/results, pending dependent definitions and arbitrary external library
behavior retain their separate requirements.

Each helper has at most sixteen parameters and thirty-two top-level statements;
a 128-node preflight rejects complex bodies. A scope admits at most thirty-two
helper definitions and thirty-two returns per helper, sharing the existing
4096-node/depth-128 write-index limit. Cycles reuse summaries and argument-source
unions. Graph exhaustion retains conservative helper/global dependencies while
completing the caller-local write index; unrelated local observations do not
acquire a fabricated resource contract. Definition admission is cached per
analysis, with arity checked on every call. Pure comparisons,
logical observations and resource integer telemetry establish independent
results before a larger unrelated graph can exhaust provenance discovery;
borrowed pointer-bit transformations continue to retain their loan.

The preceding frontend misses five direct closed-helper transfer diagnostics.
Maintained C17/C++20/C++23 ARC/MRC source/PCH controls cover stores, loads,
composed round trips, recursion, dead writes, observations, resource values and
a call chain beyond the admitted definition limit. The source-free native owner
publishes helper bodies and global slots, then checks loan/resource rejection
and independent-value admission through imported chains in six configurations.
All 864 new/adjacent source/PCH commands, six source-free configurations and
100 tooling checks pass. Full current-source SDK, native ARM and whole-profile
acceptance remain pending.
