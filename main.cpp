#include <iostream>
#include <string>
#include<fstream>
#include "Activityrecordmanager.hpp"
#include "History.hpp"
#include "Task2.hpp"

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


// ARRAY PART 
int main(){
int choice;
ServiceRequest h;
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
            case 1: //improve by writing to a file
            
           

                cout << "Enter Previous Status (e.g., Registered): ";
              //  cin >> prevStatus;
               /* if(prevStatus == ""){
                    cout << "Previous syayis cannot be empty";
                }*/

                cout << "Enter New Status (e.g., Dispatched): ";
                //cin >> newStatus;
                cout << "Is Reversible? (1 for Yes, 0 for No): ";
             //   cin >> reversibleInt;
               // module.recordActivity(parcelId, desc, prevStatus, newStatus, (reversibleInt != 0));
                break;
//add error handling
            case 2:
        task2Menu();
                break;

            case 3:
               // showDataset(datasetC, "Dataset C details", "dataset3facility_c.csv");
                break; 
            case 4:
                
                break;

            case 5:
                
            case 0: break;

            default: 
            cout << "Invalid choice, please enter a value between 0 - 5" <<endl;
//error handling messages
        }
    } while (choice != 0);
    return 0;
}