   #include "Wire.h" 
   #include <SoftwareSerial.h>
    
    SoftwareSerial sSerial(10, 11); //Make digital ports 10 and 11 software RX and DX of serial comunication
    const int MPU_ADDR = 0x68; // MPU hex addres
   
    // MPU uses 16 bits to store raw data in registers
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z; 

    int16_t cal_accel_x = 0;  int16_t cal_accel_y = 0;  int16_t cal_accel_z = 0;
    int16_t cal_gyro_x = 0;   int16_t cal_gyro_y = 0;   int16_t  cal_gyro_z = 0; 

    int time_milis; //time in mls

    float accel_convert_value =  1.0; //Value for converting raw data || 1 to proces data later on pc
  
    void callibrate(int gyro_precission, int accel_precission){

      Serial.println("Callibrating sensor");
      Serial.print("Gyro +-");
      Serial.print(gyro_precission);
      Serial.println();
      
      Serial.print("accel +-");
      Serial.print(accel_precission);
      Serial.println();

      //First read
      
      Wire.write(0x3B);
      Wire.endTransmission(false); 
      Wire.requestFrom(MPU_ADDR, 14, true); 
      
      cal_accel_x =( Wire.read()<<8 | Wire.read()) - 0; 
      cal_accel_y = (Wire.read()<<8 | Wire.read()) - 0; 
      cal_accel_z = (Wire.read()<<8 | Wire.read()) -  32768/gyro_precission; //Calculate difrannce between read value and expected wchich is one g -> 32768
    
      Wire.read();  //Skip L temp
      Wire.read();  //Skip R temp
      
      cal_gyro_x = (Wire.read()<<8 | Wire.read()) - 0;
      cal_gyro_y = (Wire.read()<<8 | Wire.read()) - 0;
      cal_gyro_z = (Wire.read()<<8 | Wire.read()) - 0;
      
      //Calulate mean od 100 messurments
      for(int i=0;i<100;i++){
                delay(10);
                
                Wire.write(0x3B);
                Wire.endTransmission(false); 
                Wire.requestFrom(MPU_ADDR, 14, true); 
                
                cal_accel_x = (cal_accel_x+(Wire.read()<<8 | Wire.read()) - 0)/2; 
                cal_accel_y = (cal_accel_y+(Wire.read()<<8 | Wire.read()) - 0)/2; 
                cal_accel_z = (cal_accel_z+(Wire.read()<<8 | Wire.read()) -  32768/gyro_precission)/2;
              
                Wire.read();  //Skip L temp
                Wire.read();  //Skip R temp
                
                cal_gyro_x = (cal_gyro_x+(Wire.read()<<8 | Wire.read()) - 0)/2;
                cal_gyro_y = (cal_gyro_y+(Wire.read()<<8 | Wire.read()) - 0)/2;
                cal_gyro_z = (cal_gyro_z+(Wire.read()<<8 | Wire.read()) - 0)/2;

        
      }

      Serial.println("CALLIBRATING FINISHED");
      delay(100);
    }

    
    void setup() {
      
      sSerial.begin(115200); //Start serial comunication on pins 10|11 at 115200 bits/sec
      Serial.begin(115200);
      Wire.begin(); //Start I2C data transfer to gyro
      Wire.beginTransmission(MPU_ADDR); // Start transmision with MPU
      Wire.write(0x6B); //Rquest Acces to PowerRegister
      Wire.write(0); // Set PowerRegister to zero to restart MPU 
      Wire.endTransmission(true); //End transsmision | Send queued write requests and realese I2C
    }
    void loop() {
      Wire.beginTransmission(MPU_ADDR); 
      Wire.write(0x3B); //Request acces to data from 0x3B addres which is first accel register
      Wire.endTransmission(false); // Send queued but keep I2C for futher comunication
      Wire.requestFrom(MPU_ADDR, 14, true); //Request data from MPU(we specified data above) | request 14 bytes | MPU hold data in 2x8bit register| (x+y+z)*(left+right)*(gyro+temp+acc)=2*2*3 bytes
      //Its easier to load temperature and skip its data then jump from register to register 
      
      // Read left register shift it 8 bits to left merge with right register and save to var
      accel_x = Wire.read()<<8 | Wire.read(); 
      accel_y = Wire.read()<<8 | Wire.read(); 
      accel_z = Wire.read()<<8 | Wire.read();
      Wire.read();  //Skip L temp
      Wire.read();  //Skip R temp
      gyro_x = Wire.read()<<8 | Wire.read(); 
      gyro_y = Wire.read()<<8 | Wire.read(); 
      gyro_z = Wire.read()<<8 | Wire.read();
      
      // Send Data to Arduino
      //Use software serial so we can send data to esp and watch it trough Serial Monitor in the same time
      //We subdivide value that we got from callibration to get more precise data
      
      Serial.print(accel_x - cal_accel_x);
      Serial.print(' ');
      Serial.print(accel_y - cal_accel_y);
      Serial.print(' ');
      Serial.print(accel_z - cal_accel_z);
      Serial.print(' ');
            
      Serial.print(gyro_x - cal_gyro_x);
      Serial.print(' ');
      Serial.print(gyro_y - cal_gyro_y);
      Serial.print(' ');
      Serial.print(gyro_z - cal_gyro_z);
      Serial.print(' ');
      Serial.print(millis());
      Serial.print(';');
      Serial.println();


      

      sSerial.print(accel_x - cal_accel_x);
      sSerial.print(' ');
      sSerial.print(accel_y - cal_accel_y);
      sSerial.print(' ');
      sSerial.print(accel_z - cal_accel_z);
      sSerial.print(' ');
            
      sSerial.print(gyro_x - cal_gyro_x);
      sSerial.print(' ');
      sSerial.print(gyro_y - cal_gyro_y);
      sSerial.print(' ');
      sSerial.print(gyro_z - cal_gyro_z);
      sSerial.print(' ');
      sSerial.print(millis());
      sSerial.print(';');
      sSerial.println();
      
      delay(10); //wait 10ms | We got 30 frames per second in blender =0.034s for frame | We wait 0.001s | One loop won't take more then 0.002s that gives us about 15 messurments per frame


      if(digitalRead(7)==HIGH) callibrate(2,10); //If button is pressed calibrate 
      
    
    
    }

