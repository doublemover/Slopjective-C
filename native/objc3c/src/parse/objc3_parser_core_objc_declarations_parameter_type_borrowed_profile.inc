    param.borrowed_pointer_profile =
        std::string("borrowed-pointer:qualified=") +
        (param.borrowed_pointer_qualified ? "true" : "false") +
        ";pointer=" + (param.has_pointer_declarator ? "true" : "false");
