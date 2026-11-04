#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
using namespace std;
/* Simple Stack Program Example Using Functions in C++*/
/* Data Structure Programs,Stack Programs,C++ Examples */
#define MAX_SIZE 5


int item, choice, i;
int arr_stack[MAX_SIZE];
int top = 0;
int exit_p = 1;

void push() {
    if (top == MAX_SIZE)
        cout << "\n## Stack is Full!";
    else {
        cout << "\nEnter The Value to be pushed : ";
        cin>>item;
        cout << "\n## Position : " << top << ", Pushed Value  :" << item;
        arr_stack[top++] = item;
    }
}

void pop() {
    if (top == 0)
        cout << "\n## Stack is Empty!";
    else {
        --top;
        cout << "\n## Position : " << top << ", Popped Value  :" << arr_stack[top];
    }
}

void display() {
    cout << "\n## Stack Size : " << top;
    for (i = (top - 1); i >= 0; i--)
        cout << "\n## Position : " << i << ", Value  :" << arr_stack[i];
}


//1042am--1110am(PSeni)--1142-1200(TBhgia-30min)--work()
//staff module to record staff activity. 
void read(){

cout << "\nSimple Stack Example - Array and Functions - C++";
    do {
        cout << "\n\nStack Main Menu";

        cout << "\n1.Push \n2.Pop \n3.Display \nOthers to exit";
        cout << "\nEnter Your Choice : ";
        cin>>choice;
        switch (choice) {
            case 1:
                push();
                break;
            case 2:
                pop();
                break;
            case 3:
                display();
                break;
            default:
                exit_p = 0;
                break;
        }
    } while (exit_p);

}

template <typename T, size_t Capacity = 100>
class SafeStack {
private:
    T data[Capacity];
    int topIndex;

public:
    SafeStack() : topIndex(-1) {}

    bool empty() const { return topIndex < 0; }
    bool full() const { return topIndex >= static_cast<int>(Capacity) - 1; }

    // Returns true if successful, false if overflow occurs
    bool push(const T& value) {
        if (full()) {
            return false; // Error handled silently or logged
        }
        data[++topIndex] = value;
        return true;
    }

    // Returns true if successful, false if underflow occurs
    bool pop(T& removedValue) {
        if (empty()) {
            return false; 
        }
        removedValue = data[topIndex--];
        return true;
    }
};

void part() { // this is error handling to use in parcel
    SafeStack<int, 2> stack;

    if (!stack.push(50)) {
        std::cout << "Push failed: Stack is full.\n";
    }

    int val;
    if (!stack.pop(val)) {
        std::cout << "Pop failed: Stack is empty.\n";
    } else {
        std::cout << "Popped: " << val << "\n";
    }

}
