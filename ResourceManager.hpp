#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

// Include BEFORE Task2.hpp (Task 2 calls releaseResource)

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#include "History.hpp" //to integrate with task 4 to record activities
using namespace std;

const int RES_TYPE_COUNTER = 1;
const int RES_TYPE_DELIVERY = 2;

const int RES_AVAILABLE = 1;
const int RES_OCCUPIED = 2;
const int RES_UNAVAILABLE = 3;

const int RES_MAX = 20;
const int RES_QUEUE_MAX = 50;
const int RES_NAME_MAX_LEN = 30;
const string RES_FILE = "resource.txt";

struct Resource {
    string resourceId;
    int type;
    string name;
    int status;
    string requestId;
    string parcelId;
    bool offAfterJob;
    int jobsHandled;

    Resource() {
        type = 0;
        status = RES_AVAILABLE;
        offAfterJob = false;
        jobsHandled = 0;
    }
};

// CIRCULAR QUEUE
class ResourceQueue {
private:
    string* list;
    int maxQueueSize;
    int queueFront;
    int queueRear;
    int count;

public:
    ResourceQueue(int maxSize = RES_QUEUE_MAX) {
        maxQueueSize = maxSize;
        list = new string[maxQueueSize];
        queueFront = 0;
        queueRear = -1;
        count = 0;
    }

    ~ResourceQueue() {
        delete[] list;
    }

    bool isEmpty() { return count == 0; }
    bool isFull() { return count == maxQueueSize; }
    int size() { return count; }

    void addQueue(string elem) {
        if (isFull()) {
            cout << "Queue is full." << endl;
            return;
        }
        queueRear = (queueRear + 1) % maxQueueSize;
        list[queueRear] = elem;
        count++;
    }

    string deleteQueue() {
        if (isEmpty()) {
            cout << "Queue is empty." << endl;
            return "";
        }
        string elem = list[queueFront];
        queueFront = (queueFront + 1) % maxQueueSize;
        count--;
        return elem;
    }

    string front() {
        if (isEmpty()) return "";
        return list[queueFront];
    }

    int positionOf(string elem) {
        for (int i = 0; i < count; i++) {
            if (list[(queueFront + i) % maxQueueSize] == elem) return i + 1;
        }
        return 0;
    }

    bool remove(string elem) {
        bool found = false;
        int n = count;
        for (int i = 0; i < n; i++) {
            string item = deleteQueue();
            if (!found && item == elem) found = true;
            else addQueue(item);
        }
        return found;
    }

    void display() {
        if (isEmpty()) cout << "(none)";
        for (int i = 0; i < count; i++) {
            cout << (i > 0 ? " -> " : "") << list[(queueFront + i) % maxQueueSize];
        }
    }
};

// TEXT HELPERS
string resTypeText(int type) {
    return type == RES_TYPE_COUNTER ? "Counter" : "Delivery";
}

string resTypePlural(int type) {
    return type == RES_TYPE_COUNTER ? "counters" : "delivery personnel";
}

string resStatusText(int status) {
    if (status == RES_AVAILABLE) return "Available";
    if (status == RES_OCCUPIED) return "Occupied";
    return "Unavailable";
}

// RESOURCE MANAGER
class ResourceManager {
private:
    Resource resources[RES_MAX];
    int resourceCount;
    int nextNo[2];
    ResourceQueue freeQueues[2];
    ResourceQueue waitingQueues[2];

    int indexOf(string resourceId) {
        for (int i = 0; i < resourceCount; i++) {
            if (resources[i].resourceId == resourceId) return i;
        }
        return -1;
    }

    string makeId(int type) {
        int n = nextNo[type - 1]++;
        return string(type == RES_TYPE_COUNTER ? "C" : "D") + (n < 10 ? "0" : "") + to_string(n);
    }

    int countByStatus(int status) {
        int total = 0;
        for (int i = 0; i < resourceCount; i++) {
            if (resources[i].status == status) total++;
        }
        return total;
    }

public:
    ResourceManager() {
        resourceCount = 0;
        nextNo[0] = 1;
        nextNo[1] = 1;
        if (load()) return;
        // First run with no resource.txt: create the default resources
        addResource(RES_TYPE_COUNTER, "Main Counter 1");
        addResource(RES_TYPE_COUNTER, "Main Counter 2");
        addResource(RES_TYPE_COUNTER, "Express Counter");
        addResource(RES_TYPE_DELIVERY, "Ahmad Faizal");
        addResource(RES_TYPE_DELIVERY, "Tan Wei Ling");
        addResource(RES_TYPE_DELIVERY, "Ravi Kumar");
    }

    bool isFull() { return resourceCount >= RES_MAX; }
    int size() { return resourceCount; }
    Resource* at(int i) { return &resources[i]; }

    // PERMANENT STORAGE (resource.txt)
    // ResourceID|Type|Name|Status|RequestID|ParcelID|OffAfterJob|JobsHandled
    void save() {
        ofstream file(RES_FILE.c_str());
        if (!file) {
            cout << "[Task 3] Cannot write " << RES_FILE << endl;
            return;
        }
        file << "ResourceID|Type|Name|Status|RequestID|ParcelID|OffAfterJob|JobsHandled" << endl;
        for (int i = 0; i < resourceCount; i++) {
            Resource r = resources[i];
            string requestId = r.requestId.empty() ? "-" : r.requestId;
            string parcelId = r.parcelId.empty() ? "-" : r.parcelId;
            file << r.resourceId << "|" << r.type << "|" << r.name << "|" << r.status << "|" << requestId << "|"
                 << parcelId << "|" << (r.offAfterJob ? 1 : 0) << "|" << r.jobsHandled << endl;
        }
    }

    bool load() {
        ifstream file(RES_FILE.c_str());
        if (!file) return false;
        string line;
        getline(file, line);
        while (getline(file, line) && resourceCount < RES_MAX) {
            if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1);
            string f[8];
            int n = 0;
            size_t start = 0;
            while (n < 8) {
                size_t bar = line.find('|', start);
                if (bar == string::npos) {
                    f[n++] = line.substr(start);
                    break;
                }
                f[n++] = line.substr(start, bar - start);
                start = bar + 1;
            }
            if (n != 8) continue;

            Resource r;
            r.resourceId = f[0];
            r.type = atoi(f[1].c_str());
            r.name = f[2];
            r.status = atoi(f[3].c_str());
            r.requestId = f[4] == "-" ? "" : f[4];
            r.parcelId = f[5] == "-" ? "" : f[5];
            r.offAfterJob = f[6] == "1";
            r.jobsHandled = atoi(f[7].c_str());
            if (r.type != RES_TYPE_COUNTER && r.type != RES_TYPE_DELIVERY) continue;
            if (r.resourceId.size() < 2 || indexOf(r.resourceId) != -1) continue;
            if (r.status != RES_AVAILABLE && r.status != RES_OCCUPIED && r.status != RES_UNAVAILABLE) r.status = RES_AVAILABLE;

            resources[resourceCount++] = r;
            if (r.status == RES_AVAILABLE) freeQueue(r.type)->addQueue(r.resourceId);
            int number = atoi(r.resourceId.substr(1).c_str());
            if (number >= nextNo[r.type - 1]) nextNo[r.type - 1] = number + 1;
        }
        return resourceCount > 0;
    }

    // STARTUP SYNC (used by syncWithRequests, no activity is logged)
    void restoreOccupied(Resource* r, string requestId, string parcelId) {
        freeQueue(r->type)->remove(r->resourceId);
        r->status = RES_OCCUPIED;
        r->requestId = requestId;
        r->parcelId = parcelId;
    }

    void restoreFree(Resource* r) {
        r->requestId = "";
        r->parcelId = "";
        if (r->offAfterJob) {
            r->offAfterJob = false;
            r->status = RES_UNAVAILABLE;
            return;
        }
        r->status = RES_AVAILABLE;
        freeQueue(r->type)->addQueue(r->resourceId);
    }
    ResourceQueue* freeQueue(int type) { return &freeQueues[type - 1]; }
    ResourceQueue* waitingQueue(int type) { return &waitingQueues[type - 1]; }

    Resource* find(string resourceId) {
        int i = indexOf(resourceId);
        if (i == -1) return NULL;
        return &resources[i];
    }

    string addResource(int type, string name) {
        if (resourceCount >= RES_MAX) return "";
        Resource r;
        r.resourceId = makeId(type);
        r.type = type;
        r.name = name;
        resources[resourceCount++] = r;
        freeQueue(type)->addQueue(r.resourceId);
        save();
        // TASK 4 INTEGRATION: Log resource registration
        recordActivity("SYSTEM", "Resource Registered (" + r.resourceId + " - " + r.name + ")", "None", "Available", false);
        return r.resourceId;
    }

    void removeResource(string resourceId) {
        int i = indexOf(resourceId);
        if (i == -1) return;
        if (resources[i].status == RES_OCCUPIED) {
            cout << resourceId << " is handling " << resources[i].requestId << ". Complete or cancel it first." << endl;
            return;
        }
        freeQueue(resources[i].type)->remove(resourceId);
        for (int j = i; j < resourceCount - 1; j++) {
            resources[j] = resources[j + 1];
        }
        resourceCount--;
        save();
        cout << resourceId << " removed." << endl;
    }

    void occupy(Resource* r, string requestId, string parcelId) {
        ResourceQueue* q = freeQueue(r->type);
        if (q->front() == r->resourceId) q->deleteQueue();
        else q->remove(r->resourceId);
        r->status = RES_OCCUPIED;
        r->requestId = requestId;
        r->parcelId = parcelId;
        save();
        // INTEGRATION HOOK: Log resource assignment into activity.txt
        // This captures the required "relevant request or resource information trial and error"
        // Not reversible: "Pending" and "Assigned" are request statuses, so Task 4 undo can never match them to a parcel
        string actionDesc = "Resource Assigned (" + r->resourceId + " - " + r->name + ", Req: " + requestId + ")";
        recordActivity(parcelId, actionDesc, "Pending", "Assigned", false);
    }

    void release(string resourceId) {
        Resource* r = find(resourceId);
        if (r == NULL) {
            cout << "[Task 3] Invalid Resource ID: " << resourceId << endl;
            return;
        }
        if (r->status != RES_OCCUPIED) {
            cout << "[Task 3] Invalid release: " << resourceId << " is not occupied." << endl;
            return;
        }
// try to integrate task 4
if (!r->parcelId.empty()) {
            string actionDesc = "Resource Released (" + r->resourceId + " - " + r->name + ", Req: " + r->requestId + ")";
            recordActivity(r->parcelId, actionDesc, "Assigned", "Processed/Released", false);
        }
        cout << "[Task 3] " << resourceId << " finished " << r->requestId;
        r->requestId = "";
        r->parcelId = "";
        r->jobsHandled++;
        if (r->offAfterJob) {
            r->offAfterJob = false;
            r->status = RES_UNAVAILABLE;
            save();
            cout << " and is now unavailable." << endl;
            return;
        }
        r->status = RES_AVAILABLE;
        freeQueue(r->type)->addQueue(resourceId);
        save();
        cout << " and is available again." << endl;
        if (!waitingQueue(r->type)->isEmpty()) {
            cout << "[Task 3] Requests are waiting for " << resTypePlural(r->type)
                 << ". Use Task 3 > Assign pending requests." << endl;
        }
    }

    bool markAvailable(Resource* r) {
        if (r->status == RES_AVAILABLE) {
            cout << r->resourceId << " is already available." << endl;
            return false;
        }
        if (r->status == RES_OCCUPIED) {
            if (r->offAfterJob) {
                r->offAfterJob = false;
                save();
                cout << r->resourceId << " will stay available after " << r->requestId << "." << endl;
            } else {
                cout << r->resourceId << " is occupied by " << r->requestId
                     << ". It is freed when Task 2 completes or cancels it." << endl;
            }
            return false;
        }
        r->status = RES_AVAILABLE;
        freeQueue(r->type)->addQueue(r->resourceId);
        save();
        cout << r->resourceId << " is now available." << endl;
        return true;
    }

    void markUnavailable(Resource* r) {
        if (r->status == RES_UNAVAILABLE || r->offAfterJob) {
            cout << r->resourceId << " is already set to unavailable." << endl;
            return;
        }
        if (r->status == RES_OCCUPIED) {
            r->offAfterJob = true;
            save();
            cout << r->resourceId << " will become unavailable after " << r->requestId << "." << endl;
            return;
        }
        // TASK 4 INTEGRATION: Log resource availability exception / status change
        recordActivity("SYSTEM", "Resource Unavailable (" + r->resourceId + ")", "Available", "Unavailable", false);
        freeQueue(r->type)->remove(r->resourceId);
        r->status = RES_UNAVAILABLE;
        save();
        cout << r->resourceId << " is now unavailable." << endl;
    }

    void display() {
        bool anyOffAfterJob = false;
        cout << "\n===== RESOURCE AVAILABILITY =====" << endl;
        cout << string(96, '-') << endl;
        cout << left << "| " << setw(4) << "ID" << " | " << setw(8) << "Type" << " | " << setw(30) << "Name"
             << " | " << setw(12) << "Status" << " | " << setw(10) << "Request" << " | " << setw(8) << "Parcel"
             << " | " << setw(4) << "Jobs" << " |" << endl;
        cout << string(96, '-') << endl;
        for (int i = 0; i < resourceCount; i++) {
            Resource r = resources[i];
            if (r.offAfterJob) anyOffAfterJob = true;
            cout << "| " << setw(4) << r.resourceId << " | " << setw(8) << resTypeText(r.type) << " | " << setw(30)
                 << r.name << " | " << setw(12) << (resStatusText(r.status) + (r.offAfterJob ? "*" : "")) << " | "
                 << setw(10) << (r.requestId.empty() ? "-" : r.requestId) << " | " << setw(8)
                 << (r.parcelId.empty() ? "-" : r.parcelId) << " | " << setw(4) << r.jobsHandled << " |" << endl;
        }
        cout << string(96, '-') << right << endl;
        if (anyOffAfterJob) cout << "* becomes unavailable after the current request" << endl;
        cout << "Available: " << countByStatus(RES_AVAILABLE) << " | Occupied: " << countByStatus(RES_OCCUPIED)
             << " | Unavailable: " << countByStatus(RES_UNAVAILABLE) << endl;
        cout << "Next free counter:     ";
        freeQueue(RES_TYPE_COUNTER)->display();
        cout << "\nNext free delivery:    ";
        freeQueue(RES_TYPE_DELIVERY)->display();
        cout << "\nWaiting for counter:   ";
        waitingQueue(RES_TYPE_COUNTER)->display();
        cout << "\nWaiting for delivery:  ";
        waitingQueue(RES_TYPE_DELIVERY)->display();
        cout << endl;
    }
};

ResourceManager resourceManager;

// FOR TASK 2
void releaseResource(const char* resourceId) {
    if (resourceId == NULL) resourceId = "";
    resourceManager.release(resourceId);
}

#endif
