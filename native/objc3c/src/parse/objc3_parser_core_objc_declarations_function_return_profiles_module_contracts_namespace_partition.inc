    fn.return_namespace_collision_shadowing_profile =
        BuildNamespaceCollisionShadowingProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_namespace_collision_shadowing_profile_is_normalized =
        IsNamespaceCollisionShadowingProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_public_private_api_partition_profile =
        BuildPublicPrivateApiPartitionProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_public_private_api_partition_profile_is_normalized =
        IsPublicPrivateApiPartitionProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
