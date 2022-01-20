// ignore_for_file: file_names

import 'package:flutter/material.dart';
import 'package:flutter_app/objects/plant.dart';
import 'package:flutter_app/objects/plantCard.dart';

class selectPlant extends StatefulWidget {
  @override
  _selectPlantState createState() => _selectPlantState();
}

class _selectPlantState extends State<selectPlant> {
  plant p1 = plant(name: 'Agloenema Chinese Evergreen',img: 'assets/Agloenema_Chinese_Evergreen.jpg',sunlight_schedual: 20,water_schedual: 2);
  plant p2 = plant(name: 'Albuca Frizzle Sizzle',img: 'assets/Albuca_Frizzle_Sizzle.jpg',sunlight_schedual: 30,water_schedual: 3);
  plant p3 = plant(name: 'Calathea Ornata',img: 'assets/Calathea_Ornata.jpg',sunlight_schedual: 40,water_schedual: 4);
  plant p4 = plant(name: 'Chinese Money Plant',img: 'assets/Chinese_Money_Plant.jpg',sunlight_schedual: 50,water_schedual: 5);
  plant p5 = plant(name: 'Christmas Cactus',img: 'assets/Christmas_Cactus.jpg',sunlight_schedual: 60,water_schedual: 2);
  plant p6 = plant(name: 'Lady Palm',img: 'assets/Lady_Palm.jpg',sunlight_schedual: 70,water_schedual: 3);
  plant p7 = plant(name: 'Paddle Plant',img: 'assets/Paddle_Plant.jpg',sunlight_schedual: 80,water_schedual: 4);
  plant p8 = plant(name: 'Peace Lily',img: 'assets/Peace_Lily.jpg',sunlight_schedual: 90,water_schedual: 5);
  plant p9 = plant(name: 'Phalaenopsis Orchid',img: 'assets/Phalaenopsis_Orchid.jpg',sunlight_schedual: 100,water_schedual: 2);
  plant p10 = plant(name: 'Ponytail Palm',img: 'assets/Ponytail_Palm.jpg',sunlight_schedual: 110,water_schedual: 3);
  plant p11 = plant(name: 'Rubber Plant',img: 'assets/Rubber_Plant.jpg',sunlight_schedual: 120,water_schedual: 4);
  plant p12 = plant(name: 'Schefflera',img: 'assets/Schefflera.jpg',sunlight_schedual: 130,water_schedual: 5);
  plant p13 = plant(name: 'Spider Plant',img: 'assets/Spider_Plant.jpg',sunlight_schedual: 5,water_schedual: 1440);
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Select Plant"),
        centerTitle: true,
        backgroundColor: Colors.lightBlueAccent,
      ),
      body: SingleChildScrollView(
        child: Column(
          children: [
            SizedBox(height: 15,),
            plantCard(
              ob: p1,
              onpress: (){
                Navigator.pop(context,{
                  'name' : p1.name,
                  'water_scheduale':p1.water_schedual,
                  'sunlight_scheduale': p1.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p2,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p2.name,
                  'water_scheduale':p2.water_schedual,
                  'sunlight_scheduale': p2.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p3,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p3.name,
                  'water_scheduale':p3.water_schedual,
                  'sunlight_scheduale': p3.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p4,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p4.name,
                  'water_scheduale':p4.water_schedual,
                  'sunlight_scheduale': p4.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p5,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p5.name,
                  'water_scheduale':p5.water_schedual,
                  'sunlight_scheduale': p5.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p6,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p6.name,
                  'water_scheduale':p6.water_schedual,
                  'sunlight_scheduale': p6.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p7,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p7.name,
                  'water_scheduale':p7.water_schedual,
                  'sunlight_scheduale': p7.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p8,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p8.name,
                  'water_scheduale':p8.water_schedual,
                  'sunlight_scheduale': p8.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p9,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p9.name,
                  'water_scheduale':p9.water_schedual,
                  'sunlight_scheduale': p9.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p10,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p10.name,
                  'water_scheduale':p10.water_schedual,
                  'sunlight_scheduale': p10.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p11,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p11.name,
                  'water_scheduale':p11.water_schedual,
                  'sunlight_scheduale': p11.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p12,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p12.name,
                  'water_scheduale':p12.water_schedual,
                  'sunlight_scheduale': p12.sunlight_schedual,
                });},
            ),
            plantCard(
              ob: p13,
              onpress: (){
                Navigator.pop(context, {
                  'name' : p13.name,
                  'water_scheduale':p13.water_schedual,
                  'sunlight_scheduale': p13.sunlight_schedual,
                });},
            ),
          ],
        ),
      ),
    );
  }
}
