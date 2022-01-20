// ignore_for_file: file_names


import 'dart:async';

import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:flutter_app/objects/timeStamp.dart';
import 'package:syncfusion_flutter_charts/charts.dart';
import 'package:syncfusion_flutter_charts/sparkcharts.dart';

class tempStat extends StatefulWidget {
  @override
  _tempStatState createState() => _tempStatState();
}

class _tempStatState extends State<tempStat> {
  List<data> l = [];
  final refdatabase = FirebaseDatabase.instance.reference() ;
  int SIZE = 288 ;
  int counter = 0;
  int current = 0;

  Timer? timer;
  Timer? timer2;

  String timestamp = "" ;
  timeStamp ts = timeStamp();

  @override
  void setState(fn) {
    if(mounted) {
      super.setState(fn);
    }
  }
  @override
  void initState() {
    getData();
    get_current();
    timer = Timer.periodic(Duration(seconds: 5), (Timer t) => get_current());
    timer2 = Timer.periodic(Duration(seconds: 30), (Timer t) => getData());
  }
  @override
  Widget build(BuildContext context) {

    return Scaffold(
        appBar: AppBar(
          title: const Text("Temperature Statistics"),
          centerTitle: true,
          backgroundColor: Colors.lightBlueAccent,
        ),
        body: SafeArea(
          child: Scaffold(
            body: Column(
              children:[
                SizedBox(height: 50,),
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  children: [
                    Text('Current : ',style: TextStyle(fontSize: 30),),
                    Text('$current',style: TextStyle(fontSize: 30),),
                  ],
                ),
                SizedBox(height: 50,),
                SfCartesianChart(

                  series: <ChartSeries>[
                    LineSeries<data,int>(
                      dataSource: l,
                      xValueMapper: (data d ,_)=>d.time,
                      yValueMapper: (data d ,_)=>d.val,
                    )
                  ],
                ),
                SizedBox(height: 50,),
                Column(
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  children: [
                    Text('The last measurement was received at',style: TextStyle(fontSize: 20),),
                    Text(timestamp,style: TextStyle(fontSize: 30),),
                  ],
                ),
              ],
            ),
          ),
        )
    );
  }

  Future<void> getData() async {
    timestamp = await ts.getTimeStamp();
    await get_counter();
    l = [];
    //print(counter);
    var db = refdatabase.child("MHT");
    await db.once().then((DataSnapshot snapshot) {
      for(int i = (counter+1)%SIZE, w= 0 ; w<SIZE ; w++,i=(i+1)%SIZE ){
        //print(snapshot.value[i.toString()].toString() +"\t"+(i).toString() );
        l.add(data(val: ((snapshot.value[i.toString()])%100),time:(w)));
      }
    });
    setState(() {});

  }

  Future<void> get_counter() async {
    var db = refdatabase.child("counter");
    await db.once().then((DataSnapshot snapshot) {
      //print(snapshot.value);
      counter =  snapshot.value;
    });
  }
  Future<void> get_current() async {
    var db = refdatabase.child("MHT/current");
    await db.once().then((DataSnapshot snapshot) {
      //print(snapshot.value);
      current =  (snapshot.value%100);
    });
    setState(() {});
  }
}


class data {
  int val ;
  int time ;
  data({required this.val,required this.time});
}