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
