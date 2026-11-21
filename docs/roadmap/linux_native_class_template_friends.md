# Native class-template friends and constant-evaluation branches

Source 2.181 / System 2.107 publishes class-template friendship through ordinary,
primary, partial and nested class packages. Type packs, non-type packs,
template-template parameters, constrained heads and exact dependent/aliased
specializations keep Clang's access rules. Friend identities bind the target's
nominal owner and template head without recursively copying its definition.
Excluded and mismatched-SPI dependencies reject. Qualified friend forms marked
unsupported by native Sema remain rejected rather than silently granting access.

Canonical printing places the template head before `friend` and prints only the
friend record header. A lexical friend is emitted once by its owning record,
avoiding extra namespace forward declarations on repeated canonical replay.
During cyclic AST import, an already admitted declaration keeps its source
canonical identity even while its target graph is incomplete. Distinct nominal
owners remain distinct. Unlinked dependent friends undergo a separate name
compatibility comparison without merging their declaration chains.

The same source-free tests exposed native printing that erased `constexpr` from
`if constexpr`. Printing now preserves the branch kind; declaration expression
identity binds ordinary, constexpr, consteval and negated-consteval branches.
Published bodies containing constant-evaluation control require the new
`objc3.cxx-constant-evaluation-control.v1` capability. Unpublished function bodies
do not invent that requirement. Class friendship uses `objc3.cxx-friends.v3`;
older readers and missing or forged capability owners reject.

Four optimized C++20/C++23 and Objective-C++ ARC/MRC native configurations remove
the provider source and run two independent consumers. Controls check exact
source re-emission, alpha-renamed template parameters, changed friend targets,
private access, wrong-specialization rejection, constraints, ABI calls and both
constant-evaluation modes. The adjacent four function-friend configurations and
all 93 LLVM AST controls pass, including cyclic imports after destroying the
provider AST. All 18 selected publication regression groups, complete module-naming/SPI/codec
controls and 26 tooling checks pass (one optional skip). Maintained patch replay,
generated feature authority and undefined-name checks also pass.

These focused checks do not qualify a replacement SDK. SDK 2.62 / ABI 21 remains
the last accepted archive; current-source SDK/ABI 24 and native ARM acceptance
retain their separate delivery gates.
