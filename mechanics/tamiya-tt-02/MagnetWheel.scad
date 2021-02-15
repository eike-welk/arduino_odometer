//############################################################################## 
//                          Odometer Magnet Wheel
//############################################################################## 
// 
// Odometer for the toy car Tamiya TT-02. The odometer can be mounted at the
// differential output shaft.
//
// This part is a wheel with magnets in it. It is mounted with friction on the
// output shaft of a differential.
//
// Current State
// ====================
// 
// * The part can be mounted and it can turn.
//
// * The inner diameter is good. The wheel sticks tightly on the universal
//   joint. The part can be mounted with only little difficulty.
// * The magnet diameter is good. The magnets can be easily mounted and stick in
//   the wheel.
//
// * The hub is somewhat fragile. However this poses no problems, because in
//   operation it is supported by the universal joint.
//
// * The hub should be slightly longer, the locking mechanism rubs on the body.
//
// * The locking teeth should be bigger. The body can be easily removed even
//   when the odometer is mounted on the universal joint.
//

use <helpers.scad>

use <Magnet.scad>

    // Hub
    hub_inner_d = 16;
    hub_outer_d = 18;
    hub_bearing_w = 7;

    // Clip cone
    clip_cone_w = 2.5;
    clip_cone_teeth_h = 0.6;
    clip_cone_outer_d = hub_outer_d + clip_cone_teeth_h*2;

    // Hub
    hub_width = Magnet_w() + hub_bearing_w + clip_cone_w;

    // Holes for magnets
    mag_hole_d = Magnet_d();
    mag_hole_w = Magnet_w();
    mag_hole_circle_d = hub_outer_d + 2*(1+Magnet_d()/2);

    // Disk
    disk_outer = hub_outer_d + 2*(1+Magnet_d()+1);
    disk_width = Magnet_w();

module MagnetWheel() {
    $fa = 4;
    $fs = 0.4;

    // render()
    difference() {
        union() {
            // Hub
            cylinder(d=hub_outer_d, h=hub_width);
            // Disk    
            cylinder(d=disk_outer, h=disk_width);
            // Clip cone
            clip_cone_w1 = clip_cone_w - clip_cone_teeth_h;
            clip_cone_w2 = clip_cone_teeth_h;
            translate([0, 0, hub_width - clip_cone_w1]) 
            cylinder(
                d1=clip_cone_outer_d, 
                d2=hub_outer_d, 
                h=clip_cone_w1
            );
            translate([0, 0, hub_width - clip_cone_w]) 
            cylinder(
                d1=hub_outer_d, 
                d2=clip_cone_outer_d, 
                h=clip_cone_w2
            );
        }

        // Central hole
        translate([0, 0, -0.1]) 
        cylinder(
            d=hub_inner_d, 
            h=hub_width + 0.2
        );

        // Holes for magnets
        magnet_circle_r = mag_hole_circle_d/2;
        for(a = [0: 30: 360]) {
            translate([
                magnet_circle_r*cos(a), 
                magnet_circle_r*sin(a), 
                -0.1
            ]) 
            cylinder(d=mag_hole_d, h=mag_hole_w + 0.2);
        }

        // Slots for the clip function
        slot_length = 4;
        for (a=[0, 45, 90, 135]) {
            rotate([0, 0, a])
            translate([0, 0, hub_width - slot_length/2]) 
            cube(
                [hub_outer_d + 4, 1, slot_length + 0.01], 
                center=true
            );
        }
    }
}

// // The free space which the magnet wheel needs
// tolerance_axial = 0.5;
// tolerance_radial = 0.6;

// module MagnetWheel_negative() {
//     scale_f = (hub_outer_d + 2*tolerance_radial) / hub_outer_d;
//     // render()
//     scale([scale_f, scale_f, 1])
//     // Translate and copy 
//     translate_copy([0, 0, -tolerance_axial])
//     // close small holes and slots by rotating and copying
//     rotate_copy([0, 0, 6])
//     rotate_copy([0, 0, 6])
//     union() {
//         MagnetWheel();
//         // Close the center hole
//         cylinder(d=hub_inner_d, h=hub_width);
//         // Cylinder instead of clip cone
//         translate([0, 0, hub_width - clip_cone_w]) 
//         cylinder(
//             d=hub_outer_d + clip_cone_teeth_h*2,
//             h=clip_cone_w
//         );
//     }
// }

// Interface values
function MagnetWheel_hub_outer_d() = hub_outer_d;
function MagnetWheel_hub_bearing_w() = hub_bearing_w;
function MagnetWheel_total_w() = hub_width;
function MagnetWheel_disk_outer_d() = disk_outer;
function MagnetWheel_disk_w() = disk_width;
function MagnetWheel_lock_outer_d() = disk_outer;
function MagnetWheel_lock_w() = disk_width;

MagnetWheel();

// %MagnetWheel_negative();
