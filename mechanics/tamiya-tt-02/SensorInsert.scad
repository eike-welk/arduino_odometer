
//############################################################################## 
//                          Odometer Hall Sensor Insert
//############################################################################## 
// 
// Odometer for the toy car Tamiya TT-02. The odometer can be mounted at the
// differential output shaft.
//
// This part is the insert that carries the hall sensor. It snaps into the body.
//
// Current State
// ====================
// 
// * The current locking mechanism seems very secure.
// * The insert can reasonably easily be removed with the handles.
//
// * The insert is big enough for the sensor, and maybe also for a circuit
//   board.
//
// * It needs to be tested if the sensor gets close enough to the magnets to
//   trigger.
//

use <helpers.scad>

use <HallSensor.scad>

// Dimensions of the main part ------------------
main_l = 24;
main_w = 10;
main_h = 5;

bevel_w = 1;
wall_t = 1;

// Dimensions of the locking mechanism ----------
// Teeth
teeth_l = 2;
teeth_w = 1;
teeth_h = 3;
teeth_pos_l = 4;
teeth_pos_h = 1.0;

// Cantilever / Strip
lock_l = 15;
lock_w = 1;
lock_h = main_h;

// Grip
grip_l = 2;
grip_w = 3;
grip_h = lock_h;

// The locking part, consisting of two strips / cantilevers with teeth.
module SensorInsert_lock() {
    // Tooth for locking
    module lock_tooth() {
        linear_extrude(height=teeth_h)
        polygon(points=[
            [ 0      , 0       ],
            [-teeth_l, 0       ],
            [-teeth_l,-teeth_w ],
            [-teeth_w,-teeth_w ],
        ]);
    }

    // Outer shape of the insert
    module main_shape() {
        rotate([0, -90, 0])
        linear_extrude(height=lock_l)
        SensorInsert_front_side_2D();
    }

    // The plastic strip that carries the tooth
    module lock_strip() {
        // Create the strip, and shape it like the outer shape of the insert.
        intersection() {
            translate([-lock_l, -main_w/2,  0])
            cube(size=[lock_l, lock_w, lock_h]);

            main_shape();
        }

        // Create and position the tooth
        translate([-teeth_pos_l + teeth_l, - main_w/2,  teeth_pos_h])
        lock_tooth();

        // Create and position the grip
        translate([-lock_l, -main_w/2 - grip_w + lock_w, 0])
        cube(size=[grip_l, grip_w, grip_h]);
    }

    // Strip 1
    lock_strip();

    // Strip 2
    mirror([0, 1, 0]) 
    lock_strip();
}

// // Symmetrically bevelled front side
// module SensorInsert_front_side_2D() {
//     polygon(points=[
//         [ 0               ,  main_w/2 - bevel_w ],
//         [ bevel_w         ,  main_w/2           ],
//         [ main_h - bevel_w,  main_w/2           ],
//         [ main_h          ,  main_w/2 - bevel_w ],
//         [ main_h          , -main_w/2 + bevel_w ],
//         [ main_h - bevel_w, -main_w/2           ],
//         [ bevel_w         , -main_w/2           ],
//         [ 0               , -main_w/2 + bevel_w ],
//     ]);
// }

// // Front side which is bevelled at the top
// module SensorInsert_front_side_2D() {
//     polygon(points=[
//         [ 0               ,  main_w/2           ],
//         [ bevel_w         ,  main_w/2           ],
//         [ main_h - bevel_w,  main_w/2           ],
//         [ main_h          ,  main_w/2 - bevel_w ],
//         [ main_h          , -main_w/2 + bevel_w ],
//         [ main_h - bevel_w, -main_w/2           ],
//         [ bevel_w         , -main_w/2           ],
//         [ 0               , -main_w/2           ],
//     ]);
// }

// Front side which is bevelled at the bottom
module SensorInsert_front_side_2D() {
    polygon(points=[
        [ 0               ,  main_w/2 - bevel_w ],
        [ bevel_w         ,  main_w/2           ],
        [ main_h - bevel_w,  main_w/2           ],
        [ main_h          ,  main_w/2           ],
        [ main_h          , -main_w/2           ],
        [ main_h - bevel_w, -main_w/2           ],
        [ bevel_w         , -main_w/2           ],
        [ 0               , -main_w/2 + bevel_w ],
    ]);
}

// Front side of the hollow space inside of the insert.
module SensorInsert_front_side_negative_2D() {
    offset(delta=-wall_t) {
        SensorInsert_front_side_2D();
    }
    polygon(points=[
        [ main_h - wall_t - 1 , main_w/2 - bevel_w ],
        [ main_h + 1          , main_w/2 - bevel_w ],
        [ main_h + 1          ,-main_w/2 + bevel_w ],
        [ main_h - wall_t - 1 ,-main_w/2 + bevel_w ],
    ]);
}

module SensorInsert() {
    // Create the main part
    difference() {
        rotate([0, -90, 0])
        linear_extrude(height=main_l)
        SensorInsert_front_side_2D();

        // Hollow space in main part
        translate([-1, 0, 0])
        rotate([0, -90, 0])
        linear_extrude(height=main_l - wall_t*2 )
        SensorInsert_front_side_negative_2D();

        // Create a small indentation for the sensor
          sensor_body_l = HallSensor_body_l() + 0.5; 
          sensor_body_w = HallSensor_body_w() + 0.5; 
          sensor_body_h = HallSensor_body_h() + 0.5; 
        translate([-0.6, 0, 0.5])
        translate([
            -sensor_body_l, 
            -sensor_body_w / 2, 
            0
        ])
        cube([
            sensor_body_l, 
            sensor_body_w, 
            sensor_body_h
        ]);

        // Create a slot for the cables
        slot_l = wall_t + 0.02;
        slot_w = main_w - wall_t*2;
        slot_h = 2;
        translate([-main_l - 0.01, -slot_w/2, wall_t])
        cube(size=[slot_l, slot_w, slot_h]);
    }

    // Create the locking mechanism
    translate([-main_l, 0, 0])
    SensorInsert_lock();
}

// The hollow space needed by the sensor insert
module SensorInsert_negative() {
    // Make the hollow space slightly bigger than the insert.
    scale([1, 1.05, 1.1]) {
        // Create the main part
        rotate([0, -90, 0])
        linear_extrude(height=main_l + lock_l + 5)
        SensorInsert_front_side_2D();

        // Create pockets for the teeth of the locking mechanism.
        pocket_l = teeth_l * 1.5;
        pocket_w = teeth_w * 1.1;
        pocket_h = teeth_h * 1.0;
        mirror_copy([0, 1, 0])
        translate([
            -teeth_pos_l - main_l - (pocket_l - teeth_l)/2, 
            - main_w/2 - pocket_w + 0.01,  
            teeth_pos_h
        ])
        cube(size=[pocket_l, pocket_w, pocket_h]);
    }
}

function SensorInsert_total_h() = main_h;

SensorInsert();

%SensorInsert_negative();