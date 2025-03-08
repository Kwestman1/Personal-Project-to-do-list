# Keyword Search - Currently fixing

# Important Note
This application is not for practical use, it will NOT remember your complied lists once you quit. I am a student learning as I go, this personal project is me applying what I have learned in class to gain more practice. I am currently taking a web systems class and hope to, soon, implement a client-side dynamic page version of this project.

# 💡 New implementations TBD
- Change list name
- Create a web version to save lists using sessions and SQL.
- Password protected lists
- Upload existing file
- Print files names by (today, yesterday, last month, last year)
- Swap entry files

# Basic outline of application
1. Load Introduction
2. prompt user
3. process various commands (using hash map, file handling using <fstream> library)
4. once quit/save, Goodbye message, restart
- input:
    - -h/—help: how to use application
    - open existing file.txt
    - create a new file to write into
    - see file names
        - can select by number or filename
- commands:
    
    list commands:
    
    - print list
    - delete entry in list
    - add entry in list
    - move entry in list
    - clear list (prompted with a confirm)
    - delete list (prompted with a confirm)
    - save list (when done/want to create new list)
    - make a list a favorite
    
    file commands:
    
    - search:
        - timestamp-range search
        - matching timestamp search
        - keyword search
    - print file names
    
