#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstring>
#include "Task1.hpp" //to allow it to use
#include "Task2.hpp"
using namespace std;
// the PLAN 
/*
Maintains a history from processing module - 
review by staff(need their input from a file storing names)-- what happen to a parcel
[check for a bad case] reverse a decision[use an if else])
1. Record Parcel-Related Activities 
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
Task 4 records all actions

*/
// https://codesignal.com/learn/courses/fundamental-data-structures-stacks-and-queues-in-cpp/lessons/understanding-and-implementing-stacks-in-cpp

// Define Stack class 
/*=================================
    PARCEL CLASS
==================================*/
class Parcel
{
private:
    string parcelID;
    string currentStatus;

public:
    Parcel()
    {
        parcelID = "";
        currentStatus = "";
    }

    Parcel(string id, string status)
    {
        parcelID = id;
        currentStatus = status;
    }

    string getParcelID()
    {
        return parcelID;
    }

    string getStatus()
    {
        return currentStatus;
    }

    void setStatus(string status)
    {
        currentStatus = status;
    }
};

/*=================================
    ACTIVITY CLASS
==================================*/
class Activity
{
public:
    int activityID;
    string parcelID;
    string action;
    string previousStatus;
    string newStatus;
    bool reversible;
    bool reversed;

    Activity()
    {
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
    LINKED LIST NODE
==================================*/
struct ActivityNode
{
    Activity activity;
    ActivityNode* next;

    ActivityNode()
    {
        next = NULL;
    }
};

/*=================================
    CUSTOM STACK NODE
==================================*/
struct UndoNode
{
    int activityID;
    UndoNode* next;

    UndoNode(int id)
    {
        activityID = id;
        next = NULL;
    }
};

/*=================================
      UNDO STACK
==================================*/
class UndoStack
{
private:
    UndoNode* topNode;

public:

    UndoStack()
    {
        topNode = NULL;
    }

    bool isEmpty()
    {
        return topNode == NULL;
    }

    void push(int activityID)
    {
        UndoNode* newNode = new UndoNode(activityID);

        newNode->next = topNode;
        topNode = newNode;
    }

    int pop()
    {
        if(isEmpty())
            return -1;

        UndoNode* temp = topNode;

        int value = temp->activityID;

        topNode = topNode->next;

        delete temp;

        return value;
    }

    int peek()
    {
        if(isEmpty())
            return -1;

        return topNode->activityID;
    }
};

/*=================================
  ACTIVITY MANAGER
==================================*/
class ActivityManager
{
    //global variable? 
private:

    ActivityNode* head;

    UndoStack undoStack;

    int nextActivityID;

public:

    ActivityManager()
    {
        head = NULL;
        nextActivityID = 1;
    }

    /*=================================
        RECORD ACTIVITY
    =================================*/

    void recordActivity(
        string parcelID,
        string action,
        string previousStatus,
        string newStatus,
        bool reversible)
    {
        Activity a;

        a.activityID = nextActivityID++;
        a.parcelID = parcelID;
        a.action = action;
        a.previousStatus = previousStatus;
        a.newStatus = newStatus;
        a.reversible = reversible;
        a.reversed = false;

        ActivityNode* newNode = new ActivityNode();

        newNode->activity = a;

        if(head == NULL)
        {
            head = newNode;
        }
        else
        {
            ActivityNode* current = head;

            while(current->next != NULL)
            {
                current = current->next;
            }

            current->next = newNode;
        }

        if(reversible)
        {
            undoStack.push(a.activityID);
        }
 //file to save
    ofstream f("activity.txt", ios::app); //as an application
if(f){
    f << a.activityID << "|"
             << a.parcelID << "|"
             << a.action << "|"
             << a.previousStatus << "|"
             << a.newStatus << "|"
             << a.reversible << "|"
             << a.reversed
             << endl;

        f.close();
}

        cout << "\nActivity Recorded Successfully.\n";
   
    }

    /*=================================
          DISPLAY SINGLE ACTIVITY - it is not working
    =================================*/

    void displayActivity(Activity a)
    {
        ifstream f("activity.txt");
        if(!f){
        cout << "Error with file";
        }
        
        cout << "\nActivity ID : "
             << a.activityID;

        cout << "\nParcel ID   : "
             << a.parcelID;

        cout << "\nAction      : "
             << a.action;

        cout << "\nPrevious    : "
             << a.previousStatus;

        cout << "\nNew Status  : "
             << a.newStatus;

        cout << "\nReversible  : "
             << (a.reversible ? "Yes" : "No");

        cout << "\nReversed    : "
             << (a.reversed ? "Yes" : "No");

        cout << "\n----------------------------------\n";
    }

    /*=================================
         PARCEL HISTORY separate? 
    =================================*/

    void displayParcelHistory(string parcelID)
    {
        bool found = false;

        ActivityNode* current = head;

        cout << "\nHistory for Parcel "
             << parcelID
             << "\n";

        while(current != NULL)
        {
            if(current->activity.parcelID == parcelID)
            {
                displayActivity(current->activity);
                found = true;
            }

            current = current->next;
        }

        if(!found)
        {
            cout << "\nNo Activity History Found.\n";
        }
    }

    /*=================================
         FIND ACTIVITY
    =================================*/

    ActivityNode* findActivity(int activityID)
    {
        ActivityNode* current = head;

        while(current != NULL)
        {
            if(current->activity.activityID
               == activityID)
            {
                return current;
            }

            current = current->next;
        }

        return NULL;
    }

    /*=================================
         REVIEW PARCEL - not really working
    =================================*/

    void reviewParcel(string parcelID)
    {
        ActivityNode* current = head;

        bool found = false;

        string latestStatus = "";
        string latestAction = "";

        cout << "\n========== REVIEW ==========\n";

        while(current != NULL)
        {
            if(current->activity.parcelID == parcelID)
            {
                found = true;

                cout << "\nActivity "
                     << current->activity.activityID
                     << " : "
                     << current->activity.action;

                cout << "\nStatus : "
                     << current->activity.newStatus
                     << "\n";

                latestStatus =
                    current->activity.newStatus;

                latestAction =
                    current->activity.action;
            }

            current = current->next;
        }

        if(!found)
        {
            cout << "\nInvalid Parcel ID or No History.\n";
            return;
        }

        cout << "\nMost Recent Activity : "
             << latestAction;

        cout << "\nCurrent Status       : "
             << latestStatus;

        cout << "\n============================\n";
    }

    /*=================================
            UNDO ACTIVITY (Integrated with Task 1) but currently need improving
    ==================================*/
    void undoActivity()
    {
        if(undoStack.isEmpty())
        {
            cout << "\nNo Reversible Activity Available.\n";
            return;
        }

        int activityID = undoStack.peek();
        ActivityNode* actNode = findActivity(activityID);

        if(actNode == NULL)
        {
            cout << "\nActivity Not Found.\n";
            return;
        }

        Activity& activity = actNode->activity;

        if(activity.reversed)
        {
            cout << "\nActivity Already Reversed.\n";
            return;
        }

        if(!activity.reversible)
        {
            cout << "\nCannot Reverse Activity.\n";
            return;
        }

        cout << "\nUndo Activity #" << activity.activityID 
             << " for Parcel " << activity.parcelID << " ? (Y/N): ";

        char choice;
        cin >> choice;
        cin.ignore(10000, '\n');

        if(choice != 'Y' && choice != 'y')
        {
            cout << "\nUndo Cancelled.\n";
            return;
        }

        // Use Task1ParcelManager to safely check and revert the status
        Task1ParcelManager task1Mgr;
        if (!task1Mgr.load())
        {
            cout << "\nFailed to load parcel.txt for undo operation.\n";
            return;
        }

        char currentStatus[30];
        if (!task1Mgr.status(activity.parcelID.c_str(), currentStatus))
        {
            cout << "\nParcel Not Found in Records.\n";
            return;
        }

        // Verify state matches the new status before reverting
        if (string(currentStatus) != activity.newStatus)
        {
            cout << "\nInvalid Parcel State. Current state differs from activity log.\nUndo Rejected.\n";
            return;
        }

        // Apply previous status using Task 1's validation/transition rules
        if (!task1Mgr.setStatus(activity.parcelID.c_str(), activity.previousStatus.c_str()))
        {
            cout << "\nTask 1 rejected the status rollback (invalid transition).\nUndo Failed.\n";
            return;
        }

        activity.reversed = true;
        undoStack.pop();

        // Record the undo action itself
        recordActivity(
            activity.parcelID,
            "UNDO - " + activity.action,
            activity.newStatus,
            activity.previousStatus,
            false
        );

        cout << "\nUndo Successful. Parcel " << activity.parcelID 
             << " reverted to " << activity.previousStatus << ".\n";
    }
};
ActivityManager manager; //global variable reusing
void loadActivitiesFromFile()
{
    ifstream file("activity.txt");

    if(!file)
    {
        return;
    }

    string line;

    while(getline(file, line))
    {
        cout << line << endl;
    }

    file.close();
}
/*=================================
            MAIN
==================================*/

void task4menu()
{
    

    // Dynamically load parcels from parcel.txt instead of hardcoding
    const int maxParcels = 100;
    Parcel parcels[maxParcels];
    int totalParcels = 0;

    ifstream parcelFile("parcel.txt");
    char line[300];
    if (parcelFile) {
        parcelFile.getline(line, 300); 
        while (parcelFile.getline(line, 300) && totalParcels < maxParcels) {
            char* id = strtok(line, "|"); // strtok means terminating c strings
            // Skip name, address, etc., read until status
            char* token = NULL;
            char lastToken[50] = "";
            while ((token = strtok(NULL, "|")) != NULL) {
                strcpy(lastToken, token);
            }
          cutReturn(lastToken); //from task 2 apaprently
            if (id != NULL) {
                parcels[totalParcels] = Parcel(id, lastToken);
                totalParcels++;
            }
        }
    }
    int choice;

    do
    {
        cout << "\n================================";
        cout << "\n PARCEL ACTIVITY MANAGEMENT";
        cout << "\n================================";
        cout << "\n1. Display Recorded Activity";
        cout << "\n2. Display Parcels history?";
        cout << "\n3. Review Parcel";
        cout << "\n4. Undo Last Activity";
        cout << "\n5. Exit";
        cout << "\nChoice : ";

        cin >> choice;

        switch(choice)
        {
            //MAYbe redundant now this is all from the task 1,2,3,5
        case 1:
        {
            loadActivitiesFromFile(); 
            break;
        }

        case 2:{
           /*manager.displayAllActivities();
           string id;

            cout << "\nParcel ID : ";
            cin >> id;

            manager.displayParcelHistory(id);*/
           break;
        }
        case 3:
        {
            //necessary or can taken from the other tasks to review history is take from parcel.txt?  but not working-
            string id;

            cout << "\nParcel ID : ";
            cin >> id;

            manager.reviewParcel(id);

            //to indidvidualised from the file in question
            break;
        }

        case 4:
        {
            //compulsory without question
        //need integrating
            manager.undoActivity();
            
            break;
        }

        case 5:
            cout << "\nProgram Terminated.\n";
            break;

        default:
            cout << "\nInvalid Choice.\n";
        }

    } while(choice != 5);

}
//operation of stacks, but solve issues related..
// display history can be done with stack to display(peek operation)
// to undo a action/history use a POP 
// Record parcel acvitivty use the PUSH
//Retrieve and review ( ig push and pop or display from stack/peek )
// Empty or nullcases use isEmpty() function for stack -- also error handling in stack research on. 