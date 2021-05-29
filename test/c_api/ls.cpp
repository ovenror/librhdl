#include <gtest/gtest.h>

extern "C" {
#include "ls.h"
}

TEST(CApi_lsTest,libOnly)
{
	EXPECT_TRUE(::libOnly());
}

TEST(CApi_lsTest,libPlusNew)
{
	EXPECT_TRUE(::libPlusNew());
}

TEST(CApi_lsTest,interface)
{
	EXPECT_TRUE(::interface());
}

