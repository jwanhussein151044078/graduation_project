// ignore_for_file: file_names

import 'package:flutter/material.dart';
import 'package:flutter_spinkit/flutter_spinkit.dart';

class Loading1 extends StatefulWidget {
  @override
  _Loading1State createState() => _Loading1State();
}

class _Loading1State extends State<Loading1> {
  @override
  Widget build(BuildContext context) {
    return const Scaffold(
      backgroundColor: Colors.greenAccent,
      body: Center(
        child: SpinKitPouringHourGlassRefined(
          color: Colors.white,
          size: 180,

        ),
      )
    );
  }
}
