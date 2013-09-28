// tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gtest/gtest.h"

int _tmain(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}