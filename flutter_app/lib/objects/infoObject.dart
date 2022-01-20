// ignore_for_file: camel_case_types, file_names

class info{
  String? region ;
  String? weather;
  String? time ;
  String? sunset;
  String? sunrise;
  String? hum;
  String? temp;

  String img = 'assets/sunny_day_night.jfif';

  info({this.region,this.weather,this.time,this.sunset,this.sunrise,this.hum,this.temp}){
    set_image();
  }
  
  
  
  void set_image(){
    int t , r ,s;

    t = get_time(time!);
    r = get_time(sunrise!);
    s = get_time(sunset!);

    if(weather == "sunny"){
      if(t > r && t < s){
        img = 'assets/sunny_day.png';
      }else{
        img = 'assets/sunny_day_night.jfif';
      }
    }else{
      img = 'assets/sandclock.png';
    }
  }

  int get_time(String t){
    int val = 0 ;
    List<String> temp = t.split(':');
    val = int.parse(temp[0])*60 + int.parse(temp[1]);
    return val ;
  }
}