#include "student.h"

void printStudent(char *firstName, char *lastName) {
    printf("%s %s\n", firstName, lastName);
}

void addSubject(Student *student) {
    if (student->subjectCount >= MAX_SUBJECTS) {
        printf("Cannot add more subjects. Maximum reached.\n");
        return;
    }

    Subject *newSubject = (Subject *)malloc(sizeof(Subject));
    if (newSubject == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    printf("Enter subject ID: ");
    scanf("%d", &newSubject->id);
    getchar();

    char tempName[MAX_NAME_LENGTH];
    printf("Enter subject name: ");
    fgets(tempName, MAX_NAME_LENGTH, stdin);
    tempName[strcspn(tempName, "\n")] = '\0';

    newSubject->name = (char *)malloc(strlen(tempName) + 1);
    if (newSubject->name == NULL) {
        printf("Memory allocation failed.\n");
        free(newSubject);
        return;
    }
    strcpy(newSubject->name, tempName);

    printf("Enter grade: ");
    scanf("%f", &newSubject->grade);

    student->subjects[student->subjectCount++] = newSubject;
}

void showSubjects(Student *student) {
    if (student->subjectCount == 0) {
        printf("No subjects to display.\n");
        return;
    }
    printf("Subjects List for %s %s:\n", student->firstName, student->lastName);
    for (int i = 0; i < student->subjectCount; i++) {
        printf("ID: %d, Name: %s, Grade: %.2f\n", student->subjects[i]->id, student->subjects[i]->name, student->subjects[i]->grade);
    }
}

void calculateAverage(Student *student) {
    if (student->subjectCount == 0) {
        printf("No subjects to calculate average.\n");
        return;
    }
    float sum = 0;
    for (int i = 0; i < student->subjectCount; i++) {
        sum += student->subjects[i]->grade;
    }
    printf("Average grade: %.2f\n", sum / student->subjectCount);
}

void freeMemory(Student *student) {
    for (int i = 0; i < student->subjectCount; i++) {
        free(student->subjects[i]->name);
        free(student->subjects[i]);
    }
}