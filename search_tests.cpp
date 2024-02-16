#include <gtest/gtest.h>

#include "search.h"
#include "tests/build_index.h"
#include "tests/clean_token.h"
#include "tests/find_query_matches.h"
#include "tests/gather_tokens.h"

// Clean Tokens
TEST(CleanToken, BothPunctuation) {
    // Testing for unwanted punctiation at both ends
    string text = ".&?hello$#@";
    string expected = "hello";

    string result = cleanToken(text);

    string testFeedback = "cleanToken(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, result) << testFeedback;
}


TEST(CleanToken, MiddlePunctuation) {
    // Testing that punctuation in the words is not removed
    string text = "hEl!LO";
    string expected = "hel!lo";

    string result = cleanToken(text);

    string testFeedback = "cleanToken(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, result) << testFeedback;
}


TEST(CleanToken, StartingMiddlePunctuation) {
    // Testing that punctuation in the words is not removed while there is trailing punctuation
    string text = "h.e.l.l.o...";
    string expected = "h.e.l.l.o";

    string result = cleanToken(text);

    string testFeedback = "cleanToken(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, result) << testFeedback;
}


TEST(CleanToken, EndingMiddlePunctuation) {
    // Testing that punctuation in the words is not removed while there is leading punctuation
    string text = "...h.e.l.l.o";
    string expected = "h.e.l.l.o";

    string result = cleanToken(text);

    string testFeedback = "cleanToken(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, result) << testFeedback;
}


TEST(CleanToken, EndingNumbers) {
    // Testing for words that end with numbers and have unwanted punctuation
    string text = "...hello123...";
    string expected = "hello123";

    string result = cleanToken(text);

    string testFeedback = "cleanToken(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, result) << testFeedback;
}


// Gather Tokens
TEST(GatherTokens, LeadingSpaces) {
    // Testing for leading spaces
    string text = "   to be ora not to be";
    set<string> expected = {"to", "be", "ora", "not"};
    set<string> studentResult = gatherTokens(text);

    string testFeedback = "gatherTokens(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, studentResult) << testFeedback;
}


TEST(GatherTokens, TrailingSpaces) {
    // Testing for trailing spaces
    string text = "to be ora not to be   ";
    set<string> expected = {"to", "be", "ora", "not"};
    set<string> studentResult = gatherTokens(text);

    string testFeedback = "gatherTokens(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, studentResult) << testFeedback;
}


TEST(GatherTokens, SpacesBetweenWords) {
    // Testing for extra spaces in between words
    string text = "to   be   ora   not   to   be";
    set<string> expected = {"to", "be", "ora", "not"};
    set<string> studentResult = gatherTokens(text);

    string testFeedback = "gatherTokens(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, studentResult) << testFeedback;
}


// Build Index
TEST(BuildIndex, TinyTxt) {
    // Testing buildIndex with inverted index from tiny.txt
    string filename = "tiny.txt";
    map<string, set<string>> expected = {
        {"eggs", {"www.shoppinglist.com"}},
        {"milk", {"www.shoppinglist.com"}},
        {"fish", {"www.shoppinglist.com", "www.dr.seuss.net"}},
        {"bread", {"www.shoppinglist.com"}},
        {"cheese", {"www.shoppinglist.com"}},
        {"red", {"www.rainbow.org", "www.dr.seuss.net"}},
        {"green", {"www.rainbow.org"}},
        {"orange", {"www.rainbow.org"}},
        {"yellow", {"www.rainbow.org"}},
        {"blue", {"www.rainbow.org", "www.dr.seuss.net"}},
        {"indigo", {"www.rainbow.org"}},
        {"violet", {"www.rainbow.org"}},
        {"one", {"www.dr.seuss.net"}},
        {"two", {"www.dr.seuss.net"}},
        {"i'm", {"www.bigbadwolf.com"}},
        {"not", {"www.bigbadwolf.com"}},
        {"trying", {"www.bigbadwolf.com"}},
        {"to", {"www.bigbadwolf.com"}},
        {"eat", {"www.bigbadwolf.com"}},
        {"you", {"www.bigbadwolf.com"}},
    };
    map<string, set<string>> studentIndex;
    int studentNumProcesed = buildIndex(filename, studentIndex);

    string indexTestFeedback = "buildIndex(\"" + filename + "\", ...) index incorrect\n";
    EXPECT_EQ(expected, studentIndex) << indexTestFeedback;

    string retTestFeedback = "buildIndex(\"" + filename + "\", ...) return value incorrect\n";
    EXPECT_EQ(4, studentNumProcesed) << retTestFeedback;
}


TEST(BuildIndex, InvalidFilename) {
    // Testing for invalid file
    string filename = "tinyer.txt";
    map<string, set<string>> expected = {
    };
    map<string, set<string>> studentIndex;
    int studentNumProcesed = buildIndex(filename, studentIndex);

    string indexTestFeedback = "buildIndex(\"" + filename + "\", ...) index incorrect\n";
    EXPECT_EQ(expected, studentIndex) << indexTestFeedback;

    string retTestFeedback = "buildIndex(\"" + filename + "\", ...) return value incorrect\n";
    EXPECT_EQ(0, studentNumProcesed) << retTestFeedback;
}


// Find Query Match
TEST(FindQueryMatches, FirstMissing) {
    // Tests for first term missing from index
    set<string> expected;

    // SingleTerm
    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "missing"));

    // Union
    expected = {"uic.edu"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "missing to"));
    
    // Intersection
    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "missing +there"));

    // Difference
    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "missing -laws"));

    // LongCombinedQueries
    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "missing laws aviation -to +hello"));
}


TEST(FindQueryMatches, LaterMissing) {
    // Tests for a middle term missing from index
    set<string> expected;
    
    // Intersection
    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "hello +missing"));

    // Difference
    expected = {"example.com", "uic.edu", "random.org"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "all -missing"));

    // LongCombinedQueries
    expected = {"example.com"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "there laws missing -to +hello"));
}