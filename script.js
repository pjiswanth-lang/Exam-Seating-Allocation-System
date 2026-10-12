
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
        roll, name, subject,
        hall: "", seat: 0, allocated: false
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
        alert("Enter a hall name and positive capacity.");
        return;
    }

    if (halls.some(h => h.name.toLowerCase() === name.toLowerCase())) {
        alert("This hall already exists.");
        return;
    }

    halls.push({ name, capacity, used: 0 });

    document.getElementById("hallName").value = "";
    document.getElementById("capacity").value = "";

    displayHalls();
    showStatus("Hall added successfully.");
}

function allocateSeats() {
    if (!students.length || !halls.length) {
        alert("Add students and halls first.");
        return;
    }

    const totalCapacity = halls.reduce(
        (sum, hall) => sum + hall.capacity, 0
    );

    if (students.length > totalCapacity) {
        alert("Not enough seats. Add another hall.");
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
        student.seat = ++hall.used;
        student.allocated = true;
    });

    displayStudents();
    displayHalls();
    showStatus("Seats allocated successfully.");
}

function displayStudents() {
    const table = document.getElementById("studentTable");
    table.innerHTML = "";

    students.forEach(s => {
        const row = document.createElement("tr");

        [
            s.roll, s.name, s.subject,
            s.allocated ? s.hall : "Not allotted",
            s.allocated ? s.seat : "-"
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

    halls.forEach(h => {
        const p = document.createElement("p");
        p.textContent =
            h.name + " | Capacity: " + h.capacity +
            " | Seats used: " + h.used;
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
        " | Seat: " + (student.allocated ? student.seat : "-");
}

function validatePlan() {
    const totalCapacity = halls.reduce(
        (sum, hall) => sum + hall.capacity, 0
    );

    const allocated = students.filter(s => s.allocated);
    const keys = allocated.map(s => s.hall + "|" + s.seat);
    const duplicate = new Set(keys).size !== keys.length;

    const capacityExceeded = halls.some(h =>
        allocated.filter(s => s.hall === h.name).length > h.capacity
    );

    const unallocated = students.length - allocated.length;

    if (
        students.length > 0 &&
        unallocated === 0 &&
        students.length <= totalCapacity &&
        !duplicate &&
        !capacityExceeded
    ) {
        showStatus("Validation successful. All allocations are valid.");
    } else {
        showStatus(
            "Validation failed. Check unallocated students, duplicate seats " +
            "and hall capacity."
        );
    }
}

function showStatus(message) {
    document.getElementById("status").textContent = message;
}
