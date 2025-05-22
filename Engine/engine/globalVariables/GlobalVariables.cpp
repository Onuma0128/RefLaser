#include "GlobalVariables.h"

#include <windows.h>
#include <filesystem>
#include <fstream>

#include "imgui.h"

GlobalVariables* GlobalVariables::instance_ = nullptr;

void GlobalVariables::RemoveKey(const std::string& groupName, const std::string& key)
{
    // グループを検索
    auto groupIt = std::find_if(datas_.begin(), datas_.end(), [&groupName](const auto& pair) {
        return pair.first == groupName;
        });

    // グループが見つからない場合、何もしない
    if (groupIt == datas_.end()) {
        return;
    }

    // グループ内のキーを検索して削除
    Group& group = groupIt->second;
    auto itemIt = std::find_if(group.begin(), group.end(), [&key](const auto& pair) {
        return pair.first == key;
        });

    if (itemIt != group.end()) {
        group.erase(itemIt); // キーを削除
    }
}

void GlobalVariables::RemoveSubGroup(const std::string& groupName, const std::string& subPathPrefix)
{
    auto groupIt = std::find_if(datas_.begin(), datas_.end(), [&groupName](const auto& pair) {
        return pair.first == groupName;
        });

    if (groupIt == datas_.end()) {
        return;
    }

    Group& group = groupIt->second;

    // 指定のprefixを含むキーを全て削除
    group.erase(
        std::remove_if(group.begin(), group.end(), [&subPathPrefix](const auto& pair) {
            return pair.first.starts_with(subPathPrefix + "/");
            }),
        group.end()
    );
}

void GlobalVariables::DeleteGroup(const std::string& groupName)
{
    auto it = std::find_if(datas_.begin(), datas_.end(), [&groupName](const auto& pair) {
        return pair.first == groupName;
        });

    if (it != datas_.end()) {
        datas_.erase(it); // メモリ上のデータを削除
    }

    // JSONファイルも削除
    std::filesystem::path filePath = kDirectoryPath + groupName + ".json";
    if (std::filesystem::exists(filePath)) {
        std::filesystem::remove(filePath);
    }
}

void GlobalVariables::SaveFile(const std::string& groupName)
{
    auto itGroup = std::find_if(datas_.begin(), datas_.end(), [&groupName](const auto& pair) {
        return pair.first == groupName;
        });

    if (itGroup == datas_.end()) {
        throw std::runtime_error("Group not found: " + groupName);
    }

    ordered_json root = ordered_json::object();
    ordered_json groupArray = ordered_json::array();

    using Item = std::variant<int32_t, float, Vector3, bool>;

    std::vector<ordered_json> topLevelItems;
    std::vector<std::pair<std::string, std::vector<std::pair<std::string, Item>>>> subGroupItems;

    for (auto& [fullKey, item] : itGroup->second) {
        auto parts = SplitPath(fullKey);
        if (parts.empty()) continue;

        if (parts.size() == 1) {
            const std::string& key = parts[0];
            ordered_json obj;

            if (std::holds_alternative<int32_t>(item)) {
                obj[key] = std::get<int32_t>(item);
            } else if (std::holds_alternative<float>(item)) {
                obj[key] = std::get<float>(item);
            } else if (std::holds_alternative<Vector3>(item)) {
                Vector3 vec = std::get<Vector3>(item);
                obj[key] = { vec.x, vec.y, vec.z };
            } else if (std::holds_alternative<bool>(item)) {
                obj[key] = std::get<bool>(item);
            }

            topLevelItems.push_back(obj);
        } else {
            // サブグループに分解
            std::string groupName_ = parts[0];
            std::string remainingPath;
            for (size_t i = 1; i < parts.size(); ++i) {
                if (!remainingPath.empty()) remainingPath += "/";
                remainingPath += parts[i];
            }

            // vectorから検索・追加
            auto it = std::find_if(subGroupItems.begin(), subGroupItems.end(),
                [&groupName_](const auto& pair) { return pair.first == groupName_; });

            if (it == subGroupItems.end()) {
                subGroupItems.emplace_back(groupName_, std::vector<std::pair<std::string, Item>>{ { remainingPath, item } });
            } else {
                it->second.emplace_back(remainingPath, item);
            }
        }
    }

    // トップレベルアイテムを順番に追加
    for (const auto& item : topLevelItems) {
        groupArray.push_back(item);
    }

    // サブグループを順番に構築して追加
    for (const auto& [groupKey, items] : subGroupItems) {
        ordered_json groupJson = ordered_json::object();

        for (const auto& [path, item] : items) {
            auto subParts = SplitPath(path);
            ordered_json* current = &groupJson;
            for (size_t i = 0; i + 1 < subParts.size(); ++i) {
                current = &(*current)[subParts[i]];
            }

            const std::string& leafKey = subParts.back();

            if (std::holds_alternative<int32_t>(item)) {
                (*current)[leafKey] = std::get<int32_t>(item);
            } else if (std::holds_alternative<float>(item)) {
                (*current)[leafKey] = std::get<float>(item);
            } else if (std::holds_alternative<Vector3>(item)) {
                Vector3 vec = std::get<Vector3>(item);
                (*current)[leafKey] = { vec.x, vec.y, vec.z };
            } else if (std::holds_alternative<bool>(item)) {
                (*current)[leafKey] = std::get<bool>(item);
            }
        }

        groupArray.push_back({ { groupKey, groupJson } });
    }

    root[groupName] = groupArray;
    OutputToFile(groupName, root);
}


void GlobalVariables::OutputToFile(const std::string& groupName, ordered_json root)
{
    // ディレクトリが無ければ作成
    std::filesystem::path dir(kDirectoryPath);
    if (!std::filesystem::exists(kDirectoryPath)) {
        std::filesystem::create_directory(kDirectoryPath);
    }
    // 書き込むjsonファイルのフルパスを作成
    std::string filePath = kDirectoryPath + groupName + ".json";
    // 書き込み用ファイルストリーム
    std::ofstream ofs;
    // ファイルを書き込み用に開く
    ofs.open(filePath);

    // ファイルオープンに失敗
    if (ofs.fail()) {
        std::string message = "Failed open data fail for write.";
        MessageBoxA(nullptr, message.c_str(), "GrobalVariables", 0);
        assert(0);
        return;
    }

    // ファイルにjson文字列を書き込む
    ofs << std::setw(4) << root << std::endl;
    // ファイルを閉じる
    ofs.close();
}

std::vector<std::string> GlobalVariables::SplitPath(const std::string& path) const
{
    std::vector<std::string> result;
    std::stringstream ss(path);
    std::string segment;
    while (std::getline(ss, segment, '/')) {
        result.push_back(segment);
    }
    return result;
}

void GlobalVariables::DisplayGroupItemsRecursive(const std::string& prefix, Group& group)
{
    // サブグループ用のマップ（Item* を保持する）
    std::vector<std::pair<std::string, std::vector<std::pair<std::string, Item*>>>> subGroups;

    for (auto& [fullKey, item] : group) {
        auto parts = SplitPath(fullKey);
        if (parts.empty()) continue;

        if (parts.size() == 1) {
            // キーが1階層だけの項目はそのまま表示
            const std::string& itemName = parts[0];

            auto comboIt = comboOptions_.find(fullKey);

            // Typeを選べる
            if (comboIt != comboOptions_.end()) {
                int32_t* ptr = std::get_if<int32_t>(&item);
                int currentType = *ptr;
                const std::vector<const char*>& options = comboIt->second;

                if (ImGui::Combo(itemName.c_str(), &currentType, options.data(), static_cast<int>(options.size()))) {
                    *ptr = currentType;
                }
                continue;
                // Buttonになる
            } else if (std::holds_alternative<int32_t>(item) && buttonKeys_.count(fullKey)) {
                int32_t* ptr = std::get_if<int32_t>(&item);
                ImGui::Text("%s : %d", itemName.c_str(), *ptr);
                ImGui::SameLine();
                if (ImGui::Button(std::string("+##" + itemName).c_str())) {
                    (*ptr)++;
                }
                ImGui::SameLine();
                if (ImGui::Button(std::string("-##" + itemName).c_str())) {
                    if (*ptr > 0) (*ptr)--;
                }
                continue;
                // int32_t
            } else if (std::holds_alternative<int32_t>(item)) {
                int32_t* ptr = std::get_if<int32_t>(&item);
                ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
            }
            // float
            if (std::holds_alternative<float>(item)) {
                float* ptr = std::get_if<float>(&item);
                ImGui::DragFloat(itemName.c_str(), ptr, 0.01f);
            }
            // Vector3
            if (std::holds_alternative<Vector3>(item) && itemName != "color") {
                Vector3* ptr = std::get_if<Vector3>(&item);
                ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f);
            }
            if (std::holds_alternative<Vector3>(item) && itemName == "color") {
                Vector3* ptr = std::get_if<Vector3>(&item);
                ImGui::ColorEdit3(itemName.c_str(), reinterpret_cast<float*>(ptr));
            }
            // bool
            if (std::holds_alternative<bool>(item)) {
                bool* ptr = std::get_if<bool>(&item);
                ImGui::Checkbox(itemName.c_str(), ptr);  // ← ここでポインタで反映される！
            }
        } else {
            // サブグループに分類（Item* で渡す）
            std::string subGroupName = parts[0];
            std::string remainingPath;
            for (size_t i = 1; i < parts.size(); ++i) {
                remainingPath += parts[i];
                if (i < parts.size() - 1) remainingPath += "/";
            }
            auto it = std::find_if(subGroups.begin(), subGroups.end(), [&](const auto& pair) {
                return pair.first == subGroupName;
                });

            if (it != subGroups.end()) {
                it->second.emplace_back(remainingPath, &item);
            } else {
                subGroups.emplace_back(subGroupName, std::vector<std::pair<std::string, Item*>>{ { remainingPath, & item } });
            }

        }
    }

    // サブグループの表示
    for (auto& [subName, subGroup] : subGroups) {
        if (ImGui::TreeNode(subName.c_str())) {
            DisplayGroupItemsRecursiveWithPointers(prefix + subName, subGroup);
            ImGui::TreePop();
        }
    }
}

void GlobalVariables::DisplayGroupItemsRecursiveWithPointers(const std::string& prefix, std::vector<std::pair<std::string, Item*>>& group)
{
    for (auto& [key, itemPtr] : group) {
        std::string itemName = key;
        std::string fullKey = prefix.empty() ? itemName : prefix + "/" + itemName;
        auto comboIt = comboOptions_.find(fullKey);

        if (comboIt != comboOptions_.end()) {
            int32_t* ptr = std::get_if<int32_t>(itemPtr);
            const std::vector<const char*>& options = comboIt->second;
            int current = *ptr;
            if (ImGui::Combo((itemName + "##" + fullKey).c_str(), &current, options.data(), static_cast<int>(options.size()))) {
                *ptr = current;
            }
        } else if (std::holds_alternative<int32_t>(*itemPtr) && buttonKeys_.count(fullKey)) {
            int32_t* ptr = std::get_if<int32_t>(itemPtr);
            ImGui::Text("%s : %d", itemName.c_str(), *ptr);
            ImGui::SameLine();
            if (ImGui::Button(std::string("+##" + itemName).c_str())) {
                (*ptr)++;
            }
            ImGui::SameLine();
            if (ImGui::Button(std::string("-##" + itemName).c_str())) {
                if (*ptr > 0) (*ptr)--;
            }
            continue;
        } else if (std::holds_alternative<int32_t>(*itemPtr)) {
            int32_t* ptr = std::get_if<int32_t>(itemPtr);
            ImGui::SliderInt((itemName + "##" + fullKey).c_str(), ptr, 0, 100);
        } else if (std::holds_alternative<float>(*itemPtr)) {
            float* ptr = std::get_if<float>(itemPtr);
            ImGui::DragFloat((itemName + "##" + fullKey).c_str(), ptr, 0.01f);
        } else if (std::holds_alternative<Vector3>(*itemPtr) && itemName != "color") {
            Vector3* ptr = std::get_if<Vector3>(itemPtr);
            ImGui::DragFloat3((itemName + "##" + fullKey).c_str(), reinterpret_cast<float*>(ptr), 0.01f);
        } else if (std::holds_alternative<Vector3>(*itemPtr) && itemName == "color") {
            Vector3* ptr = std::get_if<Vector3>(itemPtr);
            ImGui::ColorEdit3((itemName + "##" + fullKey).c_str(), reinterpret_cast<float*>(ptr));
        } else if (std::holds_alternative<bool>(*itemPtr)) {
            bool* ptr = std::get_if<bool>(itemPtr);
            ImGui::Checkbox((itemName + "##" + fullKey).c_str(), ptr);
        }
    }
}

void GlobalVariables::LoadJsonRecursive(const std::string& basePath, const ordered_json& node, const std::string& groupName)
{
    if (node.is_array()) {
        for (const auto& item : node) {
            if (!item.is_object()) continue;

            for (auto it = item.begin(); it != item.end(); ++it) {
                const std::string& key = it.key();
                const auto& value = it.value();

                // basePathはそのまま、groupNameは含めない！
                std::string currentPath = basePath.empty() ? key : basePath + "/" + key;

                if (value.is_object()) {
                    LoadJsonRecursive(currentPath, value, groupName); // 再帰（ツリー構造）
                } else if (value.is_array() && value.size() == 3 && value[0].is_number()) {
                    AddValue(groupName, currentPath, Vector3{ value[0], value[1], value[2] });
                } else if (value.is_number_integer()) {
                    AddValue(groupName, currentPath, value.get<int32_t>());
                } else if (value.is_number_float()) {
                    AddValue(groupName, currentPath, static_cast<float>(value.get<double>()));
                } else if (value.is_boolean()) {
                    AddValue(groupName, currentPath, value.get<bool>());
                }
            }
        }
    } else if (node.is_object()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            const std::string& key = it.key();
            const auto& value = it.value();

            std::string currentPath = basePath.empty() ? key : basePath + "/" + key;

            if (value.is_object()) {
                LoadJsonRecursive(currentPath, value, groupName);
            } else if (value.is_array() && value.size() == 3 && value[0].is_number()) {
                AddValue(groupName, currentPath, Vector3{ value[0], value[1], value[2] });
            } else if (value.is_number_integer()) {
                AddValue(groupName, currentPath, value.get<int32_t>());
            } else if (value.is_number_float()) {
                AddValue(groupName, currentPath, static_cast<float>(value.get<double>()));
            } else if (value.is_boolean()) {
                AddValue(groupName, currentPath, value.get<bool>());
            }
        }
    }
}

void GlobalVariables::LoadFiles()
{
    // ディレクトリを確認
    std::filesystem::path dir(kDirectoryPath);
    if (!std::filesystem::exists(kDirectoryPath)) {
        return;
    }
    std::filesystem::directory_iterator dir_it(kDirectoryPath);
    for (const std::filesystem::directory_entry& entry : dir_it) {
        // ファイルパスを取得
        const std::filesystem::path& filePath = entry.path();
        // ファイル拡張子を取得
        std::string extension = filePath.extension().string();
        // .jsonファイル以外はスキップ
        if (extension.compare(".json") != 0) {
            continue;
        }
        // ファイル読み込み
        LoadFile(filePath.stem().string());
    }

}

void GlobalVariables::LoadFile(const std::string& groupName)
{
    std::string filePath = kDirectoryPath + groupName + ".json";
    std::ifstream ifs(filePath);

    if (ifs.fail()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    ordered_json root;
    ifs >> root;
    ifs.close();

    if (root.find(groupName) == root.end()) {
        throw std::runtime_error("Group not found in JSON file: " + groupName);
    }

    const auto& groupData = root[groupName];

    CreateGroup(groupName);

    auto groupIt = std::find_if(datas_.begin(), datas_.end(), [&groupName](const auto& pair) {
        return pair.first == groupName;
        });

    if (groupIt == datas_.end()) {
        throw std::runtime_error("Failed to create group: " + groupName);
    }

    LoadJsonRecursive("", groupData, groupName);
}

GlobalVariables* GlobalVariables::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new GlobalVariables;
    }
    return instance_;
}

void GlobalVariables::Update()
{
#ifdef _DEBUG

    static char searchBuffer[128] = ""; // 検索文字列用バッファ

    if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }

    // 検索バーを追加
    ImGui::InputText("Search Groups", searchBuffer, sizeof(searchBuffer));

    // 各グループ
    for (auto& groupPair : datas_) {
        const std::string& groupName = groupPair.first;
        Group& group = groupPair.second;

        // TreeNode（グループ名）だけを検索
        if (strlen(searchBuffer) > 0 && groupName.find(searchBuffer) == std::string::npos) {
            continue;
        }

        if (ImGui::TreeNode(groupName.c_str())) {
            ImGui::PushItemWidth(140.0f);
            DisplayGroupItemsRecursive("", group);
            if (ImGui::Button("Export")) {
                SaveFile(groupName);
                std::string message = std::format("{}.json saved.", groupName);
                MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();

#endif // _DEBUG
}

void GlobalVariables::Finalize()
{
    delete instance_;
    instance_ = nullptr;
}

void GlobalVariables::CreateGroup(const std::string& groupName)
{
    // std::find_ifを使ってグループを検索
    auto it = std::find_if(datas_.begin(), datas_.end(), [&groupName](const auto& pair) {
        return pair.first == groupName;
        });

    // 存在しない場合、新しいグループを追加
    if (it == datas_.end()) {
        datas_.emplace_back(groupName, Group{});
    }
}
