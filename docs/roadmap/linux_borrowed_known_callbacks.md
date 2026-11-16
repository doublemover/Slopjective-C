# Permanent borrowed results through known callback bindings

Source 2.155 / System 2.81 extends the complete bounded permanent-result proof
to immutable local/global function-pointer bindings and immutable by-value
higher-order helper parameters. The checker follows actual caller bindings,
local aliases, constexpr targets and C++ defaults, then proves the selected
function's complete return graph. Pointer and callable substitutions share the
existing AST/CFG work budget and depth bound.

The actual callable type must match the selected function, preserving physical
calling convention and ownership/effect contracts. Unknown, writable, volatile
or TLS function-pointer slots stay conservative. Indirect unknown targets,
references, replaceable functions, incompatible casts, instance methods,
recursion and exhausted work cannot acquire permanent-storage provenance.
The public borrowed-result owner obligation remains intact.

`objc3.borrowed-result-proofs.v4` is a hard reader cutover. The preceding v3
reader cannot admit these new bodies. Encoders and decoders also reject proof
markers fabricated on namespace declarations.

All eighteen maintained source/PCH compiler commands pass across C17/C++20/23
ARC/MRC, including unknown/mutable/volatile/TLS and calling-convention-cast
rejections. Six publication profiles and four native source-free configurations
exercise higher-order forwarding through an exported constexpr callback after
provider removal. This source implementation does not replace the accepted SDK.
