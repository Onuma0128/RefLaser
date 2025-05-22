#include "JsonAdapter.h"
#include <fstream>

// ---------- JsonAdapter ファイル I/O ----------
Json JsonAdapter::Load(const std::string& filePath)
{
    std::ifstream ifs(filePath);
    if (!ifs) { return Json{}; }

    Json j;
    ifs >> j;
    return j;
}

bool JsonAdapter::Save(const std::string& filePath, const Json& data)
{
    std::ofstream ofs(filePath);
    if (!ofs) { return false; }

    ofs << data.dump(4);      // インデント 4 スペース
    return true;
}

// ---------- Vector3 変換 ----------
void nlohmann::adl_serializer<Vector3>::to_json(json& j, const Vector3& v)
{
    j = json::array({ v.x, v.y, v.z });
}

void nlohmann::adl_serializer<Vector3>::from_json(const json& j, Vector3& v)
{
    if (j.is_array() && j.size() == 3)
    {
        v.x = j[0].get<float>();
        v.y = j[1].get<float>();
        v.z = j[2].get<float>();
    } else { v = { 0.0f, 0.0f, 0.0f }; }
}

// ---------- Quaternion 変換 ----------
void nlohmann::adl_serializer<Quaternion>::to_json(json& j, const Quaternion& q)
{
    j = json::array({ q.x, q.y, q.z, q.w });
}

void nlohmann::adl_serializer<Quaternion>::from_json(const json& j, Quaternion& q)
{
    if (j.is_array() && j.size() == 4)
    {
        q.x = j[0].get<float>();
        q.y = j[1].get<float>();
        q.z = j[2].get<float>();
        q.w = j[3].get<float>();
    } else { q = { 0.0f, 0.0f, 0.0f, 1.0f }; }
}
