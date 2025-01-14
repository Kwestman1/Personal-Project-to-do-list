#include "File.h"
#include "Input.h"

// main helpers
void new_list(MasterFiles &master, string &name);
bool process_name(string &name, MasterFiles &master);

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
            auto iter_fn = k_search.find("F:" + name);
            if (iter_fn == k_search.end()) {
                return -1; // no dupes
            }
            else {
                return iter_fn->second.size();
            }
        }
        void print_filenames(uint32_t start, int32_t end) {
            if (end == -1) {end = master_files.size();}
            for (uint32_t i = start; i < end; i++) {
                cout << i << ". " << master_files[i].file_name << "\n";
            }
        }
        string get_name(uint32_t idx) {
            return master_files[idx].file_name;
        }
        uint64_t create_timestamp(uint32_t year, uint32_t month = 1, uint32_t day = 1) {
            std::tm tm = {};
            tm.tm_year = year - 1900; // tm_year is years since 1900
            tm.tm_mon = month - 1;   // tm_mon is 0-based
            tm.tm_mday = day;

            return static_cast<uint64_t>(std::mktime(&tm));
        }
        void update_indices(uint32_t idx) {
            for (auto &file : master_files) {
                for (auto &entry : k_search["F:" + file.file_name]) {
                if (entry > idx) {
                    entry++; 
                }
                }
            }
        }
        uint32_t get_size() {
            return master_files.size();
        }
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