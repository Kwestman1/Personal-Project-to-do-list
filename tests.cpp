#include "Master.h"
#include <cassert>

void test_search_by_date(string& input);

// Helper function to create a file
File make_file(std::string name, uint64_t timestamp, bool favorite = false) {
    File f;
    f.file_name = name;
    f.comp_timestamp = timestamp;
    f.favorite = favorite;
    return f;
}

// helper to simulate input
void add_file_with_mock_input(MasterFiles& mf, File& file) {
    std::istringstream simulated_input("q\n");
    std::cin.rdbuf(simulated_input.rdbuf()); // Redirect input
    mf.add_file(file, true); // Preserve timestamp
    std::cin.rdbuf(std::cin.rdbuf()); // Restore cin after call
}

// Test case ensuring the timestamp is not modified
void test_add_file_keeps_timestamp() {
    MasterFiles mf;
    File f1 = make_file("test1", 1700000001); // Predefined timestamp
    time_t initial_time = f1.comp_timestamp;

    add_file_with_mock_input(mf, f1);

    assert(mf.get_files()[0].comp_timestamp == initial_time); // Ensure timestamp is unchanged
    std::cout << "test_add_file_keeps_timestamp passed.\n";
}

void test_empty_masterfiles() {
    MasterFiles mf;
    assert(mf.get_files().size() == 0);
    assert(mf.get_num_dupes("test") == -1);
    std::cout << "test_empty_masterfiles passed.\n";
}

void test_duplicate_filenames() {
    MasterFiles mf;
    File f1 = make_file("dupe", 1700000000);
    File f2 = make_file("dupe", 1700000001);
    add_file_with_mock_input(mf, f1);
    add_file_with_mock_input(mf, f2);
    cout << mf.get_num_dupes("dupe") << "\n";
    assert(mf.get_num_dupes("dupe") == 2);
    std::cout << "test_duplicate_filenames passed.\n";
}

void test_invalid_index_access() {
    MasterFiles mf;
    try {
        mf.print_filenames(0, 1); // No files exist
        std::cout << "test_invalid_index_access passed.\n";
    } catch (...) {
        std::cerr << "test_invalid_index_access failed.\n";
    }
}

void test_timestamp_creation() {
    MasterFiles mf;
    assert(mf.create_timestamp(2023, 12, 31) > mf.create_timestamp(2000, 1, 1));  // Chronological order
    assert(mf.create_timestamp(2000, 2, 29) != 0); // Leap year check
    assert(mf.create_timestamp(1900, 2, 29) == 0); // Invalid leap year (1900 is not a leap year)
    std::cout << "test_timestamp_creation passed.\n";
}

void test_sorting_behavior() {
    MasterFiles mf;
    File f1 = make_file("a", 1700000001);
    File f2 = make_file("b", 1700000002, true); // Favorite
    File f3 = make_file("c", 1700000000);
    add_file_with_mock_input(mf, f1);
    add_file_with_mock_input(mf, f2);
    add_file_with_mock_input(mf, f3);

    // Should be sorted: f2 (fav), f1 (timestamp), f3 (lowest timestamp)
    assert(mf.get_name(0) == "b");
    assert(mf.get_name(1) == "a");
    assert(mf.get_name(2) == "c");
    std::cout << "test_sorting_behavior passed.\n";
}

void test_sorting_same_timestamp() {
    MasterFiles mf;
    File f1 = make_file("delta", 1700000001);
    File f2 = make_file("alpha", 1700000001);
    File f3 = make_file("charlie", 1700000001);
    
    add_file_with_mock_input(mf, f1);
    add_file_with_mock_input(mf, f2);
    add_file_with_mock_input(mf, f3);

    // Alphabetical order should apply since timestamps are identical
    assert(mf.get_name(0) == "alpha");
    assert(mf.get_name(1) == "charlie");
    assert(mf.get_name(2) == "delta");

    std::cout << "test_sorting_same_timestamp passed.\n";
}

void test_sorting_multiple_favorites() {
    MasterFiles mf;
    File f1 = make_file("x", 1700000001, true);
    File f2 = make_file("y", 1700000003, true);
    File f3 = make_file("z", 1700000002, false);
    File f4 = make_file("a", 1700000004, true);  // Favorite, newest timestamp

    add_file_with_mock_input(mf, f1);
    add_file_with_mock_input(mf, f2);
    add_file_with_mock_input(mf, f3);
    add_file_with_mock_input(mf, f4);

    // Expected order: f4 (fav, newest), f2 (fav, second newest), f1 (fav, third newest), f3 (not fav, last)
    assert(mf.get_name(0) == "a");
    assert(mf.get_name(1) == "y");
    assert(mf.get_name(2) == "x");
    assert(mf.get_name(3) == "z");

    std::cout << "test_sorting_multiple_favorites passed.\n";
}

void test_search_by_date(string& input) {
    MasterFiles mf;

    // Add sample files with different timestamps
    File f1 = make_file("file1", mf.create_timestamp(2023, 5, 10));
    File f2 = make_file("file2", mf.create_timestamp(2023, 5, 10));
    File f3 = make_file("file3", mf.create_timestamp(2023, 5, 11));
    File f4 = make_file("file4", mf.create_timestamp(2023, 6, 1));
    File f5 = make_file("file5", mf.create_timestamp(2024, 1, 1));
    File f6 = make_file("file6", mf.create_timestamp(2024, 2, 29)); // Leap day
    File f7 = make_file("file7", mf.create_timestamp(2024, 12, 1));
    File f8 = make_file("file8", mf.create_timestamp(2024, 12, 31));

    add_file_with_mock_input(mf, f1);
    add_file_with_mock_input(mf, f2);
    add_file_with_mock_input(mf, f3);
    add_file_with_mock_input(mf, f4);
    add_file_with_mock_input(mf, f5);
    add_file_with_mock_input(mf, f6);
    add_file_with_mock_input(mf, f7);
    add_file_with_mock_input(mf, f8);

    std::istringstream simulated_input(input);
    std::streambuf *orig_cin = std::cin.rdbuf(simulated_input.rdbuf()); // Redirect std::cin

    // Call the function
    mf.search_by_date();

    // Restore original std::cin
    std::cin.rdbuf(orig_cin);
}

void test_update_indices() {
    MasterFiles mf;
    File f1 = make_file("test1", 1700000001);
    File f2 = make_file("test2", 1700000002);
    add_file_with_mock_input(mf, f1);
    add_file_with_mock_input(mf, f2);
    mf.update_indices(0);

    // Ensure indices are updated properly
    assert(mf.get_num_dupes("test1") == 1);
    assert(mf.get_num_dupes("test2") == 1);
    std::cout << "test_update_indices passed.\n";
}

void test_search_wildcards() {
    MasterFiles mf;
    File f1 = make_file("hello", 1700000001);
    File f2 = make_file("world", 1700000002);
    File f3 = make_file("abc", 1700000003);
    File f4 = make_file("xyz", 1700000004);
    File f5 = make_file("hello", 1700000005);
    add_file_with_mock_input(mf, f1);
    add_file_with_mock_input(mf, f2);
    add_file_with_mock_input(mf, f3);
    add_file_with_mock_input(mf, f4);
    add_file_with_mock_input(mf, f5);
    std::unordered_set<uint32_t> results;

    mf.search_with_wildcards("*xyh*", results, "F:");
    std::cout << "Results for '*xyh*': ";
    for (uint32_t idx : results) {
        std::cout << idx << " ";
    }
    std::cout << "\n";
    assert(results.size() == 0); // matches none

    mf.search_with_wildcards("*ab*", results, "F:");
    std::cout << "Results for '*ab*': ";
    for (uint32_t idx : results) {
        std::cout << idx << " ";
    }
    std::cout << "\n";
    assert(results.size() == 1); // abc

    results.clear();
    mf.search_with_wildcards("*lo*", results, "F:");
    std::cout << "Results for '*lo*': ";
    for (uint32_t idx : results) {
        std::cout << idx << " ";
    }
    std::cout << "\n";
    assert(results.size() == 2);  // Should match "hello.txt (2)"

    results.clear();
    mf.search_with_wildcards("*o*", results, "F:");
    std::cout << "Results for '*o*': ";
    for (uint32_t idx : results) {
        std::cout << idx << " ";
    }
    std::cout << "\n";
    assert(results.size() == 3);  // Should match both hello (2) and world

    results.clear(); 
    // if user searches for a txt
    mf.search_with_wildcards("*.txt", results, "F:");
    cout << "result size: " << results.size() << "\n";
    assert(results.size() == 5);  // Should match all

    results.clear(); 
    // if user searches for a txt
    mf.search_with_wildcards("*o.txt", results, "F:");
    cout << "result size: " << results.size() << "\n";
    assert(results.size() == 2);  // Should match hello (2)

    std::cout << "test_search_wildcards passed.\n";
}

int main() {

    // !! to test comment out file.set_time(); in Master.cpp process_commands !!
    test_add_file_keeps_timestamp();
    test_empty_masterfiles();
    test_duplicate_filenames();
    test_invalid_index_access();
    test_timestamp_creation();
    test_sorting_behavior();
    test_sorting_same_timestamp();
    test_sorting_multiple_favorites();
    test_update_indices();
    test_search_wildcards();

    // Provide actual dates for test_search_by_date
    std::string date;

    // Exact date
    date = "2023\n5\n10\n";
    test_search_by_date(date);
    // Month only
    date = "2023\n5\n\n";
    test_search_by_date(date);
    // Year only
    date = "2023\n\n\n";
    test_search_by_date(date);
    // Out of range
    date = "2025\n5\n10\n";
    test_search_by_date(date);
    // Leap day
    date = "2024\n2\n29\n";
    test_search_by_date(date);
    // Month and year only
    date = "2024\n12\n\n";
    test_search_by_date(date);

    /* Invalid input loop ~ passes
    date = "abcd\n5\n10\n";
    test_search_by_date(date);
    date = "2027\n5\n10\n";
    test_search_by_date(date);
    // Non Leap day
    date = "2023\n2\n29\n";
    test_search_by_date(date);
    */

    std::cout << "All Master File tests passed\n";

    return 0;
}
