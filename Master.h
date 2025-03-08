#pragma once

#include "File.h"
#include "Input.h"

// Forward declaration
class MasterFiles;

// --------------------- FUNCTORS --------------------- //

struct Sorter {
    bool operator()(const File &a, const File &b) const {
        if (a.favorite != b.favorite) return a.favorite > b.favorite;
        if (a.comp_timestamp != b.comp_timestamp) return a.comp_timestamp > b.comp_timestamp;
        return a.print_file_name < b.print_file_name; // Alphabetical tie-breaker
    }
};

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

    inline void print_filenames() {
         Input m;
         if (master_files.empty()) {
             m.print_empty_message();
             return;
         }
         for (uint32_t i = 0; i < master_files.size(); i++) {
             std::cout << i << ". Filename: " << master_files[i].print_file_name << ", last edited: " 
                     << master_files[i].print_timestamp 
                     << (master_files[i].favorite ? " \u2B50" : "") << "\n";
         }
     }

    inline std::string get_name(uint32_t idx) {
        return master_files[idx].print_file_name;
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

        // Step 1: Remove old indices (only affected entries)
        std::vector<std::string> affected_keys;
        for (uint32_t i = startIdx; i < master_files.size(); i++) {
            affected_keys.push_back("F:" + master_files[i].hash_file_name);
        }
        for (const auto& key : affected_keys) {
            auto it = k_search.find(key);
            if (it != k_search.end()) {
                auto& vec = it->second;
                vec.erase(std::remove_if(vec.begin(), vec.end(), [&](uint32_t idx) {
                    return idx >= startIdx;  // Remove only affected indices
                }), vec.end());
            }
        }
        // Step 2: Re-add correct indices for affected entries
        for (uint32_t i = startIdx; i < master_files.size(); i++) {
            std::string key = "F:" + master_files[i].hash_file_name;
            k_search[key].push_back(i);
        }
    }

    inline uint32_t insert_file(File& file) {  // Accept rvalue reference
        auto it = std::lower_bound(master_files.begin(), master_files.end(), file, Sorter());
        uint32_t insertPos = std::distance(master_files.begin(), it);
        master_files.insert(it, std::move(file));  // Move into the vector
        return insertPos;
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
    void search_with_wildcards(const std::string &pattern, std::unordered_set<uint32_t> &matching_indices, const char prefix);
    void process_commands(uint32_t master_idx);
    void search_by_date();
    void list_found(const std::string &name);
    void delete_phrase(const std::string &phrase, uint32_t idx);
    void add_phrase(const std::string &phrase, uint32_t idx, const std::string &prefix);
    uint64_t create_timestamp(uint32_t year, uint32_t month, uint32_t day);
};
