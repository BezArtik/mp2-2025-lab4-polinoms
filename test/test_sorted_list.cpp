#include <gtest.h>
#include "SortedList.h"
#include <vector>

TEST(SortedList, empty_is_empty) {
    SortedList<int> list;
    EXPECT_TRUE(list.is_empty());
}

TEST(SortedList, empty_has_zero_size) {
    SortedList<int> list;
    EXPECT_EQ(list.size(), 0);
}

TEST(SortedList, with_elements_is_not_empty) {
    SortedList<int> list = { 1,5,6,3,4 };
    EXPECT_FALSE(list.is_empty());
}

TEST(SortedList, with_elements_has_size) {
    SortedList<int> list = { 1,2,3,4 };
    EXPECT_EQ(list.size(), 4);
}

TEST(SortedList, single_element_list) {
    SortedList<int> list = { 42 };
    EXPECT_FALSE(list.is_empty());
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.front(), 42);
    EXPECT_EQ(list.back(), 42);
}


TEST(SortedList, front_is_correct) {
    SortedList<int> list = { 1,2,3,4 };
    EXPECT_EQ(list.front(), 1);
}

TEST(SortedList, back_is_correct) {
    SortedList<int> list = { 1,2,3,4 };
    EXPECT_EQ(list.back(), 4);
}

TEST(SortedList, can_insert_elements) {
    SortedList<int> list;
    list.insert(2);
    list.insert(1);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 2);
}

TEST(SortedList, can_insert_back_elements) {
    SortedList<int> list;
    list.insert_back(2);
    list.insert_back(1);
    EXPECT_EQ(list.front(), 2);
    EXPECT_EQ(list.back(), 1);
}

TEST(SortedList, insert_duplicate_values) {
    SortedList<int> list = { 1, 2, 2, 2, 3 };
    auto it = list.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);
}

TEST(SortedList, insert_same_value_multiple_times) {
    SortedList<int> list;
    list.insert(5);
    list.insert(5);
    list.insert(5);
    EXPECT_EQ(list.size(), 3);
}

TEST(SortedList, insert_unique_does_not_save_duplicates) {
    SortedList<int> list;
    list.insert_unique(1);
    list.insert_unique(2);
    list.insert_unique(2);
    list.insert_unique(2);
    list.insert_unique(3);
    list.insert_unique(4);
    int i = 0;
    for (const auto& x : list) {
        EXPECT_EQ(x, i + 1);
        ++i;
    }
}

TEST(SortedList, can_insert_by_iterators) {
    std::vector<int> vec = { 1,2,3,4,5,6,7 };
    auto it = vec.begin();
    ++it;
    ++it;
    SortedList<int> list;
    list.insert(it, vec.end());
    for (auto it1 = list.begin(); it1 != list.end(); ++it1) {
        EXPECT_EQ(*it, *it1);
        ++it;
    }
}

TEST(SortedList, can_erase_elements) {
    SortedList<int> list = { 1,2,3,4,5,6 };
    auto it = list.begin();
    list.erase(it);
    int i = 1;
    for (it = list.begin(); it != list.end(); ++it) {
        EXPECT_EQ(*it, i + 1);
        ++i;
    }
}

TEST(SortedList, erase_from_empty_list) {
    SortedList<int> list;
    EXPECT_NO_THROW(list.erase(list.begin()));
}

TEST(SortedList, erase_all_elements) {
    SortedList<int> list = { 1, 2, 3 };
    auto it = list.begin();
    while (it != list.end()) {
        it = list.erase(it);
    }
    EXPECT_TRUE(list.is_empty());
    EXPECT_EQ(list.size(), 0);
}

TEST(SortedList, erase_last_element) {
    SortedList<int> list = { 1, 2, 3 };
    auto it = --list.end();
    list.erase(it);
    EXPECT_EQ(list.back(), 2);
    EXPECT_EQ(list.size(), 2);
}

TEST(SortedList, cleared_is_empty) {
    SortedList<int> list = { 1,2,3,4,5 };
    list.clear();
    EXPECT_TRUE(list.is_empty());
}

TEST(SortedList, cleared_is_zero_size) {
    SortedList<int> list = { 1,2,3,4,5 };
    list.clear();
    EXPECT_EQ(list.size(), 0);
}

TEST(SortedList, can_work_with_iterators) {
    SortedList<int> list;
    for (size_t i = 0; i < 10; ++i) {
        list.insert(i + 1);
    }
    int i = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        EXPECT_EQ(*it, i + 1);
        ++i;
    }
}

TEST(SortedList, iterator_invalidation) {
    SortedList<int> list = { 1, 2, 3, 4, 5 };
    auto it = list.begin();
    ++it;
    list.erase(list.begin());
    EXPECT_EQ(*it, 2);
}

TEST(SortedList, end_iterator_operations) {
    SortedList<int> list = { 1, 2, 3 };
    auto it = list.end();
    --it;
    EXPECT_EQ(*it, 3);
}

TEST(SortedList, const_iterators) {
    const SortedList<int> list = { 1, 2, 3, 4 };
    int sum = 0;
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 10);
}

TEST(SortedList, find_existing_element) {
    SortedList<int> list = { 1, 2, 3, 4, 5 };
    auto it = list.find(3);
    ASSERT_NE(it, list.end());
    EXPECT_EQ(*it, 3);
}

TEST(SortedList, find_non_existing_element) {
    SortedList<int> list = { 1, 2, 3, 4, 5 };
    auto it = list.find(42);
    EXPECT_EQ(it, list.end());
}

TEST(SortedList, find_in_empty_list) {
    SortedList<int> list;
    auto it = list.find(1);
    EXPECT_EQ(it, list.end());
}

TEST(SortedList, elements_are_sorted_automatically) {
    SortedList<int> list = { 6,3,2,5,4,1 };
    int i = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        EXPECT_EQ(*it, i + 1);
        ++i;
    }
}

TEST(SortedList, elements_are_sorted_with_other_comparator) {
    SortedList<int, std::greater<int>> list = { 6,3,2,5,4,1 };
    int i = 0;
    for (auto it = --list.end(); it != list.begin(); --it) {
        EXPECT_EQ(*it, i + 1);
        ++i;
    }
}

TEST(SortedList, copy_constructor) {
    SortedList<int> list = { 1,2,3,4,5 };
    SortedList<int> list1(list);
    auto it1 = list.begin();
    auto it2 = list1.begin();
    while (it1 != list.end() && it2 != list1.end()) {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}

TEST(SortedList, copy_assign_constructor) {
    SortedList<int> list = { 1,2,3,4,5 }, list1;
    list1 = list;
    auto it1 = list.begin();
    auto it2 = list1.begin();
    while (it1 != list.end() && it2 != list1.end()) {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}

TEST(SortedList, move_constructor) {
    SortedList<int> list1 = { 1, 2, 3, 4, 5 };
    SortedList<int> list2(std::move(list1));

    EXPECT_TRUE(list1.is_empty());
    EXPECT_EQ(list2.size(), 5);
    int i = 1;
    for (const auto& val : list2) {
        EXPECT_EQ(val, i++);
    }
}

TEST(SortedList, move_assignment) {
    SortedList<int> list1 = { 1, 2, 3 };
    SortedList<int> list2 = { 4, 5, 6 };

    list2 = std::move(list1);

    EXPECT_TRUE(list1.is_empty());
    EXPECT_EQ(list2.size(), 3);
    int i = 1;
    for (const auto& val : list2) {
        EXPECT_EQ(val, i++);
    }
}

TEST(SortedList, can_sort_elements) {
    SortedList<int> list;
    const int N = 1'000'000;

    for (int i = N; i > 0; --i) {
        list.insert_back(i);
    }
    list.sort();

    int i = 1;
    for (const auto& x : list) {
        EXPECT_EQ(x, i);
        ++i;
    }
}

TEST(SortedList, large_number_of_elements) {
    SortedList<int> list;
    const int N = 1'000'000;

    for (int i = N; i > 0; --i) {
        list.insert(i);
    }

    EXPECT_EQ(list.size(), N);

    int i = 1;
    for (const auto& x : list) {
        EXPECT_EQ(x, i);
        ++i;
    }
}