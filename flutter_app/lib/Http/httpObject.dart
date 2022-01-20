// ignore_for_file: file_names

import 'dart:convert';
import 'package:http/http.dart';

class worldTime{

  Future<DateTime> get_time(String region)async{
    try {
      //print("////////////////////////////////////////////////////////");
      Uri uri = Uri.parse(
          'http://worldtimeapi.org/api/timezone/Europe/' + region);
      Response res = await get(uri);
      //print(res.body);
      while(res.statusCode != 200){
        res = await get(uri);
      }
      Map data = jsonDecode(res.body);
      String datetime = data['datetime'];

      DateTime now = DateTime.parse(datetime);
      String offset = data['utc_offset'].substring(1,3);
      now = now.add(Duration(hours: int.parse(offset)));
      return now;
    }catch(e){
      print(e);
      return get_time2(region);
    }
    return DateTime(0,0,0,0,0,0) ;
  }

  Future<DateTime> get_time2(String region)async{
    try {
      //print("////////////////////////////////////////////////////////");
      Uri uri = Uri.parse(
          'https://timeapi.io/api/Time/current/zone?timeZone=Europe/'+ region);
      Response res = await get(uri);
      //print(res.body);
      while(res.statusCode != 200){
        res = await get(uri);
      }
      Map data = jsonDecode(res.body);

      return DateTime(data['year'],data['month'],data['day'],data['hour'],data['minute']);//now;
    }catch(e){
      print(e);
    }
    return DateTime(0,0,0,0,0,0) ;
  }

}

class Wheather{
  Future<List<String>> get_weather(String region)async{
    List<String> l=[];
    try{
      Uri uri = Uri.parse('http://api.openweathermap.org/data/2.5/weather?q='+region+'&appid=105e47047406c9c217f325fa9e02482b');
      Response res = await get(uri);
      Map data = jsonDecode(res.body);
      String coord = data['coord'].toString();
      //print(coord);
      coord = coord.replaceAll(",", '').replaceAll('{', '').replaceAll('}', '');
      //print(coord);
      List temp = coord.split(' ') ;
      l.add(temp[1]);
      l.add(temp[3]);


      String weather = data['weather'].toString();
      //print(weather);
      weather = weather.replaceAll(',','');
      //print(weather);
      temp = weather.split(' ') ;
      //print(temp);
      l.add(temp[3]);


      String main = data['main'].toString();
      //print(main);
      main = main.replaceAll(',','');
      //print(main);
      temp = main.split(' ') ;
      //print(temp);

      int tem = (double.parse(temp[1])-273.15).round();
      //print(tem);
      l.add(tem.toString());
      String buff = temp[11].toString().replaceAll(']','').replaceAll('}','');
      int hum = int.parse(buff);
      //print(hum);
      l.add(hum.toString());
    }catch(e){
      print(e);
    }
    return l ;
  }
}

class DayTime{
  Future<List<String>> get_dayTime(String lon,String lat)async{
    List<String> l = [];
    int h = 0 ;
    int m = 0 ;
    try{
      Uri uri = Uri.parse('http://api.sunrise-sunset.org/json?lat='+lat+'&lng='+lon);
      Response res = await get(uri);
      Map data = jsonDecode(res.body);
      Map data2 = jsonDecode(json.encode(data['results']));
      String sr = data2['sunrise'];
      List<String> temp = sr.split(' ');
      //print(temp[0]);

      List<String> temp2 = temp[0].split(':');
      //print(temp2);

      if(temp[1] == 'AM'){
        if(temp2[0].length == 1){
          temp2[0] = '0'+(int.parse(temp2[0])+3).toString();
        }
        if(temp2[1].length == 1){
          temp2[1] = '0'+temp2[1];
        }
      }
      l.add(temp2[0]+':'+temp2[1]);

      sr = data2['sunset'];
      temp = sr.split(' ');
      print(temp[0]);
      temp2 = temp[0].split(':');
      if(temp[1] == 'PM'){
        if(temp2[0].length == 1){
          temp2[0] = '0'+ (int.parse(temp2[0])+15).toString();
        }
        if(temp2[1].length == 1){
          temp2[1] = '0'+temp2[1];
        }
      }
      l.add(temp2[0]+':'+temp2[1]);



    }catch(e){
      print(e);
    }
    return l ;
  }
}