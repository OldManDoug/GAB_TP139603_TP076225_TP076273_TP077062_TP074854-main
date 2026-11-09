#ifndef TASK1_HPP
#define TASK1_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "History.hpp"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

bool parcelActiveRequest(const char* id); // defined in Task2.hpp, true while a service request is still active
void parcelClosed(const char* id,const char* reason); // defined in Task2.hpp, stops the request of a cancelled or missing parcel

// Task 1 uses a custom singly linked list for the parcel master records.
// The text file is shared with Task 2 and uses the agreed ten-column format.
struct Task1Parcel {
    std::string id, recipient, address, postcode, sender, content;
    std::string weight, type, arrival, status;
};

struct Task1ParcelNode {
    Task1Parcel data;
    Task1ParcelNode* next;
    Task1ParcelNode(const Task1Parcel& parcel) : data(parcel), next(NULL) {}
};

class Task1ParcelManager {
    Task1ParcelNode* head;
    int count;

    static const char* header() {
        return "ParcelID|RecipientName|Address|Postcode|SenderAddress|Content|WeightKg|ParcelType|ArrivalTime|Status";
    }
    static bool validText(const std::string& value) {
        return !value.empty() && value.find_first_not_of(" \t") != std::string::npos &&
            value.find('|') == std::string::npos && value.find('\n') == std::string::npos &&
            value.find('\r') == std::string::npos;
    }
    static bool validRecipient(const std::string& value) {
        if (!validText(value) || value.size() >= 50) return false;
        for (std::size_t i=0; i<value.size(); ++i) {
            unsigned char c=static_cast<unsigned char>(value[i]);
            if (std::isalpha(c) || c>=128) return true;
        }
        return false;
    }
    static bool validWeight(const std::string& value) {
        char* end=NULL;
        double weight=std::strtod(value.c_str(),&end);
        return end!=value.c_str() && *end=='\0' && weight>0.0 && std::isfinite(weight);
    }
    static bool parse(const std::string& line, Task1Parcel& p) {
        std::string fields[10];
        std::size_t begin=0;
        for (int i=0;i<10;++i) {
            std::size_t end=line.find('|',begin);
            if ((i<9 && end==std::string::npos) || (i==9 && end!=std::string::npos)) return false;
            fields[i]=line.substr(begin,end==std::string::npos?end:end-begin);
            begin=end==std::string::npos?line.size():end+1;
            if (!validText(fields[i])) return false;
        }
        p.id=fields[0]; p.recipient=fields[1]; p.address=fields[2]; p.postcode=fields[3];
        p.sender=fields[4]; p.content=fields[5]; p.weight=fields[6]; p.type=fields[7];
        p.arrival=fields[8]; p.status=fields[9];
        return validRecipient(p.recipient) && validWeight(p.weight) && validStatus(p.status);
    }
    static bool validStatus(const std::string& value) {
        const char* allowed[]={"Registered","Prepared for Collection","Assigned for Delivery",
            "Special Handling","Return to Sender","On Hold","Collected","Delivered",
            "Handled","Returned","Cancelled","Missing"};
        for (int i=0;i<12;++i) if (value==allowed[i]) return true;
        return false;
    }
    Task1ParcelNode* find(const std::string& id) const {
        for (Task1ParcelNode* n=head;n;n=n->next) if (n->data.id==id) return n;
        return NULL;
    }
    bool save() const {
        const std::string temp="parcel.txt.tmp";
        {
            std::ofstream out(temp.c_str(),std::ios::trunc);
            if (!out) return false;
            out << header() << '\n';
            for (Task1ParcelNode* n=head;n;n=n->next) {
                const Task1Parcel& p=n->data;
                out<<p.id<<'|'<<p.recipient<<'|'<<p.address<<'|'<<p.postcode<<'|'<<p.sender
                   <<'|'<<p.content<<'|'<<p.weight<<'|'<<p.type<<'|'<<p.arrival<<'|'<<p.status<<'\n';
            }
            if (!out) return false;
        }
#ifdef _WIN32
        if (MoveFileExA(temp.c_str(),"parcel.txt",MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)) return true;
#else
        if (std::rename(temp.c_str(),"parcel.txt")==0) return true;
#endif
        std::remove(temp.c_str());
        return false;
    }
    static std::string ask(const char* prompt) {
        std::string value;
        std::cout<<prompt;
        std::getline(std::cin,value);
        return value;
    }
    static void display(const Task1Parcel& p) {
        std::cout<<p.id<<" | "<<p.recipient<<" | "<<p.address<<" | "<<p.postcode
            <<" | Sender: "<<p.sender<<" | Content: "<<p.content<<" | "<<p.weight
            <<" kg | "<<p.type<<" | Arrived: "<<p.arrival<<" | Status: "<<p.status<<'\n';
    }
    static bool isTerminal(const std::string& s) {
        return s=="Collected" || s=="Delivered" || s=="Returned" || s=="Cancelled" || s=="Missing";
    }
    static bool allowedTransition(const std::string& from,const std::string& to) {
        if (from==to || isTerminal(from)) return false;
        if (to=="Missing" || to=="Cancelled") return true;
        if (to=="Registered") return true; // Task 2 cancellation resets the parcel.
        if (from=="Registered") return to=="Prepared for Collection" || to=="Assigned for Delivery" ||
            to=="Special Handling" || to=="Return to Sender" || to=="On Hold" ||
            to=="Collected" || to=="Delivered" || to=="Handled" || to=="Returned";
        if (from=="Prepared for Collection") return to=="Collected";
        if (from=="Assigned for Delivery") return to=="Delivered" || to=="On Hold";
        if (from=="Special Handling") return to=="Handled" || to=="On Hold";
        if (from=="Handled") return to=="Prepared for Collection" || to=="Assigned for Delivery" ||
            to=="Return to Sender" || to=="On Hold";
        if (from=="Return to Sender") return to=="Returned" || to=="On Hold";
        if (from=="On Hold") return to=="Special Handling" || to=="Prepared for Collection" ||
            to=="Assigned for Delivery" || to=="Return to Sender";
        return false;
    }
public:
    Task1ParcelManager() : head(NULL), count(0) {}
    ~Task1ParcelManager() { clear(); }
    Task1ParcelManager(const Task1ParcelManager&)=delete;
    Task1ParcelManager& operator=(const Task1ParcelManager&)=delete;

    void clear() {
        while (head) { Task1ParcelNode* old=head; head=head->next; delete old; }
        count=0;
    }
    bool load() {
        std::ifstream in("parcel.txt");
        if (!in) return false;
        std::string first,line;
        if (!std::getline(in,first)) return false;
        if (!first.empty() && first.back()=='\r') first.pop_back();
        if (first!=header()) return false;
        clear();
        while (std::getline(in,line)) {
            if (!line.empty() && line.back()=='\r') line.pop_back();
            if (line.empty()) continue;
            Task1Parcel p;
            if (!parse(line,p) || find(p.id)) { clear(); return false; }
            Task1ParcelNode* n=new Task1ParcelNode(p);
            if (!head) head=n;
            else { Task1ParcelNode* tail=head; while(tail->next) tail=tail->next; tail->next=n; }
            ++count;
        }
        return in.eof();
    }
    // Read-only snapshot interface for Task 5's tree index. File order stays intact.
    template <typename Visitor>
    void forEachParcel(Visitor visit) const {
        for (Task1ParcelNode* n=head; n; n=n->next) visit(n->data);
    }
    bool updateCategory(const char* id, const std::string& category) {
        if (!id || !validText(category)) return false;
        Task1ParcelNode* n=find(id);
        if (!n || isTerminal(n->data.status)) return false;
        std::string old=n->data.type;
        n->data.type=category;
        if (save()) return true;
        n->data.type=old;
        return false;
    }
    bool exists(const char* id) const { return id && *id && find(id)!=NULL; }
    bool recipient(const char* id,char* out) const {
        if (!id||!out) return false;
        Task1ParcelNode* n=find(id);
        if (!n || n->data.recipient.size()>=50) return false;
        std::strcpy(out,n->data.recipient.c_str()); return true;
    }
    bool status(const char* id,char* out) const {
        if (!id||!out) return false;
        Task1ParcelNode* n=find(id);
        if (!n || n->data.status.size()>=30) return false;
        std::strcpy(out,n->data.status.c_str()); return true;
    }

    // reversible=false when another module owns the change (Task 2 requests, Task 4 undo itself)
    bool setStatus(const char* id,const char* value,bool reversible=true) {
        if (!id||!value||!validStatus(value)) return false;
        Task1ParcelNode* n=find(id);
        if (!n||!allowedTransition(n->data.status,value)) return false;

        std::string old=n->data.status;
        n->data.status=value;

        if (save()) {
            // When status is saved, it is sent to activity record to history-task 4
            recordActivity(id, "Status Update", old, value, reversible);
            return true;
        }

        n->data.status=old;
        return false;

    }
    bool registerParcel(const Task1Parcel& input,std::string& assignedId) {
        if (!validRecipient(input.recipient)||!validText(input.address)||!validText(input.postcode)||
            !validText(input.sender)||!validText(input.content)||!validWeight(input.weight)||
            !validText(input.type)||!validText(input.arrival)) return false;
        char id[24]; int number=count+1;
        do { std::sprintf(id,"P%03d",number++); } while(find(id));
        Task1Parcel p=input; p.id=id; p.status="Registered";
        Task1ParcelNode* n=new Task1ParcelNode(p);
        if (!head) head=n;
        else { Task1ParcelNode* tail=head; while(tail->next) tail=tail->next; tail->next=n; }
        ++count;
        if (save()) { assignedId=p.id; return true; }
        if (head==n) head=NULL;
        else { Task1ParcelNode* prev=head; while(prev->next!=n) prev=prev->next; prev->next=NULL; }
        delete n; --count; return false;
    }
    bool updateDetails(const char* id,const std::string& recipientName,const std::string& address,
                       const std::string& postcode) {
        if (!id||!validRecipient(recipientName)||!validText(address)||!validText(postcode)) return false;
        Task1ParcelNode* n=find(id);
        if (!n || isTerminal(n->data.status)) return false;
        Task1Parcel old=n->data;
        n->data.recipient=recipientName; n->data.address=address; n->data.postcode=postcode;
        if (save()) return true;
        n->data=old; return false;
    }
    bool removeParcel(const char* id) {
        if (!id||!*id) return false;
        if (parcelActiveRequest(id)) return false;
        Task1ParcelNode *prev=NULL,*n=head;
        while(n&&n->data.id!=id) { prev=n; n=n->next; }
        if (!n || (n->data.status!="Registered"&&n->data.status!="Cancelled")) return false;
        Task1ParcelNode* after=n->next;
        if(prev) prev->next=after; else head=after;
        --count;
        if(save()) { delete n; return true; }
        if(prev) prev->next=n; else head=n;
        n->next=after; ++count; return false;
    }
    void displayAll() const {
        if(!head) { std::cout<<"There are no parcels to display.\n"; return; }
        for(Task1ParcelNode* n=head;n;n=n->next) display(n->data);
    }
    void searchAndDisplay(const std::string& id) const {
        Task1ParcelNode* n=find(id);
        if(!n) std::cout<<"Parcel not found.\n"; else display(n->data);
    }
    void displayStatus(const char* wanted) const {
        bool found=false;
        for(Task1ParcelNode* n=head;n;n=n->next) if(n->data.status==wanted) { display(n->data); found=true; }
        if(!found) std::cout<<"No parcels in this queue.\n";
    }
    // The master linked list is kept in arrival order; selecting the first
    // matching route and processing it provides FIFO handling without a second structure.
    bool processNext(const char* queued,const char* completed) {
        bool skipped=false;
        for(Task1ParcelNode* n=head;n;n=n->next) {
            if(n->data.status==queued) {
                const std::string id=n->data.id;
                // a parcel with a Task 2 request is finished through Task 2 so its counter/driver is released
                if(parcelActiveRequest(id.c_str())) { std::cout<<id<<" has an active service request; process it in Task 2.\n"; skipped=true; continue; }
                if(!setStatus(id.c_str(),completed)) {
                    std::cout<<"Parcel processing failed; no status was changed.\n"; return false;
                }
                std::cout<<"Processed "<<id<<"; status updated to "<<completed<<".\n"; return true;
            }
        }
        if(!skipped) std::cout<<"Cannot process an empty queue.\n";
        return false;
    }
    void menu() {
        int choice=0;
        do { //this part need to clean leh
            std::cout<<"\n===== TASK 1: PARCEL INTAKE, SORTING & PROCESSING =====\n"
                <<"1 Register parcel  2 Display all  3 Search/view by ID\n"
                <<"4 Classify/route incoming parcel  5 Process next action queue\n"
                <<"6 Update recipient/delivery details  7 Cancel parcel  8 Report missing\n"
                <<"9 Remove parcel  10 Display action queues  0 Back\nChoice: ";
            std::string line; std::getline(std::cin,line);
            if(line.empty()||line.find_first_not_of("0123456789")!=std::string::npos) { std::cout<<"Invalid choice.\n"; continue; }
            choice=std::atoi(line.c_str());
            if(choice<0||choice>10) { std::cout<<"Invalid choice.\n"; continue; }
            if(choice==1) {
                Task1Parcel p;
                std::cout<<"Recipient name: "; std::getline(std::cin,p.recipient);
                std::cout<<"Delivery address: "; std::getline(std::cin,p.address);
                std::cout<<"Postcode: "; std::getline(std::cin,p.postcode);
                std::cout<<"Sender address: "; std::getline(std::cin,p.sender);
                std::cout<<"Content description: "; std::getline(std::cin,p.content);
                std::cout<<"Weight in kg: "; std::getline(std::cin,p.weight);
                std::cout<<"Parcel type: "; std::getline(std::cin,p.type);
                std::cout<<"Arrival date/time: "; std::getline(std::cin,p.arrival);
                std::string id;
                if(registerParcel(p,id)) std::cout<<"Registered as "<<id<<" with status Registered.\n";
                else std::cout<<"Registration rejected: required information is invalid or parcel.txt cannot be saved.\n";
            } else if(choice==2) displayAll();
            else if(choice==3) { std::string id; std::cout<<"Parcel ID: "; std::getline(std::cin,id); searchAndDisplay(id); }
            else if(choice==4) {
                std::string id; std::cout<<"Parcel ID: "; std::getline(std::cin,id);
                Task1ParcelNode* n=find(id);
                if(!n) { std::cout<<"Parcel not found.\n"; continue; }
                if(parcelActiveRequest(id.c_str())) { std::cout<<"This parcel has an active service request; cancel it in Task 2 before routing it again.\n"; continue; }
                if(n->data.status!="Registered"&&n->data.status!="On Hold"&&n->data.status!="Handled") {
                    std::cout<<"This parcel is already routed or completed; it cannot be routed again.\n"; continue;
                }
                std::cout<<"1 Recipient collection  2 Campus delivery  3 Special handling\n4 Return to sender  5 Hold for investigation\nAction: ";
                std::getline(std::cin,line);
                const char* routes[]={"Prepared for Collection","Assigned for Delivery","Special Handling","Return to Sender","On Hold"};
                if(line.size()!=1||line[0]<'1'||line[0]>'5') { std::cout<<"Invalid route.\n"; continue; }
                std::cout<<(setStatus(id.c_str(),routes[line[0]-'1'])?"Parcel routed successfully.\n":"Route is not valid for the parcel's current status.\n");
            } else if(choice==5) {
                std::cout<<"1 Collection  2 Campus delivery  3 Special handling  4 Return to sender\nQueue: ";
                std::getline(std::cin,line);
                if(line=="1") processNext("Prepared for Collection","Collected");
                else if(line=="2") processNext("Assigned for Delivery","Delivered");
                else if(line=="3") processNext("Special Handling","Handled");
                else if(line=="4") processNext("Return to Sender","Returned");
                else std::cout<<"Invalid queue choice.\n";
            } else if(choice==6) {
                std::string id,name,address,postcode;
                std::cout<<"Parcel ID: "; std::getline(std::cin,id);
                std::cout<<"Updated recipient: "; std::getline(std::cin,name);
                std::cout<<"Updated delivery address: "; std::getline(std::cin,address);
                std::cout<<"Updated postcode: "; std::getline(std::cin,postcode);
                std::cout<<(updateDetails(id.c_str(),name,address,postcode)?"Parcel details updated.\n":"Update rejected: parcel not found, completed, or information invalid.\n");
            } else if(choice==7) {
                std::string id; std::cout<<"Parcel ID to cancel: "; std::getline(std::cin,id);
                if(setStatus(id.c_str(),"Cancelled",false)) { std::cout<<"Parcel cancelled.\n"; parcelClosed(id.c_str(),"Cancelled"); }
                else std::cout<<"Cancellation rejected: parcel not found or already completed.\n";
            } else if(choice==8) {
                std::string id; std::cout<<"Parcel ID reported missing: "; std::getline(std::cin,id);
                if(setStatus(id.c_str(),"Missing",false)) { std::cout<<"Parcel marked Missing.\n"; parcelClosed(id.c_str(),"Missing"); }
                else std::cout<<"Update rejected: parcel not found or already completed.\n";
            } else if(choice==9) {
                std::string id; std::cout<<"Parcel ID to remove: "; std::getline(std::cin,id);
                if(parcelActiveRequest(id.c_str())) { std::cout<<"Removal rejected: parcel has an active service request. Finish or cancel it in Task 2 first.\n"; continue; }
                std::cout<<(removeParcel(id.c_str())?"Parcel removed.\n":"Removal rejected: only Registered or Cancelled parcels may be removed.\n");
            } else if(choice==10) {
                std::cout<<"Registered arrivals:\n"; displayStatus("Registered");
                std::cout<<"Recipient collection:\n"; displayStatus("Prepared for Collection");
                std::cout<<"Campus delivery:\n"; displayStatus("Assigned for Delivery");
                std::cout<<"Special handling:\n"; displayStatus("Special Handling");
                std::cout<<"Return to sender:\n"; displayStatus("Return to Sender");
                std::cout<<"On hold:\n"; displayStatus("On Hold");
            }
        } while(choice!=0);
    }
};

// Task 2's documented shared-data interface. Reloading means changes made by
// another module earlier in the integrated program are visible immediately.
inline bool parcelExists(const char* id) { Task1ParcelManager p; return p.load()&&p.exists(id); }
inline bool getParcelRecipient(const char* id,char* out) { Task1ParcelManager p; return p.load()&&p.recipient(id,out); }
inline bool getParcelStatus(const char* id,char* out) { Task1ParcelManager p; return p.load()&&p.status(id,out); }
// Task 2 owns the request, so its parcel updates are logged as not reversible
inline void updateParcelStatus(const char* id,const char* value) { Task1ParcelManager p; if(p.load()) p.setStatus(id,value,false); }

#endif
