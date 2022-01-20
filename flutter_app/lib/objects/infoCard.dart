// ignore_for_file: file_names

import 'dart:async';

import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:flutter_app/Http/httpObject.dart';
import 'package:flutter_spinkit/flutter_spinkit.dart';
import '../myicon_icons.dart';
import 'infoObject.dart';

class infoCard extends StatefulWidget {
  info? ob ;

  infoCard({this.ob});

  @override
  _infoCardState createState() => _infoCardState();
}

class _infoCardState extends State<infoCard> {
  final refdatabase = FirebaseDatabase.instance.reference() ;
  Timer? timer;
  Timer? timer2;
  String region = '--';


  worldTime time = worldTime();
  DateTime? t ;
  String tt = '--';

  Wheather  wheather = Wheather();
  String lon = '--';
  String lat = '--';
  String main = '--';
  String temp = '--';
  String humidity = '--';

  DayTime dayTime = DayTime();
  String sunrise = '--';
  String sunset  = '--';

  Color color = Colors.black;
  @override
  void initState() {
    super.initState();
    get_data();
    checkForData();
    timer  = Timer.periodic(Duration(seconds: 10), (Timer t) => checkForData());
    timer2 = Timer.periodic(Duration(minutes: 1), (Timer t) => get_weather());
  }
  @override
  Widget build(BuildContext context) {

    if(region == '--'){
      return Container(
        height: 250,
        child: Card(
            color: Colors.black54,
            child: Center(

              child: SpinKitFadingCircle(

                color: Colors.white70,
                size: 180,

              ),
            )
        ),
      );
    }
    return Card(
        shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(15)),
        child: Container(
          decoration: BoxDecoration(
            image: DecorationImage(
              image: AssetImage(widget.ob!.img),
              fit: BoxFit.fill,
              alignment: Alignment.center,
            ),
            borderRadius: BorderRadius.circular(15)
          ),
          child: Column(
            mainAxisSize: MainAxisSize.max,
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              SizedBox(height: 30,),
              Text(region,style: TextStyle(fontSize: 45,color: Colors.white , fontWeight:FontWeight.w200),),
              SizedBox(height: 25,),
              Text(main,style: TextStyle(fontSize: 30,color: Colors.white, fontWeight:FontWeight.w300),),
              SizedBox(height: 10,),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Text('Temputare :'+temp,style: TextStyle(fontSize: 12,color:color, fontWeight:FontWeight.w700),),
                  SizedBox(width: 10,),
                  Text('Humidity :'+humidity,style: TextStyle(fontSize: 12,color:color, fontWeight:FontWeight.w700),),
                ],
              ),
              SizedBox(height: 25,),
              Row(
                mainAxisSize: MainAxisSize.max,
                mainAxisAlignment: MainAxisAlignment.spaceAround,
                children: [
                  Column(
                    children: [
                      Icon(Myicon.weather_sunset_up,color: Colors.amberAccent,),
                      SizedBox(height: 5,),
                      Text(sunrise,style: TextStyle(fontSize: 12,color: Colors.white),),
                    ],
                  ),
                  Text(tt,style: TextStyle(fontSize: 25,color: Colors.white),),
                  Column(
                    children: [
                      Icon(Myicon.weather_sunset_down,color: Colors.amberAccent,),
                      SizedBox(height: 5,),
                      Text(sunset,style: TextStyle(fontSize: 12,color: Colors.white),),
                    ],
                  ),

                ],
              ),
              SizedBox(height: 25,)
            ],
          ),
        )
      );
  }

  checkForData()async{
    //print('date');
    DateTime t = await time.get_time(region);
    String h = t.hour.toString();
    if(h.length == 1 ){
      h = '0'+h;
    }
    String m = t.minute.toString();
    if(m.length == 1){
      m = '0'+m;
    }
    tt = h+':'+m;
    setImg();
    setState((){});
  }

  get_weather()async{
    //print('weather');
    List l = await wheather.get_weather(region);
    lon = l[0];
    lat = l[1];
    main = l[2];
    temp = l[3];
    humidity = l[4];
    setState((){});
  }

  get_dayTime()async{
    List l = await dayTime.get_dayTime(lon, lat);
    sunrise = l[0];
    sunset  = l[1];
    setState((){});

  }

  Future<void> get_region()async{
    var db = refdatabase.child("Region");
    await db.once().then((DataSnapshot snapshot) {
      region = snapshot.value ;
      //print('r = '+r);
    });

  }

  void setImg(){
    int t , r ,s;

    t = get_time(tt);
    r = get_time(widget.ob!.sunrise!);
    s = get_time(widget.ob!.sunset!);
    if(main == "Clear"){
      if(t > r && t < s){
        widget.ob!.img = 'assets/sunny_day.png';
        color = Colors.black;
      }else{
        widget.ob!.img = 'assets/sunny_day_night.jfif';
        color = Colors.black;
      }
    }else{
      if(t > r && t < s){
        widget.ob!.img = 'assets/cloudy-sky_day.jpg';
        color = Colors.black;
      }else{
        widget.ob!.img = 'assets/cloudy_sky_night.jpg';
        color = Colors.white70;
      }
    }

  }

  int get_time(String t){
    int val = 0 ;
    List<String> temp = t.split(':');
    val = int.parse(temp[0])*60 + int.parse(temp[1]);
    return val ;
  }
  void get_data()async{
    await get_region();
    //print('region = '+region);
    checkForData();
    await get_weather();
    get_dayTime();
  }
}
