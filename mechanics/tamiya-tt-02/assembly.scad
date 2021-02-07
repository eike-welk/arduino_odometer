
//############################################################################## 
//                          Odometer Magnet Wheel
//############################################################################## 
// 
// Odometer for the toy car Tamiya TT-02. The odometer can be mounted at the
// differential output shaft.
//
// This file contains an assembly of the odometer.

use <MagnetWheel.scad>
use <Body.scad>
use <SensorInsert.scad>
use <HallSensor.scad>

// Magnet wheel
rotate([0, 90, 0]) 
MagnetWheel();

// Body
%
translate([2.1, 0, 0]) 
rotate([0, 90, 0]) 
Body();

// Sensor insert
translate([
    2.1 + Body_insert_pos_h(), 
    0, 
    Body_insert_pos_l()
]) 
rotate([0, 90, 0]) 
union() {
    SensorInsert();

    // Place Hall sensor in the sensor insert
    translate([-0.6, 0, 0.5])
    translate([0, 0, HallSensor_body_h()])
    rotate([180, 0, 0])
    HallSensor();
}
