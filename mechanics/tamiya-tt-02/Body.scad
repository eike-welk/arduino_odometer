//############################################################################## 
//                          Odometer Body
//############################################################################## 
// 
// Odometer for the toy car Tamiya TT-02. The odometer can be mounted at the
// differential output shaft.
//
// This part is the body of the odometer.
//
// Current State
// ====================
// 
// * The body fits into the car.
// * The magnet wheel can turn.
// * Wheel and body click together nicely.
// * Body and wheel stay together well aligned. However they can be separated
//   with little force.
//
// * The body should not be longer than its current length. It is close to the
//   limit.
//
// * The current supports are somewhat difficult to remove.
//
// * The "bone" of the universal joint rubs strongly against the wheel hub.
// 

use <helpers.scad>

use <MagnetWheel.scad>
use <SensorInsert.scad>

// Bearing - cylindrical part
bearing_inner_d = MagnetWheel_hub_outer_d() + 0.6; // The wheel hub is widened by pressure
bearing_outer_d = bearing_inner_d + 3;
bearing_h = MagnetWheel_hub_bearing_w() - 0.5;

// Flat part - the hall sensor slides into this section
flat_l = 40;
flat_w = 12;
flat_h = MagnetWheel_hub_bearing_w() + 2;
flat_pos_h = -1;
// Brace - reinforcement at root of the flat part
brace_l = 5.6;
brace_w = flat_w + 2;
brace_h = flat_h;
// // Plate that closes the bottom
// // TODO: This should really be just the flat part
// bottom_l = flat_l - MagnetWheel_disk_outer_d()/2 - 1;
// bottom_w = flat_w;
// bottom_h = 2;
// bottom_pos_l = MagnetWheel_disk_outer_d()/2 + 1;

// Strips - to keep the odometer from turning
strip_l = 50;
strip_w = 1.2;
strip_h = 2.5;
strip_ang = 30;

// Position of the sensor insert
insert_pos_l = bearing_inner_d/2;
insert_pos_h = -0.005;

module Body() {
    $fa = 4;
    $fs = 0.4;

    difference() {
        union() {
            // Bearing - cylindrical part
            cylinder(d=bearing_outer_d, h=bearing_h);

            // Flat part
            translate([-flat_l, -flat_w/2, flat_pos_h])
            cube([flat_l, flat_w, flat_h]);
            
            // Brace at root of flat part
            translate([
                -brace_l/2 - bearing_inner_d/2, 
                -brace_w/2, 
                flat_pos_h
            ]) 
            cube([brace_l, brace_w, brace_h]);

            // // Brace at end of flat part
            // translate([
            //     -flat_l, 
            //     -flat_w/2, 
            //     0
            // ]) 
            // cube([brace_l, flat_w, brace_h]);

            // // Bottom
            // translate([-bottom_l - bottom_pos_l, -bottom_w/2, -bottom_h])
            // cube(size=[bottom_l, bottom_w, bottom_h]);

            // Strips
            mirror_copy([0, 1, 0])
            translate([
                -flat_l + strip_w*1.8, 
                -flat_w/2 + strip_w/2, 
                flat_h + flat_pos_h - strip_h
            ]) 
            rotate([0, 0, 180 + strip_ang]) 
            translate([0, -strip_w/2, 0])
            cube([strip_l, strip_w, strip_h]);
        }

        // Create the necessary space for the magnet wheel
        // MagnetWheel_negative();
        // Hole in bearing
        translate([0, 0, -0.1])
        cylinder(d=bearing_inner_d, h=bearing_h + 0.2);
        // Space for locking mechanism
        translate([0, 0, bearing_h])
        cylinder(d=bearing_outer_d, h=5);
        // Space for the magnet disk
        translate([0, 0, -5])
        cylinder(d=MagnetWheel_disk_outer_d() + 2, h=5);

        
        // Create the necessary space for the sensor insert.
        translate([-insert_pos_l + 0.1, 0, insert_pos_h])
        SensorInsert_negative();

        // Remove the thin wall where the insert touches the bearing's hole
        cut_l = 1;
        cut_w = 6;
        translate([-cut_l/2 - bearing_inner_d/2, -cut_w/2, -0.01]) 
        cube(size=[cut_l, cut_w, SensorInsert_total_h() + 0.5]);
    }
}

function Body_total_h() = flat_h;
function Body_insert_pos_l() = insert_pos_l;
function Body_insert_pos_h() = insert_pos_h;

Body();