#include <Servo.h> 
#define us_N 3
#define default_angle 30

// SENSORS
int servoPin = 3;
Servo servo; 

int speakerPin = 2;

int trigPin[us_N] = {13, 6, 4}; // 0 - levo, 1 - srednje, 2 - desno
int echoPin[us_N] = {12, 7, 5};

// OTHER VARIABLES
int delayTime = 20;

int stepUp = 0;
int stepDown = 0;

int inf = 2000; // 2323 max

void setup()
{
    Serial.begin(9600);

    // INITIALIZATION
    for(int i = 0; i < us_N; i++)
        pinMode(trigPin[i], OUTPUT), pinMode(echoPin[i], INPUT);

    servo.attach(servoPin);
    servo.write(default_angle);

    beep(91, 5, 10, 100);

    Serial.println(calibration());
}

//=======================================================================// SPEAKER

void beep(int note, int n, int ntime, int dtime)
{
    for(int i = 0; i < n; i++)
    {
        tone(speakerPin, note, ntime * (1000 / note));
        delay(dtime);
    }
}

//=======================================================================// ANGLES
int calculateMinimumAngle()
{
  	int angle=45;
  	while(getDistance(1)<400)
  	    servo.write(++angle);
  	return angle;
}
/*
int calculateMinimumAngle()
{
  	servo.write(45);
  	int angle=(int)asin(getDistance(1)*sqrt(2)/820);
  	return angle;
  
}
*/
int angleCalib(int k)
{
    beep(64, 1, 100, 1000);
    if(k == 0)
    {
        servo.write(k);
        beep(13, 1, 10, 100);
        return getDistance(1);
    }
    
    servo.write(k);
    Serial.println("krece kalibracija (gledajte pravo u zid, imate 2 sekunde)");
    beep(13, 2, 10, 100);
    delay(2000);
    int distance1 = getDistance(1);
    Serial.println("kalibracija (pomerite se jedan korak napred, imate 3 sekunde)");
    beep(13, 3, 10, 100);
    delay(3000);
    int distance2 = getDistance(1);
    Serial.println("kalibracija zavrsena");
    beep(13, 1, 10, 100);

    if(distance1 - distance2 <= 0) return -1;
    return distance1 - distance2;
}

int calibration()
{
    int sDown = angleCalib(0); // CALIBRATION WHEN SERVO ANGLE IS 0 (down angle)
    int sUp = angleCalib(default_angle); // CALIBRATION WHEN SERVO ANGLE IS 30 (flat angle)
    sDown = sUp;

    if(sUp <= -1 || sDown <= -1)
    {
        beep(17, 5, 10, 100);
        return 0;
    }
    stepUp = sUp;
    stepDown = sDown;
    beep(81, 5, 10, 100);
    return 1;
}

int getDistance(int n)
{
    digitalWrite(trigPin[n], LOW);
    delayMicroseconds(2);
    
    digitalWrite(trigPin[n], HIGH);
    delayMicroseconds(10);
    
    digitalWrite(trigPin[n], LOW);
    
    return (pulseIn(echoPin[n], HIGH) * 0.034) / 2;
}

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

int checkObstacle(int angle)
{
    servo.write(angle);
    int distance = getDistance(1);

    if(angle <= 25 && distance <= stepUp) return 1;
    if(angle <= 13 && distance <= stepDown) return 1;
    return 0;
}

void loop()
{
    for(int i = 35; i >= 0; i--)
    {
        if(checkObstacle(i)) beep(57, 5, 10, 100);
        delay(delayTime); 
    }
    for(int i = 0; i < 35; i++)
    {
        if(checkObstacle(i)) beep(57, 5, 10, 100);
        delay(delayTime); 
    }
    /*Serial.print("korak: ");
    Serial.println(calibration(stepUp, stepDown));
    delay(3000);*/
    /*
    for(int i = 0; i < 30; i++)
    {
        servo.write(i);
        printDistances();
        delayMicroseconds(delayTime); 
    }

    for(int i = 30; i >= 0; i--)
    {
        servo.write(i);
        printDistances();
        delayMicroseconds(delayTime); 
    }*/
}
