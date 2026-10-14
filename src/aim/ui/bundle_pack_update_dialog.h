#pragma once

#include <memory>
#include <string>
#include <vector>

namespace aim {

class BundlePackUpdateDialog {
 public:
  virtual ~BundlePackUpdateDialog() {}

  struct Item {
    std::string bundle_pack_name;
    std::string url;
  };
  virtual void NotifyOpen(const std::vector<Item>& items) = 0;

  virtual void Draw(std::vector<std::string>* updated_bundle_packs) = 0;
};

std::unique_ptr<BundlePackUpdateDialog> CreateBundlePackUpdateDialog();

}  // namespace aim
