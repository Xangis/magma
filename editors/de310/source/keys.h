// Values returned by getkey() - very system-specific

#ifndef _KEYS_H_

#ifndef __LINUX__
#define K_Null         0
#define K_Ctrl_A       1
#define K_Ctrl_B       2
#define K_Ctrl_C       3
#define K_Break        3
#define K_Ctrl_D       4
#define K_Ctrl_E       5
#define K_Ctrl_F       6
#define K_Ctrl_G       7
#define K_Bell         7
#define K_Ctrl_H       8
#define KEY_BACKSPACE  8
#define K_Backspace    KEY_BACKSPACE
#define K_Ctrl_I       9
#define K_Tab          9
#define K_Ctrl_J      10
#define K_Linefeed    10
#define K_Ctrl_K      11
#define K_Ctrl_L      12
#define K_Formfeed    12
#define K_Ctrl_M      13
#define K_Return      13
#define K_Enter       13
#define K_Ctrl_N      14
#define K_Ctrl_O      15
#define K_Ctrl_P      16
#define K_Ctrl_Q      17
#define K_Ctrl_R      18
#define K_Ctrl_S      19
#define K_Ctrl_T      20
#define K_Ctrl_U      21
#define K_Ctrl_V      22
#define K_Ctrl_W      23
#define K_Ctrl_X      24
#define K_Ctrl_Y      25
#define K_Ctrl_Z      26
#define K_Escape      27

#define K_FirstPrint  32  // first "printable" ascii key
#define K_LastPrint  126  // last "printable" ascii key

// all of the following are returned after a null character has been sent

#define K_Alt_BS     514  // Alt-Backspace
#define K_Alt_Q      516
#define K_Alt_W      517
#define K_Alt_E      518
#define K_Alt_R      519
#define K_Alt_T      520
#define K_Alt_Y      521
#define K_Alt_U      522
#define K_Alt_I      523
#define K_Alt_O      524
#define K_Alt_P      525
#define K_Alt_LBrack 526  // Alt-Left bracket  ([ or {)
#define K_Alt_RBrack 527  // Alt-Right bracket (] or })
#define K_Alt_A      530
#define K_Alt_S      531
#define K_Alt_D      532
#define K_Alt_F      533
#define K_Alt_G      534
#define K_Alt_H      535
#define K_Alt_J      536
#define K_Alt_K      537
#define K_Alt_L      538
#define K_Alt_SColon 539  // Alt-Semicolon
#define K_Alt_Colon  539  // Alt-Colon
#define K_Alt_Quote  540  // Alt-Quote (" or ')
#define K_Alt_BQuote 541  // Alt-"Backquote" (`)
#define K_Alt_Tilde  541  // Alt-Tilde (~)
#define K_Alt_BSlash 543  // Alt-Backslash
#define K_Alt_Pipe   543  // Alt-Pipe (|)
#define K_Alt_Z      544
#define K_Alt_X      545
#define K_Alt_C      546
#define K_Alt_V      547
#define K_Alt_B      548
#define K_Alt_N      549
#define K_Alt_M      550
#define K_Alt_Comma  551
#define K_Alt_Less   551  // Alt-Less than symbol (<)
#define K_Alt_Period 552
#define K_Alt_Great  552  // Alt-Greater than symbol (>)
#define K_F1         559
#define K_F2         560
#define K_F3         561
#define K_F4         562
#define K_F5         563
#define K_F6         564
#define K_F7         565
#define K_F8         566
#define K_F9         567
#define K_F10        568
#define K_Home       571
#define K_UpArrow    572
#define K_PageUp     573
#define K_Alt_KMinus 574  // Alt-Keypad minus
#define K_LeftArrow  575
#define K_RightArrow 577
#define K_Alt_KPlus  578  // Alt-Keypad plus
#define K_End        579
#define K_DownArrow  580
#define K_PageDown   581
#define K_Delete     583
#define K_Shift_F1   584
#define K_Shift_F2   585
#define K_Shift_F3   586
#define K_Shift_F4   587
#define K_Shift_F5   588
#define K_Shift_F6   589
#define K_Shift_F7   590
#define K_Shift_F8   591
#define K_Shift_F9   592
#define K_Shift_F10  593
#define K_Ctrl_End   617
#define K_Ctrl_Home  619
#define K_Alt_Minus  630
#define K_Alt_Plus   631
#define K_F11        633
#define K_F12        634
#define K_Shift_F11  635
#define K_Shift_F12  636
#define K_Ctrl_Tab   648
#define K_Alt_Home   651
#define K_Alt_End    659
#else                     // __LINUX__
#define K_Null         0
#define K_Ctrl_A       1
#define K_Ctrl_B       2
#define K_Ctrl_C       3
#define K_Break        3
#define K_Ctrl_D       4
#define K_Ctrl_E       5
#define K_Ctrl_F       6
#define K_Ctrl_G       7
#define K_Bell         7
#define K_Ctrl_H       8
#define K_Backspace    KEY_BACKSPACE
#define K_Ctrl_I       9
#define K_Tab          9
#define K_Ctrl_J      10
#define K_Linefeed    10
#define K_Ctrl_K      11
#define K_Ctrl_L      12
#define K_Formfeed    12
#define K_Ctrl_M      13
#define K_Return      13
#define K_Enter       13
#define K_Ctrl_N      14
#define K_Ctrl_O      15
#define K_Ctrl_P      16
#define K_Ctrl_Q      17
#define K_Ctrl_R      18
#define K_Ctrl_S      19
#define K_Ctrl_T      20
#define K_Ctrl_U      21
#define K_Ctrl_V      22
#define K_Ctrl_W      23
#define K_Ctrl_X      24
#define K_Ctrl_Y      25
#define K_Ctrl_Z      26
#define K_Escape      27

#define K_FirstPrint  32  // first "printable" ascii key
#define K_LastPrint  126  // last "printable" ascii key

// all of the following are returned after a null character has been sent

#define K_Alt_BS     514  // Alt-Backspace
#define K_Alt_Q      516
#define K_Alt_W      517
#define K_Alt_E      518
#define K_Alt_R      519
#define K_Alt_T      520
#define K_Alt_Y      521
#define K_Alt_U      522
#define K_Alt_I      523
#define K_Alt_O      524
#define K_Alt_P      525
#define K_Alt_LBrack 526  // Alt-Left bracket  ([ or {)
#define K_Alt_RBrack 527  // Alt-Right bracket (] or })
#define K_Alt_A      530
#define K_Alt_S      531
#define K_Alt_D      532
#define K_Alt_F      533
#define K_Alt_G      534
#define K_Alt_H      535
#define K_Alt_J      536
#define K_Alt_K      537
#define K_Alt_L      538
#define K_Alt_SColon 539  // Alt-Semicolon
#define K_Alt_Colon  539  // Alt-Colon
#define K_Alt_Quote  540  // Alt-Quote (" or ')
#define K_Alt_BQuote 541  // Alt-"Backquote" (`)
#define K_Alt_Tilde  541  // Alt-Tilde (~)
#define K_Alt_BSlash 543  // Alt-Backslash
#define K_Alt_Pipe   543  // Alt-Pipe (|)
#define K_Alt_Z      544
#define K_Alt_X      545
#define K_Alt_C      546
#define K_Alt_V      547
#define K_Alt_B      548
#define K_Alt_N      549
#define K_Alt_M      550
#define K_Alt_Comma  551
#define K_Alt_Less   551  // Alt-Less than symbol (<)
#define K_Alt_Period 552
#define K_Alt_Great  552  // Alt-Greater than symbol (>)
#define K_F1         559
#define K_F2         560
#define K_F3         561
#define K_F4         562
#define K_F5         563
#define K_F6         564
#define K_F7         565
#define K_F8         566
#define K_F9         567
#define K_F10        568
#define K_Home       KEY_HOME
#define K_UpArrow    KEY_UP
#define K_PageUp     KEY_PPAGE
#define K_Alt_KMinus 574  // Alt-Keypad minus
#define K_LeftArrow  KEY_LEFT
#define K_RightArrow KEY_RIGHT
#define K_Alt_KPlus  578  // Alt-Keypad plus
#define K_End        KEY_END
#define K_DownArrow  KEY_DOWN
#define K_PageDown   KEY_NPAGE
#define K_Delete     583
#define K_Ctrl_End   617
#define K_Ctrl_Home  619
#define K_Alt_Minus  630
#define K_Alt_Plus   631
#define K_F11        633
#define K_F12        634
#define K_Ctrl_Tab   648
#define K_Alt_Home   651
#define K_Alt_End    659
#endif

#define _KEYS_H_
#endif
