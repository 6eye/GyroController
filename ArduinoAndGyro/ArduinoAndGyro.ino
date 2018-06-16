   #include "Wire.h" 
   #include <SoftwareSerial.h>
    
    SoftwareSerial sSerial(10, 11); //Make digital ports 10 and 11 software RX and DX of serial comunication
    const int MPU_ADDR = 0x68; // MPU hex addres
   
    // MPU uses 16 bits to store raw data in registers
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z; 
    int time_milis; //time in mls

     float accel_convert_value =  1.0; //Value for converting raw data || 1 to proces data later on pc
  
    
    void setup() {
      
      sSerial.begin(115200); //Start serial comunication on pins 10|11 at 115200 bits/sec
      
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
      

      sSerial.print(accel_x/accel_convert_value,16);
      sSerial.print(' ');
      sSerial.print(accel_y/accel_convert_value,16);
      sSerial.print(' ');
      sSerial.print(accel_z/accel_convert_value,16);
      sSerial.print(' ');
            
      sSerial.print(gyro_x);
      sSerial.print(' ');
      sSerial.print(gyro_y);
      sSerial.print(' ');
      sSerial.print(gyro_z);
      sSerial.print(' ');
      sSerial.print(';');
      sSerial.println();
      
      delay(10); //wait 10ms | We got 30 frames per second in blender =0.034s for frame | We wait 0.001s | One loop won't take more then 0.002s that gives us about 15 messurments per frame
    }

