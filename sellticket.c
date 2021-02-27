#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Struct for keeping the sold tickets information
struct Sold {
    char *flight_name;
    char *passenger_name;
    char *wanted;
    char *sold;
};

struct Stack {
    int top;
    unsigned capacity;
};

// function to create a stack of given capacity. It initializes size of
// stack as 0
struct Stack *createStack(unsigned capacity) {
    struct Stack *stack = (struct Stack *) malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    stack->top = -1;
    return stack;
}

// Stack is full when top is equal to the last index
int isFull(struct Stack *stack) {
    return stack->top == stack->capacity - 1;
}

// Stack is empty when top is equal to -1
int isEmptyStack(struct Stack *stack) {
    return stack->top == -1;
}

// Function to add an item to stack.  It increases top by 1
void pushStack(struct Stack *stack, int item) {
    if (isFull(stack))
        return;
    stack->capacity = item;
}

//queue struct data for keeping the information of the people added to the queue
typedef struct node {
    char *flight_name;
    char *passenger_name;
    // Lower values indicate higher priority
    int priority;
    int size;
    struct node *next;

} Node;

// Function to Create A New Node
Node *newNode(int p, char *flight_name, char *passenger_name, int size) {
    Node *temp = (Node *) malloc(sizeof(Node));
    temp->flight_name = flight_name;
    temp->passenger_name = passenger_name;
    temp->priority = p;
    temp->size = size;
    temp->next = NULL;

    return temp;
}


// Removes the element with the
// highest priority form the list
void pop(Node **head) {
    Node *temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

// Function to push according to priority
void push(Node **head, int p, char *flight_name, char *passenger_name, int size) {
    Node *start = (*head);

    // Create new Node
    Node *temp = newNode(p, flight_name, passenger_name, size);

    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    if ((*head)->priority > p) {

        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    } else {

        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
               start->next->priority < p) {
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

// Function to check is list is empty
int isEmpty(Node **head) {
    return (*head) == NULL;
}

//function to display the tickets sold for the specified class
void displayAll(struct Sold *tickets, FILE *fout, char *class, int *sold_count) {
    int k;
    for (k = 0; k < *sold_count; k++) {
        if (!strcmp(tickets[k].sold, class)) {
            fprintf(fout, "%s\n", tickets[k].passenger_name);
        }
    }

}

//function to add people to the business queue according to their priority
void enqueueBusiness(char *scanbuf, int *bus_count, char *flight, char *passenger, int *priority_dip, Node **business) {
    *bus_count = *bus_count + 1;
    if (!strcmp(scanbuf, "diplomat")) {
        if (*bus_count == 1) {
            *business = newNode(*priority_dip, flight, passenger, *bus_count);
        } else {
            push(business, *priority_dip, flight, passenger, *bus_count);
        }
        *priority_dip = *priority_dip - 1;
    } else {
        if (*bus_count == 1) {
            *business = newNode(*bus_count, flight, passenger, *bus_count);
        } else {
            push(business, *bus_count, flight, passenger, *bus_count);
        }
    }
}


//function to add people to the economy queue according to their priority
void enqueueEconomy(char *scanbuf, int *eco_count, char *flight, char *passenger, int *priority_vet, Node **economy) {
    *eco_count = *eco_count + 1;
    if (!strcmp(scanbuf, "veteran")) {
        if (*eco_count == 1) {
            *economy = newNode(*priority_vet, flight, passenger, *eco_count);
        } else {
            push(economy, *priority_vet, flight, passenger, *eco_count);
        }
        *priority_vet = *priority_vet - 1;
    } else {
        if (*eco_count == 1) {
            *economy = newNode(*eco_count, flight, passenger, *eco_count);
        } else {
            push(economy, *eco_count, flight, passenger, *eco_count);
        }
    }
}


//function to add people to the standard queue
void enqueueStandard(int *std_count, char *flight, char *passenger, Node **standard) {
    *std_count = *std_count + 1;
    if (*std_count == 1) {
        *standard = newNode(*std_count, flight, passenger, *std_count);
    } else {
        push(standard, *std_count, flight, passenger, *std_count);
    }
}

//function to execute the info command
void info(char *scanbuf, struct Sold *tickets, Node **business, Node **economy, Node **standard, int *sold_count,
          FILE *fout) {
    int k;
    for (k = 0; k < *sold_count; k++) {
        if (!strcmp(tickets[k].passenger_name, scanbuf)) {
            fprintf(fout, "info %s %s %s %s\n", tickets[k].passenger_name, tickets[k].flight_name, tickets[k].wanted,
                    tickets[k].sold);
            return;
        }
    }
    if (!isEmpty(business)) {
        Node *iter_b = (*business);
        while (iter_b->next != NULL) {
            if (!strcmp(iter_b->passenger_name, scanbuf)) {
                fprintf(fout, "info %s %s business none", iter_b->passenger_name, iter_b->flight_name);
                return;;
            }
        }
    } else if (!isEmpty(economy)) {
        Node *iter_e = (*economy);
        while (iter_e->next != NULL) {
            if (!strcmp(iter_e->passenger_name, scanbuf)) {
                fprintf(fout, "info %s %s economy none\n", iter_e->passenger_name, iter_e->flight_name);
                return;;
            }
        }
    } else if (!isEmpty(standard)) {
        Node *iter_s = (*standard);
        while (iter_s->next != NULL) {
            if (!strcmp(iter_s->passenger_name, scanbuf)) {
                fprintf(fout, "info %s %s standard none", iter_s->passenger_name, iter_s->flight_name);
                return;
            }
        }
    } else {
        fprintf(fout, "error\n");
    }
}


//function which reads the file and executes the commands respectively
void readCommands(char *filename, char *outname) {
    FILE *fptr;
    FILE *fout;
    int len;
    int priority_dip = 0; //priority integer for diplomats
    int priority_vet = 0; //priority integer for veterans
    int bus_count = 0; //count for how many people added to the business queue
    int eco_count = 0; //count for how many people added to the economy queue
    int std_count = 0; //count for how many people added to the standard queue
    int sold_count = 0; //count for how many tickets are sold
    int standard_seat = 0; //count for how many tickets are sold for the standard seats
    int business_seat = 0; //count for how many tickets are sold for the business seats
    int economy_seat = 0; //count for how many tickets are sold for the economy seats
    fptr = fopen(filename, "r");
    fout = fopen(outname, "w");
    fseek(fptr, 0, SEEK_END); //to know the total bytes of the file
    len = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    Node *business = NULL; //queue for keeping the information of the people added to the business queue
    Node *economy = NULL; //queue for keeping the information of the people added to the economy queue
    Node *standard = NULL; //queue for keeping the information of the people added to the standard queue
    struct Sold *tickets = malloc(
            sizeof(struct Sold)); //allocate the memory for the struct array to keep the sold tickets information
    struct Stack *business_stack = NULL; //stack to keep the business seat number information
    struct Stack *economy_stack = NULL; //stack to keep the economy seat number information
    struct Stack *standard_stack = NULL; //stack to keep the standard seat number information
    char *scanbuf = malloc(sizeof(char) * len); //to allocate the memory as the length of the file
    fscanf(fptr, "%s", scanbuf);
    while (!feof(fptr)) { //read the file

        if (!strcmp(scanbuf, "") || !strcmp(scanbuf, "\n")) { //empty line

        } else if (!strcmp(scanbuf, "addseat")) { //check if the first word of the line is addseat
            fscanf(fptr, "%s", scanbuf);
            char *flight = malloc(sizeof(char) * strlen(scanbuf));
            strcpy(flight, scanbuf);
            fscanf(fptr, "%s", scanbuf);
            char *class = malloc(sizeof(char) * strlen(scanbuf));
            strcpy(class, scanbuf);
            fscanf(fptr, "%s", scanbuf);
            if (!strcmp(class, "business")) {
                if (business_stack == NULL) {
                    business_stack = createStack(atoi(scanbuf));
                    fprintf(fout, "addseat %s %d %d %d\n", flight, business_stack->capacity, 0, 0);
                } else {
                    pushStack(business_stack, business_stack->capacity + atoi(scanbuf));
                    fprintf(fout, "addseat %s %d %d %d\n", flight, business_stack->capacity, economy_stack->capacity,
                            standard_stack->capacity);
                }
            } else if (!strcmp(class, "economy")) {
                if (economy_stack == NULL) {
                    economy_stack = createStack(atoi(scanbuf));
                    fprintf(fout, "addseat %s %d %d %d\n", flight, business_stack->capacity, economy_stack->capacity,
                            0);
                } else {
                    pushStack(economy_stack, economy_stack->capacity + atoi(scanbuf));
                    fprintf(fout, "addseat %s %d %d %d\n", flight, business_stack->capacity, economy_stack->capacity,
                            standard_stack->capacity);
                }
            } else if (!strcmp(class, "standard")) {
                if (standard_stack == NULL) {
                    standard_stack = createStack(atoi(scanbuf));
                    fprintf(fout, "addseat %s %d %d %d\n", flight, business_stack->capacity, economy_stack->capacity,
                            standard_stack->capacity);
                } else {
                    pushStack(standard_stack, standard_stack->capacity + atoi(scanbuf));
                    fprintf(fout, "addseat %s %d %d %d\n", flight, business_stack->capacity, economy_stack->capacity,
                            standard_stack->capacity);
                }
            }

        } else if (!strcmp(scanbuf, "enqueue")) { //check if the first word of the line is enqueue
            fscanf(fptr, "%s", scanbuf);
            char *flight = malloc(sizeof(char) * strlen(scanbuf));
            strcpy(flight, scanbuf);
            fscanf(fptr, "%s", scanbuf);
            char *class = malloc(sizeof(char) * strlen(scanbuf));
            strcpy(class, scanbuf);
            fscanf(fptr, "%s", scanbuf);
            char *passenger = malloc(sizeof(char) * strlen(scanbuf));
            strcpy(passenger, scanbuf);
            fscanf(fptr, "%s", scanbuf);
            if (!strcmp(class, "business")) {
                enqueueBusiness(scanbuf, &bus_count, flight, passenger, &priority_dip, &business);
                fprintf(fout, "queue %s %s %s %d\n", business->flight_name, business->passenger_name, class, bus_count);

            } else if (!strcmp(class, "economy")) {
                enqueueEconomy(scanbuf, &eco_count, flight, passenger, &priority_vet, &economy);
                fprintf(fout, "queue %s %s %s %d\n", economy->flight_name, economy->passenger_name, class, eco_count);

            } else if (!strcmp(class, "standard")) {
                enqueueStandard(&std_count, flight, passenger, &standard);
                fprintf(fout, "queue %s %s %s %d\n", standard->flight_name, standard->passenger_name, class, std_count);

            }

            if (strcmp(scanbuf, "diplomat") != 0 && strcmp(scanbuf, "veteran") != 0) {
                continue;
            }

        } else if (!strcmp(scanbuf, "sell")) { //check if the first word of the line is sell
            fscanf(fptr, "%s", scanbuf);
            int p;
            if (business_stack->capacity <= bus_count) {
                for (p = 0; p < business_stack->capacity; p++) {
                    if (!strcmp(business->flight_name, scanbuf)) {
                        sold_count++;
                        tickets = realloc(tickets, sizeof(struct Sold) * sold_count);
                        tickets[sold_count - 1].flight_name = business->flight_name;
                        tickets[sold_count - 1].passenger_name = business->passenger_name;
                        tickets[sold_count - 1].wanted = "business";
                        tickets[sold_count - 1].sold = "business";
                        pop(&business);
                        business_seat++;
                    }
                }
            } else {
                for (p = 0; p < bus_count; p++) {
                    if (!strcmp(business->flight_name, scanbuf)) {
                        sold_count++;
                        tickets = realloc(tickets, sizeof(struct Sold) * sold_count);
                        tickets[sold_count - 1].flight_name = business->flight_name;
                        tickets[sold_count - 1].passenger_name = business->passenger_name;
                        tickets[sold_count - 1].wanted = "business";
                        tickets[sold_count - 1].sold = "business";
                        pop(&business);
                        business_seat++;
                    }
                }
            }
            int a;
            if (economy_stack->capacity <= eco_count) {
                for (a = 0; a < economy_stack->capacity; a++) {
                    if (!strcmp(economy->flight_name, scanbuf)) {
                        sold_count++;
                        tickets = realloc(tickets, sizeof(struct Sold) * sold_count);
                        tickets[sold_count - 1].flight_name = economy->flight_name;
                        tickets[sold_count - 1].passenger_name = economy->passenger_name;
                        tickets[sold_count - 1].wanted = "economy";
                        tickets[sold_count - 1].sold = "economy";
                        pop(&economy);
                        economy_seat++;
                    }
                }
            } else {
                for (a = 0; a < eco_count; a++) {
                    if (!strcmp(economy->flight_name, scanbuf)) {
                        sold_count++;
                        tickets = realloc(tickets, sizeof(struct Sold) * sold_count);
                        tickets[sold_count - 1].flight_name = economy->flight_name;
                        tickets[sold_count - 1].passenger_name = economy->passenger_name;
                        tickets[sold_count - 1].wanted = "economy";
                        tickets[sold_count - 1].sold = "economy";
                        pop(&economy);
                        economy_seat++;
                    }
                }
            }
            int k;
            if (standard_stack->capacity <= std_count) {
                for (k = 0; k < standard_stack->capacity; k++) {
                    if (!strcmp(standard->flight_name, scanbuf)) {
                        sold_count++;
                        tickets = realloc(tickets, sizeof(struct Sold) * sold_count);
                        tickets[sold_count - 1].flight_name = standard->flight_name;
                        tickets[sold_count - 1].passenger_name = standard->passenger_name;
                        tickets[sold_count - 1].wanted = "standard";
                        tickets[sold_count - 1].sold = "standard";
                        pop(&standard);
                        standard_seat++;
                    }
                }
            } else {
                for (k = 0; k < std_count; k++) {
                    if (!strcmp(standard->flight_name, scanbuf)) {
                        sold_count++;
                        tickets = realloc(tickets, sizeof(struct Sold) * sold_count);
                        tickets[sold_count - 1].flight_name = standard->flight_name;
                        tickets[sold_count - 1].passenger_name = standard->passenger_name;
                        tickets[sold_count - 1].wanted = "standard";
                        tickets[sold_count - 1].sold = "standard";
                        pop(&standard);
                        standard_seat++;
                    }
                }
            }
            if (standard_stack->capacity - standard_seat > 0) {
                if ((bus_count - business_seat) > standard_stack->capacity - standard_seat) {
                    for (p = 0; p < standard_stack->capacity - standard_seat; p++) {
                        if (!strcmp(business->flight_name, scanbuf)) {
                            sold_count++;
                            tickets = realloc(tickets, sizeof(struct Sold) * sold_count);
                            tickets[sold_count - 1].flight_name = business->flight_name;
                            tickets[sold_count - 1].passenger_name = business->passenger_name;
                            tickets[sold_count - 1].wanted = "business";
                            tickets[sold_count - 1].sold = "standard";
                            pop(&business);
                        }
                    }
                    standard_seat += (standard_stack->capacity - standard_seat);
                } else if ((bus_count - business_seat) < standard_stack->capacity - standard_seat) {
                    for (p = 0; p < bus_count - business_seat; p++) {
                        if (!strcmp(business->flight_name, scanbuf)) {
                            sold_count++;
                            tickets = realloc(tickets, sizeof(struct Sold) * sold_count);
                            tickets[sold_count - 1].flight_name = business->flight_name;
                            tickets[sold_count - 1].passenger_name = business->passenger_name;
                            tickets[sold_count - 1].wanted = "business";
                            tickets[sold_count - 1].sold = "standard";
                            pop(&business);
                        }
                    }
                    standard_seat += (bus_count - business_seat);
                }
            }
            if (standard_stack->capacity - standard_seat > 0) {
                if ((eco_count - economy_seat) > standard_stack->capacity - standard_seat) {
                    for (p = 0; p < standard_stack->capacity - standard_seat; p++) {
                        if (!strcmp(economy->flight_name, scanbuf)) {
                            sold_count++;
                            tickets = realloc(tickets, sizeof(struct Sold) * sold_count);
                            tickets[sold_count - 1].flight_name = economy->flight_name;
                            tickets[sold_count - 1].passenger_name = economy->passenger_name;
                            tickets[sold_count - 1].wanted = "economy";
                            tickets[sold_count - 1].sold = "standard";
                            pop(&economy);
                        }
                    }
                    standard_seat += (standard_stack->capacity - standard_seat);
                } else if ((eco_count - economy_seat) < standard_stack->capacity - standard_seat) {
                    for (p = 0; p < eco_count - economy_seat; p++) {
                        if (!strcmp(economy->flight_name, scanbuf)) {
                            sold_count++;
                            tickets = realloc(tickets, sizeof(struct Sold) * sold_count);
                            tickets[sold_count - 1].flight_name = economy->flight_name;
                            tickets[sold_count - 1].passenger_name = economy->passenger_name;
                            tickets[sold_count - 1].wanted = "economy";
                            tickets[sold_count - 1].sold = "standard";
                            pop(&business);
                        }
                    }
                    standard_seat += (eco_count - economy_seat);
                }
            }


        } else if (!strcmp(scanbuf, "close")) { //check if the first word of the line is close

        } else if (!strcmp(scanbuf, "report")) { //check if the first word of the line is report
            fscanf(fptr, "%s", scanbuf);
            fprintf(fout, "report %s\n", scanbuf);
            fprintf(fout, "business %d\n", business_seat);
            displayAll(tickets, fout, "business", &sold_count);
            fprintf(fout, "economy %d\n", economy_seat);
            displayAll(tickets, fout, "economy", &sold_count);
            fprintf(fout, "standard %d\n", standard_seat);
            displayAll(tickets, fout, "standard", &sold_count);
        } else if (!strcmp(scanbuf, "info")) {  //check if the first word of the line is info
            fscanf(fptr, "%s", scanbuf);
            info(scanbuf, tickets, &business, &economy, &standard, &sold_count, fout);
        }
        fscanf(fptr, "%s", scanbuf);

    }
    if (!isEmpty(&business)) { //free the business queue
        while ((business)->next != NULL) {
            pop(&business);
        }
        pop(&business);
    }
    if (!isEmpty(&economy)) { //free the economy queue
        while ((economy)->next != NULL) {
            pop(&economy);
        }
        pop(&economy);
    }
    if (!isEmpty(&standard)) { //free the standard queue
        while ((standard)->next != NULL) {
            pop(&standard);
        }
        pop(&standard);
    }
    free(tickets); //free the tickets struct array
    free(business_stack); //free the business stack
    free(economy_stack); //free the economy stack
    free(standard_stack); //free the standard stack
    free(scanbuf); //free the char array for reading the file
    fclose(fptr); //close the input file which has been opened before
    fclose(fout); //close the output file which has been opened before
}


int main(int argc, char *argv[]) {
    readCommands(argv[1], argv[2]);
    return 0;
}