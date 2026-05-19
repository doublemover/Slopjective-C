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
