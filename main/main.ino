#include <Servo.h>
#include <SD.h>
#include <TMRpcm.h>
#include <SPI.h>

#define default_angle 30

// SENSORS
int servoPin = 3;
Servo servo; 

int sdSelectPin = 10;
TMRpcm speaker;

char *sound_format = (char*)"  .wav";

int speakPin = 9;

int trigPin[3] = {8, 6, 4}; // 0 - levo, 1 - srednje, 2 - desno
int echoPin[3] = {2, 7, 5};

// OTHER VARIABLES

float soundSpeed = 0.034;
int stepSize = 30;
int angle = 30;

void setup()
{
    Serial.begin(9600);

    // INITIALIZATION
    for(int i = 0; i < 3; i++)
        pinMode(trigPin[i], OUTPUT), pinMode(echoPin[i], INPUT);

    servo.attach(servoPin);
    servo.write(default_angle);

    speaker.speakerPin = speakPin;

    if (!SD.begin(sdSelectPin))
    {
        beep(97, 10, 10, 1000);
        Serial.println("SD fail!");
        return;
    }
    
    speaker.setVolume(3);
    delay(1000);

    if(!calibration()) 
    {
        beep(97, 5, 10, 1000);
        Serial.println("Calibration fail!");
        return;
    }
}

//=======================================================================// SPEAKER

void beep(int note, int n, int ntime, int dtime)
{
    for(int i = 0; i < n; i++)
    {
        tone(speakPin, note, ntime * (1000 / note));
        delay(dtime);
    }
}

void intToChar(int p)
{
    if (p < 10)
        sound_format[0] = '0', sound_format[1] = p + 48;
    else
        sound_format[0] = (p / 10) + 48, sound_format[1] = (char)(p % 10) + 48;
}

void speak(int stepNum)
{
    intToChar(stepNum);
    speaker.play((char*)sound_format);
}

//=======================================================================// CALIBRATION

int angleCalib(int k)
{
    if(k == 0)
    {
        servo.write(k);
        return getDistance(1);
    }
    
    //servo.write(k);
    Serial.println("Calibration begins!\n > Look straight at the wall; you have 3 seconds");
    speaker.play((char*)"calStart.wav");
    
    delay(3000);
    
    int distance1 = getDistance(1);
    speaker.play((char*)"calStep.wav");
    Serial.println(" > Move 1 step forward; you have 4 seconds");
    
    delay(4000);
    
    int distance2 = getDistance(1);
    speaker.play((char*)"calEnd.wav");
    Serial.println("Calibration finished!");
    
    delay(2000);

    return abs(distance1 - distance2);
}

int calibration()
{
    //int sDown = angleCalib(0);            // CALIBRATION WHEN SERVO ANGLE IS 0 (down angle)
    stepSize = angleCalib(default_angle);   // CALIBRATION WHEN SERVO ANGLE IS 30 (flat angle)
    
    if(stepSize <= 0 && stepSize >= 100) return 0;
    Serial.print("Step length: ");
    Serial.println(stepSize);
    return 1;
}

//=======================================================================// CALCULATIONS

int distanceToSteps(int distance)
{
    return distance / stepSize;
}

int getDistance(int n)
{
    digitalWrite(trigPin[n], LOW);
    delayMicroseconds(2);
    
    digitalWrite(trigPin[n], HIGH);
    delayMicroseconds(10);
    
    digitalWrite(trigPin[n], LOW);
    
    return (pulseIn(echoPin[n], HIGH) * soundSpeed) / 2;
}

//=======================================================================// OTHER FUNCTIONS

void printDistances()
{
    Serial.print("Distance 1: ");
    Serial.println(getDistance(0));
    Serial.print("Distance 2: ");
    Serial.println(getDistance(1));
    Serial.print("Distance 3: ");
    Serial.println(getDistance(2));
    Serial.println("======================");
}

//=======================================================================// CHECKING

bool checkObstacle(int distance, int dsize, int k)
{
    if(angle >= 25 && distance <= dsize && k == 1) return true;
    if(angle >= 25 && dsize <= distance && k == 0) return true;
    return false;
}

bool obstacleExists()
{
    int distance = getDistance(1);

    if(distanceToSteps(distance) <= 1)
    {
        if(checkObstacle(getDistance(0), stepSize, 0)) speaker.play((char*)"pL.wav"), delay(1800);
        if(checkObstacle(getDistance(2), stepSize, 0)) speaker.play((char*)"pR.wav"), delay(1800);
        
        return true;
    }

    if(checkObstacle(distance, 300, 1))
    {
        speak(distanceToSteps(distance));
        delay(1500);
        speaker.stopPlayback();
        return true;
    }
    
    speaker.play((char*)"pF.wav");
    delay(1500);
    speaker.stopPlayback();
    return false;
}

//=======================================================================// LOOP

void loop()
{
    delay(1000);
    obstacleExists();
}
