  void ApplyMessageReceiverMetadataAnnotations(Expr &message) {
    message.nil_receiver_semantics_enabled = message.receiver->kind == Expr::Kind::NilLiteral;
    message.nil_receiver_foldable = message.nil_receiver_semantics_enabled;
    message.nil_receiver_requires_runtime_dispatch = !message.nil_receiver_foldable;
    message.nil_receiver_folding_symbol = BuildNilReceiverFoldingSymbol(
        message.nil_receiver_foldable, message.nil_receiver_requires_runtime_dispatch, message.message_send_form);
    message.nil_receiver_semantics_is_normalized = true;
    // dispatch-surface classification anchor: super receivers stay explicit while direct dispatch remains reserved.
    // dispatch-site modeling anchor: parsing preserves raw receiver
    // spelling only; whole-program frontend normalization classifies
    // instance/class/super/direct/dynamic families before sema consumes the
    // message send.
    // dispatch legality/selector-resolution freeze anchor: message sends remain legal only for normalized unary/keyword selectors with an
    // explicit receiver form; unresolved or ambiguous selector resolution stays
    // fail-closed and direct dispatch remains reserved.
    // selector-resolution implementation anchor: parser still keeps
    // one normalized selector spelling plus explicit receiver syntax only;
    // lane-B sema is now responsible for exact concrete self/super/known-class
    // resolution and fail-closed ambiguity diagnostics.
    // super/direct/dynamic legality expansion anchor: parser still
    // does not invent direct-dispatch syntax or alternate receiver forms;
    // lane-B sema now owns rejecting illegal `super` sites while preserving the
    // same raw dynamic receiver spellings for runtime method-family accounting.
    // dispatch lowering ABI freeze anchor: parser continues to hand
    // selector text through as lowered cstring material only. Selector-handle
    // lookup and canonical runtime-dispatch cutover stay in lane-C lowering;
    // non-canonical dispatch entrypoints remain absent until lowering owns them.
    // runtime call ABI generation anchor: parser still does not pick
    // the runtime entrypoint itself. Lowering now cuts normalized instance and
    // runtime ARC helper API surface anchor: parser-owned ARC and
    // ownership spelling remains purely descriptive; the private ARC helper
    // ABI stays a lowering/runtime concern rather than a source-level promise.
    // runtime ARC helper implementation anchor: parser-owned ARC and
    // ownership spelling remains purely descriptive, but the supported
    // property/weak/autorelease-return slice now continues into a live linked
    // runtime-helper path rather than stopping at a freeze-only boundary.
    // ownership-debug/runtime-validation anchor: parser-owned ARC
    // and ownership spelling still does not promise any source-level debug
    // surface; lane-D owns the private runtime snapshot hooks above these same
    // source packets.
    // class sends over to objc3_runtime_dispatch_i32 while reserved
    // super/dynamic/direct handling stays fail-closed at the owner boundary.
    // live-dispatch cutover anchor: lowering now moves normalized
    // dynamic sends onto objc3_runtime_dispatch_i32 too, removing the last
    // live-path retired-route dependency while keeping reserved direct
    // dispatch fail closed in IR lowering.
    // lookup/dispatch runtime freeze anchor: parser still does not
    // own selector interning, metadata-backed lookup tables, method caches, or
    // slow-path runtime resolution. It hands normalized selector text and
    // receiver form through to the canonical runtime-owned lookup/dispatch
    // boundary for lane-D to extend later.
    // selector-table anchor: metadata-backed selector lookup tables
    // are now runtime-owned behind that preserved boundary.
    // method-cache / slow-path anchor: parser still does not resolve
    // methods from class metadata or own cache behavior. It preserves the
    // normalized selector text plus receiver-family classification that the
    // runtime now consumes for deterministic class/metaclass slow-path lookup.
    // protocol/category-aware resolution anchor: parser still does
    // not resolve category tiers or protocol declaration graphs. It preserves
    // the normalized selector text, receiver family, and protocol/category
    // declaration surface that the runtime now consumes for deterministic
    // category-backed live lookup and protocol-backed negative lookup evidence.
    // live-dispatch gate anchor: parser still only publishes the
    // normalized selector/receiver classification surface. Proof that sends
    // execute through the live runtime path rather than the non-authoritative test surface
    // remains a lane-E evidence responsibility rooted in C004/D004 and handed
    // off to E002 for smoke/closeout replacement.
    // live-dispatch smoke/replay closeout anchor: execution-smoke
    // proof consumes the canonical runtime dispatch symbol from the published
    // parser/sema handoff rather than treating the non-authoritative test surface as the
    // primary proof surface.
    message.super_dispatch_enabled = IsSuperDispatchReceiver(*message.receiver);
    message.super_dispatch_requires_class_context = message.super_dispatch_enabled;
    message.super_dispatch_symbol = BuildSuperDispatchSymbol(
        message.super_dispatch_enabled, message.super_dispatch_requires_class_context, message.message_send_form);
    message.super_dispatch_semantics_is_normalized = true;
    message.method_family_name = objc3c::support::ClassifyMethodFamilyFromSelector(message.selector);
    message.method_family_returns_retained_result = message.method_family_name == "init" ||
                                                   message.method_family_name == "copy" ||
                                                   message.method_family_name == "mutableCopy" ||
                                                   message.method_family_name == "new";
    message.method_family_returns_related_result = message.method_family_name == "init";
    message.method_family_semantics_symbol = BuildMethodFamilySemanticsSymbol(
        message.method_family_name, message.method_family_returns_retained_result,
        message.method_family_returns_related_result);
    message.method_family_semantics_is_normalized = true;
    message.runtime_link_host_link_required = message.nil_receiver_requires_runtime_dispatch;
    message.runtime_link_host_link_elided = !message.runtime_link_host_link_required;
    message.runtime_link_host_link_declaration_parameter_count = message.dispatch_abi_runtime_arg_slots + 2u;
    message.runtime_dispatch_bridge_symbol = kRuntimeLinkHostLinkDispatchSymbol;
    message.runtime_link_host_link_symbol = BuildRuntimeLinkHostLinkSymbol(
        message.runtime_link_host_link_required, message.runtime_link_host_link_elided,
        message.dispatch_abi_runtime_arg_slots, message.runtime_link_host_link_declaration_parameter_count,
        message.runtime_dispatch_bridge_symbol, message.message_send_form);
    message.runtime_link_host_link_is_normalized = true;
    message.dispatch_surface_kind = Expr::DispatchSurfaceKind::Unclassified;
    message.dispatch_surface_family_symbol.clear();
    message.dispatch_surface_entrypoint_family_symbol.clear();
    message.dispatch_surface_is_normalized = false;
  }
