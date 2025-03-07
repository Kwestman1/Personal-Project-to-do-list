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
    option = menu.get_menu_option(1, 5);
    if (option == 5) {
      break;
    }
    if (option == 1) {
      menu.print_help();
    }
    else if (option == 2) {
      do {
        cout << "Enter list name: ";
        cin >> name;
      } while (!menu.is_valid_name(name));
      if(process_name(name, m)) {
        break;
      }
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
      menu.print_search_options();
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
  return 1;
}