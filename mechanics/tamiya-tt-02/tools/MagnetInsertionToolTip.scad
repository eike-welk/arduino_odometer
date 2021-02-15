//############################################################################## 
//                    Odometer - Magnet Insertion Tool Tip
//############################################################################## 
// 
// Odometer for the toy car Tamiya TT-02. The odometer can be mounted at the
// differential output shaft.
//
// This part is a tool, to insert magnets into the magnet wheel with the right
// orientation.
//

module MagnetInsertionToolTip() {
    $fa = 4;
    $fs = 0.4;

    magnet_diameter = 2;
    magnet_height = 2;

    module tip_1() {
        difference() {
            union(){
                cylinder(d1=magnet_diameter+2, d2=8, h=10);
                translate([0, 0, 10]) 
                cylinder(d=10, h=2);
            }
            cylinder(d=magnet_diameter, h=magnet_height + 0.5);
        };
    }

    translate([0, 0, 12]) {
        translate([0, 0, -12]) 
        tip_1();
        mirror([0, 0, 1]) {
            translate([0, 0, -12]) 
            tip_1();
        }
    }
}

MagnetInsertionToolTip();