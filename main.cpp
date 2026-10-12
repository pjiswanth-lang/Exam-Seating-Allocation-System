
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <string>
using namespace std;

struct Student {
    int roll;
    string name, subject, hall;
    int seat = 0;
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

// Graph: each vertex represents a seat.
// Edges connect adjacent seats in the same hall.
vector<vector<int>> graph;
vector<string> seatLabels;

void rebuildIndex() {
    studentIndex.clear();

    for (int i = 0; i < (int)students.size(); i++)
        studentIndex[students[i].roll] = i;
}

void addStudent() {
    Student s;

    cout << "Enter roll number: ";
    cin >> s.roll;

    if (studentIndex.count(s.roll)) {
        cout << "Duplicate roll number!\n";
        return;
    }

    cin.ignore(10000, '\n');

    cout << "Enter student name: ";
    getline(cin, s.name);

    cout << "Enter subject: ";
    getline(cin, s.subject);

    students.push_back(s);
    rebuildIndex();

    cout << "Student added successfully.\n";
}

void addHall() {
    Hall h;

    cin.ignore(10000, '\n');

    cout << "Enter hall name: ";
    getline(cin, h.name);

    for (const Hall &existing : halls) {
        if (existing.name == h.name) {
            cout << "Hall already exists!\n";
            return;
        }
    }

    cout << "Enter hall capacity: ";
    cin >> h.capacity;

    if (h.capacity <= 0) {
        cout << "Capacity must be positive.\n";
        return;
    }

    halls.push_back(h);
    cout << "Hall added successfully.\n";
}

void allocateSeats() {
    if (students.empty() || halls.empty()) {
        cout << "Add students and halls first.\n";
        return;
    }

    int totalCapacity = 0;

    for (const Hall &h : halls)
        totalCapacity += h.capacity;

    if (students.size() > (size_t)totalCapacity) {
        cout << "Insufficient hall capacity. Allocation cancelled.\n";
        return;
    }

    sort(students.begin(), students.end(),
         [](const Student &a, const Student &b) {
             return a.roll < b.roll;
         });

    rebuildIndex();

    for (Student &s : students) {
        s.allocated = false;
        s.hall = "";
        s.seat = 0;
    }

    for (Hall &h : halls)
        h.used = 0;

    // Select the hall with the most remaining seats.
    priority_queue<pair<int, int>> pq;

    for (int i = 0; i < (int)halls.size(); i++)
        pq.push({halls[i].capacity, i});

    for (Student &s : students) {
        while (!pq.empty() &&
               halls[pq.top().second].used >=
               halls[pq.top().second].capacity) {
            pq.pop();
        }

        if (pq.empty()) {
            cout << "Allocation failed.\n";
            return;
        }

        int hi = pq.top().second;
        pq.pop();

        Hall &h = halls[hi];

        s.hall = h.name;
        s.seat = ++h.used;
        s.allocated = true;

        int remaining = h.capacity - h.used;

        if (remaining > 0)
            pq.push({remaining, hi});
    }

    cout << "Seat allocation completed.\n";
    buildSeatGraph();
}

void buildSeatGraph() {
    graph.clear();
    seatLabels.clear();

    int vertex = 0;

    for (const Hall &h : halls) {
        int first = vertex;

        for (int seat = 1; seat <= h.capacity; seat++) {
            seatLabels.push_back(
                h.name + "-Seat-" + to_string(seat)
            );
            graph.push_back({});
            vertex++;
        }

        // Adjacent seats form graph edges.
        for (int i = first; i + 1 < vertex; i++) {
            graph[i].push_back(i + 1);
            graph[i + 1].push_back(i);
        }
    }
}

void displayPlan() {
    cout << "\n--- STUDENT SEATING PLAN ---\n";

    cout << "Roll\tName\t\tSubject\t\tHall\tSeat\n";

    for (const Student &s : students) {
        cout << s.roll << "\t"
             << s.name << "\t\t"
             << s.subject << "\t\t"
             << (s.allocated ? s.hall : "Not allotted")
             << "\t"
             << (s.allocated ? to_string(s.seat) : "-")
             << '\n';
    }

    cout << "\n--- HALL-WISE PLAN ---\n";

    for (const Hall &h : halls) {
        cout << "\nHall: " << h.name
             << " | Capacity: " << h.capacity << '\n';

        for (const Student &s : students) {
            if (s.allocated && s.hall == h.name) {
                cout << "Seat " << s.seat
                     << ": " << s.name
                     << " (" << s.roll << ")\n";
            }
        }
    }
}

void searchStudent() {
    int roll;

    cout << "Enter roll number: ";
    cin >> roll;

    auto it = studentIndex.find(roll);

    if (it == studentIndex.end()) {
        cout << "Student not found.\n";
        return;
    }

    const Student &s = students[it->second];

    cout << "Name: " << s.name
         << "\nSubject: " << s.subject << '\n';

    if (s.allocated)
        cout << "Hall: " << s.hall
             << "\nSeat: " << s.seat << '\n';
    else
        cout << "Seat not allocated.\n";
}

void sortStudents() {
    sort(students.begin(), students.end(),
         [](const Student &a, const Student &b) {
             return a.roll < b.roll;
         });

    rebuildIndex();

    cout << "Students sorted by roll number.\n";
    displayPlan();
}

void validatePlan() {
    set<pair<string, int>> usedSeats;
    map<string, int> hallCounts;
    int unallocated = 0;
    bool valid = true;

    for (const Student &s : students) {
        if (!s.allocated) {
            unallocated++;
            continue;
        }

        if (!usedSeats.insert({s.hall, s.seat}).second) {
            cout << "Duplicate seat found!\n";
            valid = false;
        }

        hallCounts[s.hall]++;
    }

    for (const Hall &h : halls) {
        if (hallCounts[h.name] > h.capacity) {
            cout << "Capacity exceeded in " << h.name << '\n';
            valid = false;
        }
    }

    cout << "\n--- VALIDATION REPORT ---\n";
    cout << "Students: " << students.size() << '\n';
    cout << "Halls: " << halls.size() << '\n';
    cout << "Unallocated students: " << unallocated << '\n';

    if (valid && unallocated == 0)
        cout << "All allocations are valid.\n";
    else
        cout << "Check the allocation results.\n";
}

void bfs() {
    if (graph.empty()) {
        cout << "Allocate seats first.\n";
        return;
    }

    vector<bool> visited(graph.size(), false);
    queue<int> q;

    q.push(0);
    visited[0] = true;

    cout << "BFS traversal: ";

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        cout << seatLabels[u] << " ";

        for (int v : graph[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }

    cout << '\n';
}

void dfsVisit(int u, vector<bool> &visited) {
    visited[u] = true;
    cout << seatLabels[u] << " ";

    for (int v : graph[u]) {
        if (!visited[v])
            dfsVisit(v, visited);
    }
}

void dfs() {
    if (graph.empty()) {
        cout << "Allocate seats first.\n";
        return;
    }

    vector<bool> visited(graph.size(), false);

    cout << "DFS traversal: ";
    dfsVisit(0, visited);
    cout << '\n';
}

int main() {
    int choice;

    do {
        cout << "\n=== EXAM SEATING ALLOCATION SYSTEM ===\n";
        cout << "1. Add Student\n";
        cout << "2. Add Examination Hall\n";
        cout << "3. Allocate Seats\n";
        cout << "4. Display Seating Plan\n";
        cout << "5. Search Student\n";
        cout << "6. Sort Students\n";
        cout << "7. Validate Allocation\n";
        cout << "8. BFS Graph Traversal\n";
        cout << "9. DFS Graph Traversal\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Exiting.\n";
            break;
        }

        switch (choice) {
            case 1: addStudent(); break;
            case 2: addHall(); break;
            case 3: allocateSeats(); break;
            case 4: displayPlan(); break;
            case 5: searchStudent(); break;
            case 6: sortStudents(); break;
            case 7: validatePlan(); break;
            case 8: bfs(); break;
            case 9: dfs(); break;
            case 0: cout << "Program ended.\n"; break;
            default: cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    return 0;
}
