struct Objc3RetainReleaseOperationSiteCollection {
  bool deterministic = true;
  std::vector<Objc3RetainReleaseOperationSiteMetadata> sites;
};

static void AppendRetainReleaseOperationSite(
    Objc3RetainReleaseOperationSiteCollection &collection,
    bool ownership_qualified,
    bool insert_retain,
    bool insert_release,
    bool insert_autorelease) {
  collection.sites.push_back(BuildRetainReleaseOperationSiteMetadata(
      ownership_qualified,
      insert_retain,
      insert_release,
      insert_autorelease));
}

static void CollectRetainReleaseOperationSitesFromFunctionInfo(
    const FunctionInfo &info,
    Objc3RetainReleaseOperationSiteCollection &collection) {
  const std::size_t arity = info.arity;
  if (info.param_has_ownership_qualifier.size() != arity ||
      info.param_ownership_insert_retain.size() != arity ||
      info.param_ownership_insert_release.size() != arity ||
      info.param_ownership_insert_autorelease.size() != arity) {
    collection.deterministic = false;
    return;
  }
  for (std::size_t i = 0; i < arity; ++i) {
    AppendRetainReleaseOperationSite(collection,
                                     info.param_has_ownership_qualifier[i],
                                     info.param_ownership_insert_retain[i],
                                     info.param_ownership_insert_release[i],
                                     info.param_ownership_insert_autorelease[i]);
  }
  AppendRetainReleaseOperationSite(collection,
                                   info.return_has_ownership_qualifier,
                                   info.return_ownership_insert_retain,
                                   info.return_ownership_insert_release,
                                   info.return_ownership_insert_autorelease);
}

static void CollectRetainReleaseOperationSitesFromMethodInfo(
    const Objc3MethodInfo &info,
    Objc3RetainReleaseOperationSiteCollection &collection) {
  const std::size_t arity = info.arity;
  if (info.param_has_ownership_qualifier.size() != arity ||
      info.param_ownership_insert_retain.size() != arity ||
      info.param_ownership_insert_release.size() != arity ||
      info.param_ownership_insert_autorelease.size() != arity) {
    collection.deterministic = false;
    return;
  }
  for (std::size_t i = 0; i < arity; ++i) {
    AppendRetainReleaseOperationSite(collection,
                                     info.param_has_ownership_qualifier[i],
                                     info.param_ownership_insert_retain[i],
                                     info.param_ownership_insert_release[i],
                                     info.param_ownership_insert_autorelease[i]);
  }
  AppendRetainReleaseOperationSite(collection,
                                   info.return_has_ownership_qualifier,
                                   info.return_ownership_insert_retain,
                                   info.return_ownership_insert_release,
                                   info.return_ownership_insert_autorelease);
}

static void CollectRetainReleaseOperationSitesFromPropertyInfo(
    const Objc3PropertyInfo &info,
    Objc3RetainReleaseOperationSiteCollection &collection) {
  AppendRetainReleaseOperationSite(collection,
                                   info.has_ownership_qualifier,
                                   info.ownership_insert_retain,
                                   info.ownership_insert_release,
                                   info.ownership_insert_autorelease);
}

static Objc3RetainReleaseOperationSiteCollection
CollectRetainReleaseOperationSitesFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3RetainReleaseOperationSiteCollection collection;
  for (const auto &entry : surface.functions) {
    CollectRetainReleaseOperationSitesFromFunctionInfo(entry.second, collection);
  }
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      CollectRetainReleaseOperationSitesFromMethodInfo(method_entry.second, collection);
    }
    for (const auto &property_entry : entry.second.properties) {
      CollectRetainReleaseOperationSitesFromPropertyInfo(property_entry.second, collection);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      CollectRetainReleaseOperationSitesFromMethodInfo(method_entry.second, collection);
    }
    for (const auto &property_entry : entry.second.properties) {
      CollectRetainReleaseOperationSitesFromPropertyInfo(property_entry.second, collection);
    }
  }
  return collection;
}

static void CollectRetainReleaseOperationSitesFromFunctionTypeMetadata(
    const Objc3SemanticFunctionTypeMetadata &metadata,
    Objc3RetainReleaseOperationSiteCollection &collection) {
  const std::size_t arity = metadata.arity;
  if (metadata.param_has_ownership_qualifier.size() != arity ||
      metadata.param_ownership_insert_retain.size() != arity ||
      metadata.param_ownership_insert_release.size() != arity ||
      metadata.param_ownership_insert_autorelease.size() != arity) {
    collection.deterministic = false;
    return;
  }
  for (std::size_t i = 0; i < arity; ++i) {
    AppendRetainReleaseOperationSite(collection,
                                     metadata.param_has_ownership_qualifier[i],
                                     metadata.param_ownership_insert_retain[i],
                                     metadata.param_ownership_insert_release[i],
                                     metadata.param_ownership_insert_autorelease[i]);
  }
  AppendRetainReleaseOperationSite(collection,
                                   metadata.return_has_ownership_qualifier,
                                   metadata.return_ownership_insert_retain,
                                   metadata.return_ownership_insert_release,
                                   metadata.return_ownership_insert_autorelease);
}

static void CollectRetainReleaseOperationSitesFromMethodTypeMetadata(
    const Objc3SemanticMethodTypeMetadata &metadata,
    Objc3RetainReleaseOperationSiteCollection &collection) {
  const std::size_t arity = metadata.arity;
  if (metadata.param_has_ownership_qualifier.size() != arity ||
      metadata.param_ownership_insert_retain.size() != arity ||
      metadata.param_ownership_insert_release.size() != arity ||
      metadata.param_ownership_insert_autorelease.size() != arity) {
    collection.deterministic = false;
    return;
  }
  for (std::size_t i = 0; i < arity; ++i) {
    AppendRetainReleaseOperationSite(collection,
                                     metadata.param_has_ownership_qualifier[i],
                                     metadata.param_ownership_insert_retain[i],
                                     metadata.param_ownership_insert_release[i],
                                     metadata.param_ownership_insert_autorelease[i]);
  }
  AppendRetainReleaseOperationSite(collection,
                                   metadata.return_has_ownership_qualifier,
                                   metadata.return_ownership_insert_retain,
                                   metadata.return_ownership_insert_release,
                                   metadata.return_ownership_insert_autorelease);
}

static void CollectRetainReleaseOperationSitesFromPropertyTypeMetadata(
    const Objc3SemanticPropertyTypeMetadata &metadata,
    Objc3RetainReleaseOperationSiteCollection &collection) {
  AppendRetainReleaseOperationSite(collection,
                                   metadata.has_ownership_qualifier,
                                   metadata.ownership_insert_retain,
                                   metadata.ownership_insert_release,
                                   metadata.ownership_insert_autorelease);
}

static Objc3RetainReleaseOperationSiteCollection
CollectRetainReleaseOperationSitesFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3RetainReleaseOperationSiteCollection collection;
  for (const auto &metadata : handoff.functions_lexicographic) {
    CollectRetainReleaseOperationSitesFromFunctionTypeMetadata(metadata, collection);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      CollectRetainReleaseOperationSitesFromMethodTypeMetadata(method_metadata, collection);
    }
    for (const auto &property_metadata : interface_metadata.properties_lexicographic) {
      CollectRetainReleaseOperationSitesFromPropertyTypeMetadata(property_metadata, collection);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      CollectRetainReleaseOperationSitesFromMethodTypeMetadata(method_metadata, collection);
    }
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      CollectRetainReleaseOperationSitesFromPropertyTypeMetadata(property_metadata, collection);
    }
  }
  return collection;
}
