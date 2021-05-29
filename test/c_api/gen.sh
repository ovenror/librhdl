cppheader() {
	cat << EOF
#include <gtest/gtest.h>

extern "C" {
#include "$1.h"
}

EOF
}

cpptest() {
	cat << EOF
TEST(CApi_$1Test,$2)
{
	EXPECT_TRUE(::$2());
}

EOF
}

for csrc in *.c
do
	name=$(echo $csrc | sed 's/\.c//')
	grep '^int .*()' $csrc | sed 's/$/;/' > $name.h
	
	cppheader $name > $name.cpp

	for func in $(sed 's/int //' $name.h | sed 's/();$//')
	do
		cpptest $name $func >> $name.cpp	
	done
done

