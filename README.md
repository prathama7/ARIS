# ARIS ( Automated Retrieval Information System )


## Description
A C++ command-line file management system that helps users search for files, perform file operations and analyze storage usage. It automatically indexes common user directories and provides filtering and storage insights.

## Features
**File Search**:
- Search for files by name across multiple directories
- Displays file metadata (name, path, size, last modified date)

**Storage Analysis**: 
- Analyze folder contents with file type breakdown and size sorting
- Sort files by size or date
- Human-readable file size formatting

**File Operations**:
- Open the file once it is found
- Append the file with user-entered text
- Move the file to any targeted directory/folder
- Confirm deletion before permanently removing a file

## Requirements
- C++17 or higher
- Windows OS (uses Windows file paths)
- MinGW/GCC compiler

## How to Run
```bash
g++ -std=c++17 main.cpp FileManager.cpp -o main.exe
.\main.exe
```

## Usage

### Mode 1: File Search
1. Select option 1 from the main menu
2. Choose whether to add additional search folders except common locations (Desktop, Downloads, Pictures etc.)
3. Enter file name to search (partial matches from the start of filename)
4. View results with full file details
5. Once file is found, perform file operations:
a. Open the selected file  
b. Insert text into text documents only  
c. Move file into different folders  
d. Delete file permanently  
e. Search again
6. Repeat operations until user types 'y' or "Y"
7. Type 'quit' to exit

### Mode 2: Storage Analysis
1. Select option 2 from the main menu
2. Choose a folder to analyze 
3. Select how many top files to display (5, 10 or 20)
4. Choose sort method (by size or date)
5. View folder summary, file type breakdown and top files
6. Export the analysis in your existing folder

## Project Structure
```
ARIS/
├── main.cpp           # Main program entry point and user interface
├── FileManager.h      # FileManager class declaration
└── FileManager.cpp    # FileManager implementation
```

