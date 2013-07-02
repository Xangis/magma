#ifndef _ARMOR_H_

// armor types (val0)

#define ARMOR_TYPE_NONE             0
#define ARMOR_TYPE_LOWEST           1
#define ARMOR_TYPE_LEATHER          1
#define ARMOR_TYPE_STUDDED_LEATHER  2
#define ARMOR_TYPE_PADDED_LEATHER   3
#define ARMOR_TYPE_RING             4
#define ARMOR_TYPE_HIDE             5
#define ARMOR_TYPE_SCALE            6
#define ARMOR_TYPE_CHAIN            7
#define ARMOR_TYPE_SPLINT           8
#define ARMOR_TYPE_BANDED           9
#define ARMOR_TYPE_PLATE           10  // semi-bendable joints
#define ARMOR_TYPE_FIELD_PLATE     11  // not so bendable
#define ARMOR_TYPE_FULL_PLATE      12  // neigh near unbendable
#define ARMOR_TYPE_HIGHEST         12

// armor thickness (val3)

// these values aren't really cut-and-dry..  apply as you wish

#define ARMOR_THICKNESS_LOWEST           1
#define ARMOR_THICKNESS_VERY_THIN        1
#define ARMOR_THICKNESS_THIN             2
#define ARMOR_THICKNESS_AVERAGE          3
#define ARMOR_THICKNESS_THICK            4
#define ARMOR_THICKNESS_VERY_THICK       5
#define ARMOR_THICKNESS_HIGHEST          5

// primary armor locations when worn (val1)

#define ARMOR_WEAR_LOWEST       1
#define ARMOR_WEAR_FINGER       1
#define ARMOR_WEAR_NECK         2
#define ARMOR_WEAR_BODY         3
#define ARMOR_WEAR_HEAD         4
#define ARMOR_WEAR_LEGS         5
#define ARMOR_WEAR_FEET         6
#define ARMOR_WEAR_HANDS        7
#define ARMOR_WEAR_ARMS         8
#define ARMOR_WEAR_ABOUT        9
#define ARMOR_WEAR_WAIST       10
#define ARMOR_WEAR_WRIST       11
#define ARMOR_WEAR_EYES        12
#define ARMOR_WEAR_FACE        13
#define ARMOR_WEAR_EAR         14
#define ARMOR_WEAR_QUIVER      15
#define ARMOR_WEAR_BADGE       16
#define ARMOR_WEAR_BACK        17
#define ARMOR_WEAR_ATTACHBELT  18
#define ARMOR_WEAR_HORSEBODY   19
#define ARMOR_WEAR_TAIL        20
#define ARMOR_WEAR_NOSE        21
#define ARMOR_WEAR_HORNS       22
#define ARMOR_WEAR_HIGHEST     22

// bitvectors for where armor covers

// break up body, legs, and arms a fair amount since they're rather large
// areas compared to everything else - joints are covered separately ..
// will probably make armor that covers a joint hamper dex/agi/move a bit
// automatically based on material type and maybe weight (and of course
// which joint it is..)

#define NUMB_ARMOR_BODY_FLAGS    8
#define ARMOR_BODY_ALL           BIT_1
#define ARMOR_BODY_FRONT_UPPER   BIT_2
#define ARMOR_BODY_FRONT_LOWER   BIT_3
#define ARMOR_BODY_BACK_UPPER    BIT_4
#define ARMOR_BODY_BACK_LOWER    BIT_5
#define ARMOR_BODY_RIBS_UPPER    BIT_6
#define ARMOR_BODY_RIBS_LOWER    BIT_7
#define ARMOR_BODY_SHOULDERS     BIT_8

#define NUMB_ARMOR_LEGS_FLAGS    7
#define ARMOR_LEGS_ALL           BIT_1
#define ARMOR_LEGS_FRONT_UPPER   BIT_2  // above knees
#define ARMOR_LEGS_FRONT_KNEES   BIT_3
#define ARMOR_LEGS_FRONT_LOWER   BIT_4  // below knees
#define ARMOR_LEGS_BACK_UPPER    BIT_5
#define ARMOR_LEGS_BACK_KNEES    BIT_6
#define ARMOR_LEGS_BACK_LOWER    BIT_7

#define NUMB_ARMOR_ARMS_FLAGS    7
#define ARMOR_ARMS_ALL           BIT_1
#define ARMOR_ARMS_FRONT_UPPER   BIT_2  // above the elbow
#define ARMOR_ARMS_FRONT_ELBOW   BIT_3
#define ARMOR_ARMS_FRONT_LOWER   BIT_4  // below the elbow
#define ARMOR_ARMS_BACK_UPPER    BIT_5
#define ARMOR_ARMS_BACK_ELBOW    BIT_6
#define ARMOR_ARMS_BACK_LOWER    BIT_7

// horsebody stuff

#define NUMB_ARMOR_HORSE_FLAGS   8
#define ARMOR_HORSE_ALL          BIT_1
#define ARMOR_HORSE_FRONT_TOP    BIT_2  // from middle to head - let's say
                                        // saddles go here
#define ARMOR_HORSE_FRONT_SIDES  BIT_3
#define ARMOR_HORSE_FRONT_BOTTOM BIT_4
#define ARMOR_HORSE_BACK_TOP     BIT_5  // from middle to ass
#define ARMOR_HORSE_BACK_SIDES   BIT_6
#define ARMOR_HORSE_BACK_BOTTOM  BIT_7
#define ARMOR_HORSE_ASS_END      BIT_8  // the ass end of the horse, baby

// these are broken up quite a bit because what happens to you when hit
// on the head is heavily dependent on where exactly you are hit

#define NUMB_ARMOR_HEAD_FLAGS    9
#define ARMOR_HEAD_ALL           BIT_1
#define ARMOR_HEAD_FRONT_UPPER   BIT_2  // face area above bottom of nose
#define ARMOR_HEAD_SIDES_UPPER   BIT_3  // ears
#define ARMOR_HEAD_BACK_UPPER    BIT_4  // back of head, skull itself
#define ARMOR_HEAD_CROWN         BIT_5  // top of head - lights out, mommy
#define ARMOR_HEAD_FRONT_LOWER   BIT_6  // face area below nose
#define ARMOR_HEAD_SIDES_LOWER   BIT_7  // side of jaw
#define ARMOR_HEAD_BACK_LOWER    BIT_8  // back of head, where skull stops
                                        // and flesh of neck begins
#define ARMOR_HEAD_CHIN          BIT_9  // a dagger through the bottom of
                                        // the chin.  shrug.

// no need to really break these up, you get hit in the front of the neck and
// you're generally fucked - back of neck, maybe not as bad, unless it's a
// hard crushing blow or a REALLY hard slashing blow.

#define NUMB_ARMOR_NECK_FLAGS    3
#define ARMOR_NECK_ALL           BIT_1
#define ARMOR_NECK_FRONT         BIT_2
#define ARMOR_NECK_BACK          BIT_3

// doubt many people are gonna be taking foot shots, but ..

#define NUMB_ARMOR_FEET_FLAGS    5
#define ARMOR_FEET_ALL           BIT_1
#define ARMOR_FEET_TOP           BIT_2
#define ARMOR_FEET_BOTTOM        BIT_3
#define ARMOR_FEET_TOES          BIT_4  // this little piggy got cut off
#define ARMOR_FEET_BACK          BIT_5  // feel the pain of everyone..

// hands are pretty important but there's not much there, so not many
// 'slots'

#define NUMB_ARMOR_HANDS_FLAGS   4
#define ARMOR_HANDS_ALL          BIT_1
#define ARMOR_HANDS_FINGERS      BIT_2  // not all gloves have fingers
#define ARMOR_HANDS_HAND         BIT_3  // the 'main' area of the hand itself
#define ARMOR_HANDS_WRIST        BIT_4

// about-body stuff is generally assumed to cover the back, but not be
// another breastplate or heavy armor..  hmm.  this one is a bit ambiguous,
// so we'll see what all needs to be changed, if anything

// cloaks could technically also cover the arms, but ..  this is good
// enough

#define NUMB_ARMOR_ABOUT_FLAGS   6
#define ARMOR_ABOUT_ALL          BIT_1
#define ARMOR_ABOUT_BACK_UPPER   BIT_2
#define ARMOR_ABOUT_BACK_LOWER   BIT_3
#define ARMOR_ABOUT_RIBS_UPPER   BIT_4
#define ARMOR_ABOUT_RIBS_LOWER   BIT_5
#define ARMOR_ABOUT_SHOULDERS    BIT_6

// heh, the wonderful waist slot.  not much here to cover.  hell, I
// could probably assume all waist slot items are belts, but..

// really, waist items aren't gonna be worth jack shit..  I guess we
// could check if you hit the guy in the waist everytime he gets hit
// in the lower torso - but I'm not real sure what the 'waist' area is
// exactly - what, about 2"?  heh

// this is another candidate for items that don't protect but just imbue
// magical crap onto you

/*
#define NUMB_ARMOR_WAIST_FLAGS   3
#define ARMOR_WAIST_ALL          BIT_1
#define ARMOR_WAIST_FRONT        BIT_2
#define ARMOR_WAIST_BACK         BIT_3
*/

// wristwear includes bracers, so let's allow it to give some protection
// to the lower arms if they're not wimpy bracelets

#define NUMB_ARMOR_WRIST_FLAGS       5
#define ARMOR_WRIST_ALL              BIT_1
#define ARMOR_WRIST_LOWER_ARMS_FRONT BIT_2
#define ARMOR_WRIST_LOWER_ARMS_BACK  BIT_3
#define ARMOR_WRIST_FRONT            BIT_4
#define ARMOR_WRIST_BACK             BIT_5

// eyewear - yes, that's right, eyepatches that cover up yer damn eye are
// gonna reduce dexterity and agility and whatever else I can come up with..
// so we have crap for specific eyes and for transparent eyewear (glasses,
// magical patches that you can see through one way but not the other,
// whatever).  note that there is no 'ALL' flag for this since that wouldn't
// make any sense

#define NUMB_ARMOR_EYES_FLAGS         4
#define ARMOR_EYE_LEFT               BIT_1
#define ARMOR_EYE_RIGHT              BIT_2
#define ARMOR_EYE_LEFT_TRANSPARENT   BIT_3
#define ARMOR_EYE_RIGHT_TRANSPARENT  BIT_4

// facewear.. this is interesting, it's a bit redundant with headwear, but..
// let's just assume that facewear never guards the sides of the head or the
// eyes..  that works nicely

#define NUMB_ARMOR_FACE_FLAGS        3
#define ARMOR_FACE_ALL               BIT_1
#define ARMOR_FACE_UPPER             BIT_2  // from bottom of nose up
#define ARMOR_FACE_LOWER             BIT_3  // below nose

// earrings.  now this is really interesting.  I guess they could guard
// the side of the face and of course your ears.  but they're generally
// tiny..  why the hell are men running around with earrings in their
// ears, anyway?  whoever thought up these slots is batty

// how many earrings cover up your whole ear?  about none?  bleh..  this
// slot is hard to make fit in with the new combat system

// might just make earrings useless for protection and thusly only good
// for their magical properties.  and of course, to look sexy for all your
// other manly buddies

/*
#define NUMB_ARMOR_EARS_FLAGS       3
#define ARMOR_EARS_ALL              BIT_1
#define ARMOR_EARS_UPPER_FACE_SIDE  BIT_2
#define ARMOR_EARS_EARS             BIT_3
*/

// quivers don't provide any reasonable protection, don't bother counting them

// badges..  kinda like earrings.  I'd say the same thing..  they can imbue
// magical properties (+dex, perm haste, whatever) but are useless for
// protection

#define _ARMOR_H_
#endif
