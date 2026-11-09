#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstring>
#include "Task1.hpp" //to allow it to use
#include "Task2.hpp"
#include "Task5.hpp"
#include "History.hpp"
using namespace std;
// the PLAN
/*operation of stacks, but solve issues related..
 display history can be done with stack to display(peek operation)
to undo a action/history use a POP
Record parcel acvitivty use the PUSH
Retrieve and review ( ig push and pop or display from stack/peek )
Empty or nullcases use isEmpty() function for stack -- also error handling in stack research on.
Maintains a history from processing module -
review by staff(need their input from a file storing names)-- what happen to a parcel
[check for a bad case] reverse a decision[use an if else])
1. Record Parcel-Related Activities (ALL FILES, ENTIRE SYSTEM)
Record important activities performed during parcel processing, with sufficient information to
identify the parcel, activity and relevant time or sequence. -- when array storr
2. Display Activity History
Allow staff to display recent activities and the activity history for a selected parcel, including the order in which activities occurred.
3. Retrieve and Review Previous Activities
Allow staff to select a parcel and review previous activities, including processing sequence, previous status, most recent activity and relevant request or resource information.
4. Reverse or Undo an Appropriate Recent Activity confirm
Provide a mechanism for reversing an appropriate recent activity where permitted. Identify the activity, eligibility, restore relevant parcel/request/resource status where necessary.
5. Handle Activity-History Exceptions information and update related
Handle no activity history, invalid Parcel ID, an activity that cannot be reversed, an activity
already reversed, invalid activity selection, or an operation that would result in an invalid parcel
status.
- concepts apply: undoing, pop and push(redo and undo)
---- USE STACK PROGRAMMING FOR THE MODULE---

*/
// https://codesignal.com/learn/courses/fundamental-data-structures-stacks-and-queues-in-cpp/lessons/understanding-and-implementing-stacks-in-cpp

/*=================================
    CUSTOM STACK NODE FOR UNDO
==================================*/
struct UndoNode {
    int activityID;
    string parcelID;
    string action;
    string previousStatus;
    string newStatus;
    UndoNode* next;

    UndoNode(int id, string pId, string act, string prev, string nSt) {
        activityID = id;
        parcelID = pId;
        action = act;
        previousStatus = prev;
        newStatus = nSt;
        next = NULL;
    }
};

/*=================================
      LIFO UNDO STACK
==================================*/
class UndoStack {
private:
    UndoNode* topNode;

public:
    UndoStack() { topNode = NULL; }

    bool isEmpty() { return topNode == NULL; }

    void push(int id, string pId, string act, string prev, string nSt) {
        UndoNode* newNode = new UndoNode(id, pId, act, prev, nSt);
        newNode->next = topNode;
        topNode = newNode;
    }

    UndoNode pop() {
        if (isEmpty()) return UndoNode(-1, "", "", "", "");
        UndoNode* temp = topNode;
        UndoNode val = *temp;
        topNode = topNode->next;
        delete temp;
        return val;
    }

    UndoNode peek() {
        if (isEmpty()) return UndoNode(-1, "", "", "", "");
        return *topNode;
    }
};

/*=================================
  SAVE AN UNDO FLAG INTO activity.txt
  field 5 = reversible, field 6 = reversed
==================================*/
inline bool updateActivityFlag(int activityID, string parcelID, string action, int field, string value) {
    ifstream in("activity.txt");
    if (!in) return false;
    ofstream out("activity.tmp");
    if (!out) return false;

    bool changed = false;
    string line;
    while (getline(in, line)) {
        string parts[7];
        int count = 0;
        size_t start = 0;
        while (count < 7) {
            size_t bar = line.find('|', start);
            if (bar == string::npos) {
                parts[count++] = line.substr(start);
                break;
            }
            parts[count++] = line.substr(start, bar - start);
            start = bar + 1;
        }
        if (!changed && count == 7 && atoi(parts[0].c_str()) == activityID && parts[1] == parcelID
            && parts[2] == action && parts[5] == "1" && parts[6] == "0") {
            parts[field] = value;
            line = parts[0];
            for (int i = 1; i < 7; i++) line += "|" + parts[i];
            changed = true;
        }
        out << line << endl;
    }
    in.close();
    out.close();
    ::remove("activity.txt");
    ::rename("activity.tmp", "activity.txt");
    return changed;
}

/*=================================
  ACTIVITY MANAGER (Undo & Exceptions)
==================================*/
class ActivityManager {
private:
    UndoStack undoStack;

    // An activity that can never be undone any more (the parcel has moved on):
    // take it off the stack and save it as not reversible, so it does not block older activities
    void dropStale(UndoNode item) {
        undoStack.pop();
        updateActivityFlag(item.activityID, item.parcelID, item.action, 5, "0");
        cout << "Activity #" << item.activityID << " can no longer be undone and was removed from the undo list.\n";
    }

public:
    ActivityManager() {}

    // Hydrate stack from activity.txt on startup
    void loadUndoStackFromFile() {
        ifstream file("activity.txt");
        if (!file) return;

        string line;
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

            if (count >= 7) {
                int id = atoi(fields[0]);
                string parcelID = fields[1];
                string action = fields[2];
                string prev = fields[3];
                string newSt = fields[4];
                bool rev = (string(fields[5]) == "1");
                bool revd = (string(fields[6]) == "1");

                // Only push reversible and non-reversed items onto LIFO stack
                if (rev && !revd) {
                    undoStack.push(id, parcelID, action, prev, newSt);
                }
            }
        }
        file.close();
    }

    // Requirement 4 & 5: Reverse recent activity with full exception handling
    void undoActivity() {
        if (undoStack.isEmpty()) {
            cout << "\n[Exception] No Reversible Activity Available (Stack is Empty).\n";
            return;
        }

        UndoNode topItem = undoStack.peek();
        if (topItem.activityID == -1) {
            cout << "\n[Exception] Stack read error.\n";
            return;
        }

        // Check eligibility BEFORE asking the staff to confirm
        // Integration with Task 2: an open service request owns the parcel status
        if (parcelActiveRequest(topItem.parcelID.c_str())) {
            cout << "\n[Exception] Parcel " << topItem.parcelID
                 << " has an active service request. Finish or cancel it in Task 2 before undoing.\n";
            return;
        }

        // Validate via Task1ParcelManager
        Task1ParcelManager task1Mgr;
        if (!task1Mgr.load()) {
            cout << "\n[Exception] Failed to load parcel.txt records.\n";
            return;
        }

        char currentStatus[30];
        if (!task1Mgr.status(topItem.parcelID.c_str(), currentStatus)) {
            cout << "\n[Exception] Invalid Parcel ID or parcel missing from records.\n";
            dropStale(topItem);
            return;
        }

        if (string(currentStatus) != topItem.newStatus) {
            cout << "\n[Exception] Invalid Parcel State. Current status (" << currentStatus
                 << ") differs from log (" << topItem.newStatus << "). Undo Rejected.\n";
            dropStale(topItem);
            return;
        }

        cout << "\nUndo Activity #" << topItem.activityID
             << " (Parcel: " << topItem.parcelID << ", Action: " << topItem.action
             << ") ? (Y/N): ";
        char choice;
        cin >> choice;
        cin.ignore(10000, '\n');

        if (choice != 'Y' && choice != 'y') {
            cout << "\nUndo Cancelled.\n";
            return;
        }

        // Apply rollback status via Task 1 transition rules
        if (!task1Mgr.setStatus(topItem.parcelID.c_str(), topItem.previousStatus.c_str(), false)) {
            cout << "\n[Exception] Task 1 rejected state rollback (invalid transition rule).\n";
            dropStale(topItem);
            return;
        }

        undoStack.pop();
        // Save it as reversed so it is not offered again after a restart
        updateActivityFlag(topItem.activityID, topItem.parcelID, topItem.action, 6, "1");

        // Log the reversal
        recordActivity(topItem.parcelID, "UNDO - " + topItem.action, topItem.newStatus, topItem.previousStatus, false, true);

        cout << "\nUndo Successful! Parcel " << topItem.parcelID
             << " restored to status: " << topItem.previousStatus << ".\n";
    }
};

/*=================================
          TASK 4 MENU
==================================*/
inline void task4menu() {
    ActivityManager manager;

    manager.loadUndoStackFromFile();

    int choice;
    do {
        cout << "\n================================";
        cout << "\n TASK 4: ACTIVITY & UNDO MANAGEMENT";
        cout << "\n================================";
        cout << "\n1. Display All Recorded Activities";
        cout << "\n2. Display History for Selected Parcel";
        cout << "\n3. Review & Retrieve Parcel History";
        cout << "\n4. Undo Last Activity (LIFO)";
        cout << "\n5. Back to Main Menu";
        cout << "\nChoice : ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1:
                displayAllActivitiesFromFile();
                break;
            case 2: {
                string id;
                cout << "Enter Parcel ID: ";
                getline(cin, id);
                displayParcelHistoryFromFile(id);
                break;
            }
            case 3: {
                string id;
                cout << "Enter Parcel ID to review: ";
                getline(cin, id);
                reviewParcelHistory(id);
                break;
            }
            case 4:
                manager.undoActivity();
                break;
            case 5:
                cout << "Returning to Main Menu...\n";
                break;
            default:
                cout << "Invalid choice.\n";
        }
    } while (choice != 5);
}
