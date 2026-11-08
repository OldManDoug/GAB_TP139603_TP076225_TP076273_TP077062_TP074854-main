#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

/*=================================
    ACTIVITY STRUCT
==================================*/
struct Activity {
    int activityID;
    string parcelID;
    string action;
    string previousStatus;
    string newStatus;
    bool reversible;
    bool reversed;

    Activity() {
        activityID = 0;
        parcelID = "";
        action = "";
        previousStatus = "";
        newStatus = "";
        reversible = false;
        reversed = false;
    }
};

/*=================================
    FILE LOGGING (Record Activity)
==================================*/
inline void recordActivity(string parcelID, string action, string previousStatus, string newStatus, bool reversible, bool reversed = false) {
    int nextID = 1;
    ifstream inFile("activity.txt");
    string line;
    if (inFile) {
        while (getline(inFile, line)) {
            if (!line.empty()) nextID++;
        }
        inFile.close();
    }

    ofstream f("activity.txt", ios::app);
    if (f) {
        f << nextID << "|"
          << parcelID << "|"
          << action << "|"
          << previousStatus << "|"
          << newStatus << "|"
          << (reversible ? "1" : "0") << "|"
          << (reversed ? "1" : "0") << endl;
        f.close();
    }
}

/*=================================
    DISPLAY ALL RECORDED ACTIVITIES
==================================*/
inline void displayAllActivitiesFromFile() {
    ifstream file("activity.txt");
    if (!file) {
        cout << "\nNo activity history found (activity.txt missing).\n";
        return;
    }

    string line;
    cout << "\n=========================================";
    cout << "\n       ALL RECORDED ACTIVITIES           ";
    cout << "\n=========================================\n";
    bool hasContent = false;
    while (getline(file, line)) {
        if (line.empty()) continue;
        hasContent = true;
        
        char buffer[300];
        strcpy(buffer, line.c_str());
        char* fields[10];
        int count = 0;
        char* token = strtok(buffer, "|");
        while (token != NULL && count < 10) {
            fields[count++] = token;
            token = strtok(NULL, "|");
        }

        if (count >= 6) {
            cout << "Activity ID : " << fields[0] << endl;
            cout << "Parcel ID   : " << fields[1] << endl;
            cout << "Action      : " << fields[2] << endl;
            cout << "Previous    : " << fields[3] << endl;
            cout << "New Status  : " << fields[4] << endl;
            cout << "Reversible  : " << (string(fields[5]) == "1" ? "Yes" : "No") << endl;
            cout << "Reversed    : " << (count > 6 && string(fields[6]) == "1" ? "Yes" : "No") << endl;
            cout << "-----------------------------------------\n";
        }
    }
    file.close();
    if (!hasContent) cout << "Activity log is empty.\n";
}

/*=================================
    DISPLAY SINGLE PARCEL HISTORY
==================================*/
inline void displayParcelHistoryFromFile(string targetParcelID) {
    ifstream file("activity.txt");
    if (!file) {
        cout << "\nactivity.txt not found.\n";
        return;
    }

    string line;
    bool found = false;
    cout << "\n========== HISTORY FOR PARCEL " << targetParcelID << " ==========\n";

    while (getline(file, line)) {
        if (line.empty()) continue;

        char buffer[300];
        strcpy(buffer, line.c_str());
        char* fields[10];
        int count = 0;
        char* token = strtok(buffer, "|");
        while (token != NULL && count < 10) {
            fields[count++] = token;
            token = strtok(NULL, "|");
        }

        if (count >= 6 && string(fields[1]) == targetParcelID) {
            found = true;
            cout << "Activity ID : " << fields[0] << endl;
            cout << "Parcel ID   : " << fields[1] << endl;
            cout << "Action      : " << fields[2] << endl;
            cout << "Previous    : " << fields[3] << endl;
            cout << "New Status  : " << fields[4] << endl;
            cout << "Reversible  : " << (string(fields[5]) == "1" ? "Yes" : "No") << endl;
            if (count > 6) {
                cout << "Reversed    : " << (string(fields[6]) == "1" ? "Yes" : "No") << endl;
            }
            cout << "-----------------------------------------\n";
        }
    }
    file.close();

    if (!found) {
        cout << "No activity history found for Parcel ID: " << targetParcelID << "\n";
    }
}

/*=================================
    REVIEW / RETRIEVE PREVIOUS ACTIVITIES
==================================*/
inline void reviewParcelHistory(string targetParcelID) {
    ifstream file("activity.txt");
    if (!file) {
        cout << "\nactivity.txt not found.\n";
        return;
    }

    string line;
    bool found = false;
    string latestStatus = "";
    string latestAction = "";

    cout << "\n========== REVIEW PARCEL: " << targetParcelID << " ==========\n";

    while (getline(file, line)) {
        if (line.empty()) continue;

        char buffer[300];
        strcpy(buffer, line.c_str());
        char* fields[10];
        int count = 0;
        char* token = strtok(buffer, "|");
        while (token != NULL && count < 10) {
            fields[count++] = token;
            token = strtok(NULL, "|");
        }

        if (count >= 6 && string(fields[1]) == targetParcelID) {
            found = true;
            cout << "Activity " << fields[0] << " : " << fields[2] << "\n";
            cout << "Status   : " << fields[4] << "\n\n";
            latestStatus = fields[4];
            latestAction = fields[2];
        }
    }
    file.close();

    if (!found) {
        cout << "Invalid Parcel ID or No History Recorded.\n";
        return;
    }

    cout << "Most Recent Activity : " << latestAction << "\n";
    cout << "Current Status       : " << latestStatus << "\n";
    cout << "===============================================\n";
}

#endif