#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>

using namespace std; 

struct FileData {
    string name;
    string path;
    size_t size;
    time_t lastModified;
};

// Helper functions
string trim(const string &str);
string formatFileSize(size_t bytes);
string formatTime(time_t t);

class FileManager {
public:
    void buildIndex(const string &rootPath);
    void buildFullIndex(const vector<string> &paths);

    vector<FileData> searchFiles(const string &fileName, bool silent = false);

    void displaySearchResults(const vector<FileData> &results);
    vector<FileData> analyzeStorage(const string &folderPath, int numFiles, int sortChoice);
    bool exportAnalysis(const vector<FileData> &files, const string &exportPath);


    bool openFile(const string &filePath);
    bool insertText(const string &filePath);
    bool safeDelete(const string &filePath);
    bool moveFile(const string &sourcePath, const string &newPath, const string &userProfile);

    vector<FileData> collectFilesFromPath(const string &rootPath);
    string selectFileFromResults(const vector<FileData> &results, const string &operation);

    bool sendFile(const string &filePath);

private:
    unordered_map<string, vector<FileData>> indexByName;
};

#endif
