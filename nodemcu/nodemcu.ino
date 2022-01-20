#include "WiFi.h"
#include "utilities.h"

//#include <FirebaseArduino.h>
#include <Arduino.h>



#include <SoftwareSerial.h>
/*
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
*/
//#include "fire_base.h"
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"  



#define DAY_NUMBER 3
#define SIZEOF_TABLE (DAY_NUMBER*96) 

//#define FIREBASE_HOST "finalproject-514ae-default-rtdb.firebaseio.com"   //--> https://finalproject-514ae-default-rtdb.firebaseio.com/
//#define FIREBASE_AUTH "AIzaSyAEt-wg4LgFFBxzmyvQbYEZkWi_7aOAY1w"//"QEhQuHZnUCIOTKbm6YyDF19BLYojByH1LIxNRxmz"




#define API_KEY "AIzaSyAEt-wg4LgFFBxzmyvQbYEZkWi_7aOAY1w"
#define DATABASE_URL "https://finalproject-514ae-default-rtdb.firebaseio.com/" 

class myFireBase{

private:

public:
  //Define Firebase Data object
  FirebaseData fbdo;

  FirebaseAuth auth;
  FirebaseConfig config;

  bool signupOK = false;

  bool signin(){
    /* Assign the api key (required) */
    config.api_key = API_KEY;
    /* Assign the RTDB URL (required) */
    config.database_url = DATABASE_URL;
    if(Firebase.signUp(&config, &auth, "", "")){
      //Serial.println("ok");
      signupOK = true;
    }else{
      //Serial.println("no");
    }
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
  }

  int get_int(String path){
    if (Firebase.RTDB.getInt(&fbdo, path)) {
      if (fbdo.dataType() == "int") {
        //Serial.println(fbdo.intData());
        return fbdo.intData();
      }else{
        return -1 ;
      }
    }else {
      //Serial.println(fbdo.errorReason());
      return -1;
    }
  }

  bool set_int(String path , int val){
    if (Firebase.RTDB.setInt(&fbdo, path, val)) {
      //Serial.println("PASSED");
      //Serial.println("PATH: " + fbdo.dataPath());
      //Serial.println("TYPE: " + fbdo.dataType());
      return true;
    }else {
      //Serial.println("FAILED");
      //Serial.println("REASON: " + fbdo.errorReason());
      return false ;
    }
  }

  bool set_string(String path , String str){
    if (Firebase.RTDB.setString(&fbdo, path, str)) {
      //Serial.println("PASSED");
      //Serial.println("PATH: " + fbdo.dataPath());
      //Serial.println("TYPE: " + fbdo.dataType());
      return true;
    }else {
      //Serial.println("FAILED");
      //Serial.println("REASON: " + fbdo.errorReason());
      return false;
    }
  }
  /**** counter geter seter ****/
  bool set_counter(int counter){
    return set_int("counter",counter);
  }
  int get_counter(){
    return get_int("counter");
  }
  ///////////////////////////////

  /**** enforce geter ****/
  int get_enforce(){
    return get_int("Enforce");
  }
  ////////////////////////

  /**** is op getter ****/
  bool set_isop(int x){
    return set_int("is_op",x);
  }
  ///////////////////////

  /**** time getter setter ****/
  bool get_time(int* y , int* d , int* m){
    int yy = get_int("TIME/y");
    int dd = get_int("TIME/d");
    int mm = get_int("TIME/m");
    if(yy == -1 || dd == -1 || mm == -1){
      return false ;
    }
    *y = yy;
    *d = dd;
    *m = mm;
    return true ;
  }

  bool set_time(int y , int d , int m){
    bool yy = set_int("TIME/y",y);
    bool dd = set_int("TIME/d",d);
    bool mm = set_int("TIME/m",m);
    return yy && dd && mm ;
  }
  ///////////////////////////////

  /**** region setter ****/
  bool set_region(String r){
    return set_string("Region",r);
  }
  /////////////////////////

  /**** pos setter ****/
  bool set_pos(int x , int y){
    bool xx = set_int("POS/X",x);
    bool yy = set_int("POS/Y",y);
    return xx && yy;
  }
  //////////////////////

  /**** plant info getter ****/
  int get_water_scheduale(){
    return get_int("Plant_info/water_scheduale");
  }
  int get_sunlight_scheduale(){
    return get_int("Plant_info/sunlight_scheduale");
  }
  /////////////////////////////

  /**** mht setter ****/
  bool set_mht(int m , int h , int t , int counter){ 
    int mht = (m*10000)+(h*100)+(t);
    if(counter < 0){
      return set_int("MHT/current",mht);
    }else{
      String c = String(counter);
      return set_int("MHT/"+c,mht);
    }
  }

  bool set_swb(int s , int w , int b , int counter){ 
    int swb = (s*10000)+(w*100)+(b);
    if(counter < 0){
      return set_int("SWB/current",swb);
    }else{
      String c = String(counter);
      return set_int("SWB/"+c,swb);
    }
  }

  
  
};






struct R_data{
  int16_t x ;        // pos 
  int16_t y ;        // pos
  int16_t is_sun ;   
  int16_t temp ;
  int16_t hum ;
  int16_t soil ;
  int16_t water_level ;
  int16_t is_operate ;
  int16_t battery_percentage ;
  int16_t error ;
};

struct T_data{
  bool enforce;          // to enforce the robot to operate in any condition .
  bool ok     ;          // if 0 mean that is no sunlight due to weather condition or day time .
  bool is_night ;
  int16_t sunlight_amount ; // the period to keep the plant in the sunlight (in minutes).
  int16_t water_scedual   ; // how many time should the plant be water in a day .  
};

SoftwareSerial mySerial(D3, D4);

const char* ssid     = "VODAFONE_463A";
const char* password = "6bc92ffe";

int counter = 0 ;
int utc_offset = 3 ;
String region ;
String lon , lat ;
String weather ;
int sunrise , sunset ;
int _year , day_of_year ;
int _time ;



const size_t r_data_len = sizeof(struct R_data);
const size_t t_data_len = sizeof(struct T_data);
struct R_data r_data = {0,0,0,0,0,0,0,0,0};
struct T_data t_data ;

unsigned long mil ;

myFireBase fb ;
void setup() {
  delay(1000);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D8,OUTPUT);
  digitalWrite(D2,HIGH);
  digitalWrite(D1,LOW);
  digitalWrite(D8,LOW);
  
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  mySerial.begin(9600);
  Serial.write("OK");
  delay(100);
  
  //Serial.println('\n');
  connect(ssid,password);
  //Serial.println("connecting to fire base");
  fb.signin();
  
  //////////////////////// get time ///////////////////////////////////
  _time = get_time(&region,&utc_offset,&_year,&day_of_year);
  while(_time == -1 ){
    _time = get_time(&region,&utc_offset,&_year,&day_of_year);  
  }
  Serial.println();
  Serial.println(_time);
  //Serial.println(String("region = "+region+"\n"+"utc_offset = "+utc_offset+"\nyear = "+_year+"\nday of year = "+day_of_year));
  //////////////////////  get weather condition ///////////////////////
  
  
  weather = get_wheather(region,&lon,&lat);
  //Serial.println(String("lon = "+lon+"\n"+"lat = "+lat));
  
  ////////////////////    get sunrise sunset time /////////////////////
  
  get_sunrise_sunset(lon,lat , &sunrise , &sunset , utc_offset);
  Serial.println("sunrise = "+String(sunrise)+"\n"+"sunset = "+String(sunset));
    
  
  //Firebase.begin(FIREBASE_HOST , FIREBASE_AUTH);
  
  
  /////////////////  get the last counter value from the database /////////////
  counter = fb.get_counter();
  //Serial.println("counter = "+String(counter));
  
  while(counter < 0){
    counter = fb.get_counter();  
    //Serial.println("counter = "+String(counter));  
  }
  //Serial.println("counter = "+String(counter));
  
  /////////// get the last time stamp from the db    //////////////////////
  int y , d , m ;
  bool flag ;
  flag = fb.get_time(&y,&d,&m);
  while(!flag){
    flag = fb.get_time(&y,&d,&m);
  }
  //Serial.println("y = "+String(y)+"\td = "+String(d)+"\tm = "+String(m));

  ///////////////////////////// adjust the counter /////////////////////////
  //Serial.println();
  
  adjust_counter(y,d,m);
  fb.set_counter(counter);
  fb.set_time(_year,day_of_year,_time);
  
  //clear_table(SIZEOF_TABLE-1,0);
  Serial.write("OK");
  //Serial.write("\n");
  //set_counter();
  //set_date(_year,day_of_year ,_time);

  //set_pozisition(0,0);


  ////////////////// now every thing is ready send a msg to nucleo to start work ////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  
  mil = millis()+60000;
  Serial.flush();
  digitalWrite(D2,LOW);
  digitalWrite(D1,HIGH);
}

void loop() {
  
  if (Serial.available()> 0){
    //Serial.println(Serial.readBytes((char*)&r_data,r_data_len));
    Serial.readBytes((char*)&r_data,r_data_len);
    /*
    uint8_t* structPtr = (uint8_t*) &r_data;
    for (byte i = 0; i < r_data_len; i++){
      Serial.print(*structPtr++, HEX);
      Serial.print("   ");
    }Serial.println();
    */
    fb.set_pos(r_data.x,r_data.y);
    fb.set_mht(r_data.soil,r_data.hum,r_data.temp,-1);
    fb.set_swb(r_data.is_sun,r_data.water_level,r_data.battery_percentage,-1);
    fb.set_isop(r_data.is_operate);
    //print_R_data(r_data);
    digitalWrite(D1,LOW);
    digitalWrite(D1,HIGH);
  }
  
  
  t_data.enforce = fb.get_enforce();
  t_data.sunlight_amount = fb.get_sunlight_scheduale();
  t_data.water_scedual = fb.get_water_scheduale();
  bool check1 = check_condition1();
  bool check2 = check_condition2();
  
  t_data.ok = (check1 && check2) ;
  mySerial.write((char*)&t_data,t_data_len);
  
  
  //send_r_data(r_data);
  if(millis()>= mil){  /// for each 15 min 
    int d ;
    //    to do
    
    mil = millis()+60000;
    counter += 1 ;
    counter = counter%SIZEOF_TABLE ;
    

    fb.set_mht(r_data.soil,r_data.hum,r_data.temp,counter);
    fb.set_swb(r_data.is_sun,r_data.water_level,r_data.battery_percentage,counter);
    fb.set_isop(r_data.is_operate);


    
    
    int t = get_time(&region,&utc_offset,&_year,&d);
    if(t == -1 ){
      _time = _time+1;
    }else{
      _time = t ;
    }
    weather = get_wheather(region,&lon,&lat);
    if(day_of_year != d){
      get_sunrise_sunset(lon,lat , &sunrise , &sunset , utc_offset);
    }
    day_of_year = d;
    fb.set_counter(counter);
    fb.set_time(_year,day_of_year,_time);
    //Serial.println("writing to the db");
  }
}

bool check_condition1(){  // check wheather condition ;
  Serial.println("check_condition1");
  if(weather == "Clear"){
    return true ;
  }
  return false ;
}

bool check_condition2(){ // checks day time 
  Serial.println("time = "+String(_time)+"\tsunrise = "+String(sunrise)+"\t"+"sunset = "+String(sunset));
  if(_time >= sunrise && _time <= sunset){
    t_data.is_night = false ;
    return true ;
  }
  t_data.is_night = true ;
  return false ;
}

/*
int set_pozisition(int x , int y){
  if(set_int("POS/X",x) == 0){
    return 0;  
  }
  if(set_int("POS/Y",y) == 0){
    return 0;  
  }
  return 1 ;
}
*/
void adjust_counter(int y,int d,int m){
  
  int old_counter = counter ;
  int dif ;
  if(day_of_year >= d){
    dif = ((day_of_year*1440)+_time)-((d*1440)+m);
  }else{
    dif = day_of_year-d+365;
    dif = (dif*1440) - m + _time ;
  }
  /*
  if(dif >= DAY_NUMBER*1440 ){
    //// reset the dabaes
    counter = SIZEOF_TABLE;
    old_counter = 0 ;
  }else{
    counter += (int)(dif/15);
    counter = counter% SIZEOF_TABLE ;
  }
  */
  if(dif >= SIZEOF_TABLE ){
    //// reset the dabaes
    counter = SIZEOF_TABLE-1;
    old_counter = 0 ;
  }else{
    counter += (int)(dif);
    counter = counter% SIZEOF_TABLE ;
  }
  
  Serial.println("old counter = "+String(old_counter) +"\tnew counter = "+String(counter) );
  clear_table(counter , old_counter);
}

int clear_table(int counter , int old_counter){
  int i = counter ;
  int j = old_counter;
  bool statu = LOW ;
  while(j != i){
    Serial.println(j);
    statu = !statu ;
    digitalWrite(D8,HIGH);
    fb.set_swb(0,0,0,j);
    fb.set_mht(0,0,0,j);
    
    j = (++j)% SIZEOF_TABLE;
  }digitalWrite(D8,LOW);
  
  /*
  while(i != old_counter){
    Serial.println(i);
    fb.set_swb(0,0,0,i);
    fb.set_mht(0,0,0,i);
    
    //set_temperature(0,i);
    //set_humidity(0,i);
    //set_soil_moisture(0,i);
    //set_battery_percentage(0,i);
    //set_water_level(0,i);
    
    i = (--i)% SIZEOF_TABLE ;
    //Serial.println("\t"+String(i));
  }*/
}
/*

int set_date(int y , int d , int m){
  int flag = 1 ;
  if(set_int("TIME/y",y) == -1){
    flag = -1 ;
  }
  if(set_int("TIME/d",d) == -1){
    flag = -1 ;
  }
  if(set_int("TIME/m",m) == -1){
    flag = -1 ;
  }
  return flag ;
}
int set_temperature(int val , int counter){
  return set_int(String("Temperature/"+String(counter)),val);
}
int set_humidity(int val , int counter){
  return set_int(String("Humidity/"+String(counter)),val);
}
int set_soil_moisture(int val , int counter){
  return set_int(String("Moisture/"+String(counter)),val);
}
int set_battery_percentage(int val , int counter){
  return set_int(String("Battery/"+String(counter)),val);
}
int set_water_level(int val , int counter){
  return set_int(String("WaterLevel/"+String(counter)),val);
}
int set_MHT(int m , int h , int t , int counter){
  int val = (m*10000)+(h*100)+(t);
  return set_int(String("MHT/"+String(counter)),val);
}
int set_SWB(int s , int w , int b , int counter){
  int val = (s*10000)+(w*100)+(b);
  return set_int(String("SWB/"+String(counter)),val);
}
int get_date(int *y , int *d , int *m){
  int temp = get_int("TIME/y");
  if(temp < 0){
    return -1 ;  
  }else{
    *y = temp ;
  }
  temp = get_int("TIME/d");
  if(temp < 0){
    return -1 ;  
  }else{
    *d = temp ;
  }
  temp = get_int("TIME/m");
  if(temp < 0){
    return -1 ;  
  }else{
    *m = temp ;
  }
  return 1 ;
}

int set_counter(){
  return set_int("counter",counter);
}
int get_counter(){
  return get_int("counter");
}
int set_int(String path , int val){
  int i = 0 ;
  Firebase.setInt(path ,val);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setInt(path ,val);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  return 1 ;
}

int get_int(String path){
  int i = 0 ;
  int val = Firebase.getInt(path);
  while(Firebase.failed() && i < 5){
    delay(20);
    val = Firebase.getInt(path);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  return val ;
}
*/
void print_R_data(struct R_data data){
  Serial.println("X = "+String(data.x)+"\tY = "+String(data.y));
  Serial.println("is sun = "+String(data.is_sun));
  Serial.println("temp = "+String(data.temp)+"\thum = "+String(data.hum));
  Serial.println("soil = "+String(data.soil));
  Serial.println("water level = "+String(data.water_level));
  Serial.println("is operate = "+String(data.is_operate));
  Serial.println("is operate = "+String(data.battery_percentage));
  Serial.println("error = "+String(data.error));    
}
/*
int send_r_data(struct R_data data){
  int i = 0 ;
  Firebase.setInt("POS/X" ,data.x);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setInt("POS/X" ,data.x);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  i = 0 ;
  Firebase.setInt("POS/Y" , data.y);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setInt("POS/Y" ,data.y);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  i = 0 ;
  Firebase.setFloat("Temperature" , data.temp);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setFloat("Temperature" , data.temp);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  i = 0 ;
  Firebase.setFloat("humidity" , data.hum);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setFloat("humidity" , data.hum);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  i = 0 ;
  Firebase.setInt("soilMoisture" , data.soil);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setInt("soilMoisture" , data.soil);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  i = 0 ;
  Firebase.setInt("soilMoisture" , data.soil);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setInt("soilMoisture" , data.soil);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  i = 0 ;
  Firebase.setInt("WaterTankLevel" , data.water_level);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setInt("WaterTankLevel" , data.water_level);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  i = 0 ;
  Firebase.setInt("SunFound" , data.is_sun);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setInt("SunFound" , data.is_sun);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  i = 0 ;
  Firebase.setInt("Operating" , data.is_operate);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setInt("Operating" , data.is_operate);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  i = 0 ;
  Firebase.setInt("Error_code" , data.error);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setInt("Error_code" , data.error);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  i = 0 ;
  i = 0 ;
  Firebase.setInt("battery_percentage" , data.battery_percentage);
  while(Firebase.failed() && i < 5){
    delay(20);
    Firebase.setInt("battery_percentage" , data.battery_percentage);
    i++;
    if(i == 5){
      return -1 ;  
    }
  }
  return 1 ;
}*/
