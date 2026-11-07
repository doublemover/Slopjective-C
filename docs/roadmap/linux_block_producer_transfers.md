# Native Block producer ownership transfers

System source 2.50 adopts direct native Block producer results into strong
locals, scalar/nested-array members, default member initializers, assignments and
single consumed direct-call arguments. Retained and reclaimed results already
carry native ownership; these destinations do not invent a second heap-copy
operation. Sequenced scalar work may precede the producer. Primitive inputs and
literal default arguments use the bounded direct-call recipe.

A failed producer does not initialize its destination. Completed earlier array
leaves and fields still unwind in reverse order. A produced assignment value
remains owned while evaluating the destination address; failure releases that
value and preserves the old slot. Default member initializers execute in the
private ownership graph even when its client disabled ordinary default-init
visiting. Nullable constructor handlers therefore distinguish a real throwing
producer from an invented copy after a nonthrowing call.

This admission does not erase temporary regions. Mixed/conditional producer
ownership, discarded or immediately invoked owned results, ordinary unconsumed
arguments, multiple consumed arguments, constructor/message consumption,
weak destinations and indirect, instance or Objective-C producers remain separate
obligations.
Evaluated default arguments cannot hide managed work behind empty AST child
ranges. Resource/borrowed/async/checked-effect producer signatures keep their
independent contracts.

Source-free admission cuts over to `objc3.cxx-block-expressions.v3`; v1/v2 are
rejected. Subobject v5 and automatic-owner-unwind v2 remain current. The real
`NativeBlockValues` package now supplies retained and shared producers plus an
explicit consumed disposal entry. ARC and MRC provider/consumer executions verify
local/member adoption, returned values, consumed calls and balanced disposal.

All 66 affected compiler cases, 48 AST controls, six new source-free profiles,
eighteen adjacent frontend groups, codec/isolation controls and 120 exact
production decoder replays pass. The decoder corpus includes real C17/C++23
producer artifacts in addition to mutation, C-inline and incomplete-template
artifacts, with changed-byte and unsupported-capability controls. Twelve
ASan/UBSan native configurations / 42 commands pass, plus four instrumented
provider/consumer executions. Controlled call/return-copy faults cover every
nested leaf and default member, plus constructor/destructor bodies, local
bindings, consumed entry, reclaimed returns and assignment-address failure.
Exact copy counts defend the absence of extra caller copies.

Extension ABI 24, native frame 1, runtime services 4 and schema 2.2 are unchanged.
These are development-source checks. Full matched analysis, one replacement SDK
and installed package qualification follow the remaining implementation sprint.
