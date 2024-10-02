#include <gtest/gtest.h>

extern "C" {
#include "objects.h"
}

TEST(CApi_objectsTest,rootNamespace)
{
	EXPECT_TRUE(::rootNamespace());
}

TEST(CApi_objectsTest,entities)
{
	EXPECT_TRUE(::entities());
}

TEST(CApi_objectsTest,wrong)
{
	EXPECT_TRUE(::wrong());
}

