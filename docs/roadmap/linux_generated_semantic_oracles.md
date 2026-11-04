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
combinations. Coverage records every operation, operation pairs and all four
active-arm transitions for both scalar carrier kinds. Model changes require
review of the public contract and curated transitions; expected output must not
be edited simply to match a compiler result.

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
mismatch class. Compile/link errors, crashes, timeouts and a different mismatch
cannot make a candidate interesting. It retains the original, emits the reduced
program, and compiles/runs a fresh replay. Unexpected semantic failures invoke
the same reducer with their original transport, standard and optimization.

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

Version 2 passes **33 positive cases and seven controls**, followed by a fresh
CLI reduction replay. Its reviewed bank covers **38/50 declared model transition
categories** and **38/196 operation pairs**. These denominators describe the
small supported model, independently of compiler source coverage. The tooling
suite has 103 model/eligibility tests, including valid operand endpoints,
adjacent invalid values, INT_MIN/INT_MAX and incompatible-manifest controls.

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
