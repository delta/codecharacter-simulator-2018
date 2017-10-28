#include "gtest/gtest.h"

using namespace std;

/**
 * Tests to make sure tests are testing (testing tests)
 */
TEST(Test, TestingTest) { EXPECT_GT(5, 2); }

/**
 * Run Tests
 */
int main(int argc, char *argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
