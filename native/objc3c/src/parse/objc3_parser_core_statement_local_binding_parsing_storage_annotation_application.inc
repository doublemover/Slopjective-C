      if (saw_local_storage_annotation) {
        stmt->let_stmt->cleanup_attribute_declared =
            local_storage_annotation.cleanup_attribute_declared;
        stmt->let_stmt->cleanup_sugar_declared =
            local_storage_annotation.cleanup_sugar_declared;
        stmt->let_stmt->cleanup_function_symbol =
            local_storage_annotation.cleanup_function_symbol;
        stmt->let_stmt->cleanup_profile_is_normalized =
            local_storage_annotation.cleanup_profile_is_normalized;
        stmt->let_stmt->cleanup_profile =
            local_storage_annotation.cleanup_profile;
        stmt->let_stmt->resource_attribute_declared =
            local_storage_annotation.resource_attribute_declared;
        stmt->let_stmt->resource_sugar_declared =
            local_storage_annotation.resource_sugar_declared;
        stmt->let_stmt->resource_close_symbol =
            local_storage_annotation.resource_close_symbol;
        stmt->let_stmt->resource_invalid_expression =
            local_storage_annotation.resource_invalid_expression;
        stmt->let_stmt->resource_profile_is_normalized =
            local_storage_annotation.resource_profile_is_normalized;
        stmt->let_stmt->resource_profile =
            local_storage_annotation.resource_profile;
      }
