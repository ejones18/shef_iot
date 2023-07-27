/**
 * @file testCheckImg.ino
 * @brief Unittests for the check_img function.
 * 
 * @author Ethan Jones
 * @date 2023-05-12
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h> //Include Google test framework - see: https://github.com/google/googletest for instructions to run

#include "../camera_utils.h"

using namespace testing;

/**
 * @brief A mock class for the fs::FS interface.
 */
class MockFS : public fs::FS {
public:
    MOCK_METHOD(File, open, (const char*), (override));
};

/**
 * @brief Tests the check_img function when the file size is greater than 100 bytes.
 */
TEST(CheckImgTest, ReturnsTrueWhenFileSizeIsGreaterThan100) {
    MockFS mock_fs;
    EXPECT_CALL(mock_fs, open(FILE_PHOTO))
        .WillOnce(Return(FileMock{120})); //Define expected behaviour

    EXPECT_TRUE(check_img(mock_fs));
}

/**
 * @brief Tests the check_img function when the file size is less than or equal to 100 bytes.
 */
TEST(CheckImgTest, ReturnsFalseWhenFileSizeIsLessThanOrEqualTo100) {
    MockFS mock_fs;
    EXPECT_CALL(mock_fs, open(FILE_PHOTO))
        .WillOnce(Return(FileMock{50})); //Define expected behaviour

    EXPECT_FALSE(check_img(mock_fs));
}
