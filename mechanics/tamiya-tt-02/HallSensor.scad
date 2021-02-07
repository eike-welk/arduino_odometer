
//############################################################################## 
//                          Odometer Body
//############################################################################## 
// 
// Odometer for the toy car Tamiya TT-02. The odometer can be mounted at the
// differential output shaft.
//
// This part is the Hall Sensor itself.
//

// Dimensions of "Infineon TLE 4905L"
body_l = 3.29;
body_w = 4.16;
body_h = 1.52;

bevel_w = 0.8;
bevel_h = 0.8;

wire_l = 20;
wire_w = 0.4;
wire_h = 0.2;
wire_pos_h = body_h - 0.79;
wire_dist = 1.27;

module HallSensor() {
    // Wire
    module Wire() {
        color("LightSteelBlue")
        translate([0, -wire_w/2, -wire_h/2])
        rotate([0, -90, 0])
        cube(size=[wire_h, wire_w, wire_l]);
    }

    // Body of the Hall Sensor
    color("Gray")
    rotate([0, -90, 0])
    linear_extrude(height=body_l)
    polygon(points=[
        [ 0                ,  body_w/2           ],
        [ body_h - bevel_h ,  body_w/2           ],
        [ body_h           ,  body_w/2 - bevel_w ],
        [ body_h           , -body_w/2 + bevel_w ],
        [ body_h - bevel_h , -body_w/2           ],
        [ 0                , -body_w/2           ],
    ]);

    // Create and place the wires
    translate([-body_l, wire_dist, wire_pos_h])
    Wire();
    translate([-body_l, 0, wire_pos_h])
    Wire();
    translate([-body_l, -wire_dist, wire_pos_h])
    Wire();
}

function HallSensor_body_l() = body_l;
function HallSensor_body_w() = body_w;
function HallSensor_body_h() = body_h;

HallSensor();