#include <iostream>
#include <string>
#include<fstream>
#include "History.hpp"
#include "Task1.hpp"
#include "ResourceManager.hpp"
#include "Task2.hpp"
#include "ResourceAllocation.hpp"
#include "Activityrecordmanager.hpp"
#include "Task5.hpp"

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



// Task 2 menu 
void task2Menu() {
    int choice;

    do {

        cout << "\n===== Task 2: Service Request Management =====" << endl;
        cout << "1. Create service request" << endl;
        cout << "2. Display active requests" << endl;
        cout << "3. Show next request" << endl;
        cout << "4. Process next request" << endl;
        cout << "5. Process selected request by ID" << endl;
        cout << "6. Cancel request by ID" << endl;
        cout << "7. Search request" << endl;
        cout << "8. Display finished requests" << endl;
        cout << "0. Back" << endl;
        cout << "Choose an option (0-8): ";
        choice = readChoice();

        switch (choice) {
            case 1: createRequest();           break;
            case 2: displayActiveRequests();   break;
            case 3: showNextRequest();         break;
            case 4: nextRequest();      break;
            case 5: selectRequest();       break;
            case 6: cancelRequest();       break;
            case 7: searchRequest();           break;
            case 8: displayFinishRequests(); break;
            case 0: break;
            default: cout << "Invalid choice, try again." << endl;
        }
    } while (choice != 0);
}

//main meny 
int main(){
int choice;
loadRequests();   // task 2, read servicerequest.txt when program starts
syncWithRequests();   // task 3, match resource.txt with the requests

// task 4 , 1, 3, 5 read when prigram starts

    do {
        cout << "\n===== Product Smart Management System main menu =====" <<endl;
        cout << "1. Go to task 1" <<endl;
        cout << "2. Go to task 2" <<endl;
        cout << "3. Go to task 3" <<endl;
        cout << "4. Go to task 4" <<endl;
        cout << "5. Go to task 5" <<endl;
        cout << "0. Exit System" <<endl;
        cout << "Choose an option (0-5): ";
        choice = readChoice();

        switch (choice) {
            case 1: {
                Task1ParcelManager parcelManager;
                if (!parcelManager.load()) {
                    cout << "Could not load parcel.txt. Check its header and pipe-delimited parcel records." << endl;
                } else {
                    parcelManager.menu();
                }
                break;
            }
//add error handling
            case 2:
                task2Menu();
                break;

            case 3:
                  task3Menu();
               break; 
            case 4:
                task4menu();
                break;

            case 5:
                {
                    Task5ParcelManager categories;
                    categories.menu();
                }
                break;
            case 0: break;

            default: 
            cout << "Invalid choice, please enter a value between 0 - 5" <<endl;
//error handling messages
        }
    } while (choice != 0);
    saveRequests();   // task 2, write servicerequest.txt before exit
    //before exit save each tasks 1,3,4,5

    return 0;
}