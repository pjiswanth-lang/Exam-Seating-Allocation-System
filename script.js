
let students = [];
let halls = [];

function addStudent() {
    const roll = Number(document.getElementById("roll").value);
    const name = document.getElementById("studentName").value.trim();
    const subject = document.getElementById("subject").value.trim();

    if (!Number.isInteger(roll) || roll <= 0 || !name || !subject) {
        alert("Enter a valid roll number, name and subject.");
        return;
    }

    if (students.some(s => s.roll === roll)) {
        alert("This roll number already exists.");
        return;
    }

    students.push({
        roll: roll,
        name: name,
        subject: subject,
        hall: "",
        seat: "",
        allocated: false
    });

    document.getElementById("roll").value = "";
    document.getElementById("studentName").value = "";
    document.getElementById("subject").value = "";

    displayStudents();
    showStatus("Student added successfully.");
}

function addHall() {
    const name = document.getElementById("hallName").value.trim();
    const capacity = Number(document.getElementById("capacity").value);

    if (!name || !Number.isInteger(capacity) || capacity <= 0) {
        alert("Enter a hall name and a positive whole-number capacity.");
        return;
    }

    if (halls.some(h => h.name.toLowerCase() === name.toLowerCase())) {
        alert("This hall already exists.");
        return;
    }

    halls.push({
        name: name,
        capacity: capacity,
        used: 0
    });

    document.getElementById("hallName").value = "";
    document.getElementById("capacity").value = "";

    displayHalls();
    showStatus("Examination hall added successfully.");
}

function allocateSeats() {
    if (students.length === 0 || halls.length === 0) {
        alert("Add students and examination halls first.");
        return;
    }

    const totalCapacity = halls.reduce(
        (total, hall) => total + hall.capacity, 0
    );

    if (students.length > totalCapacity) {
        alert("Not enough seats! Add more halls or increase capacity.");
        return;
    }

    students.sort((a, b) => a.roll - b.roll);

    halls.forEach(h => h.used = 0);

    let hallIndex = 0;

    students.forEach(student => {
        while (
            hallIndex < halls.length &&
            halls[hallIndex].used >= halls[hallIndex].capacity
        ) {
            hallIndex++;
        }

        const hall = halls[hallIndex];

        student.hall = hall.name;
        student.seat = hall.used + 1;
        student.allocated = true;

        hall.used++;
    });

    displayStudents();
    displayHalls();
    showStatus("All students have been allocated seats.");
}

function displayStudents() {
    const table = document.getElementById("studentTable");
    table.innerHTML = "";

    students.forEach(student => {
        const row = document.createElement("tr");

        [
            student.roll,
            student.name,
            student.subject,
            student.allocated ? student.hall : "Not allotted",
            student.allocated ? student.seat : "-"
        ].forEach(value => {
            const cell = document.createElement("td");
            cell.textContent = value;
            row.appendChild(cell);
        });

        table.appendChild(row);
    });
}

function displayHalls() {
    const list = document.getElementById("hallList");
    list.innerHTML = "";

    halls.forEach(hall => {
        const p = document.createElement("p");
        p.textContent =
            hall.name + " — Capacity: " + hall.capacity +
            ", Seats used: " + hall.used;
        list.appendChild(p);
    });
}

function searchStudent() {
    const roll = Number(document.getElementById("searchRoll").value);

    const student = students.find(s => s.roll === roll);
    const result = document.getElementById("searchResult");

    if (!student) {
        result.textContent = "Student not found.";
        return;
    }

    result.textContent =
        "Name: " + student.name +
        " | Subject: " + student.subject +
        " | Hall: " + (student.allocated ? student.hall : "Not allotted") +
        " | Seat: " + (student.allocated ? student.seat : "Not allotted");
}

function validatePlan() {
    const totalCapacity = halls.reduce(
        (total, hall) => total + hall.capacity, 0
    );

    const allocated = students.filter(s => s.allocated);
    const seatKeys = allocated.map(s => s.hall + "|" + s.seat);
    const duplicateSeats = new Set(seatKeys).size !== seatKeys.length;

    const capacityExceeded = halls.some(
        h => allocated.filter(s => s.hall === h.name).length > h.capacity
    );

    const allAllocated =
        students.length > 0 && allocated.length === students.length;

    const valid =
        allAllocated &&
        students.length <= totalCapacity &&
        !duplicateSeats &&
        !capacityExceeded;

    if (valid) {
        showStatus("Validation successful: no duplicate seats or capacity violations.");
    } else {
        showStatus(
            "Validation failed. Check seat allocation, duplicate seats, " +
            "and hall capacity."
        );
    }
}

function showStatus(message) {
    document.getElementById("status").textContent = message;
}
