
//############################################################################## 
//                          Odometer Magnet Wheel
//############################################################################## 
// 
// Odometer for the toy car Tamiya TT-02. The odometer can be mounted at the
// differential output shaft.
//
// This part is the magnet that triggers the hall sensor. A number of magnets
// are mounted in the magnet wheel. 

// Interface values
function Magnet_d() = 2;
function Magnet_w() = 2; 

module Magnet() {
    $fa = 4;
    $fs = 0.4;

    cylinder(d=Magnet_d(), h=Magnet_w());
}

Magnet();
