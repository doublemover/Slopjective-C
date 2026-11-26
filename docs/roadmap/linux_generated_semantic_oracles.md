# Independent generated semantic oracles

Issue [#8288](https://github.com/doublemover/Slopjective-C/issues/8288) adds a small
executable reference model alongside the authored native witnesses. Python owns
the expected state/lifetime trace before compilation. Each native result must
match that model, even when optimized variants agree with one another.

## Selected defined subset

The initial profile is Linux x86-64, Objective-C++20/23 with ARC. Fourteen typed
operations cover scalar Optional/Result active arms, copy/self/alias assignment,
success/error provider calls, managed object creation/copy/clear, ordered scopes,
defer and partial C++ construction failure. Objects have logical IDs independent
of process addresses. Observations include the selected value, object survival,
weak zeroing and exactly-once last destruction. Raw ARC retain counts and
intermediate copy-elision counts are not used as expected results.

Programs contain at most 64 operations and 32 logical allocations. Slots are
always initialized, values and additions stay within bounded signed ranges, and
observable effects are sequenced. No move-from observation, floating-point
reassociation or unspecified effect ordering is generated. MRC ownership
adaptation and concurrency are outside this model. Existing authored ownership,
diagnostic, sanitizer and concurrency witnesses retain their separate scope.

The seed bank begins with reviewed transitions and then adds deterministic
combinations. Two separate reviewed traces cover rare ownership and carrier
transitions independently of those combinations. Qualification requires all 50
declared model transitions and all nine ordered ownership-operation pairs.
Coverage of the other operation pairs is recorded for comparison. Changes to
expected output require review against the public contract and curated traces.

## Executable routes and controls

Each default seed runs source, PCH and source-free semantic-interface consumers
at O0/O2, a ThinLTO route, and C++23 private-local renaming, comment insertion and
pure-expression grouping. Interface consumers use a fresh case directory/cache;
provider source and declaration headers move outside its search tree before
compilation. The retained original sources remain available as evidence.

A private provider-body change preserves the public interface identity while
changing the expected executable result. A public return-ownership change must
change that identity and reject a consumer with the stale lock.

Five test mutants flip a Result arm, omit defer, duplicate a destructor event,
corrupt a provider result or alter error propagation. They retain defined
behavior and exit zero, with ASan/UBSan enabled on generated provider/consumer
objects. Their oracle mismatch is mandatory; a sanitizer finding cannot satisfy
that control. A corrected program uses the same sanitizer configuration.

The typed deletion reducer preserves well-formed operations and the same
mismatch class. A candidate must compile, exit cleanly and reproduce that class.
The reduced record retains both model traces, the original seed or reviewed
witness, and the exact native inputs used in the final replay. The Result-arm
control reduces and replays with ASan/UBSan enabled. Unexpected semantic
failures use the same reducer with their original transport, standard and
optimization.

## Running and replaying

After the matched LLVM campaign:

```sh
python3 -B -m scripts.objc3c_toolchain.semantic_oracles \
  --ir-receipt out/llvm-assurance/ir/qualification.json \
  --output out/semantic-oracles

python3 -B -m scripts.objc3c_toolchain.semantic_oracles \
  --ir-receipt out/llvm-assurance/ir/qualification.json \
  --program-json out/semantic-oracles/reduction.json \
  --output out/semantic-replay
```

Explicit matching `--facade`, `--services` and `--runtime` paths also support
development runs. Custom seeds are bounded by `--seeds`; unsupported inputs fail
before compilation. Evidence includes typed operations, expected model traces,
generated sources, transformations, artifacts and hashes, exact compiler/link
commands, loader libraries, real header dependencies, timeout outcomes and
per-route oracle/differential disagreement. The model/generator version and owner
hashes bind replay inputs. Version 2 also requires an exact machine-readable
eligibility manifest before invoking a tool: target and integer/pointer widths,
GNUstep runtime, language/ownership/profile, operation bounds and hash, logical
lifetime assumptions, reviewed relation precondition IDs, fault-control mode and
the complete closed runtime witness. Missing, old or incompatible manifests fail
admission. Reduced witnesses retain original and reduced manifests.

The LLVM assurance workflow runs the reviewed seed bank after its compiler/IR
campaign, reusing that exact tool/runtime bundle. Dispatch campaigns add three
reproducible seeds derived from the tested revision; `--seed-rotation` replays
that expansion locally. Receipts record seeds, compile/command counts and elapsed
command time. The existing artifact upload retains the
semantic sources, reports, controls and reductions. This adds no schedule or SDK
archive build. Coverage-guided frontend fuzzing, concurrent schedules and native
platform expansion retain their own issue owners.

## Completed native checkpoint

The earlier matched development campaign passed **33 positive cases and seven
controls**, followed by a fresh CLI reduction replay. It covered **38/50
declared model transitions** and **38/196 operation pairs**. The expanded
reviewed bank covers **50/50 declared model transitions**, **9/9 ordered
ownership-operation pairs** and **58/196 total operation pairs**. Seven native
programs and four fresh matched builds passed on the published x86-64 SDK,
including the reviewed ARC witness via source, PCH and an authentic source-free
interface.

### Sanitizer-clean mutation and fresh reduction replay

The published x86-64 SDK at seal
`d7de09cc1d27fb7add8c2f4792d34d488384300df8d248318b3539e0cfcf2fd0`
supplied Clang, its compiler resources, LLD and the installed GNUstep runtime
and service archive. The admitted LLVM 23.1.5 symbolizer SHA-256 was
`35bec8ab8108b5396f61d1b3664629699bd9c9f59983179bf3eb32cc8cd5a6d2`.
The generated provider and consumer both referenced ASan and UBSan symbols.
Every listed program exited zero and its log contained zero ASan, UBSan or
LeakSanitizer findings. The independent model check compared the full state
and cleanup traces.

| Program on seed 7          | Model comparison    | First observed difference             |
| -------------------------- | ------------------- | ------------------------------------- |
| Corrected control          | Equal               | None                                  |
| Flipped Result arm         | `value-or-lifetime` | Expected Ok, observed Err             |
| Omitted defer              | `cleanup-order`     | Nine expected events, seven observed  |
| Duplicate destructor event | `cleanup-order`     | Nine expected events, eleven observed |
| Changed provider result    | `value-or-lifetime` | Expected 6, observed 7                |
| Changed error propagation  | `value-or-lifetime` | Expected 9, observed 10               |

Eight deletion trials reduced the 24-operation Result-arm mismatch to
`err(slot=1, value=-16)`. The model expected Err(-16), and the executable
produced Ok(-16). A fresh matched build retained both sanitizer flags and
reproduced the same trace with SHA-256
`2e7b9cd73b6364f22b006606dab367c8a77a3539a20ccc3538a80bd80d3988b7`.

| Retained native evidence                           | SHA-256                                                            |
| -------------------------------------------------- | ------------------------------------------------------------------ |
| Controls and reduction trials `qualification.json` | `e95c19cfb188837ea3a1f36f075edee9e2265e8a77338ff7cdba0a66bf14607e` |
| Reduction input `reduction.json`                   | `8c248c5243ba12ed73a4ca9dca848ed1e5bd52022ae103d9d9b28d6ff35ee70e` |
| Fresh replay `qualification.json`                  | `711fe716263e459e7b50811e00aaa51cb0df21b40b9c863ec36e45003e0a10b2` |

This local checkpoint covers the installed x86-64 compiler and instrumented
generated programs using the shipped runtime. The hosted matched development
campaign supplies compiler-instrumentation and full matrix evidence.

### Earlier generation experiment

A non-promoting generation experiment compares uniform and pair-balanced tails
on identical seeds, compiler/runtime inputs, 24-operation lengths and 180-second
wall/CPU budgets per strategy. Each runs three native programs against the model.
Uniform covers 38 model categories and 38 pairs; pair-balanced covers 35 categories
and 40 pairs. Their measured wall times are 7.73 and 7.93 seconds in this sample.
This shows no overall improvement, so adaptive selection remains disabled.
Target-source coverage remains with #8290; no speedup or full-language coverage
claim follows from this experiment.

Run the comparison after a current semantic campaign:

```sh
python3 -B -m scripts.objc3c_toolchain.semantic_generation \
  --baseline out/semantic-oracles/qualification.json \
  --facade /path/to/matched/objc3c --services /path/to/matched/services.a \
  --runtime /path/to/matched/libobjc.so --output out/semantic-generation
```

The earlier version-1 checkpoint remains historical evidence:

The default bank passes **33 positive cases and seven controls**. The expanded
six-seed campaign passes **63 positive cases and seven controls**, with **103
native compilations / 401 commands**. All fourteen modeled operations and all
four active-arm transitions for both carriers execute. The five deliberately
wrong programs exit zero under ASan/UBSan and fail the independent oracle.

Eight typed reduction trials reduce a 24-operation failure to one operation;
a separate CLI invocation rebuilds it and reproduces the same mismatch. The
private-body/stable-identity and stale-public-ownership controls also pass.
The [issue inventory](../support/linux_open_issue_review.json) records the exact
receipt hashes. These results use the admitted instrumented development tools;
they do not qualify an SDK archive or extend the model to MRC/concurrency.
