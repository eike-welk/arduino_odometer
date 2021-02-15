//############################################################################## 
//                     Odometer - Magnet Insertion Tool Grip
//############################################################################## 
// 
// Odometer for the toy car Tamiya TT-02. The odometer can be mounted at the
// differential output shaft.
//
// This part is a tool, to insert magnets into the magnet wheel with the right
// orientation.
//

use <MagnetInsertionToolTip.scad>

module MagnetInsertionToolGrip() {
    $fa = 4;
    $fs = 0.4;

    total_l = 120;

    grip_thick_w = 27;
    grip_thick_l = 55;

    tip_outer_d = 11;

    smooth_radius = 3;

    // Thick part of the grip, consisting of spherical surfaces.
    module grip_thick(){
        scale([
            grip_thick_w / (grip_thick_w + smooth_radius*2), 
            grip_thick_w / (grip_thick_w + smooth_radius*2), 
            grip_thick_l / (grip_thick_l + smooth_radius*2)
        ])
        minkowski() {
            sphere(r=smooth_radius);
            intersection() {
                // Main part: intersection of spheres.
                sphere_r = 50;
                sphere_center_r = grip_thick_w/2 - sphere_r ;
                intersection_for (a = [0:60:360]) {
                    rotate([0, 0, a]) 
                    translate([sphere_center_r, 0, 0]) 
                    sphere(r=sphere_r);
                }
                
                // Rounded ends of the thick part
                union() {
                    end_cap_sphere_r = 12;
                    // Upper end cap
                    translate([0, 0, grip_thick_l/2 - end_cap_sphere_r]) 
                    sphere(r=end_cap_sphere_r);
                    // Cylinder to make intersection useful
                    cylinder(
                        d=grip_thick_w*1.5, 
                        h=(grip_thick_l - end_cap_sphere_r*2) * 1.2, 
                        center=true);
                    // Lower end cap
                    translate([0, 0, -grip_thick_l/2 + end_cap_sphere_r]) 
                    sphere(r=end_cap_sphere_r);
                }
            }
        }
    }

    slim_l = total_l - grip_thick_l;
    
    // Slim long part that extends to the grip
    module grip_slim() {
        difference() {
            // Slender cone that holds the tip
            translate([0, 0, 0]) 
            cylinder(
                d1=tip_outer_d, 
                d2=15, 
                h=slim_l + 10
            );

            // Conical hole that holds the tip
            cylinder(d1=8, d2=4, h=10);
        }
    }

    // Thick part
    translate([0, 0, grip_thick_l/2 + slim_l]) 
    grip_thick();

    // Thin part
    grip_slim();
}


// test if tip fits really into the grip
difference() {
    union() {
        MagnetInsertionToolGrip();

        translate([0, 0, -14.5]) 
        MagnetInsertionToolTip();
    }
    
    translate([0, -25, -20]) 
    cube(size=[50, 50, 200]);
}


// MagnetInsertionToolGrip();