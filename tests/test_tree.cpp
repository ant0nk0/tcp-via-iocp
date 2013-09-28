#include "../common/btree.h"
#include "gtest/gtest.h"

#include <functional>
#include <vector>

class TreeFixture: public testing::Test
{
    typedef unsigned value_type;

public:
    std::size_t walked_elements_cnt;
    Utils::BinaryTree<value_type> tree;
    std::vector<value_type> elements;

    void CallWalk()
    {
        tree.Walk
        (
            [this] (const value_type& value)
            {
                ++walked_elements_cnt;
                elements.push_back(value);
            }
        );
    }

    virtual void SetUp() 
    {
        walked_elements_cnt = 0;
    }

    virtual void TearDown() {}
};


TEST_F(TreeFixture, TestWalkEmptyTree)
{
    CallWalk();
    EXPECT_EQ(walked_elements_cnt, 0);
    EXPECT_TRUE(elements.empty());
}

TEST_F(TreeFixture, TestRootElement)
{
    EXPECT_TRUE(tree.Add(123));
    CallWalk();
    EXPECT_EQ(walked_elements_cnt, 1);
    EXPECT_FALSE(elements.empty());
    EXPECT_EQ(elements.front(), 123);
}

TEST_F(TreeFixture, TestRootLeftChild)
{
    EXPECT_TRUE(tree.Add(123));
    EXPECT_TRUE(tree.Add(12));
    CallWalk();
    EXPECT_EQ(walked_elements_cnt, 2);
    EXPECT_FALSE(elements.empty());
    EXPECT_EQ(elements[0], 12);
    EXPECT_EQ(elements[1], 123);
}

TEST_F(TreeFixture, TestRootRightChild)
{
    EXPECT_TRUE(tree.Add(123));
    EXPECT_TRUE(tree.Add(1234));
    CallWalk();
    EXPECT_EQ(walked_elements_cnt, 2);
    EXPECT_FALSE(elements.empty());
    EXPECT_EQ(elements[0], 123);
    EXPECT_EQ(elements[1], 1234);
}

TEST_F(TreeFixture, TestAddSameElement)
{
    EXPECT_TRUE(tree.Add(123));
    EXPECT_FALSE(tree.Add(123));
    CallWalk();
    EXPECT_EQ(walked_elements_cnt, 1);
}
