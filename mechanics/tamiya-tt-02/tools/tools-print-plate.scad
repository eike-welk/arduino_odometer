
//############################################################################## 
//                          Tools Print Plate
//############################################################################## 
// 
// Odometer for the toy car Tamiya TT-02. The odometer can be mounted at the
// differential output shaft.
//
// This file contains special tools that are useful for assembling the odometer,
// arranged for 3D printing.

use <MagnetHolderTool.scad>
use <MagnetInsertionToolTip.scad>
use <MagnetInsertionToolGrip.scad>

translate([5, 20, 0]) 
// rotate([0, 0, -90]) 
MagnetHolderTool();

translate([-11, 17, 0]) 
MagnetInsertionToolTip();

translate([60, 35, 27/2 + 0.5]) 
rotate([0, -90, 0]) 
MagnetInsertionToolGrip();
