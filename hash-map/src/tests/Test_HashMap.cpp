#include <gtest/gtest.h>
#include "HashMap.h"

template<typename K, typename V>
class Test_HashMap : public HashMap<K, V>
{
public:
    using HashMap<K, V>::getCapacity;
    using HashMap<K, V>::getLoadFactor;
    using HashMap<K, V>::getThreshold;
};

TEST(HashMap, PutGet)
{
    Test_HashMap<std::string, int> map;
    map.put("Denis", 23);
    map.put("Anna", 25);
    map.put("Димитрий", 101);
    map.put("Каппа", -201);
    EXPECT_EQ(map.size(), 4);
    EXPECT_EQ(map.get("Denis"), 23);
    EXPECT_EQ(map.get("Anna"), 25);
    EXPECT_EQ(map.get("Димитрий"), 101);
    EXPECT_EQ(map.get("Каппа"), -201);
    EXPECT_EQ(map.get("ghost"), std::nullopt);
    map.put("Denis", 25);
    EXPECT_EQ(map.get("Denis"), 25);
    map.put("Denis", 27);
    EXPECT_EQ(map.get("Denis"), 27);
}

TEST(HashMap, Size)
{
    Test_HashMap<std::string, int> map;
    EXPECT_EQ(map.size(), 0);
    EXPECT_TRUE(map.empty());
    map.put("some", 1);
    EXPECT_EQ(map.size(), 1);
    map.put("more", 2);
    EXPECT_EQ(map.size(), 2);
    ASSERT_TRUE(map.remove("more"));
    EXPECT_EQ(map.size(), 1);
}

TEST(HashMap, OverwriteValue)
{
    Test_HashMap<std::string, int> map;
    map.put("some", 1);
    EXPECT_EQ(map.get("some"), 1);
    map.put("some", 2);
    EXPECT_EQ(map.get("some"), 2);
    EXPECT_EQ(map.size(), 1);
}

TEST(HashMap, Remove)
{
    Test_HashMap<std::string, int> map;
    map.put("some", 1);
    ASSERT_TRUE(map.remove("some"));
    EXPECT_EQ(map.get("some"), std::nullopt);
    EXPECT_EQ(map.size(), 0);
    EXPECT_FALSE(map.remove("ghost"));
    EXPECT_EQ(map.size(), 0);
}

TEST(HashMap, Clear)
{
    Test_HashMap<std::string, int> map;
    map.put("some", 1);
    map.clear();
    map.clear();
    EXPECT_EQ(map.get("some"), std::nullopt);
    EXPECT_EQ(map.size(), 0);
}

TEST(HashMap, Capacity_LoadFactor_Threshold)
{
    Test_HashMap<int, int> map;
    EXPECT_EQ(map.getCapacity(), 16);
    EXPECT_EQ(map.getLoadFactor(), 0.75f);
    EXPECT_EQ(map.getThreshold(), 12);
    for (int i = 0; i < 24; ++i)
    {
        map.put(i, i);
    }
    EXPECT_EQ(map.getCapacity(), 32);
    EXPECT_EQ(map.getThreshold(), 24);
    map.put(25, 25);
    EXPECT_EQ(map.getCapacity(), 64);
    EXPECT_EQ(map.getThreshold(), 48);
    map.reset();
    EXPECT_EQ(map.getCapacity(), 16);
    EXPECT_EQ(map.getThreshold(), 12);
}

TEST(HashMap, RemoveAfterClear)
{
    Test_HashMap<std::string, int> map;
    map.put("some", 1);
    map.clear();
    EXPECT_FALSE(map.remove("some"));
}

TEST(HashMap, Reset)
{
    Test_HashMap<int, int> map;
    map.put(1, 1);
    map.reset();
    map.reset();
    EXPECT_EQ(map.size(), 0);
}

TEST(HashMap, Collisions)
{
    Test_HashMap<int, std::string> map;
    map.put(1, "one");
    map.put(17, "seventeen");
    EXPECT_EQ(map.get(1), "one");
    EXPECT_EQ(map.get(17), "seventeen");
}

TEST(HashMap, ManyInserts)
{
    Test_HashMap<int, int> map;
    const auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 20000; ++i) map.put(i, i * 10);
    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Время выполнения: " << duration.count() << "\n";
    map.reset();
    for (int j = 1; j <= 30; ++j)
    {
        const auto start_it = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 20000; ++i) map.put(i, i * 10);
        const auto end_it = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> duration_it = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << j << " Время выполнения: " << duration.count() << "\n";
        EXPECT_EQ(map.getCapacity(), 32768);
        EXPECT_EQ(map.getThreshold(), 24576);
        map.reset();
        EXPECT_EQ(map.getCapacity(), 16);
        EXPECT_EQ(map.getThreshold(), 12);
    }
}

