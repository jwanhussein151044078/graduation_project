// ignore_for_file: file_names

import 'dart:async';

import 'package:flutter/material.dart';

import 'homeObject.dart';

class myCard extends StatefulWidget {

  homeobject? ob ;
  Function()? onpress ;

  myCard({Key? key, this.ob,this.onpress}) : super(key: key);
  @override
  _myCardState createState() => _myCardState();
}

class _myCardState extends State<myCard> {


  @override
  Widget build(BuildContext context) {
    return Card(
      child: InkWell(
        onTap : widget.onpress,
        child: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            Icon(widget.ob!.icon,color: widget.ob!.iconcolor,),
            Expanded(
              child:
              ListTile(
                title: Text('${widget.ob!.name}'),
                subtitle: Text('${widget.ob!.subtitle}'),
              ),
            ),
            Text('${widget.ob!.val}',style: TextStyle(fontSize: 20))
          ],
        ),
      ),
    );
  }
}

