#ifndef MASTER_H
#define MASTER_H

#include "File.h"
#include "Input.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <ctime>

// Forward declaration
class MasterFiles; 

// main helpers
void new_list(MasterFiles &master, std::string &name);
bool process_name(std::string &name, MasterFiles &master);

// --------------------- MASTERFILES CLASS --------------------- //

class MasterFiles {
    private:
        // list commands
        unordered_map<string, vector<uint32_t>> k_search; // key: keyword, val: indices in master file 
        vector<File> master_files; // contains all filenames

    public:
        MasterFiles() {}

        // File functions:
        int32_t get_num_dupes(const string &name) {
            string key = "F:" + name;
            cout << "key in num_dupes: " << key << "\n";
            if (k_search.find(key) != k_search.end()) {
                return k_search[key].size();  
            }
            return -1; // no dupes
        }
        void print_filenames(uint32_t start, int32_t end) {
            if (end == -1) {end = master_files.size();}
            // Ensure bounds are valid
            if (start >= master_files.size() || start > static_cast<uint32_t>(end)) {
                std::cerr << "Error: Invalid index access in print_filenames().\n";
                return;  
            }
            for (uint32_t i = start; i < end; i++) {
                cout << i << ". " << master_files[i].file_name << "\n";
            }
        }
        string get_name(uint32_t idx) {
            return master_files[idx].file_name;
        }
        uint64_t create_timestamp(uint32_t year, uint32_t month = 1, uint32_t day = 1) {
            // Check for invalid dates manually
            if (month < 1 || month > 12 || day < 1 || day > 31) {
                return 0; // Invalid month/day
            }
            // Leap year check
            bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            uint32_t daysInMonth[] = { 
                31, static_cast<uint32_t>(isLeap ? 29 : 28), 31, 30, 31, 30, 
                31, 31, 30, 31, 30, 31 
            };
            if (day > daysInMonth[month - 1]) {
                return 0; // Invalid day (e.g., Feb 29 on a non-leap year)
            }
            // Construct the timestamp
            std::tm tm = {};
            tm.tm_year = year - 1900; // tm_year is years since 1900
            tm.tm_mon = month - 1;   // tm_mon is 0-based
            tm.tm_mday = day;
            tm.tm_hour = 0;
            tm.tm_min = 0;
            tm.tm_sec = 0;

            return static_cast<uint64_t>(std::mktime(&tm));
        }
        void update_indices(uint32_t startIdx) {
            std::cout << "Updating indices from position: " << startIdx << "\n";

            // Temporary storage for updating k_search
            std::unordered_map<string, std::vector<uint32_t>> updated_k_search;

            // Iterate from startIdx onward and update indices
            for (uint32_t i = startIdx; i < master_files.size(); i++) {
                master_files[i].file_idx = i;

                // Store all occurrences of the same filename
                std::string key = "F:" + master_files[i].file_name;
                updated_k_search[key].push_back(i);
            }

            // Apply the updates to k_search
            k_search = std::move(updated_k_search);
        }
        const vector<File>& get_files() const { return master_files; }
        void add_file(File file);
        void do_key_search();
        void search_with_wildcards(const string &pattern, std::unordered_set<uint32_t> &matching_indices, const string &prefix);
        void process_commands(File &file, uint32_t master_idx);
        void search_by_date();
        void list_found(const string &name);
        void delete_phrase(const string& phrase, uint32_t idx);
        void add_phrase(const string& phrase, uint32_t idx, const string& prefix);
};
// --------------------- FUNCTORS --------------------- //

struct Sorter {
    bool operator()(const File &a, const File &b) const {
        if (a.favorite != b.favorite) { // Prioritize favorited files
            return a.favorite > b.favorite;
        }
        if (a.comp_timestamp != b.comp_timestamp) { // Sort by descending timestamp
            return a.comp_timestamp > b.comp_timestamp; // Newer timestamps first
        }
        // Tie-break: Alphabetical order of file names (case-insensitive)
        return a.file_name < b.file_name;
    }
};

struct LowerFunctor {
    bool operator()(const File &file, const uint64_t target) const {
        return file.comp_timestamp < target; 
    }

    bool operator()(const uint64_t target, const File &file) const {
        return target < file.comp_timestamp;
    }
};

struct UpperFunctor {
    bool operator()(const File &file, const uint64_t target) const {
        return file.comp_timestamp <= target; 
    }

    bool operator()(const uint64_t target, const File &file) const {
        return target <= file.comp_timestamp;
    }
};

#endif // MASTER_H