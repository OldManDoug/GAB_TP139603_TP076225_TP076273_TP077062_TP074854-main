#ifndef RESOURCE_ALLOCATION_HPP
#define RESOURCE_ALLOCATION_HPP

// Include AFTER Task2.hpp (uses activeRequests and assignResource)

#include "ResourceManager.hpp"

const int T3_SVC_COLLECTION = 1;
const int T3_SVC_DELIVERY = 2;
const int T3_SVC_SPECIAL = 3;
const int T3_SVC_RETURN = 4;
const int T3_REQ_PENDING = 1;

// TASK 2 ADAPTER
int t3RequestCount() { return activeRequests.size; }
string t3RequestId(int i) { return activeRequests.arr[i].requestId; }
string t3ParcelId(int i) { return activeRequests.arr[i].parcelId; }
int t3ServiceType(int i) { return activeRequests.arr[i].serviceType; }
int t3RequestStatus(int i) { return activeRequests.arr[i].status; }

int t3FindRequest(string requestId) {
    for (int i = 0; i < t3RequestCount(); i++) {
        if (t3RequestId(i) == requestId) return i;
    }
    return -1;
}

bool t3IsPending(string requestId) {
    int i = t3FindRequest(requestId);
    return i != -1 && t3RequestStatus(i) == T3_REQ_PENDING;
}

// ALLOCATION
int resourceTypeForService(int serviceType) {
    if (serviceType == T3_SVC_COLLECTION || serviceType == T3_SVC_SPECIAL) return RES_TYPE_COUNTER;
    if (serviceType == T3_SVC_DELIVERY || serviceType == T3_SVC_RETURN) return RES_TYPE_DELIVERY;
    return 0;
}

bool assignFromFront(string requestId, string parcelId, int type) {
    string resourceId = resourceManager.freeQueue(type)->front();
    if (!assignResource(requestId.c_str(), resourceId.c_str())) {
        cout << "  " << requestId << ": rejected by Task 2." << endl;
        return false;
    }
    Resource* r = resourceManager.find(resourceId);
    resourceManager.occupy(r, requestId, parcelId);
    cout << "  " << requestId << " -> " << resourceId << " (" << r->name << ")" << endl;
    return true;
}

void purgeWaiting(int type) {
    ResourceQueue* waitQ = resourceManager.waitingQueue(type);
    int n = waitQ->size();
    for (int i = 0; i < n; i++) {
        string requestId = waitQ->deleteQueue();
        if (t3IsPending(requestId)) waitQ->addQueue(requestId);
        else cout << "  " << requestId << " left the waiting list (no longer pending)." << endl;
    }
}

int dispatchWaiting(int type) {
    purgeWaiting(type);
    ResourceQueue* waitQ = resourceManager.waitingQueue(type);
    int assigned = 0;
    while (!waitQ->isEmpty() && !resourceManager.freeQueue(type)->isEmpty()) {
        string requestId = waitQ->deleteQueue();
        if (assignFromFront(requestId, t3ParcelId(t3FindRequest(requestId)), type)) assigned++;
    }
    return assigned;
}

// INPUT HELPERS
string resReadLine(string prompt) {
    cout << prompt;
    string line;
    getline(cin, line);
    while (!line.empty() && (line[0] == ' ' || line[0] == '\t')) line.erase(0, 1);
    while (!line.empty() && (line[line.size() - 1] == ' ' || line[line.size() - 1] == '\r')) line.erase(line.size() - 1);
    return line;
}

string resReadId(string prompt) {
    string id = resReadLine(prompt);
    for (int i = 0; i < (int)id.size(); i++) {
        if (id[i] >= 'a' && id[i] <= 'z') id[i] = (char)(id[i] - 'a' + 'A');
    }
    return id;
}

int resReadInt(string prompt) {
    string line = resReadLine(prompt);
    if (!cin) return 0;
    if (line.empty() || line.size() > 6) return -1;
    for (int i = 0; i < (int)line.size(); i++) {
        if (line[i] < '0' || line[i] > '9') return -1;
    }
    return stoi(line);
}

bool resValidName(string name) {
    if (name.empty() || (int)name.size() > RES_NAME_MAX_LEN) {
        cout << "Name must be 1 to " << RES_NAME_MAX_LEN << " characters." << endl;
        return false;
    }
    return true;
}

// MENU ACTIONS
void showAvailability() {
    purgeWaiting(RES_TYPE_COUNTER);
    purgeWaiting(RES_TYPE_DELIVERY);
    resourceManager.display();
}

void registerResource() {
    cout << "\n===== REGISTER A RESOURCE =====" << endl;
    if (resourceManager.isFull()) {
        cout << "Registry is full (" << RES_MAX << " resources). Remove one first." << endl;
        return;
    }
    cout << "1. Collection counter" << endl;
    cout << "2. Delivery personnel" << endl;
    int type = resReadInt("Type (0 to cancel): ");
    if (type == 0) return;
    if (type != RES_TYPE_COUNTER && type != RES_TYPE_DELIVERY) {
        cout << "Invalid type." << endl;
        return;
    }
    string name = resReadLine("Name: ");
    if (!resValidName(name)) return;
    cout << "Registered " << resourceManager.addResource(type, name) << " - " << name << "." << endl;
    dispatchWaiting(type);
}

void renameOrRemoveResource() {
    cout << "\n===== RENAME OR REMOVE A RESOURCE =====" << endl;
    cout << "1. Rename" << endl;
    cout << "2. Remove" << endl;
    int choice = resReadInt("Choose (0 to cancel): ");
    if (choice == 0) return;
    if (choice != 1 && choice != 2) {
        cout << "Invalid choice." << endl;
        return;
    }
    string resourceId = resReadId("Resource ID: ");
    Resource* r = resourceManager.find(resourceId);
    if (r == NULL) {
        cout << "Invalid Resource ID: " << resourceId << endl;
        return;
    }
    if (choice == 2) {
        resourceManager.removeResource(resourceId);
        return;
    }
    string name = resReadLine("New name: ");
    if (resValidName(name)) {
        r->name = name;
        cout << resourceId << " renamed." << endl;
    }
}

void assignPendingRequests() {
    cout << "\n===== ASSIGN PENDING REQUESTS =====" << endl;
    if (t3RequestCount() == 0) {
        cout << "No active service requests." << endl;
        return;
    }
    int assigned = dispatchWaiting(RES_TYPE_COUNTER) + dispatchWaiting(RES_TYPE_DELIVERY);
    for (int i = 0; i < t3RequestCount(); i++) {
        if (t3RequestStatus(i) != T3_REQ_PENDING) continue;
        string requestId = t3RequestId(i);
        int type = resourceTypeForService(t3ServiceType(i));
        if (type == 0) {
            cout << "  " << requestId << ": invalid service type, skipped." << endl;
            continue;
        }
        ResourceQueue* waitQ = resourceManager.waitingQueue(type);
        if (waitQ->positionOf(requestId) > 0) continue;
        if (!resourceManager.freeQueue(type)->isEmpty()) {
            if (assignFromFront(requestId, t3ParcelId(i), type)) assigned++;
        } else if (!waitQ->isFull()) {
            waitQ->addQueue(requestId);
            cout << "  " << requestId << ": no free " << resTypePlural(type) << ", waiting list position "
                 << waitQ->size() << "." << endl;
        } else {
            cout << "  " << requestId << ": waiting list is full, stays pending." << endl;
        }
    }
    cout << "Assigned: " << assigned << " | Waiting for counter: " << resourceManager.waitingQueue(RES_TYPE_COUNTER)->size()
         << " | Waiting for delivery: " << resourceManager.waitingQueue(RES_TYPE_DELIVERY)->size() << endl;
}

void assignToChosenResource(string resourceId) {
    cout << "\n===== ASSIGN TO A CHOSEN RESOURCE =====" << endl;
    string requestId = resReadLine("Request ID (blank to cancel): ");
    if (requestId.empty()) return;
    int i = t3FindRequest(requestId);
    if (i == -1) {
        cout << "Invalid Request ID: " << requestId << endl;
        return;
    }
    if (t3RequestStatus(i) != T3_REQ_PENDING) {
        cout << requestId << " already has a resource." << endl;
        return;
    }
    int type = resourceTypeForService(t3ServiceType(i));
    if (type == 0) {
        cout << requestId << " has an invalid service type." << endl;
        return;
    }
    ResourceQueue* waitQ = resourceManager.waitingQueue(type);
    if (resourceManager.freeQueue(type)->isEmpty()) {
        cout << "No free " << resTypePlural(type) << "." << endl;
        if (waitQ->positionOf(requestId) == 0 && !waitQ->isFull()) {
            waitQ->addQueue(requestId);
            cout << requestId << " added to the waiting list (position " << waitQ->size() << ")." << endl;
        }
        return;
    }
    if (resourceId.empty()) {
        cout << "Free " << resTypePlural(type) << ": ";
        resourceManager.freeQueue(type)->display();
        resourceId = resReadId("\nResource ID (blank to cancel): ");
        if (resourceId.empty()) return;
    }
    Resource* r = resourceManager.find(resourceId);
    if (r == NULL) {
        cout << "Invalid Resource ID: " << resourceId << endl;
        return;
    }
    if (r->type != type) {
        cout << resourceId << " is a " << resTypeText(r->type) << " resource. " << requestId << " needs "
             << resTypePlural(type) << "." << endl;
        return;
    }
    if (r->status != RES_AVAILABLE) {
        cout << resourceId << " is " << resStatusText(r->status) << ". Choose another resource." << endl;
        return;
    }
    if (!assignResource(requestId.c_str(), resourceId.c_str())) {
        cout << requestId << " was rejected by Task 2." << endl;
        return;
    }
    resourceManager.occupy(r, requestId, t3ParcelId(i));
    waitQ->remove(requestId);
    cout << requestId << " assigned to " << resourceId << " (" << r->name << ")." << endl;
}

void updateResourceStatus() {
    cout << "\n===== UPDATE RESOURCE STATUS =====" << endl;
    string resourceId = resReadId("Resource ID (blank to cancel): ");
    if (resourceId.empty()) return;
    Resource* r = resourceManager.find(resourceId);
    if (r == NULL) {
        cout << "Invalid Resource ID: " << resourceId << endl;
        return;
    }
    cout << resourceId << " is " << resStatusText(r->status) << endl;
    cout << "1. Available" << endl;
    cout << "2. Occupied (assign a request)" << endl;
    cout << "3. Unavailable" << endl;
    int choice = resReadInt("New status (0 to cancel): ");
    if (choice == 1) {
        if (resourceManager.markAvailable(r)) dispatchWaiting(r->type);
    } else if (choice == 2) {
        if (r->status == RES_AVAILABLE) assignToChosenResource(resourceId);
        else cout << "Only an available resource can be assigned." << endl;
    } else if (choice == 3) {
        resourceManager.markUnavailable(r);
    } else if (choice != 0) {
        cout << "Invalid choice." << endl;
    }
}

// TASK 3 MENU
void task3Menu() {
    int choice;
    do {
        cout << "\n===== TASK 3: COLLECTION COUNTERS & DELIVERY RESOURCES =====" << endl;
        cout << "1. Display resource availability" << endl;
        cout << "2. Register a new resource" << endl;
        cout << "3. Rename or remove a resource" << endl;
        cout << "4. Assign pending service requests (automatic)" << endl;
        cout << "5. Assign a request to a chosen resource" << endl;
        cout << "6. Update resource status" << endl;
        cout << "0. Back" << endl;
        choice = resReadInt("Choose an option (0-6): ");

        switch (choice) {
            case 1: showAvailability(); break;
            case 2: registerResource(); break;
            case 3: renameOrRemoveResource(); break;
            case 4: assignPendingRequests(); break;
            case 5: assignToChosenResource(""); break;
            case 6: updateResourceStatus(); break;
            case 0: break;
            default: cout << "Invalid choice, try again." << endl;
        }
    } while (choice != 0);
}

#endif
