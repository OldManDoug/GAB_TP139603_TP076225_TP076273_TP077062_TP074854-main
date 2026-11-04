#include <iostream>
#include <string>
#include<fstream>
#include "Activityrecordmanager.hpp"
#include "History.hpp"
#include "ServiceRequest.hpp"

using namespace std;
//MENU HELPERS
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

// Choose task
int main(){
int choice;
    loadRequests();     
    do {
        cout << "\n=====  Smart Campus Parcel Collection and Delivery Management System =====" <<endl;
        cout << "1. Task 1 - Parcel Intake, Sorting and Processing Module" <<endl;
        cout << "2. Task 2 - Parcel Service and Collection Management Module" <<endl;
        cout << "3. Task 3 - Collection Counter and Delivery Resource Management Module" <<endl;
        cout << "4. Task 4 - Parcel Activity and Record Management Module" <<endl;
        cout << "5. Task 5 - Parcel Category Organisation Module" <<endl;
        cout << "0. Exit" <<endl;
        cout << "Choose an option (0-5): ";
        choice = readChoice();

        switch (choice) {
            case 1:
           //  Stackdo();
           read();
                break;
            case 2:
                task2Menu();
                break;
            case 3:
               // showDataset(datasetC, "Dataset C details", "dataset3facility_c.csv");
                break;
            case 4:

                break;

            case 5:

                break;

            case 0: break;

            default:
            cout << "Invalid choice, try again." <<endl;

        }
    } while (choice != 0);
    saveRequests();      // Task 2: write servicerequest.txt when the program exits
    return 0;
}

// TASK 2 MENU - Parcel Service and Collection Management Module
void task2Menu() {
    int choice;
    do {
        cout << "\n===== Task 2: Service Request Management =====" << endl;
        cout << "1. Create service request" << endl;
        cout << "2. Display active requests" << endl;
        cout << "3. Show next request" << endl;
        cout << "4. Process next request" << endl;
        cout << "5. Cancel request by ID" << endl;
        cout << "6. Search request" << endl;
        cout << "7. Display finished requests" << endl;
        cout << "0. Back" << endl;
        cout << "Choose an option (0-7): ";
        choice = readChoice();

        switch (choice) {
            case 1: createRequest();           break;
            case 2: displayActiveRequests();   break;
            case 3: showNextRequest();         break;
            case 4: processNextRequest();      break;
            case 5: cancelRequestById();       break;
            case 6: searchRequest();           break;
            case 7: displayFinishedRequests(); break;
            case 0: break;
            default: cout << "Invalid choice, try again." << endl;
        }
    } while (choice != 0);
}
