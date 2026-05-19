static Objc3RetainReleaseOperationSummary
PublishRetainReleaseOperationSummaryFromSites(
    const Objc3RetainReleaseOperationSiteCollection &collection) {
  Objc3RetainReleaseOperationSummary summary;
  summary.deterministic = collection.deterministic;
  for (const auto &site : collection.sites) {
    AccumulateRetainReleaseOperationSiteClassification(site, summary);
  }
  return summary;
}
