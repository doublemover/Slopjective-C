struct Objc3RetainReleaseOperationSiteMetadata {
  bool ownership_qualified = false;
  bool insert_retain = false;
  bool insert_release = false;
  bool insert_autorelease = false;
};

static Objc3RetainReleaseOperationSiteMetadata
BuildRetainReleaseOperationSiteMetadata(bool ownership_qualified,
                                        bool insert_retain,
                                        bool insert_release,
                                        bool insert_autorelease) {
  Objc3RetainReleaseOperationSiteMetadata site;
  site.ownership_qualified = ownership_qualified;
  site.insert_retain = insert_retain;
  site.insert_release = insert_release;
  site.insert_autorelease = insert_autorelease;
  return site;
}

static bool HasRetainReleaseOperationContractViolation(
    const Objc3RetainReleaseOperationSiteMetadata &site) {
  return (!site.ownership_qualified &&
          (site.insert_retain || site.insert_release || site.insert_autorelease)) ||
         (site.insert_autorelease && (site.insert_retain || site.insert_release));
}

static void AccumulateRetainReleaseOperationSiteClassification(
    const Objc3RetainReleaseOperationSiteMetadata &site,
    Objc3RetainReleaseOperationSummary &summary) {
  if (site.ownership_qualified) {
    ++summary.ownership_qualified_sites;
  }
  if (site.insert_retain) {
    ++summary.retain_insertion_sites;
  }
  if (site.insert_release) {
    ++summary.release_insertion_sites;
  }
  if (site.insert_autorelease) {
    ++summary.autorelease_insertion_sites;
  }
  if (HasRetainReleaseOperationContractViolation(site)) {
    ++summary.contract_violation_sites;
  }
}
