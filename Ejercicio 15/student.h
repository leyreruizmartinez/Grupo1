#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SUBJECTS 6
#define MAX_NAME_LENGTH 100

typedef struct {
    int id;
    char *name;
    float grade;
} Subject;

typedef struct {
    char firstName[MAX_NAME_LENGTH];
    char lastName[MAX_NAME_LENGTH];
    Subject *subjects[MAX_SUBJECTS];
    int subjectCount;
} Student;

void printStudent(char *firstName, char *lastName);
void addSubject(Student *student);
void showSubjects(Student *student);
void calculateAverage(Student *student);
void freeMemory(Student *student);

#endif