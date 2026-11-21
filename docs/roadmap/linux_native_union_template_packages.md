# Native union templates in C++ packages

Source 2.176 / System 2.102 publishes named native union templates through the
same complete member/specialization contracts as class and struct templates.
The earlier rejection of all named union-template definitions is removed;
primary declarations, partial and explicit specializations, nested templates,
dependent fields and explicitly owned active arms retain ordinary native C++
construction, destruction and layout.

The `objc3.cxx-class-templates.v2` hard cutover binds the expanded declaration
contract. Class-template claims require a real template owner; a namespace or
unrelated declaration cannot forge the capability by copying a marker.
Union kind, field layout, special-member selection, dependent bodies and complete
specialization graphs remain part of the existing structural identity.

The source-free control covers plain/partial/specialized/nested and over-aligned
unions, dependent unnamed member records, explicitly owned values with exact
copy/move/destruction counts, two native consumers, provider-to-consumer native
ABI and changed layout metadata. Native owners select and destroy their own
active arm; this does not infer a discriminator or cleanup for implicit
managed/resource union storage. Current-source SDK acceptance remains separate.

All four C++20/23 pure/Objective-C++ ARC/MRC native source-free configurations
pass, including move-only values, owning provider/consumer calls, throwing-copy
rollback and forged-owner rejection. Twelve selected existing publication groups,
91 LLVM AST controls and production codec checks also pass. Full current-source
SDK acceptance remains pending.
