///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//               ROOTMAP 3D visualisation scene for POV-ray 3.6              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////



// CAMERA ROTATION SETTING ENUMERATION    --------------------------------------------------------

// MSA TODO does POV-ray support enumerations? No search results in the documentation...
#declare NONE = 0;
#declare WHILE_GROWING = 1;
#declare AFTER_GROWING = 2;
#declare NO_GROWING = 3;
#declare CUSTOM_PATH = 4;



// SCENE SETUP DATA FILE INCLUSION    -------------------------------------------------------------


// This file defines all the often-modified configuration options for ROOTMAP raytraces -
// selection of Characteristics to visualise, minima and maxima thereof, camera rotation settings, etc.
#include "Rootmap_Scene_Setup_Data.inc"
#include "shapes.inc" // For Wire_Box



// POV-RAY GLOBAL SETTINGS    ---------------------------------------------------------------------


global_settings {
    assumed_gamma 1
    // High max_trace_level allows many stacked transparent objects (e.g. Scoreboard boxes)
    // Increase this value if "black square" artefacts are encountered;
    // decreasing this value may speed up rendering
    max_trace_level 2
    ambient_light rgb <1,1,1>
}




// TEXTURING AND MISC. VISUALISATION VARIABLES    -------------------------------------------------


#declare ROOT_FINISH = finish {
    ambient 0.2
    diffuse 0.8
    specular 0.5
}
#declare ROOT_NORMAL = normal {
    granite
    scale 1
}

// Radius of nitrate packet spheres
#declare NITRATE_RADIUS=0.04;
// Radius of grid lines
#declare GRID_LINE_RADIUS=0.01;

background {
    colour BACKGROUND_COLOUR
}

#declare NUMBER_OF_PLANTS_TO_DRAW = dimension_size(PLANTS_TO_DRAW,1);




// DISPLAY HANDLING VARIABLES    ------------------------------------------------------------------


// Characteristic selection variable forward declarations.
// These variables will be updated by the data include file

#declare CHARACTERISTIC_NAMES = array[1] {"Dummy string"}
// Initialise indices to invalid array subscripts
#declare CYAN_CHARACTERISTIC_INDEX = -1;
#declare MAGENTA_CHARACTERISTIC_INDEX = -1;
#declare YELLOW_CHARACTERISTIC_INDEX = -1;


// Set up timing conditions for animation and rotation
#declare HORIZONTAL_ROTATION_CLOCK = clock;
#declare VERTICAL_ROTATION_CLOCK = clock;
#declare ANIMATION_CLOCK = clock;
#switch (HORIZONTAL_CAMERA_ROTATION)
    #case (WHILE_GROWING)
        // Both full growth animation and rotation
        // occur during normal clock range
        #break
    #case (AFTER_GROWING)
        // Full growth animation completes at clock == (1.0 - HORIZONTAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE)
        #declare ANIMATION_CLOCK = clock / (1.0 - HORIZONTAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE);
        // rotation begins at clock == (1.0 - HORIZONTAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE) and completes at clock == 1.0
        #declare HORIZONTAL_ROTATION_CLOCK = (1.0/HORIZONTAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE)*clock - (1.0 - HORIZONTAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE)/HORIZONTAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE;
        // Note: single-frame data is a special case - see "ROOTMAP GEOMETRY IMPORT AND PLACEMENT"
        #break
    #case (NO_GROWING)
        // Root system is fully grown for all frames
        #declare ANIMATION_CLOCK=1.0;
        #break
    #case (CUSTOM_PATH)
        // Insert user-defined rotation and animation clocks here.
        #break
    #else
    // Animation clock = clock; no rotation
    #declare HORIZONTAL_ROTATION_CLOCK = 0;
#end

#switch (VERTICAL_CAMERA_ROTATION)
    #case (WHILE_GROWING)
        // Both full growth animation and rotation
        // occur during normal clock range
        #break
    #case (AFTER_GROWING)
        // Full growth animation completes at clock == (1.0 - VERTICAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE)
        #local ani = ANIMATION_CLOCK;
        //#debug concat("\nAG. ANI: ",str(ani,5,5))
        #declare ANIMATION_CLOCK = clock / (1.0 - VERTICAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE);
        #if(ani<ANIMATION_CLOCK)
            //#debug concat("\nClamping animation clock to lower value ANI, because ANI < vertical animation clock which is ",str(ANIMATION_CLOCK,5,5))
            #declare ANIMATION_CLOCK = ani;
        #end
        // rotation begins at clock == (1.0 - VERTICAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE) and completes at clock == 1.0
        #declare VERTICAL_ROTATION_CLOCK = (1.0/VERTICAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE)*clock - (1.0 - VERTICAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE)/VERTICAL_CAMERA_ROTATION_AFTER_GROWING_TIMESHARE;
        // Note: single-frame data is a special case - see "ROOTMAP GEOMETRY IMPORT AND PLACEMENT"
        #break
    #case (NO_GROWING)
        // Root system is fully grown for all frames
        #local ani = ANIMATION_CLOCK;
        //#debug concat("\nNG. ANI: ",str(ani,5,5))
        #declare ANIMATION_CLOCK=1.0;
        #if(ani<ANIMATION_CLOCK)
            //#debug concat("\nClamping animation clock to lower value ANI, because ANI < vertical animation clock which is ",str(ANIMATION_CLOCK,5,5))
            #declare ANIMATION_CLOCK = ani;
        #end
        #break
    #case (CUSTOM_PATH)
        // Insert user-defined rotation and animation clocks here.
        #break
    #else
    // Animation clock = clock; no rotation
    #declare VERTICAL_ROTATION_CLOCK = 0;
#end

#debug concat("\nGROWTH ANIMATION CLOCK: ",str(ANIMATION_CLOCK,5,5))
#debug concat("\nHORIZONTAL ROTATION CLOCK: ",str(HORIZONTAL_ROTATION_CLOCK,5,5))
#debug concat("\nVERTICAL ROTATION CLOCK: ",str(VERTICAL_ROTATION_CLOCK,5,5))
#debug concat("\nPOV-RAY CLOCK: ",str(clock,5,5))

// UTILITY FUNCTIONS AND MACROS    ----------------------------------------------------------------


#local MINR = 0.01;
#local MAXR = 0.1;

#local CalcRadius = function(r,rmax)
{
    MINR + (MAXR - MINR) * log(r+1) / log(rmax+1)
}

#macro SET_CHARACTERISTIC_INDICES(characteristicSummaries)
    #local i = 0;
    #while(i<dimension_size(characteristicSummaries,1))
        #if(strcmp(characteristicSummaries[i],concat(CYAN_PROCESS,", ",CYAN_CHARACTERISTIC))=0)
            #declare CYAN_CHARACTERISTIC_INDEX = i;
        #end
        #if(strcmp(characteristicSummaries[i],concat(MAGENTA_PROCESS,", ",MAGENTA_CHARACTERISTIC))=0)
            #declare MAGENTA_CHARACTERISTIC_INDEX = i;
        #end
        #if(strcmp(characteristicSummaries[i],concat(YELLOW_PROCESS,", ",YELLOW_CHARACTERISTIC))=0)
            #declare YELLOW_CHARACTERISTIC_INDEX = i;
        #end
        #local i = i+1;
    #end
#end



// ROOTMAP GEOMETRY DISPLAY MACROS    -------------------------------------------------------------


// Root Segment macro
#macro RS(pNum,order,r,xx1,yy1,zz1,xx2,yy2,zz2)

    #local root_colour = rgb <ROOT_RED+order*ROOT_LIGHTENING_PER_ORDER,ROOT_GREEN+order*ROOT_LIGHTENING_PER_ORDER,ROOT_BLUE+order*ROOT_LIGHTENING_PER_ORDER>;
    #local idx = 0;
    #while(idx<NUMBER_OF_PLANTS_TO_DRAW)
        #if(PLANTS_TO_DRAW[idx]=pNum)
            #local root_segment_is_zero_length = true;
            #local rr = r*ROOT_RADIUS_MULTIPLIER;
            #if (xx1 != xx2)
                #local root_segment_is_zero_length = false;
            #end
            #if (yy1 != yy2)
                #local root_segment_is_zero_length = false;
            #end
            #if (zz1 != zz2)
                #local root_segment_is_zero_length = false;
            #end
            #if (root_segment_is_zero_length)
                sphere {
                    <xx1,yy1,zz1>, rr
            #else
                union {
                    cylinder {
                        <xx1,yy1,zz1>, <xx2,yy2,zz2>, rr
                    }
                    sphere {
                        <xx1,yy1,zz1>, rr
                    }
                    sphere {
                        <xx2,yy2,zz2>, rr
                    }
            #end
                texture {
                        pigment { color root_colour }
                        finish { ROOT_FINISH }
                        normal { ROOT_NORMAL }
                    }
                }
            #break
        #end
        #local idx = idx+1;
    #end
#end

// Scoreboard Box macro
// This macro is called with the following arguments:
// the two defining coordinates of a Scoreboard box; and
// an array of Characteristic values for this box at this time.
#macro SB(xx1,yy1,zz1,xx2,yy2,zz2,chVals)
    // Retrieve values for Characteristics to be visualised
    #local cyan = 0.0;
    #if(CYAN_CHARACTERISTIC_INDEX>=0)
        #local cyan = (chVals[CYAN_CHARACTERISTIC_INDEX] - CYAN_VALUE_MIN) /  (CYAN_VALUE_MAX - CYAN_VALUE_MIN);
        #if(cyan>CYAN_COLOUR_MAX)      #local cyan = CYAN_COLOUR_MAX;
        #else #if(cyan<CYAN_COLOUR_MIN) #local cyan = CYAN_COLOUR_MIN; #end   #end
    #end
    #local magenta = 0.0;
    #if(MAGENTA_CHARACTERISTIC_INDEX>=0)
        #local magenta = (chVals[MAGENTA_CHARACTERISTIC_INDEX] - MAGENTA_VALUE_MIN) /  (MAGENTA_VALUE_MAX - MAGENTA_VALUE_MIN);
        #if(magenta>MAGENTA_COLOUR_MAX)      #local magenta = MAGENTA_COLOUR_MAX;
        #else #if(magenta<MAGENTA_COLOUR_MIN) #local magenta = MAGENTA_COLOUR_MIN; #end   #end
    #end
    #local yellow = 0.0;
    #if(YELLOW_CHARACTERISTIC_INDEX>=0)
        #local yellow = (chVals[YELLOW_CHARACTERISTIC_INDEX] - YELLOW_VALUE_MIN) /  (YELLOW_VALUE_MAX - YELLOW_VALUE_MIN);
        #if(yellow>YELLOW_COLOUR_MAX)      #local yellow = YELLOW_COLOUR_MAX;
        #else #if(yellow<YELLOW_COLOUR_MIN) #local yellow = YELLOW_COLOUR_MIN; #end   #end
    #end
    box	{
        <xx1,yy1,zz1>
        <xx2,yy2,zz2>
        pigment { rgbf 1 }
        hollow
        interior { media
                    { absorption OVERALL_BOX_INTENSITY*<cyan, magenta, yellow> }
        }
    }
#end

// Media macro for use with DF3 volumes
#macro MEDIA_2ARG(df3FileName, absorptionColour)
    media {
        emission <1,1,1>
        density {
            density_file df3 df3FileName
            color_map {
            [0.00 rgb <0,0,0>]
            [1.00 OVERALL_DF3_INTENSITY*absorptionColour]
            }
        }
    }
#end

// Overload of Media macro for use with DF3 volumes: supports a minimum threshold before colour is added (i.e. non-uniform colour map gradient)
#macro MEDIA_3ARG(df3FileName, absorptionColour, minimumThreshold)
    media {
        emission <1,1,1>
        density {
            density_file df3 df3FileName
            color_map {
            [0.00 rgb <0,0,0>]
            [minimumThreshold rgb<0,0,0>]
            [1.00 OVERALL_DF3_INTENSITY*absorptionColour]
            }
        }
    }
#end


// Rootmap Volume macro for use with DF3 volumes
#macro RV(xx1,yy1,zz1,xx2,yy2,zz2, cyanDF3File, magentaDF3File, yellowDF3File)
    box     {
        <0,0,0>, <1,1,1>
        pigment { rgbt 1 }
        hollow
        interior {
            #if(CYAN_COLOUR_MAX>0)
                #if(CYAN_MINIMUM_DF3_INTENSITY_THRESHOLD>0)
                    MEDIA_3ARG(cyanDF3File, rgb <CYAN_COLOUR_MAX,0,0>, CYAN_MINIMUM_DF3_INTENSITY_THRESHOLD)
                #else
                    MEDIA_2ARG(cyanDF3File, rgb <CYAN_COLOUR_MAX,0,0>)
                #end
            #end
            #if(MAGENTA_COLOUR_MAX>0)
                #if(MAGENTA_MINIMUM_DF3_INTENSITY_THRESHOLD>0)
                    MEDIA_3ARG(magentaDF3File, rgb <0,MAGENTA_COLOUR_MAX,0>, MAGENTA_MINIMUM_DF3_INTENSITY_THRESHOLD)
                #else
                    MEDIA_2ARG(magentaDF3File, rgb <0,MAGENTA_COLOUR_MAX,0>)
                #end
            #end
            #if(YELLOW_COLOUR_MAX>0)
                #if(YELLOW_MINIMUM_DF3_INTENSITY_THRESHOLD>0)
                    MEDIA_3ARG(yellowDF3File, rgb <0,0,YELLOW_COLOUR_MAX>, YELLOW_MINIMUM_DF3_INTENSITY_THRESHOLD)
                #else
                    MEDIA_2ARG(yellowDF3File, rgb <0,0,YELLOW_COLOUR_MAX>)
                #end
            #end
            #if(CYAN_COLOUR_MIN>0)
                #debug "adding cyan background"
                media {
                    absorption <OVERALL_DF3_INTENSITY*CYAN_COLOUR_MIN,0,0>
                }
            #end
            #if(MAGENTA_COLOUR_MIN>0)
                #debug "adding magenta background"
                media {
                    absorption <0,OVERALL_DF3_INTENSITY*MAGENTA_COLOUR_MIN,0>
                }
            #end
            #if(YELLOW_COLOUR_MIN>0)
                #debug "adding yellow background"
                media {
                    absorption <0,0,OVERALL_DF3_INTENSITY*YELLOW_COLOUR_MIN>
                }
            #end
        }
        scale <(xx2-xx1),(yy2-yy1),(zz2-zz1)>
    }

    #if(DRAW_SCOREBOARD_BOUNDARIES)
        #local PtA = <xx1,yy1,zz1>;
        #local PtB = <xx2,yy2,zz2>;
        object
        {
            Wire_Box_Union(PtA, PtB, 0.05)
            pigment { rgb<0,0,1> }
        }
    #end
#end

// Nitrate Packet macro
#macro NP(xx1,yy1,zz1,s)
// Note: s is a packet size value. Not used at present.
    sphere	{
        <xx1,yy1,zz1>,
        NITRATE_RADIUS
        pigment { NITRATE_PACKET_COLOUR }
    }
#end

// Grid Line macro
#macro GL(xx1,yy1,zz1,xx2,yy2,zz2)
    cylinder { <xx1,yy1,zz1>, <xx2,yy2,zz2>, GRID_LINE_RADIUS pigment {GRID_COLOUR} }
#end

// Wetting Front macro
#macro WF(xx1,yy1,xx2,yy2,zz,r,g,b,a)
    polygon {
        4, <xx1,yy1>, <xx1,yy2>, <xx2,yy2>, <xx2,yy1>
        texture
        {
            #ifdef(WETTING_FRONT_COLOUR)
                pigment { WETTING_FRONT_COLOUR }
            #else
                pigment { colour rgbt<r,g,b,a> }
            #end
        }
        translate <0,0,zz>
    }
#end

// Rectangular prism macro
#macro RP(xx1,yy1,zz1,xx2,yy2,zz2)
    box { <xx1,yy1,zz1>, <xx2,yy2,zz2> pigment{RECTANGULAR_PRISM_COLOUR} }
#end

// Cylinder macro
#macro C(xx1,yy1,zz1,xx2,yy2,zz2,rr)
    cylinder { <xx1,yy1,zz1>, <xx2,yy2,zz2>, rr pigment {CYLINDER_COLOUR} }
#end


// ROOTMAP GEOMETRY IMPORT AND PLACEMENT    -------------------------------------------------------


union {
    #include concat(HOME_DIRECTORY,INCLUDE_FILENAME)
    translate -CENTREPOINT

    // Do initial horizontal rotation.
    rotate <0,0,INITIAL_HORIZONTAL_ROTATION>

    #if(DATA_FRAME_COUNT=1)
        // No root system animation possible; Dedicate all frames to rotational animation
        #declare HORIZONTAL_ROTATION_CLOCK = clock;
    #end

    #switch (HORIZONTAL_CAMERA_ROTATION)
    #case (WHILE_GROWING)
    #case (NO_GROWING)
    #case (AFTER_GROWING)
        #if(HORIZONTAL_ROTATION_CLOCK>0)
            rotate <0,0,HORIZONTAL_CAMERA_ROTATION_DEGREES*HORIZONTAL_ROTATION_CLOCK>
        #end
        #break
    #case (CUSTOM_PATH)
        // Insert user-defined rotation path here.
        #break
    #else
        // Default: No camera rotation.
    #end

    // Do initial vertical rotation.
    rotate <0,INITIAL_VERTICAL_ROTATION,0>

    #switch (VERTICAL_CAMERA_ROTATION)
    #case (WHILE_GROWING)
    #case (NO_GROWING)
    #case (AFTER_GROWING)
        #if(VERTICAL_ROTATION_CLOCK>0)
            rotate <0,VERTICAL_CAMERA_ROTATION_DEGREES*VERTICAL_ROTATION_CLOCK,0>
        #end
        #break
    #case (CUSTOM_PATH)
        // Insert user-defined rotation path here.
        #break
    #else
        // Default: No camera rotation.
    #end

    translate CENTREPOINT
}




// CAMERA AND LIGHT SOURCE SETTINGS    ------------------------------------------------------------

// Uncomment the line below to draw a vertical marker line at CENTREPOINT.
//cylinder { CENTREPOINT - <0,0,20>, CENTREPOINT + <0,0,20>, 0.1 }

// Camera (note that CENTREPOINT is defined in the first Rootmap_Data.inc file)
#declare SKY = <0,0,-1>;	// Z, inverted, is the vertical axis in ROOTMAP. Z=0 is the soil surface and Z<0 is the sky
#declare CAMERAVERTICALOFFSET = 6;
#declare CAMERAHORIZONTALOFFSET = 32;
#declare LOOKATVERTICALOFFSET = 12;
#declare CAMERAPOINT = CENTREPOINT + CAMERAHORIZONTALOFFSET*<1,0,0> - CAMERAVERTICALOFFSET*SKY;
camera {
    perspective
    location CAMERAPOINT
    // up y
    // right image_width*x/image_height
    // angle 60
    sky SKY
    look_at CENTREPOINT - LOOKATVERTICALOFFSET*SKY
}

// Light sources
light_source {
    CAMERAPOINT + 10*<0,0,1> // Behind the camera
    color rgb <1,1,1>
    media_interaction off
    media_attenuation off
}

// light_source {
//     //CAMERAPOINT	// At the camera
//     CENTREPOINT - CAMERAHORIZONTALOFFSET*<1,0,0> // Behind the root system
//     colour rgb <1,1,1>
//     media_interaction off
//     media_attenuation off
// }

// light_source {
//     <5,5,-5>
//     colour rgb <1,1,1>
//     media_interaction on
//     media_attenuation on
// }

// light_source {
//     <5,5,55>
//     colour rgb <1,1,1>
//     media_interaction on
//     media_attenuation on
// }
