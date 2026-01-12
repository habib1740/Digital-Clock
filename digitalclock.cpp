#include <iostream>
#include <ctime>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#endif

using namespace std;

struct Location {
    char name[20];
    int gmtOffset;
};

struct ClockSettings {
    char currentLocation[20];
    int gmtOffset;
    int timeFormat;   // 1 = 12-hour, 2 = 24-hour
    int running;
};

struct DateTime {
    int hour;
    int minute;
    int second;
    int day;
    int month;
    int year;
};

Location locations[] = {
    {"Islamabad", 5}, {"Karachi", 5}, {"Lahore", 5}, {"Mumbai", 5},
    {"Delhi", 5}, {"Tokyo", 9}, {"Beijing", 8}, {"Seoul", 9},
    {"Bangkok", 7}, {"Singapore", 8}, {"Dubai", 4}, {"London", 0},
    {"Paris", 1}, {"Berlin", 1}, {"Rome", 1}, {"Moscow", 3},
    {"NewYork", -5}, {"LosAngeles", -8}, {"Chicago", -6}, {"Miami", -5},
    {"Sydney", 10}, {"Melbourne", 10}, {"Cairo", 2}, {"Lagos", 1}
};

int locationCount = sizeof(locations) / sizeof(locations[0]);

void delay(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

int kbhit() {
#ifdef _WIN32
    return _kbhit();
#else
    return 0;
#endif
}

char getKey() {
#ifdef _WIN32
    return _getch();
#else
    return getchar();
#endif
}

void getCurrentTime(DateTime* dt, int offset) {
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    rawtime += offset * 3600;
    timeinfo = gmtime(&rawtime);

    dt->hour = timeinfo->tm_hour;
    dt->minute = timeinfo->tm_min;
    dt->second = timeinfo->tm_sec;
    dt->day = timeinfo->tm_mday;
    dt->month = timeinfo->tm_mon + 1;
    dt->year = timeinfo->tm_year + 1900;
}

void displayTimeFormatMenu() {
    cout << "\nTime Format Selection:\n";
    cout << "1. 12-Hour Format (AM/PM)\n";
    cout << "2. 24-Hour Format\n";
    cout << "Enter choice (1 or 2): ";
}

void displayLocationMenu() {
    cout << "\n\nAvailable Cities:\n";

    for (int i = 0; i < locationCount; i++) {
        cout << locations[i].name << "(GMT";
        if (locations[i].gmtOffset >= 0) cout << "+";
        cout << locations[i].gmtOffset << ") ";
        if ((i + 1) % 4 == 0) cout << "\n";
    }

    cout << "\nEnter city name: ";
}

int setLocationByName(ClockSettings* settings, char* name) {
    for (int i = 0; name[i]; i++) {
        if (name[i] >= 'a' && name[i] <= 'z')
            name[i] = name[i] - 'a' + 'A';
    }

    for (int i = 0; i < locationCount; i++) {
        char upperLocation[20];
        strcpy(upperLocation, locations[i].name);

        for (int j = 0; upperLocation[j]; j++) {
            if (upperLocation[j] >= 'a' && upperLocation[j] <= 'z')
                upperLocation[j] = upperLocation[j] - 'a' + 'A';
        }

        if (strcmp(upperLocation, name) == 0) {
            strcpy(settings->currentLocation, locations[i].name);
            settings->gmtOffset = locations[i].gmtOffset;
            return 1;
        }
    }
    return 0;
}

void displayDigitalClock(ClockSettings* settings) {
    DateTime dt;
    getCurrentTime(&dt, settings->gmtOffset);

    int displayHour = dt.hour;
    char ampm[4] = "";

    if (settings->timeFormat == 1) {
        if (dt.hour == 0) displayHour = 12;
        else if (dt.hour > 12) displayHour = dt.hour - 12;
        strcpy(ampm, (dt.hour >= 12) ? " PM" : " AM");
    }

    cout << "Date: ";
    if (dt.day < 10) cout << "0";
    cout << dt.day << "/";
    if (dt.month < 10) cout << "0";
    cout << dt.month << "/" << dt.year;

    cout << " | Time: ";
    if (displayHour < 10) cout << "0";
    cout << displayHour << ":";
    if (dt.minute < 10) cout << "0";
    cout << dt.minute << ":";
    if (dt.second < 10) cout << "0";
    cout << dt.second << ampm;

    cout << " | GMT";
    if (settings->gmtOffset >= 0) cout << "+";
    cout << settings->gmtOffset << " (" << settings->currentLocation << ")\r";
    cout.flush();
}

void handleInput(ClockSettings* settings) {
    if (kbhit()) {
        char key = getKey();
        if (key == 'q' || key == 'Q') {
            settings->running = 0;
        }
    }
}

void runClock(ClockSettings* settings) {
    cout << "\nClock Started!\n";
    cout << "Press 'q' to quit\n\n";

    while (settings->running) {
        displayDigitalClock(settings);
        handleInput(settings);
        delay(1000);
    }

    cout << "\n\nClock stopped. Goodbye!\n";
}

int main() {
    ClockSettings settings;
    settings.running = 1;

    cout << "Digital Clock Application\n";

    // Step 1: Ask for time format
    displayTimeFormatMenu();
    cin >> settings.timeFormat;

    // Step 2: Ask for city BEFORE starting clock
    displayLocationMenu();
    char cityName[20];
    cin >> cityName;

    while (!setLocationByName(&settings, cityName)) {
        cout << "City not found! Please enter again: ";
        cin >> cityName;
    }

    // Step 3: Start clock
    runClock(&settings);

    return 0;
}
