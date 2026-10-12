
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Student {
    int roll;
    string name, subject;
    string hall;
    int seat;
    bool allocated = false;
};

struct Hall {
    string name;
    int capacity;
    int used = 0;
};

vector<Student> students;
vector<Hall> halls;
map<int, int> studentIndex;
set<pair<int, int>> occupiedSeats;

// Add student record
void addStudent() {
    Student s;

    cout << "Enter Roll Number: ";
    cin >> s.roll;

    if (studentIndex.count(s.roll)) {
        cout << "Student already exists!\n";
        return;
    }

    cin.ignore();
    cout << "Enter Student Name: ";
    getline(cin, s.name);

    cout << "Enter Subject: ";
    getline(cin, s.subject);

    studentIndex[s.roll] = students.size();
    students.push_back(s);

    cout << "Student added successfully!\n";
}

// Add examination hall
void addHall() {
    Hall h;

    cin.ignore();
    cout << "Enter Hall Name: ";
    getline(cin, h.name);

    cout << "Enter Hall Capacity: ";
    cin >> h.capacity;

    if (h.capacity <= 0) {
        cout << "Capacity must be positive!\n";
        return;
    }

    for (const Hall &existing : halls) {
        if (existing.name == h.name) {
            cout << "Hall already exists!\n";
            return;
        }
    }

    halls.push_back(h);
    cout << "Hall added successfully!\n";
}

// Allocate seats using a priority queue
void allocateSeats() {
    if (students.empty() || halls.empty()) {
        cout << "Add students and halls first!\n";
        return;
    }

    int totalCapacity = 0;
    for (const Hall &h : halls)
        totalCapacity += h.capacity;

    if ((int)students.size() > totalCapacity) {
        cout << "Not enough hall capacity!\n";
        return;
    }

    // Reset previous allocations
    for (auto &s : students) {
        s.allocated = false;
        s.hall = "";
        s.seat = 0;
    }

    for (auto &h : halls)
        h.used = 0;

    occupiedSeats.clear();

    // Sort students by roll number
    sort(students.begin(), students.end(),
         [](const Student &a, const Student &b) {
             return a.roll < b.roll;
         });

    // Rebuild student lookup after sorting
    studentIndex.clear();
    for (int i = 0; i < (int)students.size(); i++)
        studentIndex[students[i].roll] = i;

    // Priority queue: hall with most free seats first
    priority_queue<pair<int, int>> pq;

    for (int i = 0; i < (int)halls.size(); i++)
        pq.push({halls[i].capacity, i});

    for (auto &s : students) {
        while (!pq.empty() &&
               halls[pq.top().second].used >=
               halls[pq.top().second].capacity) {
            pq.pop();
        }

        if (pq.empty()) {
            cout << "No seats available!\n";
            return;
        }

        int hallIndex = pq.top().second;
        pq.pop();

        Hall &h = halls[hallIndex];
        h.used++;

        s.hall = h.name;
        s.seat = h.used;
        s.allocated = true;

        // Validate unique seat
        pair<int, int> seatKey = {hallIndex, s.seat};
        if (!occupiedSeats.insert(seatKey).second) {
            cout << "Duplicate seat detected!\n";
            return;
        }

        if (h.used < h.capacity)
            pq.push({h.capacity - h.used, hallIndex});
    }

    cout << "\nSeats allocated successfully!\n";
}

// Display seating plan
void displayPlan() {
    if (students.empty()) {
        cout << "No student records found!\n";
        return;
    }

    cout << "\n--- EXAM SEATING PLAN ---\n";
    cout << left << setw(10) << "Roll No"
         << setw(22) << "Name"
         << setw(18) << "Subject"
         << setw(15) << "Hall"
         << "Seat\n";

    for (const auto &s : students) {
        cout << left << setw(10) << s.roll
             << setw(22) << s.name
             << setw(18) << s.subject
             << setw(15)
             << (s.allocated ? s.hall : "Not allotted")
             << (s.allocated ? to_string(s.seat) : "-")
             << '\n';
    }
}

// Search student using map
void searchStudent() {
    int roll;
    cout << "Enter Roll Number to search: ";
    cin >> roll;

    auto it = studentIndex.find(roll);

    if (it == studentIndex.end()) {
        cout << "Student not found!\n";
        return;
    }

    const Student &s = students[it->second];

    cout << "\nStudent Found\n";
    cout << "Roll Number: " << s.roll << '\n';
    cout << "Name: " << s.name << '\n';
    cout << "Subject: " << s.subject << '\n';

    if (s.allocated)
        cout << "Hall: " << s.hall
             << "\nSeat Number: " << s.seat << '\n';
    else
        cout << "Seat not allocated yet.\n";
}

// Sort student records
void sortStudents() {
    sort(students.begin(), students.end(),
         [](const Student &a, const Student &b) {
             return a.roll < b.roll;
         });

    studentIndex.clear();
    for (int i = 0; i < (int)students.size(); i++)
        studentIndex[students[i].roll] = i;

    cout << "Students sorted by roll number!\n";
    displayPlan();
}

// Validate seating allocations
void validatePlan() {
    set<pair<string, int>> checkedSeats;
    bool valid = true;
    int unallocated = 0;

    for (const auto &s : students) {
        if (!s.allocated) {
            unallocated++;
            valid = false;
            continue;
        }

        if (!checkedSeats.insert({s.hall, s.seat}).second) {
            cout << "Duplicate seat found!\n";
            valid = false;
        }
    }

    for (const auto &h : halls) {
        if (h.used > h.capacity) {
            cout << "Capacity exceeded in " << h.name << '\n';
            valid = false;
        }
    }

    cout << "\n--- VALIDATION REPORT ---\n";
    cout << "Total Students: " << students.size() << '\n';
    cout << "Total Halls: " << halls.size() << '\n';
    cout << "Unallocated Students: " << unallocated << '\n';

    if (valid)
        cout << "No duplicate seats or capacity violations found.\n";
    else
        cout << "Some allocations need correction.\n";
}

int main() {
    int choice;

    do {
        cout << "\n===== EXAM SEATING ALLOCATION SYSTEM =====\n";
        cout << "1. Add Student\n";
        cout << "2. Add Examination Hall\n";
        cout << "3. Allocate Seats\n";
        cout << "4. Display Seating Plan\n";
        cout << "5. Search Student\n";
        cout << "6. Sort Students\n";
        cout << "7. Validate Seating Plan\n";
        cout << "0. Exit\n";
        cout << "Enter Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addStudent(); break;
            case 2: addHall(); break;
            case 3: allocateSeats(); break;
            case 4: displayPlan(); break;
            case 5: searchStudent(); break;
            case 6: sortStudents(); break;
            case 7: validatePlan(); break;
            case 0: cout << "Program ended.\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 0);

    return 0;
}
