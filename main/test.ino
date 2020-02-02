#include "SD.h"
#define SD_ChipSelectPin 10
#include "TMRpcm.h"
#include "SPI.h"

TMRpcm speaker;
File calibrationInfo;
bool informed = false;
double prev_left = 0, prev_right = 0;
char *sound_format = (char *)"  .wav";
void setup()
{
    // speaker.speakerPin = 9;
    Serial.begin(9600);
    if (!SD.begin(SD_ChipSelectPin))
    {
        Serial.println("SD fail");
        return;
    }
    speaker.setVolume(6);
    //int obavesten = 0;
    //speaker.play((char*)"desnaProhodna.wav");
    //speak(100, 200, 300, 100, 200, obavesten);

    //doWhatYouNeed();
}

void loop()
{
    checkWalkingCondition(getDistance(0), getDistance(1), getDistance(2));
}
//should check this part if it works

//--------------------------------------------------------
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
int distanceToSteps(int distance, int stepSize)
{
    return distance / stepSize;
}
void intToChar(char *t, int p)
{
    if (p < 10)
        t[0] = '0', t[1] = p + 48;
    else
        t[0] = (p / 10) + 48, t[1] = (char)(p % 10) + 48;
}
void speak(int stepNum)
{
    intToChar(sound_format, stepNum);
    speaker.play(sound_format);
}
void checkWalkingCondition(int distance1, int distance2, int distance3)
{
    if (distance1 < 400 && !informed)
    {
        speak((float)distance1 / 100);
        delay(2000);
        speak((float)distance2 / 100);
        delay(2000);
        speak((float)distance3 / 100);
        delay(2000);
        informed = true;
        return;
    }
    double current_left = calculateAngle(distance1, distance2);
    double current_right = calculateAngle(distance1, distance3);

    if (angleChangedDrastically(prev_left, prev_right, current_left, current_right))
    {
        int a = calculateMovingDistance(distance1, distance2);
        int b = calculateMovingDistance(distance1, distance3);
        informed = false;
        delay(500);
        //CheckWalkingCondition(getDistance(1),a,b,a,b,informed);
    }
    else
        delay(1000);
    prev_left = current_left;
    prev_right = current_right;
}
