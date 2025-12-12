#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "FileManager.h"

using namespace std;

int main() {
    FileManager fm;
    string userProfile = getenv("USERPROFILE");

    cout << "=== ARIS: File Search & Management System ===" << endl;
    cout << "Select a mode:" << endl;
    cout << "1. File Search" << endl;
    cout << "2. Storage Analysis" << endl;
    cout << endl;

    string modeChoice;
    getline(cin, modeChoice);
    modeChoice = trim(modeChoice);

    
    // FILE SEARCH MODE
    if (modeChoice == "1") {
        vector<string> searchPaths = {
            userProfile + "\\Downloads", 
            userProfile + "\\Documents",
            userProfile + "\\Pictures",
            userProfile + "\\Videos",
            userProfile + "\\Music",
            userProfile + "\\Desktop",

        };

        cout << "Searching common locations..." << endl;
        cout << "Would you like to add any additional folders? (y/n): " << endl;

        string addMore;
        getline(cin, addMore);
        addMore = trim(addMore);

        while (addMore == "y" || addMore == "Y") {
            cout << "Enter folder path: ";

            string customPath;
            getline(cin, customPath);
            customPath = trim(customPath);
            
            if (!customPath.empty()) {
                searchPaths.push_back(customPath);
                cout << "Added: " << customPath << endl;
            }
            
            cout << "Add another folder? (y/n): ";
            getline(cin, addMore);
            addMore = trim(addMore);
        } 

        cout << "Building file index..." << endl;
        fm.buildFullIndex(searchPaths);

        while (true) {
        cout << "\nEnter file name to search (or 'quit' to exit): ";
        string fileName;
        getline(cin, fileName);
        fileName = trim(fileName);

        if (fileName == "quit" || fileName == "exit" || fileName == "q")
            break;

        vector<FileData> results = fm.searchFiles(fileName);
        
        // If files found, continue with other operations
        if (!results.empty()) {
            bool continueOperations = true;
            
            while (continueOperations) {
                cout << "What would you like to do?" << endl;
                cout << "1. Open the file" << endl;
                cout << "2. Insert text into the file" << endl;
                cout << "3. Move the file" << endl;
                cout << "4. Delete the file" << endl;
                cout << "5. Search for a different file" << endl;

                cout << endl;
                cout << "Choose operation: ";
                
                string opChoice;
                getline(cin, opChoice);
                opChoice = trim(opChoice);
                
                if (opChoice == "1") {
                        string selectedPath = fm.selectFileFromResults(results, "open");
                        if (!selectedPath.empty()) {
                            fm.openFile(selectedPath);
                        }
                    }
                    else if (opChoice == "2") {
                        // Select file first, then insert
                        string selectedPath = fm.selectFileFromResults(results, "insert into");
                        if (!selectedPath.empty()) {
                            fm.insertText(selectedPath);
                        }
                }
                else if (opChoice == "3") {
                    string selectedPath = fm.selectFileFromResults(results, "move");

                    if (!selectedPath.empty()) { 
                    cout << "\nEnter destination folder path: ";
                    string destFolder;
                    getline(cin, destFolder);
                    destFolder = trim(destFolder);
                    
                    fm.moveFile(selectedPath, destFolder, userProfile);
                    }
                        
                }
                else if (opChoice == "4") {
                    string selectedPath = fm.selectFileFromResults(results, "delete");
                    if (!selectedPath.empty()) {
                        fm.safeDelete(selectedPath);
                    }            
                }
                else if (opChoice == "5") {
                    continueOperations = false;  // Break inner loop, search again
                }
                else {
                    cout << "Invalid choice.\n";
                }
                
                // After each operation, ask if they want to do more
                if (continueOperations && opChoice != "5") {
                    cout << "\nPerform another operation on these results? (y/n): ";
                    string more;
                    getline(cin, more);
                    more = trim(more);
                    
                    if (more != "y" && more != "Y") {
                        continueOperations = false;
                    }
                }
            }
        }
    }
}
    // STORAGE ANALYSIS MODE
    else if (modeChoice == "2") {
    while (true) {
    cout << "\nWhich folder would you like to analyze?"<< endl;
        cout << "1. Downloads"<< endl;
        cout << "2. Documents"<< endl;
        cout << "3. Desktop"<< endl;
        cout << "4. Pictures"<< endl;
        cout << "5. Custom"<< endl;
        cout << "(Type 'quit' to exit)" << endl;
        cout << endl;
                
        string folderChoice;
        getline(cin, folderChoice);
        folderChoice = trim(folderChoice);

        if (folderChoice == "quit" || folderChoice == "exit" || folderChoice == "q")
            break;
        
        string folderPath;
        switch (folderChoice[0]) {  // Check first character
        case '1':
            folderPath = userProfile + "\\Downloads";
            break;
        case '2':
            folderPath = userProfile + "\\Documents";
            break;
        case '3':
            folderPath = userProfile + "\\Desktop";
            break;
        case '4':
            folderPath = userProfile + "\\Pictures";
            break;
        case '5':
            cout << "Enter folder path: ";
            getline(cin, folderPath);
            folderPath = trim(folderPath);
            break;
        default:
            cout << "Invalid choice. Exiting." << endl;
            break;
        }
        
        if (!folderPath.empty()) {

        cout << "How many largest files do you want to see?" << endl;
            cout << "1. Top 5" << endl;
            cout << "2. Top 10" << endl;
            cout << "3. Top 20" << endl;

            string numChoice;
            getline(cin, numChoice);
            numChoice = trim(numChoice);
            
            int numFiles;

            switch (numChoice[0]) {
            case '1':
                numFiles = 5;
                break;
            case '2':
                numFiles = 10;
                break;
            case '3':
                numFiles = 20;
                break;
            default:
                numFiles = 10;
                cout << "Invalid choice. Showing top 10.";
                break;
            }
            
            cout << "Sort by:" << endl;
            cout << "1. Size (largest first)" << endl;
            cout << "2. Date (most recent first)" << endl;

            string sortInput;
            getline(cin, sortInput);
            sortInput = trim(sortInput);
            
            int sortChoice;

            switch (sortInput[0]) {
            case '1':
                sortChoice = 1;
                break;
            case '2':
                sortChoice = 2;
                break;
            default:
                sortChoice = 1;
                cout << "Invalid choice. Sorting by size.\n";
                break;
            }
            
            fm.analyzeStorage(folderPath, numFiles, sortChoice);

            vector<FileData> analyzedFiles = fm.collectFilesFromPath(folderPath);

            cout << "Do you want to export this analysis? (y/n): ";
            string exportChoice;
            getline(cin, exportChoice);
            exportChoice = trim(exportChoice);

            if (exportChoice == "y" || exportChoice == "Y") {
                cout << "\nEnter export file name with file type: ";
                string exportname;
                getline(cin, exportname);
                exportname = trim(exportname);

                if (!exportname.empty()) {
                    string exportPath = exportname;
                    fm.exportAnalysis(analyzedFiles, exportPath);
                } 
                else {
                    cout << "Invalid path. Export cancelled." << endl;
                }
            }

        }
    }
    }
    
    else {
        cout << "Invalid choice. Exiting." << endl;
    }

    return 0;
}
