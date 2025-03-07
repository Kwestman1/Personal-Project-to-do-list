#pragma once

#include "File.h"
#include "Input.h"

// Forward declaration
class MasterFiles;

// --------------------- MASTERFILES CLASS --------------------- //

class MasterFiles {
private:
    std::unordered_map<std::string, std::vector<uint32_t>> k_search; // Key: keyword, val: indices in master file 
    std::vector<File> master_files; // Contains all filenames

public:
    MasterFiles() {}

    // Inline function definitions
    inline int32_t get_num_dupes(const string &name) {
        string key = "F:" + name;
        if (k_search.find(key) != k_search.end()) {
            return k_search[key].size();  
        }
        return -1; // no dupes
    }

    inline void print_filenames(uint32_t start, int32_t end) {
        if (end == -1) {
            end = master_files.size();
        }
        if (start >= master_files.size() || start > static_cast<uint32_t>(end)) {
            std::cerr << "Error: Invalid index access in print_filenames().\n";
            return;
        }
        for (uint32_t i = start; i < end; i++) {
            std::cout << i << ". " << master_files[i].file_name << "\n";
        }
    }

    inline std::string get_name(uint32_t idx) {
        return master_files[idx].file_name;
    }

    inline int getCurrentYear() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm *parts = std::localtime(&now_c);
        return 1900 + parts->tm_year;
    }

    inline bool is_leap_year(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    inline int get_days_in_month(int year, int month) {
        if (month == 2) { 
            return is_leap_year(year) ? 29 : 28;
        }
        static const int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        return days_in_month[month - 1];
    }

    inline void update_indices(uint32_t startIdx) {
        if (startIdx >= master_files.size()) return;  // Prevent out-of-bounds access

        // Remove affected entries from k_search
        for (uint32_t i = startIdx; i < master_files.size(); i++) {
            std::string key = "F:" + master_files[i].file_name;
            k_search[key].clear();  // Clear existing indices for this key
        }

        // Update indices and repopulate k_search
        for (uint32_t i = startIdx; i < master_files.size(); i++) {
            master_files[i].file_idx = i;
            std::string key = "F:" + master_files[i].file_name;
            k_search[key].push_back(i);
        }
    }

    inline void delete_file(const std::string &file_name) {
        if (std::remove(file_name.c_str()) == 0) {
            std::cout << "File '" << file_name << "' deleted successfully.\n";
        } else {
            std::cerr << "Error: Could not delete file '" << file_name << "'.\n";
        }
    }

    inline const std::vector<File>& get_files() const { return master_files; }

    // Functions that should be implemented in the Master.cpp file
    void new_list(string &name);
    bool process_name(string &name);
    void add_file(File &file);
    void do_key_search();
    void search_with_wildcards(const std::string &pattern, std::unordered_set<uint32_t> &matching_indices, const std::string &prefix);
    void process_commands(uint32_t master_idx);
    void search_by_date();
    void list_found(const std::string &name);
    void delete_phrase(const std::string &phrase, uint32_t idx);
    void add_phrase(const std::string &phrase, uint32_t idx, const std::string &prefix);
    uint64_t create_timestamp(uint32_t year, uint32_t month, uint32_t day);
};

// --------------------- FUNCTORS --------------------- //

struct Sorter {
    bool operator()(const File &a, const File &b) const {
        if (a.favorite != b.favorite) return a.favorite > b.favorite;
        if (a.comp_timestamp != b.comp_timestamp) return a.comp_timestamp > b.comp_timestamp;
        return a.file_name < b.file_name; // Alphabetical tie-breaker
    }
};
