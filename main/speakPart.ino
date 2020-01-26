#include "SD.h"
#define SD_ChipSelectPin 10
#include "TMRpcm.h"
#include "SPI.h"

TMRpcm speaker;
File calibrationInfo;
void setup()
{
    // speaker.speakerPin = 9;
    Serial.begin(9600);
    /*
    if (!SD.begin(SD_ChipSelectPin))
    {
        Serial.println("SD fail");
        return;
    }
    speaker.setVolume(6);
    //int obavesten = 0;
    speaker.play((char*)"desnaProhodna.wav");
    //speak(100, 200, 300, 100, 200, obavesten);
*/
    //doWhatYouNeed();
}

void loop()
{
}
void printData(int height, int stepSize)
{
    calibrationInfo = SD.open("data.txt", FILE_WRITE);
    calibrationInfo.println(height);
    calibrationInfo.println(stepSize);
    calibrationInfo.close();
}
void readData(int &height, int &stepSize)
{
    calibrationInfo = SD.open("data.txt", FILE_READ);
    char c;
    bool b = false;
    height = 0, stepSize = 0;
    while (calibrationInfo.available())
    {
        c = calibrationInfo.read();
        if (c != '\n' && (c < '0' || c > '9'))
            return;
        if (c == '\n')
            b = true;
        if (!b)
            height = height * 10 + c - 48;
        if (b)
            stepSize = stepSize * 10 + c - 48;
    }
    calibrationInfo.close();
}
bool angleChangedDrastically(double prev_left, double prev_right, double current_left, double current_right)
{
    if (prev_left - current_left >= 25)
        return 1;
    if (prev_right - current_right >= 25)
        return 1;
    return 0;
}
bool lengthChangedDrastically(double prev_left, double prev_right, double current_left, double current_right)
{
    if (prev_left - current_left >= 25)
        return 1;
    if (prev_right - current_right >= 25)
        return 1;
    return 0;
}
double calculateAngle(int distance1, int distance2)
{
    int c = sqrt(distance1 * distance1 + distance2 * distance2);
    return 180 * asin((double)(distance2) / (double)(c)) / 3.14;
}
int calculateMovingDistance(int distance1, int distance2)
{
    int c = (int)sqrt(distance1 * distance1 + distance2 * distance2);
    int s = (distance1 + distance2 + c) / 2;
    return 2 / c * (int)(sqrt(s * (s - distance1) * (s - distance2) * (s - c)));
}
int numberOfDigits(int p)
{
    if (p < 10)
        return 1;
    if (p < 100)
        return 2;
    if (p < 1000)
        return 3;
}
void speak(float distance)
{
    String s;
    char *sound_format = (char *)" .wav";
    s = (String)distance;

    for (int i = 0; i < s.length() - 1; i++)
    {
        if (s[i] == '.')
            Serial.print("."); //speaker.play((char*)"zarez.wav");
        else
            Serial.print(s[i]); //sound_format[0] = s[i], speaker.play(sound_format);
        delay(15);
    }
    Serial.println();
}
void doWhatYouNeed(int distance1, int distance2, int distance3, double prev_left, double prev_right, int &informed)
{
    if (distance1 < 400 && !informed)
    {
        speak((float)distance1 / 100);
        informed = 1;
        return;
    }
    double current_left = calculateAngle(distance1, distance2);
    double current_right = calculateAngle(distance1, distance3);

    if (angleChangedDrastically(prev_left, prev_right, current_left, current_right))
    {
        speak((float)distance2 / 100);
        delay(30);
        speak((float)distance3 / 100);
        informed = 0;
    }
}
