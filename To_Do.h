#include <string.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <fstream>
using std::unordered_map;
using std::string;
using std::vector;
using std::cout;
using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

// --------------------- FILE CLASS --------------------- //

struct File {
    string print_timestamp;
    uint64_t comp_timestamp; 
    string file_name;
    bool favorite;
    vector<string> master_list; // contains all file entries for that list
    
    void set_time();
    // Proccess command functions:
    void print_cmd_options();
    void print_list();
    void append(uint32_t position);
    void delete_el(uint32_t position);
    void move_to_beginning(uint32_t position);
    void move_to_end(uint32_t position);
    // process_commands helpers:
    bool check_int_el(uint32_t i, uint32_t idx);

    File() : file_name{" "}, favorite{false} {
        set_time();
    }
};

void File::set_time() {
    TimePoint timestamp = Clock::now();
    std::time_t t = Clock::to_time_t(timestamp);
    // Manually adjust for the time zone offset (-8: PST, -5: EST)
    constexpr int timezone_offset_seconds = -8 * 3600;
    t += timezone_offset_seconds;
    comp_timestamp = static_cast<uint64_t>(t);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    print_timestamp = oss.str();
}

// ----- Proccess command functions ----- //

bool File::check_int_el(uint32_t i, uint32_t idx) {
    if (i >= master_list.size()) {
        return false;
    }
    return true;
}

void File::print_cmd_options() {
    cout << "File Command Options: \n";
    cout << "Type: 'a' to add an entry anywhere in list (list contents will be printed)\n";
    cout << "Type: 'd' to delete an entry anywhere in list (list contents will be printed)\n";
    cout << "Type: 'b' to move an entry to the beginning of the list (list contents will be printed)\n";
    cout << "Type: 'e' to move an entry to the end of the list (list contents will be printed)\n";
    cout << "Type: 'c' to clear the entire lists contents\n";
    cout << "Type: 'x' to delete the entire list\n";
    cout << "Type: 'p' to print the entire lists contents\n";
    cout << "Type: 'r' to see this menu again\n";
    cout << "Type: 'q' to quit\n";
}

void File::print_list() {
    for (uint32_t i = 0; i < master_list.size(); i++) {
        cout << i << ". " << master_list[i] << "\n";
    }
}

// --------------------- MASTERFILES CLASS --------------------- //

class MasterFiles {
    private:
        // list commands
        unordered_map<string, vector<uint32_t>> k_search; // key: keyword, val: indices in master file 
        vector<File> master_files; // contains all filenames

    public:
        MasterFiles() {}

        // File functions:
        uint32_t find_fn(string &name);
        void add_file(File& file);
        void print_filenames(uint32_t start, uint32_t end);
        string get_name(uint32_t idx);
        void do_search(uint32_t num);
        void process_commands(string &name);
        bool dupe_name(string &name);
        void search_by_date();
        uint64_t create_timestamp(int year, int month, int day);
        void build_keyword_map();
        void search_keyword(const string &keyword);
        void update_indices(uint32_t idx);
        void delete_phrase(const string& phrase, uint32_t idx);
        void add_phrase(const string& phrase, int idx, const string& prefix);
};

// ----- File functions ----- //

uint32_t MasterFiles::find_fn(string &name) {
  auto iter_fn = k_search.find("F:" + name);
  if (iter_fn == k_search.end()) {
    return -1;
  }
  else {
    return iter_fn->second.size();
  }
}

uint64_t MasterFiles::create_timestamp(int year, int month = 1, int day = 1) {
    std::tm tm = {};
    tm.tm_year = year - 1900; // tm_year is years since 1900
    tm.tm_mon = month - 1;   // tm_mon is 0-based
    tm.tm_mday = day;

    return static_cast<uint64_t>(std::mktime(&tm));
}

string MasterFiles::get_name(uint32_t idx) {
    return master_files[idx].file_name;
}

void MasterFiles::print_filenames(uint32_t start, uint32_t end) {
  if (end == -1) {end = master_files.size();}
  for (uint32_t i = start; i < end; i++) {
    cout << i << ". " << master_files[i].file_name << "\n";
  }
}

void MasterFiles::update_indices(uint32_t idx) {
  for (int i = idx; i < master_files.size(); ++i) {
    for (int j = 0; j < k_search["F:" + master_files[i].file_name].size(); j++) {
      if (k_search["F:" + master_files[i].file_name].at(j) > idx) {
        k_search["F:" + master_files[i].file_name].at(j)++; // remember: file indices not number for duplicate names
      }
    }
  }
}

// --------------------- FUNCTORS --------------------- //

struct Sorter {
    bool operator()(const File &a, const File &b) const {
        // Prioritize favorited files
        if (a.favorite != b.favorite) {
            return a.favorite; 
        }
        // Sort by descending timestamp
        if (a.comp_timestamp != b.comp_timestamp) {
            return a.comp_timestamp > b.comp_timestamp; // Newer timestamps first
        }
        // Tie-break: Alphabetical order of file names (case-insensitive)
        return strcasecmp(a.file_name.c_str(), b.file_name.c_str()) < 0; 
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