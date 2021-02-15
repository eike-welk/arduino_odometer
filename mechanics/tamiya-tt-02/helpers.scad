//##############################################################################
//                               Helpers
//##############################################################################
//
// Helper functions and modules


// A mirror module that retains the original
// object in addition to the mirrored one.
//
// Taken from:
// 
// https://en.wikibooks.org/wiki/OpenSCAD_User_Manual/Tips_and_Tricks#Create_a_mirrored_object_while_retaining_the_original
//
module mirror_copy(v=[1, 0, 0]) {
    children();

    mirror(v) 
    children();
}

// A rotation module that retains the original
// object in addition to the mirrored one.
module rotate_copy(a=[10, 10, 10]) {
    children();

    rotate(a) 
    children();
}

// A translation module that retains the original
// object in addition to the mirrored one.
module translate_copy(t=[0, 0, 0]) {
    children();

    translate(t) 
    children();
}
