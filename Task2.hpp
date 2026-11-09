// Parcel Service and Collection Management Module, priority queue array
//enqueue add to rear, dequeue search highest priority
#ifndef Task2_hpp
#define Task2_hpp
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "History.hpp"
using namespace std;

#define maxRequest 500

// service types, stored as numbers
const int collect = 1;
const int delivery = 2;
const int special = 3;
const int returnSender=4;

//request status, stored as numbers
const int pending=1;
const int assigned = 2;
const int completed = 3;
const int cancelled=4;
const int returned=5;

// priority rule
// special = 4, delivery = 3, return = 2, collection = 1, plus 4 if urgent
// same priority then the go earlier request , first one found in the scan

struct serviceRequest {
    char requestId[10];       // REQxxx
    char parcelId[10];        
    char recipientName[50];
    int  serviceType;         // collect, delivery, special or returnSender
    int  isUrgent;            
    int  priority;            
    char requestTime[20];     
    int  status;              // pending, assigned, completed, cancelled or returned
    char resourceId[10];      
    char remark[50];          
};

// Priority queue of the active requests
struct requestQueue {
    serviceRequest arr[maxRequest];
    int size;

    requestQueue(){
        size = 0;
    }
    bool isEmpty() {
        return size ==0;
    }

    bool isFull() {
        return size == maxRequest;
    }

    //add at the rear, no sorting here
    bool enqueue(serviceRequest req) {
        if(isFull()) {
            cout << "Request list is Full! Cannot add more requests."<< endl;
            return false;
        }
        arr[size] = req;
        size++;

        return true;

    }

    // Scan all and return the index of the highest priority request
    int findHighest() {
        int i, best;

        if(isEmpty()) {
            return -1;

        }
        best = 0;
        for (i=1 ; i<size; i++){
            if (arr[i].priority>arr[best].priority){
                best = i;
            }

        }
        return best;
    }

    // Find by request id, return the index or minus one if not found
    int findById(const char* id) {
        int i;
        for(i=0; i < size; i++) {
            if (strcmp(arr[i].requestId, id)==0) {
                return i;
            }
        }
        return -1;
    }

    // Remove the request at index and shift the rest one step left
    serviceRequest removeAt(int index) {
        int i;
        serviceRequest removed = arr[index];

        for (i = index ; i<size-1;i++){
            arr[i] = arr[i+1];
        }
        size--;
        return removed;
    }
};

requestQueue activeRequests;
serviceRequest finishedRequests[maxRequest];  
int finishedCount = 0;
int nextRequestNo = 1;                        

void saveRequests();


// small input helpers
void readText(char* text, int size){
    cin.getline(text,size);
    if (cin.fail()){
        cin.clear();
        cin.ignore(10000,'\n');
    }
}

int readNumber(){
    int n;
    if (!(cin>>n)) {
        cin.clear();
        cin.ignore(10000,'\n');
        return -1;
    }
    cin.ignore(10000,'\n');
    return n;
}

// Copy text so it never goes over the array size
void copyText(char* to,const char* from,int size) {
    strncpy(to, from, size-1);
    to[size-1] = '\0';
}

// Remove the \r 
void cutReturn(char* text) {
    int len = strlen(text);
    if(len>0 && text[len-1]=='\r'){
        text[len-1] = '\0';
    }
}

// helper functions
const char* service(int type){
    if (type == collect)
        return "Collection";
    if (type==delivery)
        return "Delivery";
    if(type == special)
        return "Special Handling";
    if (type == returnSender)
        return "Return to Sender";
    return "Unknown";
}

const char* status(int status) {
    if(status==pending)
        return "Pending";
    if(status==assigned)
        return "Assigned";
    if (status==completed)
        return "Completed";
    if(status==cancelled)
        return "Cancelled";
    if (status==returned)
        return "Returned";
    return "Unknown";
}

int calPriority(int serviceType, int isUrgent) {
    int score=0;
    if(serviceType == special)
        score = 4;
    else if (serviceType==delivery)
        score = 3;
    else if(serviceType == returnSender)
        score=2;
    else if (serviceType==collect)
        score=1;

    if (isUrgent == 1){
        score = score + 4;
    }
    return score;
}

// Check whether this service can be used at the parcel's current status
bool serviceAllowed(const char* parcelStatus,int type) {
    if (strcmp(parcelStatus,"Registered")==0)
        return true;

    if (strcmp(parcelStatus,"Prepared for Collection")==0)
        return type == collect;
    if(strcmp(parcelStatus, "Assigned for Delivery") == 0)
        return type == delivery;
    if (strcmp(parcelStatus,"Special Handling")==0)
        return type == special;
    if(strcmp(parcelStatus, "Return to Sender")==0)
        return type == returnSender;

    return false;
}


// current date and time
void timeNow(char* text) {
    time_t now = time(0);
    tm* t = localtime(&now);
    strftime(text, 20,"%Y-%m-%d %H:%M",t);
}


//one request in one line
void showRequest(serviceRequest r){
    const char* urgentText = "Normal";

    if( r.isUrgent == 1){
        urgentText = "Urgent";
    }
    cout << r.requestId << " | " << r.parcelId << " | " << r.recipientName << " | " << service(r.serviceType)
         << " | " << urgentText << " | Priority " << r.priority
         << " | " << status(r.status) << " | " << r.requestTime << endl;
}

//details of one request
void showDetails(serviceRequest r){
    const char* urgentText = "No";

    if (r.isUrgent == 1) {
        urgentText = "Yes";

    }
    cout << "Request ID    : " << r.requestId << endl;
    cout << "Parcel ID     : " << r.parcelId << endl;
    cout << "Recipient     : " << r.recipientName << endl;
    cout << "Service       : " << service(r.serviceType) << endl;
    cout << "Urgent        : " << urgentText << endl;
    cout << "Priority      : " << r.priority << endl;
    cout << "Request time  : " << r.requestTime << endl;

    cout << "Status        : " << status(r.status) << endl;
    cout << "Resource ID   : " << r.resourceId << endl;
    cout << "Remark        : " << r.remark << endl;

}

// main operations
void createRequest() {
    char parcelId[10];
    char name[50];
    char realName[50];

    char parcelStatus[30];
    char action[80];
    int type;
    int urgent;
    int i;

    if(activeRequests.isFull()){
        cout << "Request list is Full! Cannot create more requests."<< endl;
        return;
    }

    cout << "Enter Parcel ID: ";

    readText(parcelId, 10);
    if(strlen(parcelId)==0) {
        cout << "Parcel ID cannot be empty." << endl;
        return;
    }

    if(!parcelExists(parcelId)) {
        cout << "Invalid Parcel ID, parcel not found." << endl;
        return;
    }

    // parcel already finished so no more service needed
    getParcelStatus(parcelId,parcelStatus);
    if(strcmp(parcelStatus,"Collected")==0 || strcmp(parcelStatus,"Delivered") == 0 ||
       strcmp(parcelStatus,"Returned")==0 || strcmp(parcelStatus,"Cancelled") == 0 ||
       strcmp(parcelStatus,"Missing") == 0){
        cout << "Parcel is already " << parcelStatus << ", cannot create a request." << endl;
        return;
    }

    // one parcel only one active request
    for (i=0; i < activeRequests.size ; i++){
        if(strcmp(activeRequests.arr[i].parcelId,parcelId)==0){
            cout << "This parcel already has an active request (" << activeRequests.arr[i].requestId << ")." << endl;
            return;
        }
    }

    cout << "Enter recipient name: ";
    readText(name,50);
    getParcelRecipient(parcelId, realName);

    if(strcmp(name,realName) != 0){
        cout << "Recipient name does not match the parcel record." << endl;
        return;
    }

    cout << "\nService type" << endl;
    cout << "1. Collection\n2. Delivery\n3. Special Handling\n4. Return to Sender" << endl;
    cout << "Choose (1-4): ";
    type = readNumber();
    if (type<1 || type>4){
        cout << "Invalid service type." << endl;

        return;
    }
    if(!serviceAllowed(parcelStatus,type)){
        cout << "Parcel is " << parcelStatus << ", " << service(type) << " request is not allowed." << endl;
        return;
    }

    cout << "Is it urgent? (1 for Yes, 0 for No): ";
    urgent = readNumber();
    if (urgent!=0 && urgent!=1){
        cout << "Invalid urgent value." << endl;
        return;
    }

    serviceRequest req;
    sprintf(req.requestId,"REQ%03d", nextRequestNo);
    nextRequestNo++;
    copyText(req.parcelId, parcelId,10);
    copyText(req.recipientName,name,50);
    req.serviceType = type;
    req.isUrgent = urgent;
    req.priority=calPriority(type,urgent);
    timeNow(req.requestTime);
    req.status = pending;
    strcpy(req.resourceId,"-");
    strcpy(req.remark,"-");

    activeRequests.enqueue(req);
    cout << "Request " << req.requestId << " created for parcel " << req.parcelId << ", priority " << req.priority << "." << endl;
    saveRequests();

    // Task 4 history, the parcel status does not change when a request is made
    strcpy(action,"Request ");
    strcat(action,req.requestId);
    strcat(action," created (");

    strcat(action,service(req.serviceType));
    strcat(action,")");
    recordActivity(req.parcelId,action,parcelStatus,parcelStatus,false);
}

void displayActiveRequests() {
    int i;
    if(activeRequests.isEmpty()){
        cout << "No active requests." << endl;
        return;

    }
    cout << "\nActive requests (arrival order)" << endl;

    for(i=0; i <activeRequests.size ; i++) {
        showRequest(activeRequests.arr[i]);
    }
}

void showNextRequest(){
    int index = activeRequests.findHighest();
    if (index == -1) {
        cout << "No active requests." << endl;
        return;

    }
    cout << "\nNext request to handle" << endl;
    showDetails(activeRequests.arr[index]);
}

// Give the counter or delivery person back to Task 3 if the request had one
void freeResource(serviceRequest r){
    if (strcmp(r.resourceId,"-") != 0){
        releaseResource(r.resourceId);
    }

}

// Ask staff for the result then finish the request at this index
void finishRequest(int index){
    if (finishedCount>=maxRequest){
        cout << "Finished list is Full!" << endl;
        return;

    }

    showDetails(activeRequests.arr[index]);

    cout << "\n1. Completed\n2. Cancelled\n3. Returned to sender\n0. Back" << endl;
    cout << "Choose result (0-3): ";
    int result = readNumber();
    if(result==0) {
        return;
    }
    if(result<1||result> 3) {
        cout << "Invalid choice, nothing changed." << endl;
        return;
    }

    // completed or returned needs a resource first, cancel does not
    if(result != 2 && activeRequests.arr[index].status != assigned) {
        cout << "Request has no resource yet, assign one in Task 3 first." << endl;
        return;
    }

    serviceRequest req=activeRequests.arr[index];
    char newParcelStatus[30];
    char parcelStatus[30];
    char oldStatus[30];
    char action[80];
    char name[50];
    char realName[50];

    // only the real recipient can collect the parcel
   
    if(result == 1&&req.serviceType == collect){
        if(!getParcelRecipient(req.parcelId,realName)){
            cout << "Parcel record not found, request not finished." << endl;
            return;
        }
        cout << "Enter recipient name: ";
        readText(name, 50);

        if(strcmp(name, realName) != 0){
            cout << "Recipient name does not match, request not finished." << endl;
            return;
        }
        copyText(req.recipientName, realName,50);
    }


    if (result == 1) {
        req.status = completed;

        strcpy(req.remark, "Completed");
        if (req.serviceType == collect) {
            strcpy(newParcelStatus,"Collected");
        }
        else if (req.serviceType == delivery){
            strcpy(newParcelStatus,"Delivered");
        }
        else if (req.serviceType == special){
            strcpy(newParcelStatus, "Handled");
        }
        else{
            strcpy(newParcelStatus,"Returned");
        }
    }
    else if (result == 2) {
        req.status=cancelled;
        strcpy(req.remark, "Cancelled by staff");
        strcpy(newParcelStatus, "Registered");
    }
    else{
        req.status=returned;
        strcpy(req.remark,"Returned to sender");
        strcpy(newParcelStatus,"Returned");
    }

    // keep the old parcel status for the Task 4 history
    strcpy(oldStatus,"Unknown");
    getParcelStatus(req.parcelId,oldStatus);
    updateParcelStatus(req.parcelId,newParcelStatus);


    if (result != 2) {
        if (!getParcelStatus(req.parcelId, parcelStatus)) {
            cout << "Parcel record not found, request not finished." << endl;

            return;

        }
        if(strcmp(parcelStatus,newParcelStatus) != 0){
            cout << "Parcel is " << parcelStatus << ", cannot change to " << newParcelStatus << ". Request not finished." << endl;

            return;

        }
    }

    activeRequests.removeAt(index) ;
    finishedRequests[finishedCount]=req;
    finishedCount++;

    freeResource(req);
    cout << "Request " << req.requestId << " is now " << status(req.status) << "." << endl;
    saveRequests();

    // Task 4 history, not reversible because undo only puts the parcel back, not the request or resource
    if(!getParcelStatus(req.parcelId,parcelStatus))
        strcpy(parcelStatus,"Unknown");
    strcpy(action,"Request ") ;
    strcat(action,req.requestId);
    strcat(action," " );
    strcat(action,status(req.status)) ;

    recordActivity(req.parcelId,action,oldStatus, parcelStatus,false);
}

//process the request with the highest priority
void nextRequest(){
    if(activeRequests.isEmpty()){
        cout << "No active requests to process." << endl;
        return;
    }
    int index=activeRequests.findHighest();
    cout << "\nNext request to handle" << endl;
    finishRequest(index);


}

//staff select by request id
void selectRequest() {
    char id[10];
    int i;
    if(activeRequests.isEmpty()){
        cout << "No active requests to process." << endl;
        return;
    }

    cout << "Enter Request ID to process: ";
    readText(id, 10);
    int index=activeRequests.findById(id);
    if (index == -1){
        // maybe it is already finished
        for (i = 0;i<finishedCount;i++) {
            if (strcmp(finishedRequests[i].requestId,id)==0){
                cout << "Request is already " << status(finishedRequests[i].status) << ", cannot be processed again." << endl;
                return;
            }
        }
        cout << "Invalid Request ID." << endl;
        return;

    }
    finishRequest(index);
}

void cancelRequest() {
    char id[10];
    int i;

    cout << "Enter Request ID to cancel: ";
    readText(id,10);

    int index = activeRequests.findById(id);
    if (index == -1){
        for(i = 0;i<finishedCount;i++){
            if (strcmp(finishedRequests[i].requestId,id)==0){
                cout << "Request is already " << status(finishedRequests[i].status) << ", cannot cancel again." << endl;
                return;
            }
        }
        cout << "Invalid Request ID." << endl;
        return;
    }
    if (finishedCount>=maxRequest){
        cout << "Finished list is Full!" << endl;
        return;
    }

    serviceRequest req = activeRequests.removeAt(index);
    char oldStatus[30];
    char parcelStatus[30];
    char action[80];

    req.status = cancelled;
    strcpy(req.remark,"Cancelled by staff");
    finishedRequests[finishedCount] = req;
    finishedCount++;

    strcpy(oldStatus, "Unknown");
    getParcelStatus(req.parcelId,oldStatus);
    updateParcelStatus(req.parcelId,"Registered");
    freeResource(req);
    cout << "Request " << req.requestId << " cancelled." << endl;
    saveRequests();

    // Task 4 history, not reversible
    if (!getParcelStatus(req.parcelId,parcelStatus))
        strcpy(parcelStatus,"Unknown");

    strcpy(action, "Request ");
    strcat(action,req.requestId);
    strcat(action," Cancelled");
    recordActivity(req.parcelId,action, oldStatus, parcelStatus,false);
}

void searchRequest(){
    char id[10];
    int i;
    cout << "Enter Request ID: ";
    readText(id,10);

    int index = activeRequests.findById(id);
    if (index != -1) {
        showDetails(activeRequests.arr[index]);
        return;

    }
    for (i = 0;i<finishedCount;i++){
        if(strcmp(finishedRequests[i].requestId,id)==0) {
            showDetails(finishedRequests[i]);
            return;
        }
    }
    cout << "Request not found." << endl;
}

void displayFinishRequests() {
    int i;
    if(finishedCount==0) {
        cout << "No finished requests." << endl;
        return;
    }
    cout << "\nFinished requests" << endl;
    for(i = 0;i<finishedCount;i++) {
        showRequest(finishedRequests[i]);
        cout << "Remark: " << finishedRequests[i].remark << endl;
    }
}

//put a request back to pending when its resource cannot be used any more
bool unassignResource(const char* requestId) {
    int index=activeRequests.findById(requestId);
    if (index==-1){
        return false;
    }
    strcpy(activeRequests.arr[index].resourceId,"-");
    activeRequests.arr[index].status=pending;
    saveRequests();
    return true;
}

//if parcel was cancelled/ missing active request must stop too
void parcelClosed(const char* id,const char* reason){
    int i;
    int index = -1;
    char action[80];
    for(i=0;i<activeRequests.size;i++){
        if (strcmp(activeRequests.arr[i].parcelId,id)==0){
            index=i;
        }
    }
    if(index == -1){
        return;
    }
    if(finishedCount>=maxRequest){

        cout << "Finished list is Full!" << endl;
        return;
    }

    serviceRequest req = activeRequests.removeAt(index);
    req.status = cancelled;
    strcpy(req.remark,"Parcel ");
    strcat(req.remark,reason);

    finishedRequests[finishedCount]=req;
    finishedCount++;

    freeResource(req);
    cout << "Request " << req.requestId << " cancelled because the parcel is " << reason << "." << endl;
    saveRequests();

    strcpy(action,"Request ");
    strcat(action,req.requestId);
    strcat(action," Cancelled");
    recordActivity(req.parcelId,action,reason,reason,false);
}


bool parcelActiveRequest(const char* id) {
    int i;
    for (i=0;i<activeRequests.size;i++){
        if(strcmp(activeRequests.arr[i].parcelId, id) == 0){
            return true;
        }
    }

    return false;
}

//Task 3, called when a counter or delivery person is given to a request
bool assignResource(const char* requestId, const char* resourceId){
    int index=activeRequests.findById(requestId);
    if (index==-1){
        cout << "Invalid Request ID." << endl;
        return false;
    }
    if (activeRequests.arr[index].status == assigned){
        cout << "Request already has a resource assigned." << endl;
        return false;
    }

    copyText(activeRequests.arr[index].resourceId,resourceId,10);
    activeRequests.arr[index].status = assigned;
    saveRequests();

    return true;
}


// servicerequest.txt

void loadRequests() {
    ifstream file("servicerequest.txt");
    char line[300];

    int k;
    if (!file){
        cout << "servicerequest.txt not found, starting with no requests." << endl ;
        return;
    }

    file.getline(line,300);
    while (file.getline(line,300)){
        char* t[10];
        t[0] = strtok(line,"|");
        for (k=1;k<10;k++){
            t[k] = strtok(NULL, "|");
        }
        bool ok = true;

        for(k = 0;k<10;k++){
            if (t[k]==NULL){
                ok = false;
            }
        }
        if(!ok){
            continue;      // bad line, skip it
        }
        cutReturn(t[9]);


        serviceRequest req;
        copyText(req.requestId, t[0],10);
        copyText(req.parcelId,t[1],10);

        copyText(req.recipientName,t[2],50);

        req.serviceType = atoi(t[3]);
        req.isUrgent=atoi(t[4]);
        req.priority = atoi(t[5]);
        copyText(req.requestTime, t[6], 20);
        req.status = atoi(t[7]);
        copyText(req.resourceId,t[8],10);
        copyText(req.remark,t[9], 50);

        if(req.status==pending || req.status==assigned){
            activeRequests.enqueue(req);
        }
        else if (finishedCount<maxRequest){
            finishedRequests[finishedCount] = req;
            finishedCount++;
        }

        // keep the next request number bigger than any old one
        if(strlen(req.requestId)>3){
            int number = atoi(req.requestId+3);
            if(number>=nextRequestNo){
                nextRequestNo = number+1;

            }
        }
    }
}

void writeRequest(ofstream& file, serviceRequest r){
    file << r.requestId << "|" << r.parcelId << "|" << r.recipientName << "|" << r.serviceType << "|"
         << r.isUrgent << "|" << r.priority << "|" << r.requestTime << "|" << r.status << "|"
         << r.resourceId << "|" << r.remark << endl;
}

void saveRequests(){
    ofstream file("servicerequest.txt");
    int i;
    if(!file){
        cout << "Cannot write servicerequest.txt!" << endl;
        return;
    }
    file << "RequestID|ParcelID|RecipientName|ServiceType|Urgent|Priority|RequestTime|Status|ResourceID|Remark" << endl;

    for (i=0; i<activeRequests.size; i++){
        writeRequest(file, activeRequests.arr[i]);
    }
    for (i=0; i<finishedCount; i++){
        writeRequest(file,finishedRequests[i]);

    }

}

#endif
