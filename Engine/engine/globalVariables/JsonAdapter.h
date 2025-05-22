#pragma once
#include <json.hpp>
#include <string>

#include "Vector3.h"
#include "Quaternion.h"

using Json = nlohmann::json;

/* ---------------------------------------------------------
 * JsonAdapter : すべて static で構成
 * ---------------------------------------------------------
 * Json data = JsonAdapter::Load("file.json");
 * data["positions"] = Vector3{1,2,3};
 * JsonAdapter::Save("file.json", data);
 * ---------------------------------------------------------*/
class JsonAdapter
{
public:

    // ---------- ファイル I/O ----------
    static Json Load(const std::string& filePath);
    static bool Save(const std::string& filePath, const Json& data);

    // ---------- Group / Item アクセス ----------
    template<class T>
    static void Set(Json& root, const std::string& group,
        const std::string& item, const T& value)
    {
        root[group][item] = value;          // グループが無ければ自動生成
    }

    template<class T>
    static T Get(const Json& root, const std::string& group,
        const std::string& item, const T& defaultValue = {})
    {
        if (root.contains(group) && root[group].contains(item))
        {
            return root[group][item].get<T>();
        }
        return defaultValue;
    }

    template<class T>
    static T& Ref(Json& root, const std::string& group,
        const std::string& item)
    {
        return root[group][item].get_ref<T&>();   // 直接いじれる参照
    }

private:
    JsonAdapter() = delete;   // インスタンス化させない
};

/* ---------------------------------------------------------
 * Vector3 / Quaternion ⇔ json 変換
 *   - 宣言はヘッダ、定義は .cpp へ（inline なし）
 * ---------------------------------------------------------*/
namespace nlohmann
{
    template<> struct adl_serializer<Vector3>
    {
        static void to_json(json& j, const Vector3& v);
        static void from_json(const json& j, Vector3& v);
    };

    template<> struct adl_serializer<Quaternion>
    {
        static void to_json(json& j, const Quaternion& q);
        static void from_json(const json& j, Quaternion& q);
    };
} // namespace nlohmann
