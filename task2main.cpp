#include <iostream>
#include <string>
#include<fstream>
#include "Activityrecordmanager.hpp"
#include "History.hpp"
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

// ARRAY PART 
int main(){
int choice;
    do {
        cout << "\n===== Product Smart Management System =====" <<endl;
        cout << "1. Display Dataset A" <<endl;
        cout << "2. Display Dataset B" <<endl;
        cout << "3. Display Dataset C" <<endl;
        cout << "4. Age group and billing analysis" <<endl;
        cout << "5. Healthcare expenditure analysis" <<endl;
        cout << "6. Sorting experiments (insertion sort vs merge sort)" <<endl;
        cout << "7. Searching experiments (unsorted vs sorted)" <<endl;
        cout << "8. Clinical insights" <<endl;
        cout << "0. Back" <<endl;
        cout << "Choose an option (0-8): ";
        choice = readChoice();

        switch (choice) {
            case 1:
           //  Stackdo();
           read();
                break;
            case 2:
               // showDataset(datasetB, "Dataset B details","dataset2facility_b.csv" );
                break;
            case 3:
               // showDataset(datasetC, "Dataset C details", "dataset3facility_c.csv");
                break; 
            case 4:
                
                break;

            case 5:
                

            case 6:
                
                break;

            case 7:
                
                break;

            case 8:
                
                break;

            case 0: break;

            default: 
            cout << "Invalid choice, try again." <<endl;

        }
    } while (choice != 0);
    return 0;
}


