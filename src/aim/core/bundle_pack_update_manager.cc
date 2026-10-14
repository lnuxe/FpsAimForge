#include "bundle_pack_update_manager.h"

#include <future>

#include "aim/common/http.h"
#include "aim/common/log.h"
#include "aim/core/file_system.h"
#include "miniz.h"
#include "nlohmann/json.h"

namespace aim {
namespace {

std::string GetGitRepoCommitShaKey(const std::string& url) {
  return std::format("GitRepoCommitSha__{}", url);
}

class GitCommitShaManager {
 public:
  // Returns optional empty if still loading. Returns "" if results retrieved but no commit sha
  // found.
  std::optional<std::string> GetGitCommitSha(const std::string& url) {
    std::string repo = GetGithubRepoNameFromUrl(url);
    if (repo.empty()) {
      return "";
    }
    CheckFuturesForResultsAndMerge();
    auto it = repo_to_commit_sha_.find(repo);
    if (it != repo_to_commit_sha_.end()) {
      return it->second;
    }

    // Start work to get commit sha.
    repo_to_commit_sha_futures_[repo] =
        std::async(std::launch::async, GetGithubCommitShaOnBackgroundThread, repo);
    repo_to_commit_sha_[repo] = {};
    return {};
  }

 private:
  void CheckFuturesForResultsAndMerge() {
    std::vector<std::string> to_erase;
    for (auto& entry : repo_to_commit_sha_futures_) {
      std::future_status status = entry.second.wait_for(std::chrono::seconds(0));
      if (status == std::future_status::ready) {
        std::string commit_sha = entry.second.get();
        repo_to_commit_sha_[entry.first] = commit_sha;
        to_erase.push_back(entry.first);
      }
    }

    for (const std::string& repo : to_erase) {
      repo_to_commit_sha_futures_.erase(repo);
    }
  }

  // Takes the repo like mjohns/FpsAimForgeBundles
  static std::string GetGithubCommitShaOnBackgroundThread(const std::string& repo) {
    std::string url = std::format("https://api.github.com/repos/{}/branches/main", repo);
    FileDownload download;
    if (!DownloadFile(url, {kGithubApiVersionHeader}, &download)) {
      return "";
    }
    nlohmann::json json = nlohmann::json::parse(download.content);
    if (json.is_discarded()) {
      Logger::get()->warn("Unable to parse github api response: {}", download.content);
      return "";
    }

    if (!json.is_object()) {
      return "";
    }
    if (json.contains("commit") && json["commit"].is_object()) {
      const auto& commit = json["commit"];
      if (commit.contains("sha") && commit["sha"].is_string()) {
        return commit["sha"].get<std::string>();
      }
    }

    return "";
  }

  std::unordered_map<std::string, std::optional<std::string>> repo_to_commit_sha_;
  std::unordered_map<std::string, std::future<std::string>> repo_to_commit_sha_futures_;
};

class BundlePackUpdateManagerImpl : public BundlePackUpdateManager {
 public:
  BundlePackUpdateManagerImpl(FileSystem* fs,
                              BundleManager* bundle_manager,
                              LocalStore* local_store)
      : fs_(fs), bundle_manager_(bundle_manager), local_store_(local_store) {}

  // Returns map of pack_name to commit sha of new version.
  std::optional<std::unordered_map<std::string, std::string>> GetBundlePacksNeedingUpdate(
      const std::vector<std::string>& pack_names) override {
    std::unordered_map<std::string, std::string> result;
    for (const std::string& pack_name : pack_names) {
      auto maybe_pack = bundle_manager_->GetBundlePackInfo(pack_name);
      if (!maybe_pack) {
        // Skip bundle packs that can't be found.
        continue;
      }
      auto maybe_sha = git_commit_sha_manager_.GetGitCommitSha(maybe_pack->source_url());
      if (!maybe_sha) {
        // One is not done. Return overall empty until all are done.
        return {};
      }
      std::string live_commit_sha = *maybe_sha;
      if (live_commit_sha.empty()) {
        // Could not retrieve sha for pack. Consider not needing update.
        continue;
      }

      std::string existing_commit_sha =
          local_store_->Get(GetGitRepoCommitShaKey(maybe_pack->source_url()));
      if (live_commit_sha != existing_commit_sha) {
        result[pack_name] = live_commit_sha;
      }
    }

    return result;
  }

  void MarkBundlePacksUpdated(
      const std::vector<std::string>& pack_names,
      const std::unordered_map<std::string, std::string>& bundle_pack_sha_map) override {
    for (const std::string& pack_name : pack_names) {
      auto maybe_pack = bundle_manager_->GetBundlePackInfo(pack_name);
      if (!maybe_pack) {
        // Skip bundle packs that can't be found.
        continue;
      }
      auto it = bundle_pack_sha_map.find(pack_name);
      if (it == bundle_pack_sha_map.end()) {
        continue;
      }

      std::string commit_sha = it->second;
      // std::cout << std::format("Putting {} -> {}", pack_name, commit_sha) << std::endl;
      local_store_->Put(GetGitRepoCommitShaKey(maybe_pack->source_url()), commit_sha);
    }
  }

 private:
  GitCommitShaManager git_commit_sha_manager_;
  FileSystem* fs_;
  BundleManager* bundle_manager_;
  LocalStore* local_store_;
};
}  // namespace

std::unique_ptr<BundlePackUpdateManager> CreateBundlePackUpdateManager(
    FileSystem* fs, BundleManager* bundle_manager, LocalStore* local_store) {
  return std::make_unique<BundlePackUpdateManagerImpl>(fs, bundle_manager, local_store);
}

}  // namespace aim
