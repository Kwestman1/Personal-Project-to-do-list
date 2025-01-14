#include "Master.h"

// main helpers
void new_list(MasterFiles &master, string &name);
void process_name(string &name, MasterFiles &master);

// --------------------- DRIVER --------------------- //
int main() {
  cout << "Hello!! Welcome to your personalized To-Do List!\n\n";
  uint32_t option;
  string name = "";
  MasterFiles m;
  Input menu;
  while (true) {
    menu.print_options();
    option = menu.get_menu_option(1, 5);
    if (option == 5) {
      break;
    }
    if (option == 1) {
      print_help();
    }
    else if (option == 2) {
      do {
        cout << "Enter list name: ";
        cin >> name;
      } while (!menu.is_valid_name(name));
      process_name(name, m);
    }
    else if (option == 3) {
      do {
        cout << "Creating new list!\n";
        cout << "Enter list name: ";
        cin >> name;
      } while (!menu.is_valid_name(name));
      new_list(m, name);
    }
    else if (option == 4) {
      print_search_options();
      uint32_t new_opt = menu.get_menu_option(1, 2);
      if (new_opt == 1) {
        m.search_by_date();
      }
      else {
        m.do_key_search();
      }
    }
  }

  cout << "Goodbye! Thank you for using my application!\n";
  return 0;
}

// --------------------- DRIVER HELPERS --------------------- //

void process_name(string &name, MasterFiles &master) {
    uint32_t option;
    File file;
    Input menu;
    // find list in hash map
    do {
      // if doesn't exist, print new options
      cout << "Couldn't find file in database. Select new option to proceed:\n";
      cout << "1. Retype filename: enter '1'\n";
      cout << "2. Create new list named " << name << ": enter '2' \n";
      cout << "3. Print Master List of all existing file names: enter '3'\n";
      cout << "4. Preform a Search on the Master list: enter '4' \n";
      cout << "5. Quit: enter '5' \n";
      option = menu.get_menu_option(1, 5);

      if (option == 1) {
        do {
          cout << "Enter list name: ";
          cin >> name;  
        } while (!menu.is_valid_name(name));
      }
      else if (option == 2) {
        cout << "Creating new list!\n";
        new_list(master, name);
        return;
      }
      else if (option == 3) {
        master.print_filenames(0, -1);
        cout << "Enter filename or number\n";
        cin >> name;
        if (!all_of(name.begin(), name.end(), ::isdigit)) {
          uint32_t num = stoull(name);
          num = master.validate_and_get_index(num);
          name = master.get_name(num);
        }
      }
      else if (option == 4) {
        print_search_options();
        uint32_t new_opt = get_menu_option(1, 2);
        if (new_opt == 1) {
          master.search_by_date();
        }
        else {
          master.do_key_search();
        }
      }
      else if (option == 5) {
        cout << "Goodbye! Thank you for using my application!\n";
        exit(1);
      }
    } while (master.get_num_dupes(name) == -1);

    master.list_found(name);
}

void new_list(MasterFiles &master, string &name) {
  File file;
  while (master.get_num_dupes(name) != -1) {
    cout << "The name \"" << name << "\" is already in use.\n";
    cout << "Enter a new list name (or press Enter to auto-generate a unique name): ";
    string user_input;
    getline(cin, user_input);

    if (!user_input.empty()) {
      name = user_input;
    } 
    else {
      // Auto-generate a unique name by appending a number
      uint32_t dupe_count = master.get_num_dupes(name);
      name += "(" + to_string(dupe_count) + ")";
      cout << "Auto-generated new name: " << name << "\n";
    }
  }
  // Create and add the file with the finalized unique name
  file.file_name = name;
  cout << "Creating file " << file.file_name << "\n";
  master.add_file(file);
}
// --------------------- FILE COMMAND HELPERS --------------------- //

void MasterFiles::do_key_search() {
  string key;
  cout << "Enter keyword(s): ";
  cin.ignore(); // Ignore leftover newline
  getline(cin, key);

  unordered_set<uint32_t> common_file_indices;
  unordered_set<uint32_t> common_content_indices;

  search_with_wildcards(key, common_file_indices, "F:");
  search_with_wildcards(key, common_content_indices, "C:");

  // Debugging
  cout << "File indices: ";
  for (const auto &idx : common_file_indices) {cout << idx << " ";}
  cout << "\n";
  cout << "Content indices: ";
  for (const auto &idx : common_content_indices) {cout << idx << " ";}
  cout << "\n";

  // combine files into 1
  for (const auto &idx : common_content_indices) {
    common_file_indices.insert(idx);
  }

  if (!common_file_indices.empty()) {
    cout << "Search results:\n";
    for (const auto &index : common_file_indices) {
      // debugging (shouldn't print)
      if (index >= master_files.size()) {
        cerr << "Error: Index " << index << " is out of bounds!\n";
        continue;
      }
      cout << index << ". File: " << master_files[index].file_name << ", Timestamp: " << master_files[index].print_timestamp
          << ", Starred: " << (master_files[index].favorite ? "Yes" : "No") << "\n";
    }
  } 
  else {
    cout << "No files contain all specified keywords in either filenames or content.\n";
  }
}

void MasterFiles::search_with_wildcards(const string &pattern, std::unordered_set<uint32_t> &matching_indices, const std::string &prefix) {
  string regex_pattern = prefix + std::regex_replace(pattern, std::regex(R"(\*)"), ".*");
  std::regex re(regex_pattern, std::regex::icase);
  for (const auto &pair : k_search) {
    if (std::regex_match(pair.first, re)) {
      matching_indices.insert(pair.second.begin(), pair.second.end());
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
  // Create and open a file
  string f_name = file.file_name + ".txt";
  std::ofstream fin(f_name);
  // check file accordingly
  if (!fin) {
    std::cerr << "Error creating the file." << std::endl;
    exit(1);
  }
  fin.close();

  // insert file into sorted master_file
  auto it = std::lower_bound(master_files.begin(), master_files.end(), file, Sorter());
  uint32_t insertPos = std::distance(master_files.begin(), it);
  master_files.insert(it, file);
  file.file_idx = insertPos;
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
  cout << "New List successfully created!\n\n";
  process_commands(file, insertPos);
}

void MasterFiles::add_phrase(const string& phrase, int idx, const string& prefix) {
  std::istringstream iss(phrase);
  string word;
  while (iss >> word) {
    std::transform(word.begin(), word.end(), word.begin(), ::tolower); 
    string key = prefix + word; 
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

void File::add_file_contents() {
  std::ifstream fin(file_name);
  if (!fin) {
    std::cerr << "Error opening the file." << std::endl;
    exit(1);
  }
  string line;
  while (std::getline(fin, line)) {
    master_list.push_back(line); 
  }
  fin.close();
}

void MasterFiles::list_found(string &name){
  auto iter_fn = k_search.find("F:" + name);
  uint32_t idx;
  cout << "List Found!\n";
  if (iter_fn->second.size() > 1) {
    cout << "More than one List name exists\n";
    cout << "Select numbered list below: \n";
    print_filenames(iter_fn->second[0], iter_fn->second.size());
    cout << "% ";
    cin >> idx;
  }
  else if (iter_fn->second.size() == 1) {
    idx = iter_fn->second[0];
  }
  else {
    cout << "Programming Error: EMPTY value, master files indexing error";
    exit(1);
  }
  process_commands(master_files[idx], idx);
}

// --------------------- LIST COMMAND HELPERS --------------------- //

void MasterFiles::process_commands(File &file, uint32_t master_idx) {
  // open and check file accordingly
  std::ifstream fin;
  fin.open(file.file_name);
  if (!fin.is_open()) {
    cerr << "Error: Could not open file " << file.file_name << ". Please check the file name and try again.\n";
    return;
  }

  // edit time stamp of last opened 
  file.set_time();
  cout << "File opened successfully!\n";
  cout << "Updating timestamp to: " << file.print_timestamp << "\n";

  // print options
  Input menu;
  menu.print_cmd_options();

  // process commands
  char cmd = ' ';
  uint32_t pos;
  char confirm = ' ';
  do {
    cout << "% ";
    cin >> cmd;
    if (cmd == 'p' || cmd == 'a' || cmd == 'd' || cmd == 'b' || cmd == 'e') {
      file.print_list();
      if (cmd != 'p') {
        cout << "Select the position you want to edit: ";
        cin >> pos;
        pos = validate_and_get_index(pos);
      }
    }
    if (cmd == 'r') {
      menu.print_cmd_options();
    }
    else if (cmd == 'a') { // add entry (to position)
      string phrase;
      cout << "Enter Phrase: ";
      cin >> phrase;
      phrase = menu.is_valid_name(phrase);
      // Insert the phrase into the master list at the specified position
      file.master_list.insert(file.master_list.begin() + pos, phrase);
      // Add the phrase to the hash map
      add_phrase(phrase, master_idx, "C:");
    }
    else if (cmd == 'd') { // delete entry
      file.delete_el(pos);
    }
    else if (cmd == 'b') { // move to beginning
      file.move_to_beginning(pos);
    }
    else if (cmd == 'e') { // move to end
      file.move_to_end(pos);
    }
    else if (cmd == 'c') {
      cout << "Please type 'y' to confirm that you want to clear this list (any other character otherwise): ";
      cin >> confirm;
      if (confirm == 'y') {
        for (uint32_t i = 0; i < file.master_list.size(); i++) {
          delete_phrase("C:" + file.master_list[i], master_idx);
        }
        file.master_list.clear();
        cout << "List cleared\n";
      }
      else {
        cout << "no confirmation, continuing...\n";
      }
    }
    else if (cmd == 'x') {
      cout << "Please type 'y' to confirm that you want to delete this list (any other character otherwise): ";
      cin >> confirm;
      if (confirm == 'y') {
        for (uint32_t i = 0; i < file.master_list.size(); i++) {
          delete_phrase("C:" + file.master_list[i], master_idx);
        }
        file.master_list.clear();
        delete_phrase("F:" + file.file_name, master_idx);
        master_files.erase(master_files.begin() + master_idx);
        cout << "List deleted\n";
      }
      else {
        cout << "no confirmation, continuing...\n";
      }
    }
    else if (cmd == 's') {
      if(!file.favorite) {
        cout << "Starred!\n";
        file.favorite = true;
      }
      else {
        cout << "Removed star\n";
        file.favorite = false;
      }
    }
    else {
      cerr << "Error: command doesn't exist\n";
    }
    } while (cmd != 'q');

  update_indices(master_idx); // update indices to reflect priority (for ts and fav change)
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