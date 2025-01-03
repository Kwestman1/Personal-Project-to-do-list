#include "To_Do.h"
using namespace std;

// helpers
void print_options();
void print_help();
void new_list();
void process_name(string &name);

// --------------------- DRIVER --------------------- //
int main() {
  cout << "Hello! Welcome to your personalized To-Do List!\n\n";
  string option = "";
  string name = "";

  do {
    print_options();
    cin >> option;
    name = option;
    // convert to lower case
    std::transform(option.begin(), option.end(), option.begin(), [](unsigned char c){ return std::tolower(c); });
    if (isdigit(option[0])) {
      if (option.size() == 1) {
        if (option[0] == '1') {
          print_help();
        }
        else if(option[0] == '2') {
          cout << "Enter list name: ";
          cin >> name;
          process_name(name);
        }
        else if (option[0] == '3') {
          new_list();
        }
        else {
          cout << "Numbered option " << option[0] << " doesn't exist\n";
        }
      }
      else {
        cout << "Numbered option " << option << " doesn't exist\n";
      }
    }
    else {
      if (option == "-h" || option == "--help") {
        print_help();
      }
      else { // option 2, editing existing list
        process_name(name);
      }
    }
  } while (option[0] == '4' || option == "quit");

  cout << "Goodbye! Thank you for using my application!\n";
  return 1;
}

// --------------------- DRIVER HELPERS --------------------- //

void print_options() {
  cout << "Options:\n";
  cout << "1. How to use Application: enter '1' or type '-h' OR '--help'\n";
  cout << "2. Edit existing list: enter '2' or type your list's name \n";
  cout << "3. Create new list: enter '3'\n";
  cout << "4. Quit: enter '4' or type 'quit'\n";
  cout << "% ";
}

void print_help() {
  cout << "The application will prompt you with '%' symbol, indicating a option to be choosen from the options list\n";
  cout << "The symbol will be prompted after each command proccessed until you type '4' or 'quit'\n";
  cout << "Filenames are sorted by starred, timestamped then alphabetically\n";
  cout << "Your list will be saved when you quit the application\n\n";
  cout << "Due to constraints we don't allow the following:\n";
  cout << "The first character of your list name cannot be a number unless you type '2' when prompted '%'\n";
  cout << "When modifiying list contents there are no undo's, when selected to delete/clear list, you will be prompted with a confirmation. \n";
  cout << "Thank you for your understanding!\n";
}

void process_name(string &name) {
    string option = "";
    MasterFiles m;
    File file;
    // find list in hash map
    do {
      // if doesn't exist, print new options
      cout << "Couldn't find file in database. Select new option to proceed:\n";
      cout << "1. Retype filename: enter '1'\n";
      cout << "2. Create new list named " << name << ": enter '2' \n";
      cout << "3. Print Master List of existing file names: enter '3'\n";
      cout << "4. Preform a Search on the Master list: enter '4' \n";
      cout << "4. Quit: enter '5' or type 'quit'\n";
      cout << "% ";
      cin >> option;
      std::transform(option.begin(), option.end(), option.begin(), [](unsigned char c){ return std::tolower(c); });

      if (isdigit(option[0])) {
        if (option.size() == 1) {
          if (option[0] == '1') {
            cout << "Enter list name: ";
            cin >> name;
          }
          else if(option[0] == '2') {
            cout << "Creating new list!\n";
            file.file_name = name;
            m.add_file(file);
          }
          else if (option[0] == '3') {
            m.print_filenames(0, -1);
            cout << "Enter filename or number\n";
            cin >> name;
            if (isdigit(name[0])) {
              uint32_t num = stoull(name);
              name = m.get_name(num);
            }
          }
          else if (option[0] == '4') {
            string new_opt;
            do {
              cout << "Select Search Option:\n";
              cout << "1. Timestamp search (search by year, month, date): enter '1'\n";
              cout << "2. Keyword search (filename and contents): enter '2'\n";
              cout << "% ";
              cin >> new_opt;
              if (!isdigit(new_opt[0]) || new_opt.size() != 1) {
                cout << "Error: Selection out of range or not numerical\n";
              }
            } while (isdigit(new_opt[0]) && new_opt.size() == 1);
            m.do_search(stoull(option));
          }
          else if (option[0] == '5') {
            cout << "Goodbye! Thank you for using my application!\n";
            exit(1);
          }
          else {
            cout << "Numbered option " << option[0] << " doesn't exist\n";
          }
        }
      else {
        cout << "Numbered option " << option << " doesn't exist\n";
      }
    } 
    else {
      if (option == "quit") {
        cout << "Goodbye! Thank you for using my application!\n";
        return;
      }
      cout << "Error: option " << option << " doesn't exist, please enter a number only or type 'quit' \n";
    }
    } while (!m.find_fn(name));
    m.process_commands(name);
}

void new_list() {
  string name;
  cout << "Creating new list!\n";
  cout << "Enter list name: ";
  cin >> name;
  MasterFiles m;
  File file;
  
  // if dupe (check w user), add listname (1...n)
  if (m.find_fn(name) != -1) {
    bool change_name = m.dupe_name(name);
    while(!change_name && m.find_fn(name) != -1) {
      cout << "Enter list name: ";
      cin >> name;
      change_name = m.dupe_name(name);
    }
    if (change_name) {
      string num = "(" + to_string(m.find_fn(name)) + ")";
      name += num;
    }
  }
  file.file_name = name;
  m.add_file(file);
}

// --------------------- FILE COMMAND HELPERS --------------------- //

void MasterFiles::do_search(uint32_t num) {
  MasterFiles m;
  if (num == 1) { // search by yr or month or day
    m.search_by_date();
  }
  else if (num == 2) { // keyword search
    string key;
    m.build_keyword_map();
    cout << "Enter keyword: ";
    cin >> key;
    m.search_keyword(key);
  }
}

void MasterFiles::search_keyword(const string &keyword) {
  uint32_t num_found = 0;
  if (k_search.find("F:" + keyword) != k_search.end()) {
    auto iter = k_search.find("F:" + keyword);
    num_found = iter->second.size();
    cout << num_found << " List(s) found!";
    // Sort the indices in ascending order
    std::sort(iter->second.begin(), iter->second.end());
    for (const auto &index : iter->second) {
      cout << "File: " << master_files[index].file_name << ", Timestamp: " << master_files[index].print_timestamp
      << ", Starred: " << (master_files[index].favorite ? "Yes" : "No") << "\n";
    }
  }
  else {
    cout << "No matching filenames\n";
  }
  if (k_search.find("C:" + keyword) != k_search.end()) {
    auto iter = k_search.find("C:" + keyword);
    num_found = iter->second.size();
    cout << num_found << " List Entries found!\n";
    // Sort the indices in ascending order
    std::sort(iter->second.begin(), iter->second.end());
    for (const auto &index : iter->second) {
      cout << "File: " << master_files[index].file_name << ", Timestamp: " << master_files[index].print_timestamp
      << ", Starred: " << (master_files[index].favorite ? "Yes" : "No") << "\n";
      cout << "Content:\n";
      master_files[index].print_list();
    }
  }
  else {
    cout << "No matching content\n";
  }
}

void MasterFiles::build_keyword_map() {
  for (uint32_t i = 0; i < master_files.size(); ++i) {
    // Add filename keywords
    add_phrase(master_files[i].file_name, i, "F:");
    // Add content keywords
    for (const auto& phrase : master_files[i].master_list) {
      add_phrase(phrase, i, "C:");
    }
  }
}

void MasterFiles::search_by_date() {
    int year, month, day;
    std::cout << "Enter year (YYYY): ";
    std::cin >> year;

    std::cout << "Enter month (MM, optional, default=all months): ";
    std::cin >> month;
    if (std::cin.fail() || month < 1 || month > 12) {
        month = 0; // No specific month
        std::cin.clear();
    }

    std::cout << "Enter day (DD, optional, default=all days): ";
    std::cin >> day;
    if (std::cin.fail() || day < 1 || day > 31) {
        day = 0; // No specific day
        std::cin.clear();
    }

    uint64_t lower_bound = 0, upper_bound = 0;
    if (day > 0) {
        // Search for a specific day
        lower_bound = create_timestamp(year, month, day);
        upper_bound = create_timestamp(year, month, day + 1) - 1;
    } else if (month > 0) {
        // Search for a specific month
        lower_bound = create_timestamp(year, month, 1);
        upper_bound = create_timestamp(year, month + 1, 1) - 1;
    } else {
        // Search for an entire year
        lower_bound = create_timestamp(year, 1, 1);
        upper_bound = create_timestamp(year + 1, 1, 1) - 1;
    }

    auto lower_it = std::lower_bound(master_files.begin(), master_files.end(), lower_bound, LowerFunctor());
    auto upper_it = std::upper_bound(master_files.begin(), master_files.end(), upper_bound, UpperFunctor());

    std::cout << "Files in the range:\n";
    for (auto it = lower_it; it != upper_it; ++it) {
        std::cout << (it->favorite ? "[Starred] " : "") << it->print_timestamp << "\n";
    }
}

void MasterFiles::add_file(File& file) {
  // open and check file accordingly
  std::ifstream fin;
  fin.open(file.file_name);
  if (!fin.is_open()) {
    cout << "open failed\n";
    exit(1);
  }
  // add master files contents
  string word;
  while (getline(fin, word)) {
    file.master_list.push_back(word); 
  }
  fin.close();

  // insert file into sorted master_file
  auto it = std::lower_bound(master_files.begin(), master_files.end(), file, Sorter());
  uint32_t insertPos = std::distance(master_files.begin(), it);
  master_files.insert(it, file);
  // add file to hash map
  auto iter_fn = k_search.find("F:" + file.file_name);
  if (iter_fn == k_search.end()) {
    k_search["F:" + file.file_name].push_back(insertPos);
  }
  else {
    iter_fn->second.push_back(insertPos);
  }
  // Update the indices in the hash map so they match the master_files indices
  update_indices(insertPos);
  cout << "New List " << file.file_name << " created!\n";
}

bool MasterFiles::dupe_name(string &name) {
  string option;
  char opt = ' ';
  do {
    auto iter = k_search.find("F:" + name);
    uint32_t idx = iter->second.size();
    cout << "You aready have a list named " << name << " would you like to keep the name or change it?\n";
    cout << "Type: 'y' to duplicate name to: " << name << "(" << idx << ")";
    cout << "Type: 'n' to change name\n";
    cout << "Type: 'q' to quit\n";
    cout << "% ";
    cin >> option;
    opt = tolower(option[0]);
    if (option.size() != 1 || opt != 'y' || opt != 'n' || opt != 'q') {
      cout << "Error: invalid option";
    } 
  } while(option.size() != 1 || opt != 'y' || opt != 'n' || opt != 'q');

  if (opt == 'y') {
    return true;
  }
  else if (opt == 'n') {
    return false;
  }
  cout << "Goodbye! Thank you for using my application!\n";
  exit(1);
}

void MasterFiles::add_phrase(const string& phrase, int idx, const string& prefix) {
  string partial;
  for (char c : phrase) {
    if (c == ' ') {
        partial.clear(); // Reset on space
    } 
    else {
      partial += tolower(c); // Build partial keyword
      string key = prefix + partial;
      // Insert or update the hash map
      auto iter = k_search.find(key);
      if (iter == k_search.end()) {
        k_search[key].push_back(idx);
      } 
      else {
        if (std::find(iter->second.begin(), iter->second.end(), idx) == iter->second.end()) {
          iter->second.push_back(idx); // Avoid duplicate idx entries
        }
      }
    }
  }
}

// --------------------- LIST COMMAND HELPERS --------------------- //

void MasterFiles::process_commands(string &name) {
  // open and check file accordingly
  uint32_t idx = 0;
  std::ifstream fin;
  fin.open(name);
  if (!fin.is_open()) {
    cout << "open failed\n";
    exit(1);
  }
  // lookup file in hash map
  auto iter = k_search.find("F:" + name);
  if (iter == k_search.end()) {
    cout << "Programming Error: FILE doesn't exist when it should";
    exit(1);
  }
  if (iter->second.size() > 1) {
    cout << "More than one List name exists\n";
    std::sort(iter->second.begin(), iter->second.end()); // need? in theory indices should be in order
    cout << "Select numbered list below: \n";
    print_filenames(iter->second[0], iter->second.size());
    cout << "% ";
    cin >> idx;
  }
  else if (iter->second.size() == 1) {
    idx = iter->second[0];
  }
  else {
    cout << "Programming Error: EMPTY value, master files indexing error";
    exit(1);
  }
  // edit time stamp of last opened 
  master_files[idx].set_time();
  cout << "File opened successfully!\n";
  cout << "Updating timestamp to: " << master_files[idx].print_timestamp << "\n";

  // print options
  master_files[idx].print_cmd_options();

  // process commands
  char cmd = ' ';
  uint32_t pos;
  char confirm = ' ';
  do {
    cout << "% ";
    cin >> cmd;
    if (cmd == 'p' || cmd == 'a' || cmd == 'd' || cmd == 'b' || cmd == 'e') {
      master_files[idx].print_list();
      if (cmd != 'p') {
        cout << "Select the position you want to edit: ";
        cin >> pos;
      }
    }
    if (cmd == 'r') {
      master_files[idx].print_cmd_options();
    }
    else if (cmd == 'a') { // add entry (to position)
      // check that int is a valid position
      if (master_files[idx].check_int_el(pos, idx)) {
        string phrase;
        cout << "Enter Phrase: ";
        cin >> phrase;
        // Insert the phrase into the master list at the specified position
        master_files[idx].master_list.insert(master_files[idx].master_list.begin() + pos, phrase);
        // Add the phrase to the hash map
        add_phrase(phrase, idx, "C:");
      }
      else {
        cerr << "Error: position is invalid in the excerpt list\n";
      }
    }
    else if (cmd == 'd') { // delete entry
      // check that int is a valid position
      if (master_files[idx].check_int_el(pos, idx)) {
          master_files[idx].delete_el(pos);
      }
      else {
        cerr << "Error: position is invalid in the excerpt list\n";
      }
    }
    else if (cmd == 'b') { // move to beginning
      // check that int is a valid position
      if (master_files[idx].check_int_el(pos, idx)) {
        master_files[idx].move_to_beginning(pos);
      }
      else {
        cerr << "Error: position is invalid in the excerpt list\n";
      }
    }
    else if (cmd == 'e') { // move to end
      // check that int is a valid position
      if (master_files[idx].check_int_el(pos, idx)) {
        master_files[idx].move_to_end(pos);
      }
      else {
        cerr << "Error: position is invalid in the excerpt list\n";
      }
    }
    else if (cmd == 'c') {
      cout << "Please type 'y' to confirm that you want to clear this list: ";
      cin >> confirm;
      if (confirm == 'y') {
        for (uint32_t i = 0; i < master_files[idx].master_list.size(); i++) {
          delete_phrase("C:" + master_files[idx].master_list[i], idx);
        }
        master_files[idx].master_list.clear();
        cout << "List cleared\n";
      }
    }
    else if (cmd == 'x') {
      cout << "Please type 'y' to confirm that you want to delete this list: ";
      cin >> confirm;
      if (confirm == 'y') {
        for (uint32_t i = 0; i < master_files[idx].master_list.size(); i++) {
          delete_phrase("C:" + master_files[idx].master_list[i], idx);
        }
        master_files[idx].master_list.clear();
        delete_phrase("F:" + master_files[idx].file_name, idx);
        master_files.erase(master_files.begin() + idx);
        cout << "List deleted\n";
      }
    }
    else if (cmd == 's') {
      if(!master_files[idx].favorite) {
        cout << "Starred!\n";
        master_files[idx].favorite = true;
      }
      else {
        cout << "Removed star\n";
        master_files[idx].favorite = false;
      }
    }
    else {
      cerr << "Error: command doesn't exist\n";
    }
    } while (cmd != 'q');

  update_indices(idx); // update indices to reflect priority (for ts and fav change)
  fin.close();
}

void File::delete_el(uint32_t position) {
    uint32_t size = static_cast<uint32_t>(master_list.size() - 1);
    for (uint32_t i = position; i < size; i++) {
        master_list[i] = master_list[i + 1];
    }
    master_list.resize(size);
    cout << "Deleted list entry " << position << "\n";
}

void File::move_to_beginning(uint32_t position) {
    for (uint32_t i = position; i > 0; i--) {
        std::swap(master_list[i], master_list[i - 1]);
    }
    cout << "Moved list entry " << position << " to the beginning. \n";
}

void File::move_to_end(uint32_t position) {
    for (uint32_t i = position; i < master_list.size() - 1; i++) {
        std::swap(master_list[i], master_list[i + 1]);
    }
    cout << "Moved list entry " << position << " to the end.\n";
}

void MasterFiles::delete_phrase(const string& phrase, uint32_t idx) {
  string temp = "";
  // Locate the phrase parts in the hash map
  for (uint32_t i = 0; i < phrase.size(); i++) {
    if (phrase[i] == ' ') {
      temp = "";
    }
    else {
      temp += phrase[i];
      auto it = k_search.find(phrase);
      // If the phrase is not found, there's nothing to delete
      if (it != k_search.end()) {
        // Remove the idx from the vector of indices
        auto& indices = it->second;
        indices.erase(std::remove(indices.begin(), indices.end(), idx), indices.end());
        // If the vector is empty, remove the key-value pair from the hash map
        if (indices.empty()) {
          k_search.erase(it);
        }
      }
    }
  }
}