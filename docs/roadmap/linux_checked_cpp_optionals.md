# Optional checked C++ results and fresh owner regions

Source 2.127 / System 2.53 lowers `tryOptional` over qualified native indirect C++
results and native frame awaits. A successful call constructs the payload once;
a checked failure commits None without constructing or destroying a payload.
Native C++ exceptions retain their ordinary exception path. Direct/indirect,
virtual/member, Block and template calls share this lowering, including move-only
owners. Discarded results retain a private success-only owner until full-expression
cleanup, rather than abandoning a constructed result.

Fresh C++ carrier initialization also covers conditional and comma expressions,
explicit Some/None, arrays, composed fields, ordinary references and lifetime-
extended automatic references. Destination guards commit after construction and
preserve ownership while initializer temporaries are destroyed. Reference cleanup
uses the selected carrier destroyer and the same normal/exceptional handoff as
native records. Native awaits retain these owners across their argument cleanup.
Throwing-destructor payloads retain their separate region qualification limits.

Static/TLS values keep their provisional owner through initializer cleanup and
register active-arm destruction only after success. Failed initialization can
retry. Const C++ carriers retain nontrivial destruction in constant-storage
classification; their storage cannot be made immutable across destruction. Native
thread-exit controls verify one construction, reused storage and exact destruction
of the surviving payload after join. Static/TLS lifetime-extended carrier
references remain explicitly gated until their separate referent registration
and retry protocol is implemented.

The maintained property witness covers Some/None, copy/move, discarded values,
constructor/parameter/temporary faults, partial and completed arrays, composed
owners, typed exits through reference members, native-await conditionals and
references, static/TLS retry and thread teardown. Provisional reference ownership
starts before comma-LHS temporaries, so their late destructor faults cannot leak
either native-record or optional referents. Dedicated source/PCH compiler
controls cover all four profiles; a native AST regression checks constant storage
with destruction included or excluded. Source-free replay preserves the written
optional, transfer and reference operations. All 518 executable cases in the
521-case compiler suite pass (three unsupported), including 53 new dedicated
commands. All 49 AST controls, sixteen ASan/UBSan configurations / 56 commands,
thirteen frontend groups, codec/isolation and 220 exact production decoder
replays pass. Source/PCH, ARC/MRC, C17/C++20/23, exception-only and elision-disabled
execution retain exact owner counts. These remain development-source receipts.

Extension ABI 24, native frame 1, runtime services 4 and schema 2.2 are unchanged.
Native C++ carrier by-value call/return qualification, additional construction
regions, full matched analysis and one replacement SDK remain separate work.
