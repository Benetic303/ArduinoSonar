
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#define NOMINMAX

#include "raylib.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>

HANDLE hSerial;

// Function to open the serial port
bool openSerialPort(const wchar_t* portName) {
    hSerial = CreateFileW(portName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening serial port!" << std::endl;
        return false;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Failed to get serial parameters!" << std::endl;
        return false;
    }

    dcbSerialParams.BaudRate = CBR_9600; // Match Arduino baud rate
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Failed to set serial parameters!" << std::endl;
        return false;
    }

    // Set timeouts
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    SetCommTimeouts(hSerial, &timeouts);

    return true;
}

// Read a line terminated by '\n'
bool readSerialLine(std::string& lineOut) {
    char ch;
    DWORD bytesRead;
    lineOut.clear();

    while (true) {
        if (!ReadFile(hSerial, &ch, 1, &bytesRead, NULL)) {
            return false;
        }

        if (bytesRead == 0)
            continue; // No new data yet

        if (ch == '\n')
            break;

        lineOut += ch;
    }

    return true;
}

// Close the port
void closeSerialPort() {
    CloseHandle(hSerial);
}

int GetAngleValue(const std::string& data) {
    int angle;

    // Use istringstream to parse data
    std::istringstream ss(data);

    // Skip the "A:" label and extract the angle
    std::string angleLabel;
    ss >> angleLabel; // Discards "A:"
    ss >> angle; // Extracts the angle value

    return angle;
}
int GetDistanceValue(const std::string& data) {
    int distance;

    // Use istringstream to parse data
    std::istringstream ss(data);

    // Skip the "A:" label and extract the angle
    std::string label;
    ss >> label; // Discards "A:"
    ss >> label; // Discards the angle value

    // Skip the "D:" label and extract the distance

    ss >> label; // Discards "D:"
    ss >> distance; // Extracts the distance value

  
    return distance;
}

float DegreesToRadians(float degrees) {
    return degrees * (PI / 180);
}

int PolarToCartesianX(int angle, int distance) {
    int angleInRadians = DegreesToRadians(angle);
    return GetScreenHeight() / 2 - (distance * cos(angleInRadians));
}
int PolarToCartesianY(int angle, int distance) {
    int angleInRadians = DegreesToRadians(angle);
    return GetScreenHeight() / 2 - (distance * sin(angleInRadians));
}



struct Raypoint {
    int radiusOfPoint;
    
    //For Green part
    int angle;
    int distance;
    float angleInRadians = DegreesToRadians(angle);

    int PolarToCartesianX = distance * cos(angleInRadians);
    int PolarToCartesianY = distance * sin(angleInRadians);

    int x = (GetScreenWidth() / 2 - PolarToCartesianX);
    int y = (GetScreenHeight() / 2 - PolarToCartesianY);

    void Draw() {
        DrawCircle(x, y, radiusOfPoint, GREEN);
    }


    //For red part
    int distance2 = distance * 500;

    int PolarToCartesionX2 = distance2 * cos(angleInRadians);
    int PolarToCartesionY2 = distance2 * sin(angleInRadians);

    int x2 = (GetScreenWidth() / 2 - PolarToCartesionX2);
    int y2 = (GetScreenHeight() / 2 - PolarToCartesionY2);

    void DrawRed() {
        DrawCircle(x2, y2, 1, RED);
    }

};

struct FadingLine {
    Vector2 start;
    Vector2 end;
    float alpha = 1.0f;   // fully visible
};



int main() {
    if (!openSerialPort(L"\\\\.\\COM6")) {
        return 1;
    }

    std::cout << "Reading from serial..." << std::endl;

    int screenWidth = 1280;
    int screenHeight = 800;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "ArduinoRadar");
    SetTargetFPS(30);                                 // Set target FPS (maximum)

    std::vector<FadingLine> fadingLines;
    std::vector<FadingLine> fadingLinesRed;



  
    DrawCircleGradient(GetScreenWidth() / 2, GetScreenHeight() / 2, 400, BLUE, BLACK);         // Draw a gradient-filled circle
    DrawCircleLines(GetScreenWidth() / 2, GetScreenHeight() / 2, 100, WHITE);
    DrawCircleLines(GetScreenWidth() / 2, GetScreenHeight() / 2, 200, WHITE);
    DrawCircleLines(GetScreenWidth() / 2, GetScreenHeight() / 2, 300, WHITE);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
 
        std::string line;
        if (readSerialLine(line)) {
            //std::cout << "Received: " << line << std::endl;
        }


        int angle = GetAngleValue(line);
        int distance = GetDistanceValue(line);
        if (distance < -1) {
            distance = 400;
        }

        std::cout << "Angle: " << angle << " Distance: " << distance << std::endl;

        

        //*********** Converts type int from angle and distance to char and prints it on the screen ************
        std::string AtoString = std::to_string(angle);
        char const* AtoChar = AtoString.c_str();  //use char const* as target type
        std::string DtoString = std::to_string(distance);
        char const* DtoChar = DtoString.c_str();  //use char const* as target type

        DrawText("Angle:", 10, GetScreenHeight() / 100, 20, LIGHTGRAY);
        DrawText("Distance:", GetScreenWidth() - 140, GetScreenHeight() / 100, 20, LIGHTGRAY);
        DrawText(AtoChar, 80, GetScreenHeight() / 100, 20, LIGHTGRAY);
        DrawText(DtoChar, GetScreenWidth() - 40, GetScreenHeight() / 100, 20, LIGHTGRAY);

        DrawCircleGradient(GetScreenWidth() / 2, GetScreenHeight() / 2, 400, BLUE, BLACK);         // Draw a gradient-filled circle
        DrawCircleLines(GetScreenWidth() / 2, GetScreenHeight() / 2, 100, WHITE);
        DrawCircleLines(GetScreenWidth() / 2, GetScreenHeight() / 2, 200, WHITE);
        DrawCircleLines(GetScreenWidth() / 2, GetScreenHeight() / 2, 300, WHITE);


        Raypoint point;
        point.angle = angle;
        point.distance = distance;
        point.radiusOfPoint = 5;
       
        

        FadingLine newLine;
        newLine.start = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };
        newLine.end = { (float)point.x, (float)point.y };
        fadingLines.push_back(newLine);



        FadingLine newLineRed;
        newLineRed.start = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };
        newLineRed.end = { (float)point.x2, (float)point.y2 };
        fadingLinesRed.push_back(newLineRed);


        for (int i = 0; i < fadingLinesRed.size(); ) {
            FadingLine& lineRed = fadingLinesRed[i];

            Color fadedColor = Fade(RED, lineRed.alpha);
            DrawLineV(lineRed.start, lineRed.end, fadedColor);

            lineRed.alpha -= 0.01f; // fade speed
            if (lineRed.alpha <= 0.0f) {
                // Remove faded-out line
                fadingLinesRed.erase(fadingLinesRed.begin() + i);
            }
            else {
                ++i;
            }
        }

        for (int i = 0; i < fadingLines.size(); ) {
            FadingLine& line = fadingLines[i];

            Color fadedColor = Fade(GREEN, line.alpha);
            DrawLineV(line.start, line.end, fadedColor);

            line.alpha -= 0.01f; // fade speed
            if (line.alpha <= 0.0f) {
                // Remove faded-out line
                fadingLines.erase(fadingLines.begin() + i);
            }
            else {
                ++i;
            }
        }
        point.DrawRed();
        DrawLine(GetScreenWidth() / 2, GetScreenHeight() / 2, point.x2, point.y2, RED);
        point.Draw();
        DrawLine(GetScreenWidth() / 2, GetScreenHeight() / 2, point.x, point.y, GREEN);                // Draw a line
        //Could change DrawLine() or DrawLineV() to DrawLineEx() to also have thickness


      
      
        EndDrawing();


       
    }

 
  
    CloseWindow();
    return 0;


    closeSerialPort();
    return 0;
}