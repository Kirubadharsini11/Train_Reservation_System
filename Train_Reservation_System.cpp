#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SEATS 5
#define WAIT_LIMIT 3

// ---------------- STRUCTURES ----------------
typedef struct Passenger {
    int pnr;
    char name[50];
    int age;
    char source[30];
    char destination[30];
    int seat_no;
    struct Passenger *next;
} Passenger;

typedef struct Node {
    Passenger data;
    struct Node *next;
} Node;

Passenger *booked_head = NULL; // Linked list for booked passengers
Node *front = NULL, *rear = NULL; // Queue for waiting list
bool seats[MAX_SEATS + 1];
int next_pnr = 1000;

// ---------------- FUNCTIONS ----------------
void initialize() {
    for (int i = 1; i <= MAX_SEATS; i++)
        seats[i] = false;
}

int generate_pnr() {
    return next_pnr++;
}

int allocate_seat() {
    for (int i = 1; i <= MAX_SEATS; i++) {
        if (!seats[i]) {
            seats[i] = true;
            return i;
        }
    }
    return -1;
}

void free_seat(int seat_no) {
    if (seat_no >= 1 && seat_no <= MAX_SEATS)
        seats[seat_no] = false;
}

// Enqueue for waiting list
void enqueue(Passenger p) {
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = p;
    temp->next = NULL;

    if (rear == NULL)
        front = rear = temp;
    else {
        rear->next = temp;
        rear = temp;
    }
}

// Dequeue from waiting list when seat available
void dequeue() {
    if (front == NULL)
        return;

    Node *temp = front;
    front = front->next;
    if (front == NULL)
        rear = NULL;

    int seat_no = allocate_seat();
    if (seat_no == -1)
        return;

    Passenger *new_passenger = (Passenger *)malloc(sizeof(Passenger));
    *new_passenger = temp->data;
    new_passenger->seat_no = seat_no;
    new_passenger->next = booked_head;
    booked_head = new_passenger;

    printf("\nSeat available! Waiting passenger %s booked with Seat %d (PNR %d)\n",
           new_passenger->name, seat_no, new_passenger->pnr);

    free(temp);
}

// Book tickets for n passengers
void book_ticket() {
    int n;
    printf("\nEnter number of passengers to book: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        Passenger p;
        p.pnr = generate_pnr();
        printf("\nPassenger %d name: ", i + 1);
        scanf(" %[^\n]", p.name);
        printf("Age: ");
        scanf("%d", &p.age);
        printf("Enter source station: ");
        scanf(" %[^\n]", p.source);
        printf("Enter destination station: ");
        scanf(" %[^\n]", p.destination);

        int seat_no = allocate_seat();
        if (seat_no != -1) {
            p.seat_no = seat_no;
            Passenger *new_p = (Passenger *)malloc(sizeof(Passenger));
            *new_p = p;
            new_p->next = booked_head;
            booked_head = new_p;

            printf("Booked | PNR: %d | Seat: %d | %s ? %s | Name: %s\n",
                   p.pnr, seat_no, p.source, p.destination, p.name);
        } else {
            p.seat_no = -1;
            if ((rear == NULL && WAIT_LIMIT > 0) || (rear != NULL && WAIT_LIMIT > 0)) {
                enqueue(p);
                printf("Added to Waiting List | PNR: %d | %s ? %s | Name: %s\n",
                       p.pnr, p.source, p.destination, p.name);
            } else {
                printf("Waiting list full! Cannot add %s.\n", p.name);
            }
        }
    }
}

// Cancel ticket by PNR
void cancel_ticket() {
    int pnr;
    printf("\nEnter PNR to cancel: ");
    scanf("%d", &pnr);

    Passenger *temp = booked_head, *prev = NULL;
    while (temp != NULL && temp->pnr != pnr) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("No reservation found with PNR %d.\n", pnr);
        return;
    }

    free_seat(temp->seat_no);

    if (prev == NULL)
        booked_head = temp->next;
    else
        prev->next = temp->next;

    printf("Cancelled | PNR: %d | %s ? %s | Name: %s\n",
           temp->pnr, temp->source, temp->destination, temp->name);
    free(temp);

    dequeue(); // Move next person from waiting list to booking
}

// Linear Search (by Name or PNR)
void search_passenger() {
    int choice;
    printf("\nSearch by:\n1. Name\n2. PNR\nEnter choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        char name[50];
        printf("Enter name: ");
        scanf(" %[^\n]", name);
        Passenger *temp = booked_head;
        while (temp != NULL) {
            if (strcmp(temp->name, name) == 0) {
                printf("Found | PNR: %d | Seat: %d | %s ? %s | Name: %s\n",
                       temp->pnr, temp->seat_no, temp->source, temp->destination, temp->name);
                return;
            }
            temp = temp->next;
        }
    } else {
        int pnr;
        printf("Enter PNR: ");
        scanf("%d", &pnr);
        Passenger *temp = booked_head;
        while (temp != NULL) {
            if (temp->pnr == pnr) {
                printf("Found | PNR: %d | Seat: %d | %s ? %s | Name: %s\n",
                       temp->pnr, temp->seat_no, temp->source, temp->destination, temp->name);
                return;
            }
            temp = temp->next;
        }
    }
    printf("Passenger not found.\n");
}

// Display booked passengers
void display_reservations() {
    if (booked_head == NULL) {
        printf("\nNo current reservations.\n");
        return;
    }

    printf("\nBooked Passengers:\n");
    printf("PNR\tSeat\tName\t\tAge\tFrom\t\tTo\n");

    Passenger *temp = booked_head;
    while (temp != NULL) {
        printf("%d\t%d\t%-10s\t%d\t%-10s\t%-10s\n",
               temp->pnr, temp->seat_no, temp->name, temp->age, temp->source, temp->destination);
        temp = temp->next;
    }
}

// Display waiting list
void display_waiting_list() {
    if (front == NULL) {
        printf("\nNo passengers in waiting list.\n");
        return;
    }

    printf("\nWaiting List:\n");
    printf("PNR\tName\t\tAge\tFrom\t\tTo\n");

    Node *temp = front;
    while (temp != NULL) {
        printf("%d\t%-10s\t%d\t%-10s\t%-10s\n",
               temp->data.pnr, temp->data.name, temp->data.age, temp->data.source, temp->data.destination);
        temp = temp->next;
    }
}

// Display seat status
void display_seats() {
    printf("\nSeat Status:\n");
    for (int i = 1; i <= MAX_SEATS; i++) {
        printf("Seat %d: %s\n", i, seats[i] ? "Booked" : "Available");
    }
}

// ---------------- MAIN ----------------
int main() {
    int choice;
    initialize();

    while (1) {
        printf("\nTRAIN RESERVATION SYSTEM \n");
        printf("1. Book Ticket\n");
        printf("2. Cancel Ticket\n");
        printf("3. View Booked Passengers\n");
        printf("4. View Waiting List\n");
        printf("5. Search Passenger\n");
        printf("6. View Seat Availability\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: book_ticket(); break;
            case 2: cancel_ticket(); break;
            case 3: display_reservations(); break;
            case 4: display_waiting_list(); break;
            case 5: search_passenger(); break;
            case 6: display_seats(); break;
            case 7: printf("Exiting... Thank you!\n"); exit(0);
            default: printf("Invalid choice! Try again.\n");
        }
    }
}
