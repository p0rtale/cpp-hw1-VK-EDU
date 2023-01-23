#include <gtest/gtest.h>

#include <ustring.hpp>

TEST(TestUString, SizeAndLength) {
    UString ustr1 = "aaaaaaaa";
    ASSERT_EQ(ustr1.size(), 8);
    ASSERT_EQ(ustr1.length(), 8);

    UString ustr2 = std::string("私は誰ですか");
    ASSERT_EQ(ustr2.size(), 18);
    ASSERT_EQ(ustr2.length(), 6);

    UString ustr3 = "没关系";
    ASSERT_EQ(ustr3.size(), 9);
    ASSERT_EQ(ustr3.length(), 3);
}

TEST(TestUString, Empty) {
    UString ustr = "";
    ASSERT_TRUE(ustr.empty());
}

TEST(TestUString, Clear) {
    UString ustr = std::string("私は誰ですか");
    ustr.clear();
    ASSERT_EQ(ustr, "");
    ASSERT_TRUE(ustr.empty());
}

TEST(TestUString, Concat) {
    UString ustr1 = "aaaaaaaa";
    UString ustr2 = std::string("私は誰ですか");
    UString concat = ustr1 + ustr2;
    ASSERT_EQ(concat.size(), 26);
    ASSERT_EQ(concat.length(), 14);

    concat.clear();
    ASSERT_EQ(concat, "");
    ASSERT_TRUE(concat.empty());
}

TEST(TestUString, Index) {
    UString ustr = "aЮはВ";
    ASSERT_EQ(ustr[0], "a");
    ASSERT_EQ(ustr.at(0), "a");
    ASSERT_EQ(ustr[1], "Ю");
    ASSERT_EQ(ustr.at(1), "Ю");
    ASSERT_EQ(ustr[2], "は");
    ASSERT_EQ(ustr.at(2), "は");
    ASSERT_EQ(ustr[3], "В");
    ASSERT_EQ(ustr.at(3), "В");

    try {
        ustr.at(4);
        FAIL() << "Expected std::out_of_range";
    } catch(std::out_of_range const& err) {
        EXPECT_EQ(err.what(), std::string("index value is greater than the length of the string"));
    } catch(...) {
        FAIL() << "Expected std::out_of_range";
    }
}

TEST(TestUString, CodePointOneByte) {
    UString ustr;
    ustr.push_back(100);
    ustr.push_back(117);
    ustr.push_back(111);
    ustr.push_back(99);
    ASSERT_EQ(ustr, "duoc");
}

TEST(TestUString, CodePointTwoBytes) {
    UString ustr;
    ustr.push_back(1094);
    ustr.push_back(1093);
    ustr.push_back(1097);
    ustr.push_back(1073);
    ASSERT_EQ(ustr, "цхщб");
}

TEST(TestUString, CodePointThreeBytes) {
    UString ustr;
    ustr.push_back(31169);
    ustr.push_back(12399);
    ustr.push_back(35504);
    ustr.push_back(12391);
    ASSERT_EQ(ustr, "私は誰で");
}

TEST(TestUString, IsWellOneByte) {
    std::array<unsigned char, 2> bytes1 = { 0x77, 0x00 };
    UString ustr = reinterpret_cast<char*>(bytes1.data());
    ASSERT_EQ(ustr, "w");

    std::array<unsigned char, 2> bytes2 = { 0xFF, 0x00 };
    try {
        UString ustr = reinterpret_cast<char*>(bytes2.data());
        FAIL() << "Expected std::invalid_argument";
    } catch(std::invalid_argument const& err) {
        EXPECT_EQ(err.what(), std::string("invalid UTF-8 string"));
    } catch(...) {
        FAIL() << "Expected std::invalid_argument";
    }
}

TEST(TestUString, IsWellTwoBytes) {
    std::array<unsigned char, 3> bytes1 = { 0xd0, 0x94, 0x00 };
    UString ustr = reinterpret_cast<char*>(bytes1.data());
    ASSERT_EQ(ustr, "Д");

    std::array<unsigned char, 3> bytes2 = { 0b11100000, 0x94, 0x00 };
    std::array<unsigned char, 3> bytes3 = { 0xd0, 0xFF, 0x00 };
    auto cases = std::array<std::array<unsigned char, 3>, 2>{ bytes2, bytes3 };
    for (int i = 0; i < 2; ++i) {
        try {
            UString ustr = reinterpret_cast<char*>(cases[i].data());
            FAIL() << "Expected std::invalid_argument";
        } catch(std::invalid_argument const& err) {
            EXPECT_EQ(err.what(), std::string("invalid UTF-8 string"));
        } catch(...) {
            FAIL() << "Expected std::invalid_argument";
        }
    }
}

TEST(TestUString, IsWellThreeBytes) {
    std::array<unsigned char, 4> bytes1 = { 0xe7, 0xa7, 0x81, 0x00 };
    UString ustr = reinterpret_cast<char*>(bytes1.data());
    ASSERT_EQ(ustr, "私");

    std::array<unsigned char, 4> bytes2 = { 0b11110000, 0xa7, 0x81, 0x00 };
    std::array<unsigned char, 4> bytes3 = { 0xe7, 0xFF, 0x81, 0x00 };
    std::array<unsigned char, 4> bytes4 = { 0xe7, 0xa7, 0xFF, 0x00 };
    auto cases = std::array<std::array<unsigned char, 4>, 3>{ bytes2, bytes3, bytes4 };
    for (int i = 0; i < 3; ++i) {
        try {
            UString ustr = reinterpret_cast<char*>(cases[i].data());
            FAIL() << "Expected std::invalid_argument";
        } catch(std::invalid_argument const& err) {
            EXPECT_EQ(err.what(), std::string("invalid UTF-8 string"));
        } catch(...) {
            FAIL() << "Expected std::invalid_argument";
        }
    }
}

TEST(TestUString, PushPop) {
    UString ustr = "ආර关භය";
    ustr.push_back(12452);          // イ
    ASSERT_EQ(ustr, "ආර关භයイ");
    ustr.push_back(31169);          // 私
    ASSERT_EQ(ustr, "ආර关භයイ私");
    ustr.push_back(71);             // G
    ASSERT_EQ(ustr, "ආර关භයイ私G");

    ustr.pop_back();
    ASSERT_EQ(ustr, "ආර关භයイ私");
    ustr.pop_back();
    ASSERT_EQ(ustr, "ආර关භයイ");
    ustr.pop_back();
    ASSERT_EQ(ustr, "ආර关භය");

    for (size_t i = 0; i < 5; ++i) {
        ustr.pop_back();
    }
    
    ASSERT_EQ(ustr, "");
}

TEST(TestUString, InvalidPushBack) {
    try {
        UString ustr = "ආරම්භය";
        ustr.push_back(2000000);
        FAIL() << "Expected std::invalid_argument";
    } catch(std::invalid_argument const& err) {
        EXPECT_EQ(err.what(), std::string("invalid UTF-8 code"));
    } catch(...) {
        FAIL() << "Expected std::invalid_argument";
    }
}

TEST(TestUString, InvalidString) {
    try {
        std::string str = "アラララ";
        str[1] = 0;
        UString ustr = str;
        FAIL() << "Expected std::invalid_argument";
    } catch(std::invalid_argument const& err) {
        EXPECT_EQ(err.what(), std::string("invalid UTF-8 string"));
    } catch(...) {
        FAIL() << "Expected std::invalid_argument";
    }
}

TEST(TestUString, IteratorForward) {
    std::array<std::string, 8> symbs = {"パ", "K", "ス", "ю", "ミ", "E", "イ", "щ"};
    std::string str = "";
    for (const auto& symb: symbs) {
        str += symb;
    }
    UString ustr = str;

    size_t i = 0;
    auto it = ustr.begin();
    while (it != ustr.end()) {
        ASSERT_EQ(*it, symbs[i]);
        ++i;
        ++it;
    }
}

TEST(TestUString, IteratorReverse) {
    std::array<std::string, 8> symbs = {"パ", "K", "ス", "ю", "ミ", "E", "イ", "щ"};
    std::string str = "";
    for (const auto& symb: symbs) {
        str += symb;
    }
    UString ustr = str;

    size_t i = 7;
    auto it = ustr.rbegin();
    while (it != ustr.rend()) {
        ASSERT_EQ(*it, symbs[i]);
        --i;
        ++it;
    }
}

TEST(TestUString, Compare) {
    UString ustr1 = "スイ誰";
    UString ustr2 = "ススZZ誰";
    UString ustr3 = "ススZZ誰";
    UString ustr4 = "スb誰";
    UString ustr5 = "";

    ASSERT_FALSE(ustr1 == ustr2);
    ASSERT_TRUE(ustr1 != ustr2);
    ASSERT_TRUE(ustr1 < ustr2);
    ASSERT_TRUE(ustr1 <= ustr2);
    ASSERT_TRUE(ustr2 > ustr1);
    ASSERT_TRUE(ustr2 >= ustr1);

    ASSERT_TRUE(ustr2 == ustr3);
    ASSERT_TRUE(ustr2 <= ustr3);
    ASSERT_TRUE(ustr3 <= ustr2);
    ASSERT_FALSE(ustr2 != ustr3);
    ASSERT_FALSE(ustr2 < ustr3);
    ASSERT_FALSE(ustr3 < ustr2);
    ASSERT_FALSE(ustr2 > ustr3);
    ASSERT_FALSE(ustr3 > ustr2);

    ASSERT_TRUE(ustr3 > ustr4);
    ASSERT_TRUE(ustr4 < ustr3);
    ASSERT_FALSE(ustr3 == ustr4);
    ASSERT_TRUE(ustr3 != ustr4);

    ASSERT_TRUE(ustr4 > ustr5);
    ASSERT_TRUE(ustr5 < ustr4);
    ASSERT_FALSE(ustr4 == ustr5);
    ASSERT_TRUE(ustr4 != ustr5);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
