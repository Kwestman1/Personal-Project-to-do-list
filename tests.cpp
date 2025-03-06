#include "Master.h"
#include "File.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <unordered_set>

void test_search_by_date(string& input);

// Helper function to create a file
File make_file(std::string name, uint64_t timestamp, bool favorite = false) {
    File f;
    f.file_name = name;
    f.comp_timestamp = timestamp;
    f.favorite = favorite;
    return f;
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
    mf.add_file(f1);
    mf.add_file(f2);

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
    mf.add_file(f1);
    mf.add_file(f2);
    mf.add_file(f3);

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
    
    mf.add_file(f1);
    mf.add_file(f2);
    mf.add_file(f3);

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

    mf.add_file(f1);
    mf.add_file(f2);
    mf.add_file(f3);
    mf.add_file(f4);

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

    mf.add_file(f1);
    mf.add_file(f2);
    mf.add_file(f3);
    mf.add_file(f4);

    std::istringstream simulated_input(input);
    std::streambuf *orig_cin = std::cin.rdbuf(simulated_input.rdbuf()); // Redirect std::cin

    // Call the function
    mf.search_by_date();

    // Restore original std::cin
    std::cin.rdbuf(orig_cin);
}

void test_search_boundary_conditions() {
    MasterFiles mf;
    File f1 = make_file("file1", mf.create_timestamp(2023, 1, 1));  // Earliest
    File f2 = make_file("file2", mf.create_timestamp(2023, 12, 31)); // Latest
    File f3 = make_file("file3", mf.create_timestamp(2024, 1, 1)); // Out of range

    mf.add_file(f1);
    mf.add_file(f2);
    mf.add_file(f3);

    uint32_t lower_bound = mf.create_timestamp(2023, 1, 1);
    uint32_t upper_bound = mf.create_timestamp(2024, 1, 1) - 1;

    auto lower_it = std::lower_bound(mf.get_files().begin(), mf.get_files().end(), lower_bound, LowerFunctor());
    auto upper_it = std::upper_bound(mf.get_files().begin(), mf.get_files().end(), upper_bound, UpperFunctor());

    assert(std::distance(lower_it, upper_it) == 2);
    std::cout << "test_search_boundary_conditions passed.\n";
}

void test_update_indices() {
    MasterFiles mf;
    File f1 = make_file("test1", 1700000001);
    File f2 = make_file("test2", 1700000002);
    mf.add_file(f1);
    mf.add_file(f2);
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
    mf.add_file(f1);
    mf.add_file(f2);
    std::unordered_set<uint32_t> results;

    mf.search_with_wildcards("*lo*", results, "F:");
    std::cout << "Results for '*lo*': ";
    for (uint32_t idx : results) {
        std::cout << idx << " ";
    }
    std::cout << "\n";
    assert(results.size() == 1);  // Should match "hello.txt"

    results.clear();
    mf.search_with_wildcards("*o*", results, "F:");
    std::cout << "Results for '*o*': ";
    for (uint32_t idx : results) {
        std::cout << idx << " ";
    }
    std::cout << "\n";
    assert(results.size() == 2);  // Should match both

    results.clear();
    mf.search_with_wildcards("*.txt", results, "F:");
    cout << "result size: " << results.size() << "\n";
    assert(results.size() == 2);  // Should match both

    std::cout << "test_search_wildcards passed.\n";
}

int main() {
    using Clock = std::chrono::system_clock;
    std::time_t now = Clock::to_time_t(Clock::now());

    std::cout << "System Time (Local): " << std::ctime(&now);
    std::tm *utc_tm = std::gmtime(&now);
    std::cout << "UTC Time: " << std::asctime(utc_tm);
    test_empty_masterfiles();
    test_duplicate_filenames();
    test_invalid_index_access();
    test_timestamp_creation();
    test_sorting_behavior();
    test_sorting_same_timestamp();
    test_sorting_multiple_favorites();
    // exact date
    string date = "2023\n5\n10\n";
    test_search_by_date(date);
    // month only
    date = "2023\n5\n\n";
    test_search_by_date(date);
    // year only
    date = "2023\n\n\n";
    test_search_by_date(date);
    // Invalid
    date = "abcd\n5\n10\n";
    test_search_by_date(date);
    test_update_indices();
    test_search_wildcards();
    std::cout << "All Master File tests passed\n";
    return 0;
}
