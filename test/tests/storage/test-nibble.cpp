/// @file test-nibble.cpp
/// @author Glenn Galvizo
///
/// Source file for all Nibble class unit tests and the test runner. This assumes that the bright star table
/// generator in Chomp works AND is used for CONFIG.ini time = 01-2018.

#define ENABLE_TESTING_ACCESS

#include <fstream>
#include "gtest/gtest.h"

#include "storage/chomp.h"

/// Check that Nibble database is present after creating a Nibble instance.
TEST(NibbleFile, Existence) {
    std::ifstream nibble(Nibble().DATABASE_LOCATION);
    ASSERT_TRUE(nibble.good());
}

/// Check that the in-memory connection of Nibble works identically to the disk connection.
TEST(NibbleConnection, InMemoryInstance) {
    Chomp ch;
    
    // Check when we create an instance with a focus, and without.
    Nibble nb(ch.bright_table, "label"), nb3(ch.bright_table);
    Nibble::tuples_d a = nb.search_table("i, j, k", "label = 88", 3);
    Nibble::tuples_d b = nb.search_table("i, j, k", "label = 88", 3);
    nb.select_table(ch.bright_table);
    bool assertion = false;
    
    // Ensure that the focus instance is the same as the non-focus instance.
    EXPECT_EQ(nb.search_table("i, j, k", "label = 1", 1).size(), 0);
    EXPECT_EQ(nb.search_table("i, j, k", "label = 88", 1).size(), 1);
    EXPECT_FLOAT_EQ(a[0][0], 0.95763727906078);
    EXPECT_FLOAT_EQ(a[0][1], 0.264036130176979);
    EXPECT_FLOAT_EQ(a[0][2], 0.114959835221834);
    EXPECT_FLOAT_EQ(a[0][0], b[0][0]);
    EXPECT_FLOAT_EQ(a[0][1], b[0][1]);
    EXPECT_FLOAT_EQ(a[0][2], b[0][2]);
    
    // Ensure that we aren't creating random connections below.
    try {
        Nibble nb2("SomeTableThatDoesntExist", "asd");
    }
    catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
        assertion = true;
    }
    EXPECT_TRUE(assertion);
    
}

/// Check that the bright stars table can be queried using the general search method with a constraint.
TEST(NibbleSearch, SearchConstrained) {
    Chomp ch;
    Nibble nb;
    nb.select_table(ch.bright_table);
    
    // Check the method with a constraint, given an expected amount and not.
    Nibble::tuples_d a = nb.search_table("i, j, k", "label = 88", 3);
    Nibble::tuples_d b = nb.search_table("i, j, k", "label = 88 or label = 107", 6, 2);
    
    EXPECT_FLOAT_EQ(a[0][0], 0.95763727906078);
    EXPECT_FLOAT_EQ(a[0][1], 0.264036130176979);
    EXPECT_FLOAT_EQ(a[0][2], 0.114959835221834);
    EXPECT_FLOAT_EQ(b[0][0], 0.95763727906078);
    EXPECT_FLOAT_EQ(b[0][1], 0.264036130176979);
    EXPECT_FLOAT_EQ(b[0][2], 0.114959835221834);
    EXPECT_FLOAT_EQ(b[1][0], -0.0678115592262295);
    EXPECT_FLOAT_EQ(b[1][1], -0.0235197303151528);
    EXPECT_FLOAT_EQ(b[1][2], 0.997420881434317);
    
    Nibble::tuples_d c = nb.search_table("i, j, k", "label > 88", 3);
    Nibble::tuples_d d = nb.search_table("i, j, k", "label > 88", 6, 10);
    Nibble::tuples_d e = nb.search_table("i, j, k", "label > 88", 6, Nibble::NO_LIMIT);
    EXPECT_EQ(c.size(), 4558);
    EXPECT_EQ(d.size(), 10);
    EXPECT_EQ(e.size(), 4558);
}

/// Check that the bright stars table can be queried using the general search method without a constraint.
TEST(NibbleSearch, SearchNotConstrained) {
    Chomp ch;
    Nibble nb;
    nb.select_table(ch.bright_table);
    Nibble::tuples_d a = nb.search_table("i", Chomp::BRIGHT_TABLE_LENGTH, Nibble::NO_LIMIT);
    EXPECT_EQ(a.size(), Chomp::BRIGHT_TABLE_LENGTH);
    
    Nibble::tuples_d c = nb.search_table("i, j, k", 3);
    Nibble::tuples_d d = nb.search_table("i, j, k", 6, 10);
    Nibble::tuples_d e = nb.search_table("i, j, k", 6, Nibble::NO_LIMIT);
    EXPECT_EQ(c.size(), 4559);
    EXPECT_EQ(d.size(), 10);
    EXPECT_EQ(e.size(), 4559);
}

/// Check that the single search method works as intended with a constraint and without.
TEST(NibbleSearch, Single) {
    Chomp ch;
    Nibble nb;
    nb.select_table(ch.bright_table);
    EXPECT_FLOAT_EQ(nb.search_single("i", "label = 88"), 0.95763727906078);
    EXPECT_FLOAT_EQ(nb.search_single("j", "label = 88"), 0.264036130176979);
    EXPECT_FLOAT_EQ(nb.search_single("k", "label = 88"), 0.114959835221834);

    EXPECT_FLOAT_EQ(nb.search_single("i"), 0.95763727906078);
}

/// Check that the table creation method works as intended (the table persists after closing connection.
TEST(NibbleTable, Creation) {
    // Clean up our mess (if it exists).
    Nibble nb2;
    SQLite::Transaction transaction2(*nb2.conn);
    SQLite::Statement(*nb2.conn, "DROP TABLE IF EXISTS MYTABLE").exec();
    transaction2.commit();
    
    if (true) {
        // Just creating scope here. Nibble gets destroyed when this is done.
        Nibble nb;
        EXPECT_EQ (0, nb.create_table("MYTABLE", "a int"));
    }
    
    // Attempting to create a table again should return an error.
    Nibble nb;
    EXPECT_EQ(Nibble::TABLE_NOT_CREATED, nb.create_table("MYTABLE", "a int"));
    
    // Clean up our mess.
    SQLite::Transaction transaction(*nb.conn);
    SQLite::Statement(*nb.conn, "DROP TABLE MYTABLE").exec();
    transaction.commit();
}

/// Check that retrieved attributes and schema are correct for a given table.
TEST(NibbleTable, AttributeRetrieval) {
    Nibble nb;
    EXPECT_EQ (0, nb.create_table("MYTABLE", "a int, b int"));
    
    std::string schema, fields;
    nb.select_table("MYTABLE");
    nb.find_attributes(schema, fields);
    
    EXPECT_EQ("a, b", fields);
    EXPECT_EQ("a int, b int", schema);
}

/// Check that the bright stars table has an index created.
TEST(NibbleTable, PolishIndex) {
    Chomp ch;
    Nibble nb;
    nb.select_table(ch.bright_table);
    nb.polish_table("alpha");
    bool assertion = false;
    
    try {
        SQLite::Statement(*nb.conn, "CREATE INDEX HIP_BRIGHT_alpha on " + ch.bright_table + "(alpha)").exec();
    }
    catch (std::exception &e) {
        // Exception thrown while creating index means that the index exists.
        std::cout << "Exception: " << e.what() << std::endl;
        assertion = true;
    }
    
    // Delete new table and index. Rerun original bright table generation.
    SQLite::Transaction transaction(*nb.conn);
    SQLite::Statement(*nb.conn, "DROP INDEX HIP_BRIGHT_alpha").exec();
    SQLite::Statement(*nb.conn, "DROP TABLE " + ch.bright_table).exec();
    transaction.commit();
    Chomp();
    EXPECT_TRUE(assertion);
}

/// Check that the bright stars table has an index created. 'sort' is called with 'polish', this is tested too.
TEST(NibbleTable, PolishSort) {
    Chomp ch;
    Nibble nb;
    nb.select_table(ch.bright_table);
    nb.polish_table("delta");
    double a = nb.search_single("label", "rowid = 1");
    EXPECT_EQ(a, 104382);
    
    // Delete new table and index. Rerun original BSC5 table generation.
    try {
        SQLite::Transaction transaction(*nb.conn);
        SQLite::Statement(*nb.conn, "DROP INDEX HIP_BRIGHT_delta").exec();
        SQLite::Statement(*nb.conn, "DROP TABLE " + ch.bright_table).exec();
        transaction.commit();
        Chomp();
    }
    catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}

/// Test if the insertion of an entry was made.
TEST(NibbleTable, Insertion) {
    Nibble nb;
    Chomp ch;
    std::vector<double> a{0, 0, 0, 0, 0, 0, 10000000}, b;
    SQLite::Transaction transaction(*nb.conn);
    
    nb.select_table(ch.bright_table);
    nb.insert_into_table("alpha, delta, i, j, k, m, label", a);
    SQLite::Statement query(*nb.conn, "SELECT alpha, delta FROM " + ch.bright_table + " WHERE label = 10000000");
    while (query.executeStep()) {
        b.push_back(query.getColumn(0).getDouble());
        b.push_back(query.getColumn(1).getDouble());
    }
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 0);
    
    try {
        SQLite::Statement(*nb.conn, "DELETE FROM " + ch.bright_table + " WHERE label = 10000000").exec();
        transaction.commit();
    }
    catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}

/// Runs all tests defined in this file.
///
/// @param argc Argument count. Used in Google Test initialization.
/// @param argv Argument vector. Used in Google Test initialization.
/// @return The result of running all tests.
int main (int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}