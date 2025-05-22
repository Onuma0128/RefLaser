#pragma once
#include <list>
#include <unordered_set>
#include <tuple>
#include "Collider.h"

class CollisionManager
{
public:
    void CheckAllCollisions();
    void AddCollider(Collider* c) { colliders_.push_back(c); }
    void RemoveCollider(Collider* c) { colliders_.remove(c); }

private:
    // ペアをソート
    static std::pair<Collider*, Collider*> MakeKey(Collider* a, Collider* b) {
        return (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
    }

    struct PairHasher {
        size_t operator()(const std::pair<Collider*, Collider*>& p) const noexcept {
            return std::hash<Collider*>()(p.first) ^ (std::hash<Collider*>()(p.second) << 1);
        }
    };

    using PairSet = std::unordered_set<std::pair<Collider*, Collider*>, PairHasher>;

    bool Dispatch(Collider* a, Collider* b);
    void CheckCollisionPair(Collider* a, Collider* b, PairSet& thisFrame);

    std::list<Collider*> colliders_;
    PairSet previousFrame_;
};