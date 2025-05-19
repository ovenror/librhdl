#include <gtest/gtest.h>

extern "C" {
#include "object_classic.h"
}

TEST(CApi_object_classicTest,entity)
{
	EXPECT_TRUE(::entity());
}

TEST(CApi_object_classicTest,no_entity)
{
	EXPECT_TRUE(::no_entity());
}

