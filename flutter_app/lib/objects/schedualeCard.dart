// ignore_for_file: camel_case_types, file_names

import 'package:flutter/material.dart';
import 'package:flutter_app/objects/schedule.dart';

class schedualeCard extends StatefulWidget {
  scheduale? ob ;
  Function()? onpress ;
  schedualeCard({this.ob,this.onpress});
  @override
  _schedualeCardState createState() => _schedualeCardState();
}

class _schedualeCardState extends State<schedualeCard> {
  @override
  Widget build(BuildContext context) {
    return Card(
      child: InkWell(
        onTap : widget.onpress,
        child: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            Icon(Icons.eco ,color: Colors.greenAccent,),
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
                subtitle: Text('${widget.ob!.sunlight_schedual}'),
              ),
            ),
            Expanded(
              child:
              ListTile(
                title: Text('water'),
                subtitle: Text('${widget.ob!.water_schedual}'),
              ),
            ),
            //Text('${widget.ob!.val}',style: TextStyle(fontSize: 20))
          ],
        ),
      ),
    );
  }
}
