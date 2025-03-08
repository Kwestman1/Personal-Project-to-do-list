#include "Master.h"

// --------------------- DRIVER --------------------- //
int main() {
  std::ios_base::sync_with_stdio(false);
  cout << "Hello!! Welcome to your personalized To-Do List!\n";
  uint32_t option;
  string name = "";
  MasterFiles m;
  Input menu;
  while (true) {
    menu.print_options();
    option = menu.get_menu_option(1, 6);
    if (option == 6) {
      break;
    }
    if (option == 1) {
      menu.print_help();
    }
    else if (option == 2) {
      do {
        cout << "Enter list name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        getline(cin, name);
      } while (!menu.is_valid_name(name));
      if(m.process_name(name)) {
        continue;
      }
    }
    else if (option == 3) {
      do {
        cout << "Creating new list!\n";
        cout << "Enter list name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        getline(cin, name);
      } while (!menu.is_valid_name(name));
      m.new_list(name);
    }
    else if (option == 4) {
      m.print_filenames(0, m.get_files().size());
    }
    else if (option == 5) {
      menu.print_search_options();
      uint32_t new_opt = menu.get_menu_option(1, 3);
      if (new_opt == 1) {
        m.search_by_date();
      }
      else if (new_opt == 2){
        m.do_key_search();
      }
      else {
        continue;
      }
    }
  }

  cout << "Goodbye! Thank you for using my application!\n";
  return 1;
}