#pragma once

#include <string>

#include "aim/core/bundle_manager.h"
#include "aim/core/file_system.h"
#include "aim/core/local_store.h"

namespace aim {

class BundlePackUpdateManager {
 public:
  BundlePackUpdateManager() {}
  virtual ~BundlePackUpdateManager() {}

  // Returns map of pack_name to commit sha of new version.
  virtual std::optional<std::unordered_map<std::string, std::string>> GetBundlePacksNeedingUpdate(
      const std::vector<std::string>& pack_names) = 0;

  virtual void MarkBundlePacksUpdated(
      const std::vector<std::string>& pack_names,
      const std::unordered_map<std::string, std::string>& bundle_pack_sha_map) = 0;
};

std::unique_ptr<BundlePackUpdateManager> CreateBundlePackUpdateManager(
    FileSystem* fs, BundleManager* bundle_manager, LocalStore* local_store);

}  // namespace aim
