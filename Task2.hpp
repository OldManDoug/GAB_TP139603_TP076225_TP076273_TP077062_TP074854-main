#ifndef SERVICEREQUEST_HPP
#define SERVICEREQUEST_HPP
#include <iostream>
#include <fstream>
#include <cstring>
// TASK 2 - Parcel Service and Collection Management Module
// Data structure: PRIORITY QUEUE (array, requests kept in arrival order)
//   enqueue = add at the rear               -> O(1)
//   dequeue = scan for highest priority,
//             remove it, shift the rest left -> O(n)
// Same idea as the Week 8 emergency room example.


using namespace std;

#define MAX_REQUESTS 100

// ---------- Service types (stored as numbers) ----------
#define SVC_COLLECTION   1   // recipient comes to collect, can wait
#define SVC_DELIVERY     2   // parcel sent to the recipient
#define SVC_SPECIAL      3   // fragile / perishable / live animal
#define SVC_RETURN       4   // send back to sender

// ---------- Request status (stored as numbers) ----------
#define REQ_PENDING      1
#define REQ_ASSIGNED     2   // set when Task 3 gives it a counter / delivery person
#define REQ_COMPLETED    3
#define REQ_CANCELLED    4
#define REQ_RETURNED     5

// Priority rule (chosen by you):
//   priority = type score + 4 if staff marks it urgent
//   Special = 4, Delivery = 3, Return = 2, Collection = 1
//   Same priority -> the earlier request is served first (first found in the scan)

// ---------- One service request ----------
struct ServiceRequest {
    char requestId[10];       // e.g. REQ001
    char parcelId[10];        // must exist in parcel.txt
    char recipientName[50];
    int  serviceType;         // SVC_...
    int  isUrgent;            // 1 = urgent, 0 = normal (entered by staff)
    int  priority;            // worked out from type + urgent
    char requestTime[20];     // e.g. 2026-11-03 09:00
    int  status;              // REQ_...
    char resourceId[10];      // counter / delivery person from Task 3, "-" if none
    char remark[50];          // completion / cancel / return note, "-" if none
};

// ---------- Priority queue of ACTIVE requests ----------
struct RequestQueue {
    ServiceRequest arr[MAX_REQUESTS];
    int size;

    RequestQueue() { size = 0; }

    bool isEmpty() {
        // TODO: return true when there are no requests
        return true;
    }

    bool isFull() {
        // TODO: return true when size reaches MAX_REQUESTS
        return false;
    }

    // Add at the rear. NO sorting here.
    bool enqueue(ServiceRequest req) {
        // TODO: if full -> print message, return false
        // TODO: arr[size] = req; size++; return true
        return false;
    }

    // Scan the whole array and return the INDEX of the highest priority request.
    // Hint: start best = 0, loop i = 1..size-1, use a strict ">" so ties keep the earlier one.
    // Return -1 if the queue is empty.
    int findHighest() {
        // TODO
        return -1;
    }

    // Find a request by its Request ID. Return the index, or -1 if not found.
    int findById(const char* id) {
        // TODO: loop + strcmp
        return -1;
    }

    // Remove the request at position index and shift everything after it one step left.
    ServiceRequest removeAt(int index) {
        ServiceRequest removed = arr[0];   // placeholder so the file compiles
        // TODO: removed = arr[index]; shift left; size--;
        return removed;
    }
};

// ---------- Lists used by the module ----------
RequestQueue activeRequests;                  // waiting to be processed
ServiceRequest finishedRequests[MAX_REQUESTS]; // completed / cancelled / returned
int finishedCount = 0;
int nextRequestNo = 1;                        // used to make REQ001, REQ002 ...

// ---------- Helper functions ----------
const char* serviceName(int type) {
    // TODO: return "Collection", "Delivery", "Special Handling", "Return to Sender"
    return "Unknown";
}

const char* statusName(int status) {
    // TODO: return "Pending", "Assigned", "Completed", "Cancelled", "Returned"
    return "Unknown";
}

int calculatePriority(int serviceType, int isUrgent) {
    // TODO: type score (Special 4, Delivery 3, Return 2, Collection 1), +4 if urgent
    return 0;
}

void getCurrentTime(char* text) {
    // TODO (optional): use <ctime> time(), localtime(), strftime() to fill "2026-11-03 09:00"
    strcpy(text, "2026-11-03 00:00");
}

// ---------- Parcel link (Task 1 owns parcel.txt) ----------
// parcel.txt line 1 is a HEADER -> skip it when reading.
// TEMPORARY until Task 1 provides these. Keep the names so integration is easy.
bool parcelExists(const char* parcelId) {
    // TODO: open parcel.txt, read each line, compare the first field with parcelId
    return false;
}

bool getParcelRecipient(const char* parcelId, char* recipientOut) {
    // TODO: copy the recipient name of that parcel into recipientOut; false if not found
    return false;
}

void updateParcelStatus(const char* parcelId, const char* newStatus) {
    // TODO: change the last field (status) of that parcel line in parcel.txt
}

// ---------- Main operations ----------
void createRequest() {
    // TODO 1: ask Parcel ID  -> parcelExists()? else error
    // TODO 2: parcel already has an ACTIVE request? -> reject (duplicate)
    // TODO 3: ask recipient name -> must match getParcelRecipient() (incorrect recipient)
    // TODO 4: ask service type 1-4 and urgent 0/1, validate both
    // TODO 5: build the ServiceRequest (Request ID from nextRequestNo, status REQ_PENDING)
    // TODO 6: activeRequests.enqueue()
}

void displayActiveRequests() {
    // TODO: if empty -> "No active requests". Otherwise print each request in arrival order.
}

void showNextRequest() {
    // TODO: findHighest() and print it WITHOUT removing it (peek)
}

void processNextRequest() {
    // TODO 1: empty -> message
    // TODO 2: index = findHighest(); show the request
    // TODO 3: ask result: 1 Completed, 2 Cancelled, 3 Returned, 0 Back (do nothing)
    // TODO 4: removeAt(index), set status + remark, add to finishedRequests
    // TODO 5: updateParcelStatus() (Collected / Delivered / Handled / Returned / back to Registered)
}

void cancelRequestById() {
    // TODO: ask Request ID -> findById() -> -1 means invalid ID
    // TODO: removeAt(index), status REQ_CANCELLED, add to finishedRequests
}

void searchRequest() {
    // TODO: look in activeRequests first, then finishedRequests; else "not found"
}

void displayFinishedRequests() {
    // TODO: empty -> message, else print finishedRequests
}

// Hook for Task 3: called when a counter / delivery person is given to a request
bool assignResource(const char* requestId, const char* resourceId) {
    // TODO: find the request, set resourceId and status = REQ_ASSIGNED
    return false;
}

// ---------- Permanent storage (servicerequest.txt) ----------
// Line 1 is a HEADER (column names) -> skip it when loading, write it again when saving.
// Line format:
// RequestID|ParcelID|Recipient|ServiceType|Urgent|Priority|RequestTime|Status|ResourceID|Remark
void loadRequests() {
    // TODO: open file; skip the header line; each line -> ServiceRequest. Status Pending/Assigned -> activeRequests,
    //       others -> finishedRequests. Set nextRequestNo above the highest number found.
}

void saveRequests() {
    // TODO: write the header line, then activeRequests, then finishedRequests (same line format)
}

#endif
