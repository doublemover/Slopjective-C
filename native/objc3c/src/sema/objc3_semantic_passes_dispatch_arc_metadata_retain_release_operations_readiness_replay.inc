static void ApplyRetainReleaseOperationSummaryReadiness(
    Objc3RetainReleaseOperationSummary &summary,
    std::size_t operation_sites) {
  const std::size_t qualified_or_violation =
      summary.ownership_qualified_sites + summary.contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      summary.contract_violation_sites <= operation_sites &&
      summary.retain_insertion_sites <= qualified_or_violation &&
      summary.release_insertion_sites <= qualified_or_violation &&
      summary.autorelease_insertion_sites <= qualified_or_violation;
}

static Objc3RetainReleaseOperationSummary
BuildRetainReleaseOperationSummaryFromSiteCollection(
    const Objc3RetainReleaseOperationSiteCollection &collection) {
  Objc3RetainReleaseOperationSummary summary =
      PublishRetainReleaseOperationSummaryFromSites(collection);
  ApplyRetainReleaseOperationSummaryReadiness(summary, collection.sites.size());
  return summary;
}

static Objc3RetainReleaseOperationSummary
BuildRetainReleaseOperationSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildRetainReleaseOperationSummaryFromSiteCollection(
      CollectRetainReleaseOperationSitesFromIntegrationSurface(surface));
}

static Objc3RetainReleaseOperationSummary
BuildRetainReleaseOperationSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildRetainReleaseOperationSummaryFromSiteCollection(
      CollectRetainReleaseOperationSitesFromTypeMetadataHandoff(handoff));
}
