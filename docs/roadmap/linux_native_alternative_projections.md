# Native alternative argument and storage projections

Source 2.201 / System 2.127 extends the bounded pure native-helper proof from
one selected argument to a union of possible caller arguments. Every live
return contributes to a 32-bit source set. Conditional expressions, switches,
immutable aliases and nested closed helpers preserve the complete set; neither
argument order nor the first visited return can discard a resource source.

Reference projections resolve a bounded set of caller storage cells through
nested helpers, conditional lvalues and reference aliases. Assignments and
increments update all possible cells. Unknown calls and reference captures
escape every possible cell. Ordinary snapshots remain admissible when every
possible source is ordinary; unused and condition arguments keep read roles.
The existing purity, lifetime, replaceability, type, recursion and syntax gates
remain mandatory. Incomplete/cyclic cell resolution uses the conservative
unknown-role path instead of recording a partial target set.

This proof extension changes no runtime layout, interface schema or reader
capability. Provider bodies already bind the native helper implementation.
It does not prove arbitrary mutable helper state, globals, unresolved dependent
calls, runtime-selected callees or open member/virtual dispatch.

Focused source/PCH controls pass 144 commands across C17/C++20/C++23,
ARC/MRC and compatibility/strict/strict-concurrency/strict-system profiles.
They cover both alternative source positions, nested selections, both possible
write destinations, unknown-call/capture escape and argument bit 31. All four
optimized source-free provider/two-consumer executor configurations pass,
including 19 ordinary value/reference paths per consumer and exact descriptor
closure. The maintained patch reproduces authored sources and 16 tooling checks
pass. Full isolation regression and archive acceptance are tracked separately.
