// ignore_for_file: file_names

import 'package:flutter/material.dart';
import 'package:flutter_app/objects/plant.dart';

class plantCard extends StatefulWidget {
  plant? ob ;
  Function()? onpress;
  plantCard({this.ob,this.onpress});
  @override
  _plantCardState createState() => _plantCardState();
}

class _plantCardState extends State<plantCard> {
  @override
  Widget build(BuildContext context) {
    return Card(
      child: InkWell(
        onTap : widget.onpress,
        child: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            Container(
                width: 75,
                child: Image(image: AssetImage('${widget.ob!.img}'),),
            ),
            Expanded(
              child:
              ListTile(
                title: Text('Plant name'),
                subtitle: Text('${widget.ob!.name}'),
              ),
            ),

            Expanded(
              child:
              ListTile(
                title: Text('sunlight'),
                subtitle: Text('${widget.ob!.sunlight_schedual}'+' Minutes a day'),
              ),
            ),
            Expanded(
              child:
              ListTile(
                title: Text('water'),
                subtitle: Text('${widget.ob!.water_schedual}'+' Times a Day'),
              ),
            ),
            //Text('${widget.ob!.val}',style: TextStyle(fontSize: 20))
          ],
        ),
      ),
    );
  }
}
