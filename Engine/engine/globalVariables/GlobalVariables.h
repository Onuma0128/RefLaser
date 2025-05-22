#pragma once

#include <variant>
#include <map>
#include <string>
#include <vector>
#include <json.hpp>
#include <unordered_set>

#include "Vector3.h"

class GlobalVariables
{
private:
	static GlobalVariables* instance_;

	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(GlobalVariables&) = delete;
	GlobalVariables& operator=(GlobalVariables&) = delete;

	// 項目
	using Item = std::variant<int32_t, float, Vector3, bool>;
	// グループ
	using Group = std::vector<std::pair<std::string, Item>>;

public:
	// シングルトンインスタンスの取得
	static GlobalVariables* GetInstance();

	// 更新
	void Update();
	// 終了
	void Finalize();

	// グループの作成
	void CreateGroup(const std::string& groupName);
	// 項目のアクセッサ
	template <typename T>
	void SetValue(const std::string& groupName, const std::string& key, T value,
		const std::vector<const char*>& comboItems = {});
	template <typename T>
	inline T GetValue(const std::string& groupName, const std::string& key) const;
	// 項目の追加
	template <typename T>
	void AddValue(const std::string& groupName, const std::string& key, const T& value,
		const std::vector<const char*>& comboItems = {}, bool showAsButton = false);
	// 保存されていないキーの削除
	void RemoveKey(const std::string& groupName, const std::string& key);
	// グループの中のサブグループの削除
	void RemoveSubGroup(const std::string& groupName, const std::string& subPathPrefix);
	// グループの削除
	void DeleteGroup(const std::string& groupName);
	// グループの保存
	void SaveFile(const std::string& groupName);
	// ディレクトリの全ファイルの読み込み
	void LoadFiles();
	// ファイルから読み込む
	void LoadFile(const std::string& groupName);

private:
	// using宣言
	using ordered_json = nlohmann::ordered_json;
	// 全データ
	std::vector<std::pair<std::string, Group>> datas_;
	// Combo管理用
	std::unordered_map<std::string, std::vector<const char*>> comboOptions_;
	// ボタン管理
	std::unordered_set<std::string> buttonKeys_;
	// 保存先ファイルパス
	const std::string kDirectoryPath = "resources/globalVariables/";
	// ファイル書き出し
	void OutputToFile(const std::string& groupName, ordered_json root);
	// ヘルパー関数
	std::vector<std::string> SplitPath(const std::string& path) const;
	void DisplayGroupItemsRecursive(const std::string& prefix, Group& group);
	void DisplayGroupItemsRecursiveWithPointers(const std::string& prefix, std::vector<std::pair<std::string, Item*>>& group);
	void LoadJsonRecursive(const std::string& basePath, const ordered_json& node, const std::string& groupName);
};

//template<typename T>
//inline void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, T value)
//{
//	// std::find_ifを使ってグループを検索
//	auto groupIt = std::find_if(datas_.begin(), datas_.end(), [&groupName](const auto& pair) {
//		return pair.first == groupName;
//		});
//
//	// グループが見つからない場合エラーをスロー
//	if (groupIt == datas_.end()) {
//		throw std::runtime_error("Group not found: " + groupName);
//	}
//
//	// グループ内のアイテムを検索
//	Group& group = groupIt->second;
//	auto itemIt = std::find_if(group.begin(), group.end(), [&key](const auto& pair) {
//		return pair.first == key;
//		});
//
//	if (itemIt == group.end()) {
//		// アイテムが見つからない場合、新しいアイテムを追加
//		group.emplace_back(key, value);
//	} else {
//		// 既存アイテムの値を更新
//		itemIt->second = value;
//	}
//}

template<typename T>
inline void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, T value, const std::vector<const char*>& comboItems)
{
	// std::find_ifを使ってグループを検索
	auto groupIt = std::find_if(datas_.begin(), datas_.end(), [&groupName](const auto& pair) {
		return pair.first == groupName;
		});

	// グループが見つからない場合エラーをスロー
	if (groupIt == datas_.end()) {
		throw std::runtime_error("Group not found: " + groupName);
	}

	// グループ内のアイテムを検索
	Group& group = groupIt->second;
	auto itemIt = std::find_if(group.begin(), group.end(), [&key](const auto& pair) {
		return pair.first == key;
		});

	if (itemIt == group.end()) {
		// アイテムが見つからない場合、新しいアイテムを追加
		group.emplace_back(key, value);
	} else {
		// 既存アイテムの値を更新
		itemIt->second = value;
	}

	if constexpr (std::is_same_v<T, int32_t>) {
		if (!comboItems.empty()) {
			comboOptions_[key] = comboItems;
		}
	}
}

template<typename T>
inline T GlobalVariables::GetValue(const std::string& groupName, const std::string& key) const
{
	// グループの存在確認
	auto groupIt = std::find_if(datas_.begin(), datas_.end(), [&groupName](const auto& pair) {
		return pair.first == groupName;
		});
	if (groupIt == datas_.end()) {
		throw std::runtime_error("Group not found: " + groupName);
	}

	// 項目の存在確認
	const Group& group = groupIt->second;
	auto itemIt = std::find_if(group.begin(), group.end(), [&key](const auto& pair) {
		return pair.first == key;
		});
	if (itemIt == group.end()) {
		throw std::runtime_error("Item not found in group '" + groupName + "': " + key);
	}

	// 項目の型を確認して値を取得
	const Item& item = itemIt->second;
	if (std::holds_alternative<T>(item)) {
		return std::get<T>(item);
	} else {
		throw std::runtime_error("Type mismatch when getting item '" + key + "' in group '" + groupName + "'");
	}
}

template<typename T>
inline void GlobalVariables::AddValue(const std::string& groupName, const std::string& key, const T& value, const std::vector<const char*>& comboItems, bool showAsButton)
{
	auto groupIt = std::find_if(datas_.begin(), datas_.end(), [&groupName](const auto& pair) {
		return pair.first == groupName;
		});

	if (groupIt == datas_.end()) {
		CreateGroup(groupName);
		groupIt = std::find_if(datas_.begin(), datas_.end(), [&groupName](const auto& pair) {
			return pair.first == groupName;
			});
	}

	Group& group = groupIt->second;

	// すでに存在していなければ追加
	auto itemIt = std::find_if(group.begin(), group.end(), [&key](const auto& pair) {
		return pair.first == key;
		});

	if (itemIt == group.end()) {
		group.emplace_back(key, value);
	}

	// comboItems がある場合は comboOptions_ に登録
	if constexpr (std::is_same_v<T, int32_t>) {
		if (!comboItems.empty()) {
			comboOptions_[key] = comboItems;
		}
	}

	if (showAsButton) {
		buttonKeys_.insert(key);
	}
}
