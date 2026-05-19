    param.module_import_graph_profile =
        BuildModuleImportGraphProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.module_import_graph_profile_is_normalized =
        IsModuleImportGraphProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.generic_suffix_text);
    param.namespace_collision_shadowing_profile =
        BuildNamespaceCollisionShadowingProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.namespace_collision_shadowing_profile_is_normalized =
        IsNamespaceCollisionShadowingProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.public_private_api_partition_profile =
        BuildPublicPrivateApiPartitionProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.public_private_api_partition_profile_is_normalized =
        IsPublicPrivateApiPartitionProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.generic_suffix_text,
            param.object_pointer_type_name);
