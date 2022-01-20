// ignore_for_file: file_names
import 'dart:async';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:flutter_app/Http/httpObject.dart';
import 'package:flutter_app/objects/homeObject.dart';
import 'package:flutter_app/objects/infoCard.dart';
import 'package:flutter_app/objects/infoObject.dart';
import 'package:flutter_app/objects/myCard.dart';
import 'package:flutter_app/objects/schedualeCard.dart';
import 'package:flutter_app/objects/schedule.dart';
import 'package:font_awesome_flutter/font_awesome_flutter.dart';
import '../myicon_icons.dart';
import 'package:http/http.dart';
import 'dart:convert';

class Home extends StatefulWidget{



  @override
  State<Home> createState() => _HomeState();
}

class _HomeState extends State<Home> {
  final refdatabase = FirebaseDatabase.instance.reference() ;

  Object? data_from_select_plant  = {};

  Timer? timer;
  Timer? timer2;
  worldTime time = worldTime();



  scheduale sched = scheduale(
      name: 'unselected',
      sunlight_schedual: 0,
      water_schedual: 0
  );

  info inf = info(
      region: 'istanbul',//'istanbul',
      weather : 'sunny', //'sunny',
      time : '14:26',//'12:15',
      sunset: '18:00',//'18:45',
      sunrise: '07:00',//'7:04',
      temp: '10',//'25',
      hum: '60',//'44'
  );
  homeobject temp = homeobject(name: 'Tempurature', icon: FontAwesomeIcons.thermometerHalf);
  homeobject hum      = homeobject(name: 'Humidity'   ,icon: Myicon.water_percent__1_);
  homeobject mos      = homeobject(name: 'Moisture'   ,icon: Icons.dehaze);
  homeobject water    = homeobject(name: 'Water Level'   ,icon: Icons.label);
  homeobject battery  = homeobject(name: 'Battery'   ,icon: Icons.battery_std);
  homeobject sun      = homeobject(name: 'SunLight'   ,icon: Icons.wb_sunny_sharp);
  homeobject is_op    = homeobject(name: 'is operating'   ,icon: Myicon.brightness_1);
  homeobject enf      = homeobject(name: 'Enforce'   ,icon: Icons.toggle_off);

  @override
  void initState() {
    super.initState();
    sun.iconcolor = Colors.yellow;
    hum.iconcolor = Colors.lightBlueAccent;
    enf.iconcolor = Colors.greenAccent;
    is_op.iconcolor = Colors.greenAccent;
    get_region();
    //get_time(inf.region!);
    get_weather();
    get_plant();
    checkForData();
    timer = Timer.periodic(Duration(seconds: 5), (Timer t) => checkForData());
    timer2 = Timer.periodic(Duration(minutes: 15), (Timer t) => get_weather());
  }

  @override
  Widget build(BuildContext context) {
    data_from_select_plant = ModalRoute.of(context)!.settings.arguments;

    //get_time(inf.region!);
    return Scaffold(
      appBar: AppBar(
        title: const Text("HOME"),
        centerTitle: true,
        backgroundColor: Colors.lightBlueAccent,
      ),
      body: SingleChildScrollView(
        child: Column(
            children: <Widget>[
              infoCard(
                ob : inf,
              ),
              schedualeCard(
                ob: sched,
                onpress: ()async{
                  dynamic res = await Navigator.pushNamed(context,'/selectPlant');
                  setState(() {
                    if(res != null){
                      set_plant(res);
                      Map data = res as Map;
                      sched.name = data['name'];
                      sched.water_schedual = data['water_scheduale'];
                      sched.sunlight_schedual = data['sunlight_scheduale'];
                    }
                  });
                },
              ),
              myCard(
                ob: temp,
                onpress: (){
                  setState(() {
                    //press_temp();
                    print("temp");
                    Navigator.pushNamed(context, '/tempStat');
                  });
                },
              ),
              myCard(
                ob: hum,
                onpress: (){
                  setState(() {
                    print("hum");
                    Navigator.pushNamed(context, '/humidityStat');
                  });
                },
              ),
              myCard(
                ob: mos,
                onpress: (){
                  setState(() {
                    print("mos");
                    Navigator.pushNamed(context, '/moistureStat');
                  });
                },
              ),
              myCard(
                ob: water,
                onpress: (){
                  setState(() {
                    print("water");
                    Navigator.pushNamed(context, '/waterlevelStat');
                  });
                },
              ),
              myCard(
                ob: battery,
                onpress: (){
                  setState(() {
                    print("battry");
                    Navigator.pushNamed(context, '/batteryStat');
                  });
                },
              ),
              myCard(
                ob: sun,
                onpress: (){
                  setState(() {
                    print("sun");
                    Navigator.pushNamed(context, '/sunlightStat');
                  });
                },
              ),
              myCard(
                ob: is_op,
                onpress: (){
                  setState(() {
                    print("is_op");
                  });
                },
              ),
              myCard(
                ob: enf,
                onpress: (){
                  setState(() {
                    enforce();
                  });
                },
              ),

            ]
        ),
      ),
      /*
      floatingActionButton: FloatingActionButton(
        onPressed: () {  },
        child: const Text("click"),

      ),
      */
    );
  }

  void checkForData() {
    var db = FirebaseDatabase.instance.reference().child("MHT/current");
    db.once().then((DataSnapshot snapshot){
      int t = snapshot.value;
      temp.val = t%100;
      t = (t/100).toInt();
      hum.val = t%100;
      t = (t/100).toInt();
      mos.val = t ;
    });

    db = FirebaseDatabase.instance.reference().child("SWB/current");
    db.once().then((DataSnapshot snapshot){
      int t = snapshot.value;
      battery.val = t%100;
      t = (t/100).toInt();
      water.val = t%100;
      t = (t/100).toInt();
      sun.val = t ;
    });
    get_is_op();
    setState((){});
  }

  void tempfunction(homeobject ob){
    if (temp.val != null){
      temp.val = 5;
    }

  }

  void press_temp() {
    var db = FirebaseDatabase.instance.reference().child("MHT/current");
    db.once().then((DataSnapshot snapshot){
      print(snapshot.value);
    });
  }


  void enforce(){

    if(enf.icon! == Icons.toggle_off ){
      enf.icon = Icons.toggle_on;
      enf.iconcolor = Colors.red;
      enf.val = 1 ;
    }else{
      enf.icon = Icons.toggle_off;
      enf.iconcolor = Colors.greenAccent;
      enf.val = 0 ;
    }
    set_enforce(enf.val!);
  }
/*
  Future<void> get_time(String region)async{
    try {
      Uri uri = Uri.parse(
          'http://worldtimeapi.org/api/timezone/Europe/' + region);
      Response res = await get(uri);
      //print(res.body);
      Map data = jsonDecode(res.body);
      String date = data['datetime'];
      int indexof = date.indexOf('T') + 1;
      inf.time = date.substring(indexof, indexof + 5);
      inf.set_image();
    }catch(e){
      print(":asdasasd");
      print(e);
    }
    //setState((){});
  }
*/
  void get_weather()async{

  }

  void set_enforce(int val)async{
    var tableRef = refdatabase ;
    await tableRef.update(
      {
        'Enforce':val
      }
    );
    tableRef.push();
  }
  void set_plant(dynamic p)async{
    var db = refdatabase.child("Plant_info");
    await db.set(
        p
    );
    db.push();

  }

  void get_plant()async{
    var db = refdatabase.child("Plant_info");
    db.once().then((DataSnapshot snapshot) {
      sched.name = snapshot.value['name'];
      sched.sunlight_schedual = snapshot.value['sunlight_scheduale'];
      sched.water_schedual = snapshot.value['water_scheduale'];
    });
  }
  void get_region()async{
    var db = refdatabase.child("Region");
    db.once().then((DataSnapshot snapshot) {
      inf.region = snapshot.value ;
    });
  }

  void get_is_op(){
    var db = refdatabase.child("is_op");
    db.once().then((DataSnapshot snapshot) {
      is_op.val = snapshot.value ;
    });
    if(is_op.val == 1){
      is_op.iconcolor = Colors.red;
    }else{
      is_op.iconcolor = Colors.greenAccent;
    }
  }
}


