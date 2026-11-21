# Paired compiler-source generation feedback

The independent generated semantic model now has a separate, non-promoting
uniform-versus-source-feedback experiment. The older pair-balanced comparison
continues to report semantic coverage only. Neither experiment changes default
generation or qualifies an SDK by itself.

`python3 -B -m scripts.objc3c_toolchain.semantic_source_feedback --help` exposes the
entry point. It requires a current complete semantic-oracle baseline, matching
model/runtime bytes, and the completed `compiler-source-coverage` foundation for
the same compiler source tree. Supply the baseline's runtime services archive
and GNUstep library explicitly. The coverage profile is reused; the driver does
not download, rebuild or substitute a compiler. Run it while the coverage
producer's complete build tree still exists: the installed prefix alone omits
the compiled-object evidence needed to bind the denominator.

The baseline's source-build/configuration IDs must match the normal sanitizer
profile; the experiment separately records the coverage profile's IDs. Full
source-tree equality connects these distinct configurations. Completed foundation
validation checks every planned phase and follow-up command, effective CMake
settings, and exact install-time binary transitions. Historical phase-plan IDs
include pre-stage directory state and runner observations, so they are not
recomputed as if they were stable source identities.

The experiment reruns all five independent wrong-code controls under the
coverage compiler before measuring. Three to eight deterministic seeds receive
counterbalanced uniform/adaptive windows. Each window uses the same number of
eligible 24-operation programs and the same wall/CPU bounds; every external
command receives the remaining shared deadline. A common warmup precedes each
pair. The adaptive generator retains the reviewed operation/transition prefix,
mutates only eligible suffixes, bounds its parent corpus and selects using
observed target-source discoveries. The bootstrap program establishes a coverage
baseline rather than permanently dominating later discovery scores. Expected
values and lifetime traces still come from the independent interpreter.

Feedback comes from LLVM's instrumented **shared clang-cpp implementation**,
with exact ELF/profile build-ID admission. Driver, LLVM and support-tool counters
are admitted as execution evidence and excluded from the feedback denominator.
The measured units are region-entry points in patch-owned Clang source files,
not model states, fuzzer edges or patch-hunk coverage percentages. Missing,
foreign, malformed, empty or changing mappings fail the experiment. The mapping
and per-program sparse hit indices are retained as deterministic compressed
artifacts, together with generated sources, native results and command receipts.
Raw profiles, indexed profiles and bulky raw coverage exports are reclaimed;
per-case raw storage is bounded and isolated on `/tmp`.

The report contains source-coverage trajectories, semantic-coverage metrics,
eligible-program rates, CPU/wall costs and paired per-seed differences/ratios.
Uniform generation remains the default. Repeated measured benefit and review
remain prerequisites for any later adaptive promotion. This experiment covers
ARC C++20 direct-source transport; the complete PCH/interface, optimization,
ownership and language dimensions keep their independent qualification owners.

The implementation has focused generator, profile-admission, baseline,
compressed-artifact, deadline, cleanup and comparison tests. Current-source
execution with the full qualified coverage build remains an SDK/CI consolidation
item; no target-source coverage improvement is claimed by these unit controls.

The focused checkpoint passes 285 related tooling tests and CLI/compile checks.
The quick policy suite also passes all 124 controls after its independent repair
for the eight sanitizer consumer partitions. The new feedback controls are
included in task-hygiene CI; source-feedback changes select the coverage owner.
No full coverage-profile execution or adaptive improvement is claimed yet.
