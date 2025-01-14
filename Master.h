#include "File.h"
#include "Input.h"

// --------------------- MASTERFILES CLASS --------------------- //

class MasterFiles {
    private:
        // list commands
        unordered_map<string, vector<uint32_t>> k_search; // key: keyword, val: indices in master file 
        vector<File> master_files; // contains all filenames

    public:
        MasterFiles() {}

        // File functions:
        uint32_t get_num_dupes(string &name);
        void add_file(File& file);
        void print_filenames(uint32_t start, uint32_t end);
        string get_name(uint32_t idx);
        void do_key_search();
        void process_commands(File &file, uint32_t master_idx);
        void search_by_date();
        uint64_t create_timestamp(int year, int month, int day);
        void search_with_wildcards(const string &pattern, std::unordered_set<uint32_t> &matching_indices, const std::string &prefix);
        void update_indices(uint32_t idx);
        void delete_phrase(const string& phrase, uint32_t idx);
        void add_phrase(const string& phrase, int idx, const string& prefix);
        uint32_t validate_and_get_index(uint32_t idx);
        void list_found(string &name);
};

// ----- File functions ----- //

uint32_t MasterFiles::get_num_dupes(string &name) {
  auto iter_fn = k_search.find("F:" + name);
  if (iter_fn == k_search.end()) {
    return -1; // no dupes
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
  for (auto &file : master_files) {
    for (auto &entry : k_search["F:" + file.file_name]) {
      if (entry > idx) {
        entry++; 
      }
    }
  }
}

uint32_t MasterFiles::validate_and_get_index(uint32_t idx) {
    while ((idx >= master_files.size() || idx < 0) && idx != 0) {
        cerr << "Error: index out of range. Please enter a number between 0 and" << master_files.size() << ".\n";
        cout << "Enter new number: ";
        cin >> idx;
    }
    return idx;
}
// --------------------- FUNCTORS --------------------- //

struct Sorter {
    bool operator()(const File &a, const File &b) const {
        if (a.favorite != b.favorite) { // Prioritize favorited files
            return a.favorite; 
        }
        if (a.comp_timestamp != b.comp_timestamp) { // Sort by descending timestamp
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