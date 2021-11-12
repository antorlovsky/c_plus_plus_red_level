#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <utility>
#include <map>

using namespace std;

template <typename T>
class PriorityCollection {
public:
    using Id = int;

    Id Add(T object) {
        Id id = id_counter++;

        data_[id] = move(object);
        priorities_[id] = 0;
        order_.insert({0, id});

        return id;
    }

    template <typename ObjInputIt, typename IdOutputIt>
    void Add(ObjInputIt range_begin, ObjInputIt range_end, IdOutputIt ids_begin) {
        transform(range_begin, range_end, ids_begin, [this](auto& item) {
            Id id = id_counter++;

            data_[id] = move(item);
            priorities_[id] = 0;
            order_.insert({0, id});

            return id;
        });
    }

    bool IsValid(Id id) const {
        return data_.count(id) == 1;
    }

    const T& Get(Id id) const {
        return data_.at(id);
    }

    void Promote(Id id) {
        order_.erase({priorities_[id], id});
        order_.insert({++priorities_[id], id});
    }

    pair<const T&, int> GetMax() const {
        auto max_priority = order_.rbegin();
        return {data_.at(max_priority->second), max_priority->first};
    }

    pair<T, int> PopMax() {
        auto max_priority = order_.rbegin();
        pair<T, int> pop_element = {move(data_[max_priority->second]), max_priority->first};

        data_.erase(max_priority->second);
        priorities_.erase(max_priority->second);
        order_.erase(prev(order_.end()));

        return pop_element;
    }

private:
    set<pair<int, Id>> order_;
    map<Id, T> data_;
    map<Id, int> priorities_;
    int id_counter = 0;
};


class StringNonCopyable : public string {
public:
    using string::string;
    StringNonCopyable(const StringNonCopyable&) = delete;
    StringNonCopyable(StringNonCopyable&&) = default;
    StringNonCopyable& operator=(const StringNonCopyable&) = delete;
    StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
    PriorityCollection<StringNonCopyable> strings;
    const auto white_id = strings.Add("white");
    const auto yellow_id = strings.Add("yellow");
    const auto red_id = strings.Add("red");

    strings.Promote(yellow_id);
    for (int i = 0; i < 2; ++i) {
        strings.Promote(red_id);
    }
    strings.Promote(yellow_id);
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "red");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "yellow");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "white");
        ASSERT_EQUAL(item.second, 0);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestNoCopy);
    return 0;
}