#include <gtest/gtest.h>

extern "C" {
#include "namespaces.h"
}

TEST(CApi_namespacesTest,createToplevelNamespace)
{
	EXPECT_TRUE(::createToplevelNamespace());
}

TEST(CApi_namespacesTest,createNamespace)
{
	EXPECT_TRUE(::createNamespace());
}

