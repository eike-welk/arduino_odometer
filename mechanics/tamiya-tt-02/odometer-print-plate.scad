//############################################################################## 
//                          Odometer Print Plate
//############################################################################## 
// 
// Odometer for the toy car Tamiya TT-02. The odometer can be mounted at the
// differential output shaft.
//
// This file contains all mechanical parts arranged for 3D printing.

use <MagnetWheel.scad>
use <Body.scad>
use <SensorInsert.scad>

translate([30, 0, 0]) 
MagnetWheel();

translate([0, 0, Body_total_h() - 1]) 
rotate([180, 0, 0])
Body();

translate([0, -20, 0]) 
SensorInsert();
