#include <gtest/gtest.h>

extern "C" {
#include "construction.h"
}

TEST(CApi_constructionTest,instantiateStructure)
{
	EXPECT_TRUE(::instantiateStructure());
}

TEST(CApi_constructionTest,getEntityInterface)
{
	EXPECT_TRUE(::getEntityInterface());
}

TEST(CApi_constructionTest,exposeInverter)
{
	EXPECT_TRUE(::exposeInverter());
}

TEST(CApi_constructionTest,unnamedExposure)
{
	EXPECT_TRUE(::unnamedExposure());
}

TEST(CApi_constructionTest,connectInverters2)
{
	EXPECT_TRUE(::connectInverters2());
}

TEST(CApi_constructionTest,exposeComposite)
{
	EXPECT_TRUE(::exposeComposite());
}

