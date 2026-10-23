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
identify the parcel, activity and relevant time or sequence. -- when array storr
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

So use a arrays, but each number represents a parcel content
*/
// https://codesignal.com/learn/courses/fundamental-data-structures-stacks-and-queues-in-cpp/lessons/understanding-and-implementing-stacks-in-cpp

// Define Stack class 

class Stack {
    int top;
    int* arr;
    int capacity;
public:
    Stack(int size) : capacity(size), top(-1) {
        arr = new int[capacity];
    }
    ~Stack() { delete[] arr; }
    void push(int x) {
        if (top == capacity - 1) {
            cout << "Stack overflow\n";
            return;
        }
        arr[++top] = x;
    }
    //https://www.geeksforgeeks.org/dsa/implement-stack-using-array/
    void pop() {
    if (top == 0)
        cout << "\n## Stack is Empty!";
    else {
        --top;
        cout << "\n## Position : " << top << ", Popped Value  :" << arr[top];
    }
} //https://www.cpp.thiyagaraaj.com/data-structures-in-c/stack-programs/simple-stack-program-using-functions-in-c-programming
      
        
    
    int peek() {
        if (top == -1) {
            cout << "Stack is empty\n";
            return -1;
        }
        return arr[top];
    }
    bool isEmpty() { return top == -1; }
};
//operation of stacks, but solve issues related..
// display history can be done with stack to display(peek operation)
// to undo a action/history use a POP 
// Record parcel acvitivty use the PUSH
//Retrieve and review ( ig push and pop or display from stack/peek )
// Empty or nullcases use isEmpty() function for stack -- also error handling in stack research on. 