#include <iostream>
#include <string>

using namespace std;

/*=========================
  PARCEL CLASS
=========================*/

class Parcel {
private:
    string parcelID;
    string status;

public:

    Parcel() {
        parcelID = "";
        status = "";
    }

    Parcel(string id, string s) {
        parcelID = id;
        status = s;
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
        cout << "Parcel ID: "
             << parcelID
             << "\nStatus: "
             << status
             << "\n";
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

    void displayParcels() {

        if(head == NULL) {
            cout << "No parcels found.\n";
            return;
        }

        ParcelNode* current = head;

        while(current != NULL) {

            current->parcel.display();
            cout << "------------------\n";

            current = current->next;
        }
    }
};

/*=========================
  ACTIVITY CLASS
=========================*/

class Activity {
public:

    int activityID;

    string parcelID;

    string activityType;

    string previousStatus;

    string newStatus;

    bool reversible;

    bool reversed;

    Activity* next;

    Activity() {
        next = NULL;
        reversed = false;
    }
};

/*=========================
  CUSTOM STACK
=========================*/

class ActivityHistoryStack {
private:

    Activity* top;

public:

    ActivityHistoryStack() {
        top = NULL;
    }

    bool isEmpty() {
        return top == NULL;
    }

    void push(Activity* activity) {

        activity->next = top;
        top = activity;
    }

    Activity* peek() {
        return top;
    }
};

/*=========================
  ACTIVITY MANAGER
=========================*/

class ActivityManager {
private:

    ActivityHistoryStack history;

    ParcelList* parcelList;

    int activityCounter;

public:

    ActivityManager(ParcelList* pList) {

        parcelList = pList;

        activityCounter = 1;
    }

    void updateParcelStatus(
            string parcelID,
            string newStatus) {

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

        Activity* activity =
            new Activity();

        activity->activityID =
            activityCounter++;

        activity->parcelID =
            parcelID;

        activity->activityType =
            "Status Update";

        activity->previousStatus =
            oldStatus;

        activity->newStatus =
            newStatus;

        activity->reversible =
            true;

        history.push(activity);

        cout << "Status updated successfully.\n";
    }

    void displayRecentActivities() {

        if(history.isEmpty()) {

            cout << "No activity history.\n";
            return;
        }

        Activity* current =
            history.peek();

        cout << "\n=== Recent Activities ===\n";

        while(current != NULL) {

            cout
                << "Activity ID: "
                << current->activityID
                << "\nParcel: "
                << current->parcelID
                << "\nType: "
                << current->activityType
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

        if(history.isEmpty()) {

            cout << "No activity history.\n";
            return;
        }

        bool found = false;

        Activity* current =
            history.peek();

        cout << "\n=== Parcel History ===\n";

        while(current != NULL) {

            if(current->parcelID
                    == parcelID) {

                found = true;

                cout
                    << "Activity ID: "
                    << current->activityID
                    << "\nType: "
                    << current->activityType
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
            int activityID) {

        if(history.isEmpty()) {

            cout
            << "No activity history.\n";

            return;
        }

        Activity* current =
            history.peek();

        while(current != NULL) {

            if(current->activityID
                    == activityID) {

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

        cout
        << "Invalid activity selection.\n";
    }
};

/*=========================
  MENU
=========================*/

int main() {

    ParcelList parcelList;

    ActivityManager manager(
        &parcelList);

    int choice;

    do {

        cout << "\n";
        cout << "========================\n";
        cout << " PARCEL ACTIVITY SYSTEM\n";
        cout << "========================\n";
        cout << "1. Add Parcel\n";
        cout << "2. Update Parcel Status\n";
        cout << "3. Display Parcels\n";
        cout << "4. Display Activities\n";
        cout << "5. Display Parcel History\n";
        cout << "6. Review Parcel\n";
        cout << "7. Undo Activity\n";
        cout << "0. Exit\n";
        cout << "Choice: ";

        cin >> choice;

        switch(choice) {

        case 1: {

            string id;
            string status;

            cout << "Parcel ID: ";
            cin >> id;

            cout << "Initial Status: ";
            cin >> status;

            parcelList.addParcel(
                Parcel(id, status));

            cout
            << "Parcel added.\n";

            break;
        }

        case 2: {

            string id;
            string status;

            cout << "Parcel ID: ";
            cin >> id;

            cout << "New Status: ";
            cin >> status;

            manager.updateParcelStatus(
                id,
                status);

            break;
        }

        case 3: {

            parcelList.displayParcels();

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

            manager.undoActivity(id);

            break;
        }

        case 0:

            cout << "System Closed.\n";
            break;

        default:

            cout << "Invalid choice.\n";
        }

    } while(choice != 0);

    return 0;
}