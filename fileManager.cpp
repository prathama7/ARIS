#include "FileManager.h"

#include <map>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

// Trim whitespace from both ends
string trim(const string &str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");

    if (first == string::npos)
        return "";
    return str.substr(first, (last - first + 1));
}

// Format file size into human-readable string
string formatFileSize(size_t bytes) {
    string units[] = {"B", "KB", "MB", "GB"};
    double size = bytes;
    int i = 0;
    
    while (size >= 1024 && i < 3) {
        size = size / 1024;
        i++;
    }
    
    return to_string((int)size) + " " + units[i];
}

// Format time_t into readable string
string formatTime(time_t t) {
    char time[20];

    strftime(time, sizeof(time), "%Y-%m-%d %H:%M", localtime(&t));
    return string(time);
}

// Collect files from a given path
vector<FileData> FileManager::collectFilesFromPath(const string &rootPath) {
    vector<FileData> files;
    
    if (!exists(rootPath) || !is_directory(rootPath)) {
        return {};
    }

    try {
        error_code ec;
        for (const auto &entry : recursive_directory_iterator(
                rootPath, 
                directory_options::skip_permission_denied,
                ec)) {
            
            if (ec) {
                ec.clear();
                continue;
            }
            
            try {
                if (entry.is_regular_file()) {
                    path filePath = entry.path();
                    
                    string fileName = filePath.filename().string();
                    string absolutePath = absolute(filePath).string();
                    size_t fileSize = file_size(filePath);
                    
                    auto ftime = last_write_time(filePath);
                    // Convert filesystem time to system time
                    auto stime = chrono::time_point_cast<chrono::system_clock::duration>(
                        ftime - file_time_type::clock::now() + chrono::system_clock::now()
                    );
                    time_t lastMod = chrono::system_clock::to_time_t(stime);

                    // Store file data
                    FileData fd;
                    fd.name = fileName;
                    fd.path = absolutePath;
                    fd.size = fileSize;
                    fd.lastModified = lastMod;
                    
                    files.push_back(fd);
                }
            } catch (...) {
                continue;
            }
        }
    } catch (...) {
    }
    
    return files;
}

void FileManager::buildIndex(const string &rootPath) {
    // Check if path exists
    if (!exists(rootPath)) {
        cout << "  Skipping (doesn't exist): " << rootPath << endl;
        return;
    }
    
    if (!is_directory(rootPath)) {
        cout << "  Skipping (not a directory): " << rootPath << endl;
        return;
    }

    vector<FileData> files = collectFilesFromPath(rootPath);

    for (const auto &fd : files) {
        indexByName[fd.name].push_back(fd);
    }
}

// Index for multiple directories
void FileManager::buildFullIndex(const vector<string> &paths) {
    indexByName.clear();
    for (const string &path : paths) {
        cout << "Indexing: " << path << "..." << endl;
        buildIndex(path);
    }
    cout << "Indexing complete!" << endl;
}

vector<FileData> FileManager::searchFiles(const string &fileName, bool silent) {
    vector<FileData> results;
    string searchLower = fileName;
    transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

    // Search for files starting with the given name
    for (const auto &entry : indexByName) {
        string nameLower = entry.first;
        transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

       if (nameLower.find(searchLower) == 0) {
        results.insert(results.end(), entry.second.begin(), entry.second.end());
        }
    }

    if (!silent) {
        displaySearchResults(results);
    }

    return results;
}

void FileManager::displaySearchResults(const vector<FileData> &results) {
    if (results.empty()) {
        cout << "No files found." << endl;
        return;
    }

    cout << "\nFound " << results.size() << " file(s):" << endl;
    for (size_t i = 0; i < results.size(); i++) {
        cout << "--- File " << i + 1 << " ---" << endl;
        cout << "Name: " << results[i].name << endl;
        cout << "Path: " << results[i].path << endl;
        cout << "Size: " << formatFileSize(results[i].size) << endl;
        cout << "Last Modified: " << formatTime(results[i].lastModified) << endl;
        cout << endl;
    }
}

vector<FileData> FileManager::analyzeStorage(const string &folderPath, int numFiles, int sortChoice) {
    cout << "=== Analyzing Storage for: " << folderPath << " ===" << endl;
    
    if (!exists(folderPath) || !is_directory(folderPath)) {
        cout << "Invalid folder path!" << endl;
        return {};
    }

    cout << "Scanning files..." << endl;

    // Collect all files
    vector<FileData> allFiles = collectFilesFromPath(folderPath);
    
    if (allFiles.empty()) {
        cout << "No files found in this folder." << endl;
        return {};
    }
    
    // Calculate total size
    size_t totalSize = 0;
    for (const auto &file : allFiles) {
        totalSize += file.size;
    }

    cout << "=== Folder Summary ===" << endl;
    cout << "Total files: " << allFiles.size() << endl;
    cout << "Total size: " << formatFileSize(totalSize) << endl;
    cout << endl;

    cout << "=== File Type Breakdown ===" << endl;
    map<string, int> fileTypes;
    map<string, size_t> typeSizes;

    for (const auto &file : allFiles) {
        size_t dotPos = file.name.find_last_of(".");
        string ext = (dotPos != string::npos) ? file.name.substr(dotPos + 1) : "no extension";
        fileTypes[ext]++;
        typeSizes[ext] += file.size;
    }

    for (const auto &type : fileTypes) {
        cout << "  ." << type.first << ": " << type.second << " files (" 
            << formatFileSize(typeSizes[type.first]) << ")" << endl;
    }

    cout << "File type breakdown complete." << endl;
    
    // Sort files based on choice 
    if (sortChoice == 1) {
        // Sort by size using standard library sort
        sort(allFiles.begin(), allFiles.end(), [](const FileData &a, const FileData &b) {
            return a.size > b.size;
        });
    } else if (sortChoice == 2) {
        // Sort by date
        sort(allFiles.begin(), allFiles.end(), [](const FileData &a, const FileData &b) {
            return a.lastModified > b.lastModified;
        });
    }
    
    cout << "=== Sorted Files ===" << endl;

    size_t limit = min((size_t)numFiles, allFiles.size());
    
    for (size_t i = 0; i < limit; i++) {
        cout << i + 1 << ". " << allFiles[i].name << " - " << formatFileSize(allFiles[i].size) << endl;
    }
    cout << endl;

    return allFiles;
}

bool FileManager::exportAnalysis(const vector<FileData> &files, const string &exportPath) {
    ofstream outFile(exportPath);
    if (!outFile.is_open()) {
        cout << "Error: Could not open file for exporting." << endl;
        return false;
    }

    outFile << "Name,Path,Size (Bytes),Last Modified" << endl;

    for (const auto &file : files) {
        outFile << "\"" << file.name << "\","
                << "\"" << file.path << "\","
                << file.size << ","
                << formatTime(file.lastModified) << "\n";
    }

    outFile.close();
    cout << "Analysis exported successfully to: " << exportPath << endl;
    return true;
}

// Helper to select a file from multiple results
string FileManager::selectFileFromResults(const vector<FileData> &results, const string &operation) {
    if (results.empty()) {
        return "";
    }
    
    if (results.size() == 1) {
        return results[0].path;
    }
    
    cout << "\nMultiple files found." << endl;
    cout << "Enter file number to " << operation << " (0 to cancel): ";
    int choice;
    cin >> choice;
    cin.ignore();
    
    if (choice < 1 || choice > (int)results.size()) {
        cout << "Cancelled.\n";
        return "";
    }
    
    return results[choice - 1].path;
}

bool FileManager::openFile(const string &filePath) {
    cout << "Opening file: " << filePath << endl;
    
    string command = "start \"\" \"" + filePath + "\"";
    system(command.c_str());
    
    return true;
}

bool FileManager::insertText(const string &filePath) {
    // Check if file is a text file
    path fp(filePath);
    string extension = fp.extension().string();
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Only allow these specific file types
    vector<string> textExtensions = {".txt", ".csv", ".json", ".md"};
    
    bool isTextFile = false;
    for (const auto &ext : textExtensions) {
        if (extension == ext) {
            isTextFile = true;
            break;
        }
    }
    
    if (!isTextFile) {
        cout << "Error: Cannot insert text into '" << extension << "' files." << endl;
        cout << "Text insertion is only supported for: .txt, .csv, .json, .md files" << endl;
        return false;
    }

    cout << "Enter text to append (press ENTER on empty line to finish):\n";

    string content = "";
    string line;
    bool firstLine = true;

    while (true) {
        getline(cin, line);
        if (line.empty()) break;
        
        if (!firstLine) {
            content += "\n";
        }
        content += line;
        firstLine = false;
    }
    
    if (content.empty()) {
        cout << "No text entered. Cancelled." << endl;
        return false;
    }

    ofstream file(filePath, ios::app);
    if (!file.is_open()) {
        cout << "Error: Could not open file for writing.\n";
        return false;
    }

    file << "\n" << content << "\n";
    file.close();

    cout << "Text appended successfully to: " << filePath << endl;
    return true;
}

bool FileManager::moveFile(const string &sourcePath, const string &newPath, const string &userProfile) {
    string destFolder = newPath;
    
    if (!exists(destFolder)) {
        // Capitalize first letter, lowercase the rest
        string normalized = newPath;
        transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
        normalized[0] = toupper(normalized[0]);
        
        destFolder = userProfile + "\\" + normalized;
        
        if (!exists(destFolder)) {
            cout << "Error: Destination folder not found: " << newPath << endl;
            cout << "Tried: " << destFolder << endl;
            cout << "Use full path or ensure folder exists in user profile." << endl;
            return false;
        }
    }
    
    if (!is_directory(destFolder)) {
        cout << "Error: '" << destFolder << "' is not a folder." << endl;
        return false;
    }

    path source(sourcePath);
    string filename = source.filename().string();
    path destination = path(destFolder) / filename;
    
    if (exists(destination)) {
        cout << "Warning: File already exists at destination. Overwrite? (y/n): ";
        string confirm;
        getline(cin, confirm);
        if (confirm != "y" && confirm != "Y") {
            cout << "Cancelled." << endl;
            return false;
        }
    }

    try {
        rename(sourcePath, destination.string());
        cout << "File moved successfully to: " << destination.string() << endl;
        return true;
    }
    catch (const filesystem_error &e) {
        cout << "Move failed: " << e.what() << endl;
        return false;
    }
}

bool FileManager::safeDelete(const string &filePath) {
    // Confirmation before deleting permanently
    cout << "Delete " << filePath << "? (y/n): ";
    string confirm;
    getline(cin, confirm);

    if (confirm != "y" && confirm != "Y") {
        cout << "Cancelled.\n";
        return false;
    }

    try {
        remove(filePath);
        cout << "Deleted: " << filePath << "\n";
        return true;
    } catch (const filesystem_error &e) {
        cout << "Failed: " << e.what() << endl;
        return false;
    }
}

