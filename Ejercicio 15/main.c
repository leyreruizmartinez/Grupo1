#include "student.h"

int main() {
    Student student;
    student.subjectCount = 0;

    printf("Enter first name: ");
    fgets(student.firstName, MAX_NAME_LENGTH, stdin);
    student.firstName[strcspn(student.firstName, "\n")] = '\0';

    printf("Enter last name: ");
    fgets(student.lastName, MAX_NAME_LENGTH, stdin);
    student.lastName[strcspn(student.lastName, "\n")] = '\0';

    printStudent(student.firstName, student.lastName);

    char choice;
    do {
        printf("\nMenu:\n");
        printf("1. Introduce subject\n");
        printf("2. Show subjects\n");
        printf("3. Calculate average\n");
        printf("q. Quit\n");
        printf("Enter choice: ");
        scanf(" %c", &choice);

        switch (choice) {
            case '1':
                addSubject(&student);
                break;
            case '2':
                showSubjects(&student);
                break;
            case '3':
                calculateAverage(&student);
                break;
            case 'q':
                freeMemory(&student);
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 'q');

    return 0;
}
