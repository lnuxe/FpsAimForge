#include "bundle_ui.h"

#include "absl/strings/ascii.h"
#include "aim/common/files.h"
#include "aim/common/imgui_ext.h"
#include "aim/common/mat_icons.h"
#include "aim/common/proto_util.h"
#include "aim/core/application.h"
#include "aim/core/bundle_manager.h"
#include "aim/core/playlist_manager.h"
#include "aim/core/scenario_manager.h"
#include "aim/proto/bundle.pb.h"
#include "aim/ui/bundle_pack_update_dialog.h"
#include "aim/ui/object_browser.h"
#include "imgui.h"

namespace aim {
namespace {

struct CachedBundleDetails {
  i64 num_playlists = 0;
  i64 num_scenarios = 0;
};

class AddBundleDialog {
 public:
  explicit AddBundleDialog(const std::string& id) : popup_(id) {}

  void NotifyOpen(std::optional<std::string> bundle_to_copy) {
    bundle_to_copy_ = bundle_to_copy;
    bundle_name_ = "";
    popup_.Open();
  }

  std::optional<std::string> Draw(Application& app) {
    ImGui::IdGuard cid("AddBundleDialogContent");
    if (!popup_.Begin()) {
      return {};
    }

    std::optional<std::string> added_name;
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Bundle name");
    ImGui::SameLine();
    ImGui::InputText("##BundleNameInput", &bundle_name_);

    bool name_taken = app.bundle_manager().GetBundleInfo(bundle_name_).has_value();
    bool is_valid = IsValidBundleName(bundle_name_) && !name_taken;

    if (!is_valid) {
      ImGui::BeginDisabled();
    }

    if (!bundle_name_.empty() && !is_valid) {
      if (name_taken) {
        ImGui::TextFmt("Bundle name \"{}\" already exists", bundle_name_);
      } else {
        ImGui::TextFmt("Bundle name \"{}\" is invalid. Can only contains letters, numbers, and _",
                       bundle_name_);
      }
    }

    if (ImGui::Button(bundle_to_copy_ ? "Copy" : "Add")) {
      BundleInfo info;
      info.set_bundle_name(bundle_name_);
      app.bundle_manager().UpdateBundleInfo(info);

      if (bundle_to_copy_) {
        app.bundle_manager().CopyBundle(*bundle_to_copy_, bundle_name_);
      }

      added_name = bundle_name_;
      popup_.Close();
    }
    if (!is_valid) {
      ImGui::EndDisabled();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      popup_.Close();
    }

    popup_.End();
    return added_name;
  }

 private:
  std::string bundle_name_;
  std::optional<std::string> bundle_to_copy_;
  ImGui::Popup popup_;
};

class BundleUiComponentImpl : public BundleUiComponent {
 public:
  explicit BundleUiComponentImpl(UiScreen& screen) : screen_(screen), app_(screen.app()) {}

  void Show() override {
    ImGui::IdGuard cid("BundleUiComponent");

    auto to_delete = delete_confirmation_dialog_.Draw("Delete");
    if (to_delete) {
      screen_.app().bundle_manager().DeleteBundle(*to_delete);
      selected_bundle_name_ = "";
    };

    std::vector<std::string> updated_bundle_packs;
    bundle_pack_update_dialog_->Draw(&updated_bundle_packs);

    auto added_name = add_dialog_.Draw(app_);
    if (added_name) {
      selected_bundle_name_ = *added_name;
    }

    float char_x = ImGui::GetDefaultCharSizeX();
    float default_list_width = char_x * 20;
    float default_content_width = char_x * 40;
    ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable |
                            ImGuiTableFlags_BordersInnerV;
    if (ImGui::BeginTable("BundleColumns", 4, flags)) {
      ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, char_x * 20);
      ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, default_content_width);
      ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, char_x * 20);
      ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

      ImGui::TableNextColumn();

      ImGui::Spacing();

      if (ImGui::Button(std::format("{} Bundle", icons::kAdd))) {
        add_dialog_.NotifyOpen({});
      }

      ImGui::SpacedSeparator();

      ImGui::BeginChild("BundleListColumn");
      DrawBundlesList();
      ImGui::EndChild();

      ImGui::TableNextColumn();
      DrawBundleRightPanel();

      ImGui::TableNextColumn();
      DrawBundlePackList();

      ImGui::TableNextColumn();
      DrawBundlePackRightPanel();

      ImGui::EndTable();
    }
  }

 private:
  void DrawBundlePackList() {
    ImGui::Spacing();
    if (ImGui::Button(std::format("{} Import bundle pack", icons::kAdd))) {
      // BundlePackUpdateDialog::Item item;
      // item.bundle_pack_name = kDefaultBundlePackName;
      // item.url = "https://github.com/mjohns/FpsAimForgeBundles";
      // bundle_pack_update_dialog_->NotifyOpen({item});
    }
    ImGui::SpacedSeparator();

    ImGui::BeginChild("BundlePackListColumn");
    ImGui::LoopId loop_id;
    for (const std::string& bundle_pack : app_.bundle_manager().GetBundlePackNames()) {
      auto id_guard = loop_id.Get();
      if (ImGui::Selectable(" " + bundle_pack, bundle_pack == selected_bundle_pack_name_)) {
        selected_bundle_pack_name_ = bundle_pack;
      }
    }
    ImGui::EndChild();
  }

  void DrawBundlePackRightPanel() {
    auto pack = app_.bundle_manager().GetBundlePackInfo(selected_bundle_pack_name_);
    if (!pack) {
      return;
    }
    BundlePackInfo info = *pack;
    ImGui::AlignTextToFramePadding();
    ImGui::TextFmt("Bundle pack name: {}", info.bundle_pack_name());

    ImGui::AlignTextToFramePadding();
    ImGui::Text("URL");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputText("##UrlInput", info.mutable_source_url());
    absl::StripAsciiWhitespace(info.mutable_source_url());

    std::string repo_name = GetGithubRepoNameFromUrl(info.source_url());
    if (!repo_name.empty()) {
      ImGui::InputBool("Automatically check for updates",
                       PROTO_BOOL_FIELD(BundlePackInfo, &info, automatically_check_for_updates));
    } else {
      info.clear_automatically_check_for_updates();
    }

    ImGui::InputBool("Archived", PROTO_BOOL_FIELD(BundlePackInfo, &info, archived));
    ImGui::SameLine();
    ImGui::HelpMarker("Bundle pack will not be loaded");

    if (info.archived()) {
      info.clear_automatically_check_for_updates();
    }

    if (!IsEquivalentProto(info, *pack)) {
      app_.bundle_manager().UpdateBundlePackInfo(info);
      if (info.archived() != pack->archived()) {
        app_.bundle_manager().LoadBundlesFromDisk();
      }
    }
  }

  void DrawBundleRightPanel() {
    if (!selected_bundle_name_.empty()) {
      DrawSelectedBundle();
    }

    if (ImGui::Button(std::format("{} Folder", icons::kOpenInNew))) {
      OpenFolderInExplorer(app_.file_system().GetUserDataPath("bundles"));
    }
    ImGui::HelpTooltip("Open bundles folder in file explorer");
  }

  void DrawSelectedBundle() {
    auto maybe_bundle_info = app_.bundle_manager().GetBundleInfo(selected_bundle_name_);
    if (!maybe_bundle_info) {
      ImGui::Text("Could not find bundle: %s", selected_bundle_name_.c_str());
      return;
    }
    BundleInfo info = *maybe_bundle_info;

    ImGui::Text("Bundle name: %s", selected_bundle_name_.c_str());

    bool need_update = false;
    bool reload_bundles_from_disk = false;

    if (selected_bundle_name_ == kUserBundleName) {
      // Force user bundle being writable if necessary.
      if (info.readonly()) {
        info.set_readonly(false);
        need_update = true;
      }
    } else {
      bool readonly = info.readonly();
      ImGui::AlignTextToFramePadding();
      ImGui::Text("Readonly");
      ImGui::SameLine();
      if (ImGui::Checkbox("##ReadonlyInput", &readonly)) {
        info.set_readonly(readonly);
        need_update = true;
      }

      bool archived = info.archived();
      ImGui::AlignTextToFramePadding();
      ImGui::Text("Archived");
      ImGui::SameLine();
      if (ImGui::Checkbox("##ArchivedInput", &archived)) {
        info.set_archived(archived);
        need_update = true;
        reload_bundles_from_disk = true;
      }
      ImGui::SameLine();
      ImGui::HelpMarker("Bundle will not be loaded");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    CachedBundleDetails details = GetBundleDetails(selected_bundle_name_);
    ImGui::TextFmt("{} scenarios", details.num_scenarios);
    if (details.num_playlists > 0) {
      ImGui::TextFmt("{} playlists", details.num_playlists);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button(std::format("{} Copy", icons::kContentCopy))) {
      add_dialog_.NotifyOpen(selected_bundle_name_);
    }

    if (ImGui::Button(std::format("{} Delete", icons::kDelete))) {
      delete_confirmation_dialog_.NotifyOpen(std::format("Delete \"{}\"?", selected_bundle_name_),
                                             selected_bundle_name_);
    }

    if (need_update) {
      app_.bundle_manager().UpdateBundleInfo(info);
    }
    if (reload_bundles_from_disk) {
      app_.bundle_manager().LoadBundlesFromDisk();
    }
  }

  void DrawBundlesList() {
    ImGui::LoopId loop_id;
    auto bundle_infos = app_.bundle_manager().GetBundleInfos();
    for (const BundleInfo& bundle : bundle_infos) {
      auto id_guard = loop_id.Get();
      std::string display_text = " " + bundle.bundle_name();
      std::string help_text;
      if (bundle.archived()) {
        display_text = std::format("{} {}", display_text, icons::kArchive);
        help_text = "Archived";
      } else if (bundle.readonly()) {
        display_text = std::format("{} {}", display_text, icons::kEditOff);
        help_text = "Readonly";
      }
      if (ImGui::Selectable(display_text.c_str(), bundle.bundle_name() == selected_bundle_name_)) {
        selected_bundle_name_ = bundle.bundle_name();
      }
      if (help_text.size() > 0) {
        ImGui::HelpTooltip(help_text, 0.5);
      }
    }
  }

  CachedBundleDetails GetBundleDetails(const std::string& bundle_name) {
    auto it = bundle_details_map_.find(bundle_name);
    if (it != bundle_details_map_.end()) {
      return it->second;
    }

    CachedBundleDetails& details = bundle_details_map_[bundle_name];
    std::string bundle_name_prefix = bundle_name + " ";
    for (const std::string& name : *app_.playlist_manager().playlist_names()) {
      if (name.starts_with(bundle_name_prefix)) {
        details.num_playlists++;
      }
    }
    for (const std::string& name : *app_.scenario_manager().scenario_names()) {
      if (name.starts_with(bundle_name_prefix)) {
        details.num_scenarios++;
      }
    }
    return details;
  }

  UiScreen& screen_;
  Application& app_;

  ImGui::ConfirmationDialog<std::string> delete_confirmation_dialog_{"DeleteConfirmationDialog"};
  std::string selected_bundle_name_ = "AF";
  std::string selected_bundle_pack_name_ = kDefaultBundlePackName;
  AddBundleDialog add_dialog_{"AddBundleDialog"};
  std::unordered_map<std::string, CachedBundleDetails> bundle_details_map_;
  std::unique_ptr<BundlePackUpdateDialog> bundle_pack_update_dialog_ =
      CreateBundlePackUpdateDialog();
};

}  // namespace

std::unique_ptr<BundleUiComponent> CreateBundleUiComponent(UiScreen* screen) {
  return std::make_unique<BundleUiComponentImpl>(*screen);
}

}  // namespace aim
