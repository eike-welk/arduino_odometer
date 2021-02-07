//############################################################################## 
//                       Odometer - Magnet Holder Tool
//############################################################################## 
// 
// Odometer for the toy car Tamiya TT-02. The odometer can be mounted at the
// differential output shaft.
//
// This part is a tool, to hold a single magnet for various tests.
//

module MagnetHolderTool(args) {
    $fa = 4;
    $fs = 0.4;

    difference() {
        union() {
            cylinder(d=10, h=2);
            translate([0, -5, 0]) 
            cube(size=[55, 10, 2]);
        }

        cylinder(d=2, h=10, center=true);
    }
}

MagnetHolderTool();
