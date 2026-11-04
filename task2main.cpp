#include <iostream>
#include <string>
#include<fstream>
#include "Activityrecordmanager.hpp"
#include "History.hpp"

using namespace std;
//MENU HELPERS6
// readChoice - read one menu number; returns -1 if the input is not a number
int readChoice() {
    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return -1;
    }
    cin.ignore(10000, '\n');
    return choice;
}

 

// ==========================================
// Interactive Console Menu Driver
// ==========================================
int main() {
   
    int choice;
    char parcelId[30];
    char desc[100];
    char prevStatus[30];
    char newStatus[30];
    int reversibleInt;
    int actId;

    do {
        cout << "\n========================================\n";
        cout << "   PARCEL ACTIVITY MODULE (NO STL)      \n";
        cout << "========================================\n";
        cout << "1. Record Activity\n";
        cout << "2. Display Activity History\n";
        cout << "3. Review Specific Activity Details\n";
        std::cout << "4. Reverse/Undo Most Recent Activity\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice (1-5): ";
        std::cin >> choice;

        switch (choice) {
            case 1: //improve by writing to a file
            do{
                cout << "Enter Parcel ID: ";
                cin >> parcelId;
            }while(parcelId != 0);
            do{
                cout << "Enter Activity Description: ";
                cin.ignore();
                cin.getline(desc, 100);
                if(desc == ""){
cout << "Please do not description as blank";
                }}while(desc != "");

                cout << "Enter Previous Status (e.g., Registered): ";
                cin >> prevStatus;
                if(prevStatus == ""){
                    cout << "Previous syayis cannot be empty";
                }

                cout << "Enter New Status (e.g., Dispatched): ";
                cin >> newStatus;
                cout << "Is Reversible? (1 for Yes, 0 for No): ";
                cin >> reversibleInt;
               // module.recordActivity(parcelId, desc, prevStatus, newStatus, (reversibleInt != 0));
                break;
//add error handling
            case 2:
                cout << "Enter Parcel ID to display history: ";
                cin >> parcelId;
               // module.displayHistory(parcelId);
                part(); //can use the push part here. 
                break;

            case 3:
                cout << "Enter Parcel ID: ";
                cin >> parcelId;
                cout << "Enter Activity ID to review: ";
                cin >> actId;
                //module.reviewActivity(parcelId, actId);
                break;

            case 4:
            cout << "Enter Parcel ID to reverse last activity: ";
            cin >> parcelId;
               // module.reverseRecentActivity(parcelId);
                break;

            case 5:
                std::cout << "Exiting program. Goodbye!\n";
                break;

            default:
                std::cout << "[Exception] Invalid menu selection. Please choose between 1 and 5.\n";
        }
    } while (choice != 5);

    return 0;
}


