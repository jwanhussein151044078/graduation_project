// ignore_for_file: camel_case_types, file_names

import 'package:firebase_database/firebase_database.dart';

class timeStamp{
  final List<int> cal = [31,28,31,30,31,30,31,31,30,31,30,31];
  final List<String> mon = ["Jan","Feb","Mar","Apr","May",
                    "Jun","Jul","Aug","Sep","Oct",
                    "Nov","Dec"];
  final refdatabase = FirebaseDatabase.instance.reference() ;

  int d = 0;
  int m = 0;
  int y = 0;


  Future<String> getTimeStamp()async{
    String ret = "";
    var db = refdatabase.child("TIME");
    await db.once().then((DataSnapshot snapshot) {
      d = snapshot.value["d"];
      m = snapshot.value["m"];
      y = snapshot.value["y"];
    });
    int counter = 0 ;
    while(d - cal[counter] > 0){
      d = d - cal[counter] ;
      counter++;
    }
    String temp1 = ((m/60).floor()).toString();
    if(temp1.length == 1){
      temp1 = '0' + temp1 ;
    }
    String temp2 = (m%60).toString();
    if(temp2.length == 1){
      temp2 = '0' + temp2 ;
    }

    ret = temp1+":"+ temp2 +"  "+d.toString() + "/" + mon[counter] + "/" +y.toString();
    return ret ;
  }
}