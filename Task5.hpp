#ifndef Task5_HPP
#define Task5_HPP

#include "Task1.hpp"
#include "History.hpp"

using namespace std;
bool parcelActiveRequest(const char* id);

// Custom AVL tree Each node stores the complete parcel.
// Parcel ID is unique and immutable. Categories may contain many parcels.
class Task5ParcelTree {
    struct Node {
        Task1Parcel parcel;
        Node *left, *right;
        int height;
        explicit Node(const Task1Parcel& p)
            : parcel(p), left(NULL), right(NULL), height(1) {}
    };
    Node* root;
    int count;
    static int height(Node* n) { return n ? n->height : 0; }
    static void refresh(Node* n) {
        int a=height(n->left), b=height(n->right);
        n->height=1+(a>b?a:b);
    }
    static Node* rotateRight(Node* n) {
        Node* pivot=n->left;
        n->left=pivot->right;
        pivot->right=n;
        refresh(n); refresh(pivot);
        return pivot;
    }
    static Node* rotateLeft(Node* n) {
        Node* pivot=n->right;
        n->right=pivot->left;
        pivot->left=n;
        refresh(n); refresh(pivot);
        return pivot;
    }
    static Node* balance(Node* n) {
        if (!n) return NULL;
        refresh(n);
        int difference=height(n->left)-height(n->right);
        if (difference>1) {
            if (height(n->left->left)<height(n->left->right))
                n->left=rotateLeft(n->left);
            return rotateRight(n);
        }
        if (difference<-1) {
            if (height(n->right->right)<height(n->right->left))
                n->right=rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }
    static Node* insertNode(Node* n, const Task1Parcel& p, bool& inserted) {
        if (!n) { inserted=true; return new Node(p); }
        if (p.id<n->parcel.id) n->left=insertNode(n->left,p,inserted);
        else if (p.id>n->parcel.id) n->right=insertNode(n->right,p,inserted);
        else return n; // Duplicate ID: retain the original record.
        return balance(n);
    }
    static Node* removeNode(Node* n, const string& id) {
        if (!n) return NULL;
        if (id<n->parcel.id) n->left=removeNode(n->left,id);
        else if (id>n->parcel.id) n->right=removeNode(n->right,id);
        else {
            if (!n->left || !n->right) {
                Node* child=n->left?n->left:n->right;
                delete n;
                return child;
            }
            // Two children: replace with the smallest record in the right subtree.
            Node* successor=n->right;
            while (successor->left) successor=successor->left;
            n->parcel=successor->parcel;
            n->right=removeNode(n->right,successor->parcel.id);
        }
        return balance(n);
    }
    static void destroy(Node* n) {
        if (!n) return;
        destroy(n->left); destroy(n->right); delete n;
    }
    template <typename Visitor>
    static void traverse(Node* n, Visitor& visit) {
        if (!n) return;
        traverse(n->left,visit); visit(n->parcel); traverse(n->right,visit);
    }
    // Verify ordering, height and AVL balance for demo/tests.
    static bool check(Node* n, const string* low, const string* high) {
        if (!n) return true;
        int a=height(n->left), b=height(n->right);
        return (!low || n->parcel.id>*low) && (!high || n->parcel.id<*high)
            && n->height==1+(a>b?a:b) && a-b>=-1 && a-b<=1
            && check(n->left,low,&n->parcel.id) && check(n->right,&n->parcel.id,high);
    }
public:
    Task5ParcelTree() : root(NULL), count(0) {}
    ~Task5ParcelTree() { clear(); }
    Task5ParcelTree(const Task5ParcelTree&)=delete;
    Task5ParcelTree& operator=(const Task5ParcelTree&)=delete;
    void clear() { destroy(root); root=NULL; count=0; }
    int size() const { return count; }
    int treeHeight() const { return height(root); }
    bool valid() const { return check(root,NULL,NULL); }
    const Task1Parcel* find(const string& id) const {
        Node* n=root;
        while (n) {
            if (id==n->parcel.id) return &n->parcel;
            n=id<n->parcel.id?n->left:n->right;
        }
        return NULL;
    }
    bool insert(const Task1Parcel& p) {
        bool inserted=false;
        root=insertNode(root,p,inserted);
        if (inserted) ++count;
        return inserted;
    }
    bool remove(const string& id) {
        if (!find(id)) return false;
        root=removeNode(root,id); --count; return true;
    }
    bool update(const Task1Parcel& p) {
        Node* n=root;
        while (n) {
            if (p.id==n->parcel.id) { n->parcel=p; return true; }
            n=p.id<n->parcel.id?n->left:n->right;
        }
        return false;
    }
    template <typename Visitor>
    void inOrder(Visitor visit) const { traverse(root,visit); }
};

class Task5ParcelManager {
    Task1ParcelManager records;
    Task5ParcelTree tree;
    bool ready;
    static void show(const Task1Parcel& p) {
        cout<<p.id<<" | "<<p.recipient<<" | "<<p.address<<" | "<<p.postcode
            <<" | Sender: "<<p.sender<<" | Content: "<<p.content<<" | "<<p.weight
            <<" kg | Category: "<<p.type<<" | Arrived: "<<p.arrival<<" | Status: "<<p.status<<'\n';
    }
    static string ask(const char* prompt) {
        cout<<prompt;
        string text; getline(cin,text); return text;
    }
    void refreshRecord(const string& id) {
        records.forEachParcel([&](const Task1Parcel& p) {
            if (p.id==id) {
                if (!tree.update(p)) tree.insert(p);
            }
        });
    }
public:
    Task5ParcelManager() : ready(false) {}
    // Recreate on entry: all changes from Tasks 1/2/4 become visible.
    bool load() {
        ready=false;
        tree.clear();
        if (!records.load()) return false;
        records.forEachParcel([&](const Task1Parcel& p) { tree.insert(p); });
        ready=true;
        return true;
    }
    const Task5ParcelTree& index() const { return tree; }
    bool insertParcel(const Task1Parcel& p, string& id) {
        // Task 1 assigns a unique ID and the exact initial status Registered.
        if (!ready || !records.registerParcel(p,id)) return false;
        refreshRecord(id);
//IMPLEMENTATION: Log activity for Task 4 audit trail
        // not reversible: Task 4 undo restores a parcel status, and "None" is not a status
        recordActivity(id, "Parcel Registered", "None", "Registered", false);

        return true;
    }
    bool updateCategory(const string& id, const string& category) {
        if (!ready || !tree.find(id)) return false;
        string oldCategory=tree.find(id)->type;
        if (!records.updateCategory(id.c_str(),category)) return false;
        // IMPLEMENTATION: Log activity for Task 4 audit trail
        // not reversible: a category is not a parcel status, so Task 4 undo cannot restore it
        recordActivity(id, "Category Update", oldCategory, category, false);
        refreshRecord(id); return true;

    }
    bool updateDetails(const string& id, const string& name,
                       const string& address, const string& postcode) {
        if (!ready || !tree.find(id)) return false;
        string oldDetails=tree.find(id)->recipient + " / " + tree.find(id)->address;
        if (!records.updateDetails(id.c_str(),name,address,postcode)) return false;
       // IMPLEMENTATION: Log activity for Task 4 audit trail
        recordActivity(id, "Details Update", oldDetails, name + " / " + address, false);
        //AFTER DOING THIS refreshes to show it displayed
        refreshRecord(id); return true;
    }
    bool removeParcel(const string& id, string& reason) {
        if (!ready) { reason="Parcel file is not loaded."; return false; }
        if (!tree.find(id)) { reason="Parcel not found."; return false; }
        // Required Task 2 guard BEFORE touching the file or tree.
        if (parcelActiveRequest(id.c_str())) {
            reason="Parcel has an active service request. Finish or cancel it in Task 2 first.";
            return false;
        }
        if (!records.removeParcel(id.c_str())) {
            reason="Only Registered or Cancelled parcels can be removed; also check file write access.";
            return false;
        }
        // IMPLEMENTATION: Log activity for Task 4 audit trail (Usually non-reversible or terminal)
        recordActivity(id, "Parcel Removed", "Registered/Cancelled", "Removed", false);
        tree.remove(id);
        reason.clear(); return true;
    }
    void displayAll() const {
        if (!tree.size()) { cout<<"No parcels stored.\n"; return; }
        tree.inOrder([](const Task1Parcel& p) { show(p); });
    }
    void searchCategory(const string& category) const {
        int matches=0;
        tree.inOrder([&](const Task1Parcel& p) {
            if (p.type==category) { show(p); ++matches; }
        });
        if (!matches) cout<<"Category not found (use the exact category spelling).\n";
        else cout<<matches<<" parcel(s) in category "<<category<<".\n";
    }
    void menu() {
        if (!load()) {
            cout<<"Cannot load parcel.txt. Check the header, records and duplicate IDs. No changes made.\n";
            return;
        }
        while (cin) {
            cout<<"\n===== TASK 5: PARCEL CATEGORY ORGANISATION =====\n"
                <<"1. Insert new parcel\n2. Search by Parcel ID\n3. Search by category\n"
                <<"4. Display all (Parcel ID order)\n5. Update category\n"
                <<"6. Update recipient/delivery details\n7. Remove parcel\n"
                <<"8. Rebuild tree from parcel.txt\n9. Show tree statistics\n0. Back\n";
            string choice=ask("Choice: ");
            if (!cin || choice=="0") return;
            if (choice=="1") {
                Task1Parcel p;
                p.recipient=ask("Recipient name: ");
                p.address=ask("Delivery address: ");
                p.postcode=ask("Postcode: ");
                p.sender=ask("Sender address: ");
                p.content=ask("Content description: ");
                p.weight=ask("Weight in kg: ");
                p.type=ask("Category / parcel type (e.g. Normal, Fragile): ");
                p.arrival=ask("Arrival date/time: ");
                if (!cin) return;
                string id;
                if (insertParcel(p,id)) cout<<"Inserted "<<id<<" with status Registered.\n";
                else cout<<"Insert rejected: invalid information or file could not be saved.\n";
            } else if (choice=="2") {
                const Task1Parcel* p=tree.find(ask("Parcel ID: "));
                if (p) show(*p); else cout<<"Parcel not found.\n";
            } else if (choice=="3") searchCategory(ask("Category (exact spelling): "));
            else if (choice=="4") displayAll();
            else if (choice=="5" || choice=="6") {
                string id=ask("Parcel ID: ");
                if (!tree.find(id)) { cout<<"Parcel not found.\n"; continue; }
                bool changed=false;
                if (choice=="5") {
                    string category=ask("New category: ");
                    if (!cin) return;
                    changed=updateCategory(id,category);
                } else {
                    string name=ask("Updated recipient: ");
                    string address=ask("Updated delivery address: ");
                    string postcode=ask("Updated postcode: ");
                    if (!cin) return;
                    changed=updateDetails(id,name,address,postcode);
                }
                cout<<(changed?"Parcel updated.\n":"Update rejected: invalid information, terminal parcel or save failure.\n");
            } else if (choice=="7") {
                string id=ask("Parcel ID to remove: ");
                if (ask("Type YES to confirm removal: ")!="YES") {
                    cout<<"Removal cancelled.\n"; continue;
                }
                string reason;
                if (removeParcel(id,reason)) cout<<"Parcel removed.\n";
                else cout<<"Removal rejected: "<<reason<<'\n';
            } else if (choice=="8") {
                if (!load()) { cout<<"Reload failed; returning to main menu.\n"; return; }
                cout<<"Tree rebuilt from the shared parcel file.\n";
            } else if (choice=="9") {
                cout<<"Key: Parcel ID (text order) | Records: "<<tree.size()
                    <<" | Height: "<<tree.treeHeight()<<" | AVL valid: "<<(tree.valid()?"Yes":"No")<<'\n';
            } else cout<<"Invalid choice. Enter 0 to 9.\n";
        }
    }
};
#endif
