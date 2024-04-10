#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CODE 10
#define MAX_NAME 51
#define MAX_LINE 256
#define MAX_COURSE_ID 11
#define MAX_COURSES 5
#define MAX_COURSE_NAME 30
#define MAX_ID 11
#define MAX_GRADE 5
#define MAX_ATTENDANCE 20

// Structure definition
typedef struct {
    char courseCode[MAX_CODE];
    char courseName[MAX_NAME];
    int numStudentsEnrolled;
    char lecturerName[MAX_NAME];
} Course;

typedef struct {
    char studentID[MAX_ID];
    char name[MAX_NAME];
    int numCoursesEnrolled;
    char coursesEnrolled[MAX_COURSES][MAX_COURSE_NAME];
} Student;

// Function prototypes
void lecturerMenu();
void viewGrades();
void modifyGrades();
void viewAttendance();
void updateAttendance();


int main() {
    lecturerMenu();
    return 0;
}

void lecturerMenu() {
    int choice;
    do {
        printf("\n********** Lecturer Menu **********\n");
        printf("1. View Grades\n");
        printf("2. Modify Grades\n");
        printf("3. View Attendance\n");
        printf("4. Update Attendance\n");
        printf("5. Logout\n");
        printf("***********************************\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        while (getchar() != '\n');

        switch(choice) {
            case 1:
                viewGrades();
                break;
            case 2:
                modifyGrades();
                break;
            case 3:
                viewAttendance();
                break;
            case 4:
                updateAttendance();
                break;
            case 5:
                printf("Logging out...\n");
                return;
            default:
                printf("Invalid choice. Please enter a number between 1 and 5.\n");
                break;
        }
    } while(choice != 5);
}


void viewGrades() {
    FILE* file = fopen("grades.txt", "r");
    if (!file) {
        perror("Unable to open the file");
        return;
    }

    char targetStudentID[MAX_ID];
    int validID = 0; // Flag to indicate whether a valid student ID has been entered

    do {
        printf("Enter the student ID to view grades (or type 'exit' to return): ");
        scanf("%10s", targetStudentID);
        if (strcmp(targetStudentID, "exit") == 0) {
            fclose(file);
            return; // Exit if the user types 'exit'
        }

        // Clear the input buffer to handle any leftover input
        while (getchar() != '\n');

        char line[MAX_LINE];
        int found = 0; // To check if we have found any grades for the given student ID
        // Reset the file pointer to the beginning of the file
        fseek(file, 0, SEEK_SET);

        while (fgets(line, sizeof(line), file)) {
            char studentID[MAX_ID], studentName[MAX_NAME], courseCode[MAX_COURSE_ID], grade[MAX_GRADE];
            if (sscanf(line, "%[^,],%[^,],%[^,],%s", studentID, studentName, courseCode, grade) == 4) {
                if (strcmp(studentID, targetStudentID) == 0) { // Check if the student ID matches
                    if (!found) {
                        printf("Grades for Student ID %s:\n", targetStudentID);
                        printf("ID\tStudent Name\t\t\tCourse ID\tGrade\n");
                        printf("----------------------------------------------------------------\n");
                    }
                    printf("%s\t%-20s\t\t%s\t\t%s\n", studentID, studentName, courseCode, grade);
                    found = 1;
                    validID = 1; // Set the flag to indicate a valid ID was found
                }
            }
        }

        if (!found) {
            printf("Invalid choice, please try again.\n");
        }
    } while (!validID);

    fclose(file);
}

const char* validGrades[] = {"A+", "A", "B+", "B", "C+", "C", "C-", "D", "F"};
const int numValidGrades = sizeof(validGrades) / sizeof(validGrades[0]);

int isValidGrade(const char* grade) {
    for (int i = 0; i < numValidGrades; ++i) {
        if (strcmp(grade, validGrades[i]) == 0) {
            return 1;
        }
    }
    return 0; 
}


int isValidStudentID(const char* studentID) {
    FILE* file = fopen("grades.txt", "r");
    char line[MAX_LINE], id[MAX_ID];
    if (!file) {
        perror("Unable to open the file");
        return 0;
    }
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,]", id);
        if (strcmp(id, studentID) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}


int isValidCourseCode(const char* courseCode) {
    FILE* file = fopen("grades.txt", "r");
    if (!file) {
        perror("Unable to open the file");
        return 0;
    }

    char line[MAX_LINE];
    int isValid = 0; // Flag to indicate the validity of the course code

    while (fgets(line, sizeof(line), file) != NULL) {
        char readCourseCode[MAX_COURSE_ID];
        // Correctly parsing the course code as the second item
        sscanf(line, "%*[^,],%[^,]", readCourseCode);
        if (strcmp(readCourseCode, courseCode) == 0) {
            isValid = 1; // Set flag to true if a matching course code is found
            break; // Exit loop once a match is found
        }
    }

    fclose(file);
    return isValid;
}


void modifyGrades() {
    char targetStudentID[MAX_ID];
    char targetCourseCode[MAX_COURSE_ID];
    char newGrade[MAX_GRADE];

    int validStudentID = 0;
    int validCourseCode = 0;

    // Loop until a valid student ID is provided
    while (!validStudentID) {
        printf("Enter Student ID to modify grade: ");
        scanf("%10s", targetStudentID);
        getchar(); // Clear the newline character
        
        if (isValidStudentID(targetStudentID)) {
            validStudentID = 1;
        } else {
            printf("Invalid student ID, please try again.\n");
        }
    }

    // Loop until a valid course code is provided
    while (!validCourseCode) {
        printf("Enter Course Code: ");
        scanf("%10s", targetCourseCode);
        getchar(); // Clear the newline character
        
        if (isValidCourseCode(targetCourseCode)) {
            validCourseCode = 1;
        } else {
            printf("Invalid course code, please try again.\n");
        }
    }

    // Loop until a valid grade is provided
    while (1) {
        printf("Enter new Grade (A+, A, B+, B, C+, C, C-, D, F): ");
        scanf("%4s", newGrade);
        getchar(); // Clear the newline character

        if (isValidGrade(newGrade)) {
            break;
        } else {
            printf("Invalid Grade, please try again.\n");
        }
    }

    // File update logic
    FILE* file = fopen("grades.txt", "r");
    FILE* tempFile = fopen("temp_grades.txt", "w");
    char line[MAX_LINE];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        char studentID[MAX_ID], courseCode[MAX_COURSE_ID], grade[MAX_GRADE];
        sscanf(line, "%[^,],%[^,],%s", studentID, courseCode, grade);

        if (strcmp(studentID, targetStudentID) == 0 && strcmp(courseCode, targetCourseCode) == 0) {
            fprintf(tempFile, "%s,%s,%s\n", studentID, courseCode, newGrade);
            found = 1;
        } else {
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(file);
    fclose(tempFile);

    // Only update the original file if a matching record was found
    if (found) {
        remove("grades.txt");
        rename("temp_grades.txt", "grades.txt");
        printf("Updated Successfully.\n");
    } else {
        remove("temp_grades.txt"); // Cleanup: remove the temp file as it's unnecessary
        printf("No matching record found to update.\n");
    }
}


void viewAttendance() {
    char targetStudentID[MAX_ID];
    int validID = 0; // Flag to indicate whether a valid student ID has been entered
    FILE* file;

    do {
        printf("Enter the student ID to view attendance (or type 'exit' to return): ");
        scanf("%10s", targetStudentID);
        getchar(); // Clear the newline character left in the input buffer

        if (strcmp(targetStudentID, "exit") == 0) {
            return; // Exit if the user types 'exit'
        }

        file = fopen("attendance.txt", "r");
        if (!file) {
            perror("Unable to open the file");
            return;
        }

        char line[MAX_LINE];
        int found = 0; // To check if we have found any attendance records for the given student ID
        
        printf("Attendance Records for Student ID %s:\n", targetStudentID);
        printf("ID\tStudent Name\t\t\tCourse ID\tAttendance\n");
        printf("----------------------------------------------------------\n");

        while (fgets(line, sizeof(line), file)) {
            char studentID[MAX_ID], studentName[MAX_NAME], courseID[MAX_COURSE_ID], attendanceStatus[MAX_ATTENDANCE];
            if (sscanf(line, "%[^,],%[^,],%[^,],%[^\n]", studentID, studentName, courseID, attendanceStatus) == 4) {
                if (strcmp(studentID, targetStudentID) == 0) { // Check if the student ID matches
                    printf("%s\t%-20s\t\t%s\t\t%s\n", studentID, studentName, courseID, attendanceStatus);
                    found = 1;
                    validID = 1; // Indicate a valid student ID has been found
                }
            }
        }

        fclose(file);

        if (!found) {
            printf("Invalid student ID, please try again.\n");
        }
    } while (!validID); // Repeat if no valid student ID was found
}

int isValidStudentID(const char* studentID);
int isValidCourseCode(const char* courseCode);

void updateAttendance() {
    char targetStudentID[MAX_ID], targetCourseID[MAX_COURSE_ID];
    int newAttendance;
    char buffer[256]; // For reading input

    printf("Enter Student ID to update attendance or 'exit' to quit: ");
    scanf("%s", targetStudentID);
    if (strcmp(targetStudentID, "exit") == 0) return;
    while (!isValidStudentID(targetStudentID)) {
        printf("Invalid student ID, please try again or enter 'exit' to quit: ");
        scanf("%s", targetStudentID);
        if (strcmp(targetStudentID, "exit") == 0) return;
    }

    printf("Enter Course ID or 'exit' to quit: ");
    scanf("%s", targetCourseID);
    if (strcmp(targetCourseID, "exit") == 0) return;
    while (!isValidCourseCode(targetCourseID)) {
        printf("Invalid course code, please try again or enter 'exit' to quit: ");
        scanf("%s", targetCourseID);
        if (strcmp(targetCourseID, "exit") == 0) return;
    }

    do {
        printf("Enter new Attendance Status (0-100): ");
        scanf("%s", buffer);
        if (sscanf(buffer, "%d", &newAttendance) == 1 && newAttendance >= 0 && newAttendance <= 100) {
            break; // Valid input
        } else {
            printf("Invalid number, please try again.\n");
        }
    } while (1);

    FILE* file = fopen("attendance.txt", "r");
    FILE* tempFile = fopen("temp_attendance.txt", "w");
    if (!file || !tempFile) {
        perror("File opening failed");
        return;
    }

    char line[MAX_LINE];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char readStudentID[MAX_ID], readCourseID[MAX_COURSE_ID];
        // Parse the line, check if it matches the input
        if (sscanf(line, "%[^,],%*[^,],%[^,],", readStudentID, readCourseID) == 2 &&
            strcmp(readStudentID, targetStudentID) == 0 && strcmp(readCourseID, targetCourseID) == 0) {
            // Matching record found, update it
            fprintf(tempFile, "%s,%s,%s,%d%%\n", readStudentID, targetCourseID, newAttendance);
            found = 1;
        } else {
            // Write the line as it is
            fputs(line, tempFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove("attendance.txt");
        rename("temp_attendance.txt", "attendance.txt");
        printf("Attendance updated successfully.\n");
    } else {
        remove("temp_attendance.txt");
        printf("No matching record found to update.\n");
    }
}