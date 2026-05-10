struct PureContractEffectInfo {
  struct SourceLoc {
    unsigned line = 1;
    unsigned column = 1;
    bool present = false;
  };

  SourceLoc global_write_site;
  SourceLoc message_send_site;
  std::unordered_map<std::string, SourceLoc> called_functions;
};

struct PureContractCause {
  std::string token;
  unsigned line = 1;
  unsigned column = 1;
  bool present = false;
  std::string detail_token;
  unsigned detail_line = 1;
  unsigned detail_column = 1;
  bool detail_present = false;
};
