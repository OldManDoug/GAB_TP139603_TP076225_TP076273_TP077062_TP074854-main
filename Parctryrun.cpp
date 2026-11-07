#include <iostream>
#include <string>
#include <fstream> 
#include <sstream>
//files

using namespace std;
// the PLAN 
/*
Maintains a history from processing module - 
review by staff(need their input from a file storing names)-- what happen to a parcel
[check for a bad case]
reverse a decision[use an if else])
1. Record Parcel-Related Activities 
Record important activities performed during parcel processing, with sufficient information to 
identify the parcel, activity2 and relevant time or sequence. -- when array storr
2. Display Activity2 History 
Allow staff to display recent activities and the activity2 history for a selected parcel, including 
the order in which activities occurred. 
3. Retrieve and Review Previous Activities 
Allow staff to select a parcel and review previous activities, including processing sequence, 
previous status, most recent activity2 and relevant request or resource information. 
4. Reverse or Undo an Appropriate Recent Activity2 
confirm 
Provide a mechanism for reversing an appropriate recent activity2 where permitted. Identify the 
activity, 
eligibility, 
restore 
relevant 
parcel/request/resource status where necessary. 
5. Handle Activity2-History Exceptions 
information 
and 
update 
related 
Handle no activity2 history, invalid Parcel ID, an activity2 that cannot be reversed, an activity2 
already reversed, invalid activity2 selection, or an operation that would result in an invalid parcel 
status.

- concepts apply: undoing, pop and push(redo and undo)
- use array programming is easier than linky list. 
---- USE STACK PROGRAMMING FOR THE MODULE--- from AI suggestible 

---- BUT THIS MODULE MUST INTEGRATE WITH ALL 5 TASKS/MODULES... Activity2.txt should be from task 2 create request should display

*/
// https://codesignal.com/learn/courses/fundamental-data-structures-stacks-and-queues-in-cpp/lessons/understanding-and-implementing-stacks-in-cpp

/*Rationale - the stack is most suitable because it has a condition that require stack */
/*PARCEL CLASS for parcels*/

class Parcel {
private:
    string parcelID;
    string status;

public:

    Parcel() { // set as null because non argument cosntructor
        parcelID = "";
        status = "";
    }

    Parcel(string id, string s) {
        parcelID = id;
        status = s; //parameterised construstor
    }

    string getParcelID() {
        return parcelID;
    }

    string getStatus() {
        return status;
    }

    void setStatus(string s) {
        status = s;
    }

    void display() {
        //output from the file 
        string line;
ifstream f("parcel.txt"); //to read from is if to write to is out
if(!f.is_open()){
    cerr << "Cannot read the file specified";
}
while(getline(f,line)){
        cout << "Parcel ID: "
             << parcelID
             << "\nStatus: "
             << status
             << "\n";
    }
    f.close();
}
};

/*=========================
  PARCEL LINKED LIST
=========================*/

class ParcelNode {
public:
    Parcel parcel;
    ParcelNode* next;

    ParcelNode(Parcel p) {
        parcel = p;
        next = NULL;
    }
};

class ParcelList {
private:
    ParcelNode* head;

public:

    ParcelList() {
        head = NULL;
    }

    void addParcel(Parcel p) {

        ParcelNode* newNode =
            new ParcelNode(p);

        newNode->next = head;
        head = newNode;
    }

    Parcel* findParcel(string parcelID) {

        ParcelNode* current = head;

        while(current != NULL) {

            if(current->parcel.getParcelID()
                    == parcelID) {

                return &current->parcel;
            }

            current = current->next;
        }

        return NULL;
    }
    //save into a file
    void saveParcelsToFile()
{
    ofstream file("parcel.txt");

    ParcelNode* current = head;

    while(current != NULL)
    {
        file
            << current->parcel.getParcelID()
            << ","
            << current->parcel.getStatus()
            << endl;

        current = current->next;
    }

    file.close();
}
//https://www.geeksforgeeks.org/cpp/stack-and-queue-c-cpp-programs/
    void loadParcelsFromFile()
{
    ifstream file("parcels.txt");

    if(!file)
        return;

    string line;

    while(getline(file,line))
    {
        stringstream ss(line);

        string id;
        string status;

        getline(ss,id,',');

        getline(ss,status);

        addParcel(
            Parcel(id,status)
        );
    }

    file.close();
}
};

/*=========================
  ACTIVITY2 CLASS
=========================*/

class Activity2 {
public:

    int activity2ID;

    string parcelID;

    string activity2Type;

    string previousStatus;

    string newStatus;

    bool reversible;

    bool reversed;

    Activity2* next;

    Activity2() {
        next = NULL;
        reversed = false;
    }
};

/*=record a history but his needs integrate with*/

class Activity2HistoryStack {
private:

    Activity2* top;

public:

    Activity2HistoryStack() {
        top = NULL;
    }

    bool isEmpty() {
        return top == NULL;
    }

    void push(Activity2* activity2) {

        activity2->next = top;
        top = activity2;
    }

    Activity2* peek() {
        return top;
    }
};

/*=========================
  ACTIVITY2 MANAGER
=========================*/

class Activity2Manager {
private:

    Activity2HistoryStack history;

    ParcelList* parcelList;

    int activity2Counter;

public:

    Activity2Manager(ParcelList* pList) {

        parcelList = pList;

        activity2Counter = 1;
    }

    void updateParcelStatus(
            string parcelID,
            string newStatus) {
//rewrite back to the file
        Parcel* parcel =
                parcelList->findParcel(parcelID);

        if(parcel == NULL) {

            cout << "Error: Invalid Parcel ID\n";
            return;
        }

        string oldStatus =
            parcel->getStatus();

        if(oldStatus == newStatus) {

            cout << "Status unchanged.\n";
            return;
        }

        parcel->setStatus(newStatus);

        Activity2* activity2 =
            new Activity2();

        activity2->activity2ID =
            activity2Counter++;

        activity2->parcelID =
            parcelID;

        activity2->activity2Type =
            "Status Update";

        activity2->previousStatus =
            oldStatus;

        activity2->newStatus =
            newStatus;

        activity2->reversible =
            true;

        history.push(activity2);

        cout << "Status updated successfully.\n";
    }
//https://www.geeksforgeeks.org/cpp/stack-implementation-in-cpp/
    void displayRecentActivities() {
//from a file as well, maybe history.txt
        if(history.isEmpty()) {

            cout << "No activity2 history.\n";
            return;
        }

        Activity2* current =
            history.peek();

        cout << "\n=== Recent Activities ===\n";

        while(current != NULL) {

            cout
                << "Activity ID: "
                << current->activity2ID
                << "\nParcel: "
                << current->parcelID
                << "\nType: "
                << current->activity2Type
                << "\nStatus: "
                << current->previousStatus
                << " -> "
                << current->newStatus
                << "\nReversed: "
                << (current->reversed ?
                   "Yes" : "No")
                << "\n\n";

            current = current->next;
        }
    }

    void displayParcelHistory(
            string parcelID) {
//use same file to check but make one first
        if(history.isEmpty()) {

            cout << "No activity2 history.\n";
            return;
        }

        bool found = false;

        Activity2* current =
            history.peek();

        cout << "\n=== Parcel History ===\n";

        while(current != NULL) {

            if(current->parcelID
                    == parcelID) {

                found = true;

                cout
                    << "Activity ID: "
                    << current->activity2ID
                    << "\nType: "
                    << current->activity2Type
                    << "\nPrevious Status: "
                    << current->previousStatus
                    << "\nNew Status: "
                    << current->newStatus
                    << "\n\n";
            }

            current = current->next;
        }

        if(!found) {

            cout
            << "Invalid Parcel ID "
            << "or no history found.\n";
        }
    }

    void reviewParcel(
            string parcelID) {
//https://www.cpp.thiyagaraaj.com/data-structures-in-c/stack-programs/simple-stack-program-in-c-programming
        Parcel* parcel =
            parcelList->findParcel(parcelID);

        if(parcel == NULL) {

            cout
            << "Invalid Parcel ID.\n";

            return;
        }

        cout << "\n=== Parcel Information ===\n";

        parcel->display();

        cout << "\n=== Activities ===\n";

        displayParcelHistory(parcelID);
    }

    void undoActivity(
            int activity2ID) {

        if(history.isEmpty()) {

            cout
            << "No activity2 history.\n";

            return;
        }

        Activity2* current =
            history.peek();

        while(current != NULL) {

            if(current->activity2ID
                    == activity2ID) {

                if(current->reversed) {

                    cout
                    << "Activity already reversed.\n";

                    return;
                }

                if(!current->reversible) {

                    cout
                    << "Activity cannot be reversed.\n";

                    return;
                }

                Parcel* parcel =
                    parcelList->findParcel(
                        current->parcelID);

                if(parcel == NULL) {

                    cout
                    << "Parcel not found.\n";

                    return;
                }

                if(parcel->getStatus()
                        != current->newStatus) {

                    cout
                    << "Undo would create "
                    << "invalid parcel state.\n";

                    return;
                }
                //error handling up top, so here will be rewrite to file 

                parcel->setStatus(
                    current->previousStatus);

                current->reversed = true;

                cout
                << "Activity reversed.\n";

                cout
                << "Parcel "
                << parcel->getParcelID()
                << " restored to status: "
                << current->previousStatus
                << "\n";

                return;
            }

            current = current->next;
        }

        cout << "Invalid activity2 selection.\n";
    }
    //in file activities
    void saveActivitiesToFile()
{
    ofstream file("activity.txt");

    Activity2* current =
        history.peek();

    while(current != NULL)
    {
        file
          << current->activity2ID << ","
          << current->parcelID << ","
          << current->activity2Type << ","
          << current->previousStatus << ","
          << current->newStatus << ","
          << current->reversible << ","
          << current->reversed
          << endl;

        current =
            current->next;
    }

    file.close();
}
//to display 
void loadActivitiesFromFile()
{
    ifstream file("activities.txt");

    if(!file)
        return;

    string line;

    while(getline(file,line))
    {
        stringstream ss(line);

        Activity2* activity2 =
            new Activity2();

        string temp;

        getline(ss,temp,',');
        activity2->activity2ID =
            stoi(temp);

        getline(ss,
                activity2->parcelID,
                ',');

        getline(ss,
                activity2->activity2Type,
                ',');

        getline(ss,
                activity2->previousStatus,
                ',');

        getline(ss,
                activity2->newStatus,
                ',');

        getline(ss,temp,',');
        activity2->reversible =
            stoi(temp);

        getline(ss,temp);
        activity2->reversed =
            stoi(temp);

        history.push(activity2);

        if(activity2->activity2ID
            >= activity2Counter)
        {
            activity2Counter =
                activity2->activity2ID + 1;
        }
    }

    file.close();
}
};

/*=========================
  MENU
=========================*/

int main() {

    ParcelList parcelList;

parcelList.loadParcelsFromFile();

Activity2Manager manager(
    &parcelList);

manager.loadActivitiesFromFile();
    int choice;

    do {

        cout << "\n";
        cout << "========================\n";
        cout << " PARCEL ACTIVITY2 HANDLING\n";
        cout << "========================\n";
        cout << "1. Add Parcel\n";
        cout << "2. Update Parcel Status\n";
        cout << "3. Display Parcels\n";
        cout << "4. Display Activities\n";
        cout << "5. Display Parcel History\n";
        cout << "6. Review Parcel\n";
        cout << "7. Undo Activity2\n";
        cout << "0. Return to Main\n";
        cout << "Choice: ";

        cin >> choice;

        switch(choice) {

        case 1: {
//file handling above just call function? 
            string id;
            string status;

            cout << "Parcel ID: ";
            cin >> id;
//ask for input 
            cout << "Initial Status: ";
            cin >> status;
//when input done it go to file in addParcel
            parcelList.addParcel( Parcel(id, status));

            cout << "Parcel added.\n";

            break;
        }

        case 2: {
//same concept to update if possible as 1.
            string id;
            string status;

            cout << "Parcel ID: ";
            cin >> id;

            cout << "New Status: ";
            cin >> status;
//can write to file here direct instead of put in a menu
            manager.updateParcelStatus(id,status);
            //drop file
parcelList.saveParcelsToFile();
manager.saveActivitiesToFile();
            break;
        }

        case 3: {

         //   parcelList.displayParcels();
parcelList.loadParcelsFromFile(); 
            break;
        }

        case 4: {

            manager.displayRecentActivities();

            break;
        }

        case 5: {

            string id;

            cout << "Parcel ID: ";
            cin >> id;

            manager.displayParcelHistory(id);

            break;
        }

        case 6: {
//to review base around id 
            string id;

            cout << "Parcel ID: ";
            cin >> id;

            manager.reviewParcel(id);

            break;
        }

        case 7: {

            int id;

            cout << "Activity ID: ";
            cin >> id;
//find the activity2 it wants to stop or undo only 
            manager.undoActivity(id);
//drop file
            parcelList.saveParcelsToFile();
manager.saveActivitiesToFile();
            break;
        }

        case 0:
parcelList.saveParcelsToFile();

manager.saveActivitiesToFile();
//ensure its saved even 0n closing or force restart
            cout << "Thanks for using.\n";
            //call the main menu function returns
            break;

        default:

            cout << "Invalid choice.\n";
        }

    } while(choice != 0);

    return 0;
}
// talk with supervisor can i take a company project from your here back to uni for FYP then do it and then give back to the company? 