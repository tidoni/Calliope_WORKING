#include "MicroBit.h"
#include "MicroBitUARTService.h"

//Deklaration von Variablen
MicroBit uBit;
MicroBitUARTService *uart;


//Prototype von Funktionen
void startingSound(void);
int getLightValue(void);
int selfMicroImpl(void);
ManagedString msg = "";

static bool connected = false;
int oldLightValue = 0;


//Diese Funktion generiert einen Startsound
void startingSound(void){
    uBit.soundmotor.soundOn(260);
    uBit.sleep(120);
    uBit.soundmotor.soundOff();
    uBit.sleep(60);
    uBit.soundmotor.soundOn(780);
    uBit.sleep(500);
    uBit.soundmotor.soundOff();
    uBit.rgb.off();
}

//Diese Funktion liesst den Lichtsensor aus und gibt einen Wert zwischen 0-100 zurück
int getLightValue(void)
    {
        /*int value = 0;
            value = uBit.display.readLightLevel();
            if (value == 0){
                value = oldLightValue;
            } 
            else {
                oldLightValue = value;
            }  
        return round((value * 0.3921));
        */
        int value= 0;
       //uBit.display.setDisplayMode(DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE);
        for (int i=0; i<5; i++){
            
            value += uBit.display.readLightLevel();
            uBit.sleep(10);
        }  

        return round(((value/5) * 0.3921));

    }

//Diese Funktion definiert,dass p3 ein Eingangssignal ist und über die Schleife 
//werden 100 Samples gemessen, der hoechste Wert wird in der Variablen max gespeichert.
//Der Rückgabewert ist ein Int zwischen 0-100
int selfMicroImpl(void){
    AnalogIn microin(p3);
    int max = 0;
    int value = 0;

    for (int i=0; i<100; i+=1){
        value = int(microin.read()*255);
        if ( value < 127 ) {
           //Error Messung
            } 
       else if ( value > max ) {
            max = value;
            }
    }   
    max = (max * 0.7875) - 100;

    if (max > 100){
        return 100;
    }
    else{
        if (max < 0){
            return 0;
        }
        else {
            return round(max);
        }
    }
    
    
}

//Hier werden die aktuellen G Krafte die auf den Calliope wirken berechnet.
int getgStrength()
{
  double x = uBit.accelerometer.getX();
  double y = uBit.accelerometer.getY();
  double z = uBit.accelerometer.getZ();

  return (int) sqrt(x*x + y*y + z*z);
}

//Sobald ein Geraet mit dem Calliope verbunden ist, wird diese Funktion aufgerufen (siehe main) --> messageBus.listen 
// Hier werden Mikro, Temperatur, Licht, Kompass-und Beschleunigungssensor ausgelesen und über Bluetooth verschickt
void onConnected(MicroBitEvent)
{
    uBit.display.scrollAsync("C");
    uBit.rgb.off();
    connected = true;

    while(connected) {
        
        /*uart->send(
        ManagedString("M:")+ ManagedString(selfMicroImpl()) +
        ManagedString("T:")+ ManagedString(uBit.thermometer.getTemperature()) + 
        ManagedString("L:")+ ManagedString(getLightValue()) + 
        ManagedString("C:")+ ManagedString(uBit.compass.heading()) , 
        SYNC_SLEEP );
        uBit.sleep(20);

        uart->send( 
        ManagedString("AX:") +  ManagedString(uBit.accelerometer.getX()) +
        ManagedString("AY:") +  ManagedString(uBit.accelerometer.getY()) + 
        ManagedString("AZ:") +  ManagedString(uBit.accelerometer.getZ()) , 
        SYNC_SLEEP );
        uBit.sleep(20);

        uart->send( 
        ManagedString("AS:") +  ManagedString(getgStrength()) + 
        ManagedString("BA:") + ManagedString(uBit.buttonA.isPressed()) +  
        ManagedString("BB:") + ManagedString(uBit.buttonB.isPressed()) +
        ManagedString("\r\n") ,
        SYNC_SLEEP );
        */

        //Diese Funktion ist für den Empfang von einem Zeichen
        //msg = uart->read(1,ASYNC);
        //uBit.display.scroll(msg);

        ManagedString msg_micro = ManagedString("M:")+ ManagedString(selfMicroImpl());
        ManagedString msg_temp = ManagedString("T:")+ ManagedString(uBit.thermometer.getTemperature());  
        ManagedString msg_light = ManagedString("L:")+ ManagedString(getLightValue());  
        ManagedString msg_comp = ManagedString("C:")+ ManagedString(uBit.compass.heading()) ;
        ManagedString msg_accx = ManagedString("AX:") +  ManagedString(uBit.accelerometer.getX()); 
        ManagedString msg_accy = ManagedString("AY:") +  ManagedString(uBit.accelerometer.getY()); 
        ManagedString msg_accz = ManagedString("AZ:") +  ManagedString(uBit.accelerometer.getZ()); 
        ManagedString msg_accs = ManagedString("AS:") +  ManagedString(getgStrength());  
        ManagedString msg_buttonA = ManagedString("BA:") + ManagedString(uBit.buttonA.isPressed());  
        ManagedString msg_buttonB= ManagedString("BB:") + ManagedString(uBit.buttonB.isPressed());


        uart->send(
            msg_micro + 
            msg_temp + 
            msg_light + 
            msg_comp, 
            SYNC_SLEEP );
        
        uBit.sleep(20);

        uart->send(
            msg_accx +
            msg_accy + 
            msg_accz, 
            SYNC_SLEEP );

        uBit.sleep(20);

        uart->send(
            msg_accs +
            msg_buttonA + 
            msg_buttonB + 
            ManagedString("\r\n"),
            SYNC_SLEEP );
        
        uBit.sleep(50);
    }
}

// Falls die Verbindung getrennt wird erscheint ein "D" auf dem Calliope --> messageBus.listen
void onDisconnected(MicroBitEvent)
{
    connected = false;
    uBit.rgb.setColour(0xff, 0x66, 0x66, 0xff);
    uBit.display.print("D");
    uBit.sleep(5000);
    uBit.reset();  
}



int main()
{
    //Initialisierung der UBit Umgebung
    uBit.init();

        //BLUE
    uBit.rgb.setColour(0x00, 0x00, 0x00, 0xff);

    //Compass jedesmal neu kalibrieren
    //uBit.compass.calibrate();
    //Compass einmalig kalibrieren
    uBit.compass.heading();

    startingSound();

    //WHITE
    uBit.rgb.setColour(0xff, 0xff, 0xff, 0x0a);
    //In diesen Zeilen sind die MessageBus Listener definiert. 
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    
    //Start of BluetoothUARTService
    uart = new MicroBitUARTService(*uBit.ble, 32, 32);

    //Start der zusätzlichen Services werden nicht benötigt, da alls ueber UART gesendet wird
    //new MicroBitTemperatureService(*uBit.ble, uBit.thermometer);
    //new MicroBitMagnetometerService(*uBit.ble, uBit.compass);

    //AccelerometerService deaktiviert da in Kombination mit UART einfriert.
    //new MicroBitAccelerometerService(*uBit.ble, uBit.accelerometer);

    //Falls der Calliope diese Funktion aufruft kann er in den Sleep Modus gehen.    
    release_fiber();
}