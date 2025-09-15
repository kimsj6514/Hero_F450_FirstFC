#include <Wire.h>           //使用IIC库
#define address 0x0D        //QMC5883L的设备地址设置

void setup(){
  Serial.begin(9600);  //初始化串口和I2C口
  Wire.begin();
//初始化模块
  Wire.beginTransmission(address);
  Wire.write(0x09); //选择控制寄存器0X09
  Wire.write(0x1D); //设置0x09寄存器为：OSR:512Hz，RNG：+/-8Gauss，ODG：200Hz，MODE：连续工作模式
  Wire.endTransmission();
}

void loop(){

  int x,y,z; //triple axis data

  Wire.beginTransmission(address);//读写开始
  Wire.write(0x00); //选择X,Y，Z所在数据储存寄存器
  Wire.endTransmission();

//读取X、Y、Z三个方向的坐标值
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
  }
//打印坐标到串口
  Serial.print("x: ");
  Serial.print(x);
  Serial.print("  y: ");
  Serial.print(y);
  Serial.print("  z: ");
  Serial.println(z);
  delay(100);
}

