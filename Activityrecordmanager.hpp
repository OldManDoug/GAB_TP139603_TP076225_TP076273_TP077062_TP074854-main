#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;
// the PLAN 
/*
Maintains a history from processing module - 
review by staff(need their input from a file storing names)-- what happen to a parcel
[check for a bad case]
reverse a decision[use an if else])
1. Record Parcel-Related Activities 
Record important activities performed during parcel processing, with sufficient information to 
identify the parcel, activity and relevant time or sequence. 
2. Display Activity History 
Allow staff to display recent activities and the activity history for a selected parcel, including 
the order in which activities occurred. 
3. Retrieve and Review Previous Activities 
Allow staff to select a parcel and review previous activities, including processing sequence, 
previous status, most recent activity and relevant request or resource information. 
4. Reverse or Undo an Appropriate Recent Activity 
confirm 
Provide a mechanism for reversing an appropriate recent activity where permitted. Identify the 
activity, 
eligibility, 
restore 
relevant 
parcel/request/resource status where necessary. 
5. Handle Activity-History Exceptions 
information 
and 
update 
related 
Handle no activity history, invalid Parcel ID, an activity that cannot be reversed, an activity 
already reversed, invalid activity selection, or an operation that would result in an invalid parcel 
status.

- concepts apply: undoing, pop and push(redo and undo)
- use array programming is easier than linky list. 
---- USE STACK PROGRAMMING FOR THE MODULE--- from AI suggestible 
*/
// https://www.geeksforgeeks.org/cpp/stack-implementation-in-cpp/
// Define Stack class
class Stack {
private:
    // Index of the top element in the stack
    int top;
    // Array to store stack elements, with a capacity of 100
    // elements, which are files 
    int arr[100];

public:
    // Constructor to initialize an empty stack
    Stack() { top = -1; }

    // Function to add an element x to the top of the stack
    void push(int x)
    {
        // If the stack is full, print "Stack overflow" and
        // return
        if (top >= 99) {
            cout << "Stack overflow" << endl;
            return;
        }
        // Add element to the top of the stack and increment
        // top
        arr[++top] = x;
        cout << "Pushed " << x << " to stack\n";
    }

    // Function to remove the top element from the stack
    int pop()
    {
        // If the stack is empty, print "Stack underflow"
        // and return 0
        if (top < 0) {
            cout << "Stack underflow" << endl;
            return 0;
        }
        // Remove the top element from the stack and
        // decrement top
        return arr[top--];
    }

    // Function to return the top element of the stack
    int peek()
    {
        // If the stack is empty, print "Stack is empty" and
        // return 0
        if (top < 0) {
            cout << "Stack is empty" << endl;
            return 0;
        }
        // Return the top element of the stack
        return arr[top];
    }

    // Function to check if the stack is empty
    bool isEmpty()
    {
        // Return true if the stack is empty (i.e., top is
        // -1)
        return (top < 0);
    }
};

void Stackdo(){
    // Create a stack
    Stack s;
    // take from files
    fstream("productdetail.txt");
    // Push elements into the stack
    s.push(10);
    s.push(20);
    s.push(30);
    // Print the top element of the stack
    cout << "Top element is: " << s.peek() << endl;
    // Print all elements in the stack
    cout << "Elements present in stack : ";
    // While the stack is not empty
    while (!s.isEmpty()) {
        // Pop the top element from the stack and print it
        cout << s.pop() << " ";
    }
}
// https://www.cpp.thiyagaraaj.com/data-structures-in-c/stack-programs/simple-stack-program-example-using-class-in-c

//another stack for history checking