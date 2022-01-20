# include "utilities.h"
const char* wheather_key = "105e47047406c9c217f325fa9e02482b";

int get_time(String* region , int *utc_offset,int* _year , int* day_of_year){
  int tt ;
  String response = httpGETRequest("http://worldtimeapi.org/api/ip");
//  Serial.println(response);
  JSONVar myObject = JSON.parse(response);
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return -1;
  }
  JSONVar keys = myObject.keys();
//  Serial.println(keys.length());
  if(keys.length() == 15){
    char time_h[3] ;
    char time_m[3] ;
    JSONVar time = myObject["datetime"];
    int h,m ,junk;
    sscanf(time,"%d-%d-%dT%d:%d",_year,&junk,&junk,&h,&m);
    tt = (h*60)+m ;
    String zone = JSON.stringify(myObject["timezone"]);
    for(int i = 0 ; i < zone.length() ; i++){
       if(zone[i] == '/'){
          String r = String(&zone[i+1]);
          r[r.length()-1] = 0 ;
          *region = r;                  
          break;
       }
    }
    sscanf(myObject["utc_offset"],"%d",utc_offset);
    //sscanf(myObject["day_of_year"],"%d",day_of_year);
    *day_of_year = JSON.stringify(myObject["day_of_year"]).toInt();

  }else{
    return -1 ;
  }
  return tt;
}

String get_wheather(String region , String* lon , String* lat){
  String _weather;
  String url = String("http://api.openweathermap.org/data/2.5/weather?q=")+region+"&appid="+String(wheather_key);
  char request[url.length()+1] ;  
  url.toCharArray(request,url.length()+1);
  String response = httpGETRequest(request);
  //Serial.println(response);
  JSONVar myObject = JSON.parse(response);
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return "";
  }
  JSONVar keys = myObject.keys();
//  Serial.println(keys.length());
  if(keys.length() == 13){
    ////////////////////// coords ////////////////////////////////////////
//    Serial.println(myObject["coord"]);
    JSONVar coord = JSON.parse(JSON.stringify(myObject["coord"]));
    if (JSON.typeof(coord) == "undefined") {
      Serial.println("Parsing input failed!");
      return "";
    }
    JSONVar keys_coords = coord.keys();
//    Serial.println(coord["lon"]);
      *lon = JSON.stringify(coord["lon"]);
//    Serial.println(coord["lat"]);
      *lat = JSON.stringify(coord["lat"]);
    ////////////////////// weather ////////////////////////////////////////
//    Serial.println(myObject["weather"]);
    String w = JSON.stringify(myObject["weather"]);
    w = &w[1];
    w[w.length()-1] = 0 ;
    JSONVar weather = JSON.parse(w);
    if (JSON.typeof(weather) == "undefined") {
      Serial.println("Parsing input failed!");
      return "";
    }
    JSONVar keys_weather = weather.keys();
//    Serial.println(weather["main"]);
    _weather = weather["main"];  
  }else{
    return "";
  }
  return _weather;
  
}

int get_sunrise_sunset(String lon , String lat , int*sunrise , int*sunset , int utc_offset ){
  String url = String("http://api.sunrise-sunset.org/json?lat=")+lat+String("&lng=")+lon;
  char request[url.length()+1] ;  
  url.toCharArray(request,url.length()+1);
//  Serial.println(request);

  String response = httpGETRequest(request);
//  Serial.println(response);
  JSONVar myObject = JSON.parse(response);
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return -1;
  }
  JSONVar keys = myObject.keys();
//  Serial.println(keys.length());
  if(keys.length() == 2){
    if(JSON.stringify(myObject["status"]) == "\"OK\""){
      JSONVar res = JSON.parse(JSON.stringify(myObject["results"]));
      if (JSON.typeof(res) == "undefined") {
        Serial.println("Parsing input failed!");
        return -1;
      }
      JSONVar res_keys = res.keys();
//      Serial.println(res_keys.length());
      if(res_keys.length() == 10){
//        Serial.println(res["sunrise"]);
//        String r=JSON.stringify(res["sunrise"]);
//        r=&r[1];
        int h , m ;
        sscanf(res["sunrise"],"%d:%d",&h,&m);
        *sunrise = ((h+utc_offset)*60)+m;
        sscanf(res["sunset"],"%d:%d",&h,&m);
        *sunset = ((h+12+utc_offset)*60)+m;       
      }
    }
  }
  return 0 ;
}
