#include <gtest/gtest.h>

extern "C" {
#include "misconstruction.h"
}

TEST(CApi_misconstructionTest,emptyInterface)
{
	EXPECT_TRUE(::emptyInterface());
}

TEST(CApi_misconstructionTest,incompatibleDirections)
{
	EXPECT_TRUE(::incompatibleDirections());
}

TEST(CApi_misconstructionTest,wrongOp)
{
	EXPECT_TRUE(::wrongOp());
}

TEST(CApi_misconstructionTest,ambiguousConnection)
{
	EXPECT_TRUE(::ambiguousConnection());
}

TEST(CApi_misconstructionTest,statefulComponentInStatelessStructure)
{
	EXPECT_TRUE(::statefulComponentInStatelessStructure());
}

TEST(CApi_misconstructionTest,introduceCycle)
{
	EXPECT_TRUE(::introduceCycle());
}

TEST(CApi_misconstructionTest,noSuchInterface)
{
	EXPECT_TRUE(::noSuchInterface());
}

TEST(CApi_misconstructionTest,noSuchEntity)
{
	EXPECT_TRUE(::noSuchEntity());
}

TEST(CApi_misconstructionTest,entityExists)
{
	EXPECT_TRUE(::entityExists());
}

TEST(CApi_misconstructionTest,unknownStruct)
{
	EXPECT_TRUE(::unknownStruct());
}

TEST(CApi_misconstructionTest,wrongStruct)
{
	EXPECT_TRUE(::wrongStruct());
}

TEST(CApi_misconstructionTest,passThrough)
{
	EXPECT_TRUE(::passThrough());
}

