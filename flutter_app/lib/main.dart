import 'package:flutter/material.dart';
import 'package:flutter_app/Pages/Home.dart';
import 'package:flutter_app/Pages/Loading1.dart';

import 'Pages/batteryState.dart';
import 'Pages/humidityStat.dart';
import 'Pages/moistureStat.dart';
import 'Pages/selectPlant.dart';
import 'Pages/sunlightStat.dart';
import 'Pages/tempStat.dart';
import 'Pages/waterlevelStat.dart';

void main() {
  runApp(MaterialApp(

    initialRoute: '/',
    routes: {
      '/'              : (context)=> Home(), //Loading1(),
      '/selectPlant'   : (context)=> selectPlant(),
      '/sunlightStat'  : (context)=> sunlightStat(),
      '/batteryStat'   : (context)=> batteryStat(),
      '/waterlevelStat': (context)=> waterlevelStat(),
      '/moistureStat'  : (context)=> moistreStat(),
      '/humidityStat'  : (context)=> humidityStat(),
      '/tempStat'      : (context)=> tempStat(),
    },
  ));
}

