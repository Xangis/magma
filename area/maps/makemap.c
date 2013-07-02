#include <stdio.h>

int main( int argv, char *argc )
{
	FILE *fp;
	FILE *fp2;
        char infile[256];
        char outfile[256];
	int row;
	int column;
	int vnum;
	char ch;
        int terrain;
        int ext_terrain;
        int width;
        int height;
        int flag;
        int key;
        int passable = 1;
        char instr[256];
        int TRUE = 1;
        int FALSE = 0;

        puts( "What is the name of the map input file?" );
        gets( infile );
        puts( "What is the name of the map area output file?" );
        gets( outfile );
        puts( "What is the width of the map?" );
        gets( instr );
        width = atoi( instr );
        puts( "What is the height of the map?" );
        gets( instr );
        height = atoi( instr );
        puts( "What is the starting vnum of the area?" );
        gets( instr );
        vnum = atoi( instr );
        printf( "Which key scheme should be used?\n\r"
                "1) Surface map mountain continent\n\r"
                "2) Surface map one\n\r"
                "3) Underdark one\n\r"
                "4) Underdark chasms\n\r"
                "5) Lower underdark\n\r:" );
        gets( instr );
        key = atoi( instr );

        if( key < 1 || key > 5 || !height || !width || !vnum  )
        {
          printf( "Invalid value(s) for height, width, vnum, or key!\n\r" );
          return 10;
        }

	column = 0;
	row = 0;

	if( !(fp = fopen( infile, "r" )))
        {
          printf( "Cannot open input file.\n\r" );
          return 1;
        }

        if( !(fp2 = fopen( outfile, "w" )))
        {
          printf( "Cannot open output file.\n\r" );
          return 2;
        }

        // Base the area header upon they key of the map
        switch( key )
        {
           case 1:
              fprintf( fp2, "#AREADATA\nName        The Mountainous Surface Map~\nAuthor      Pesto/Thendar~\nLevels      1 50\nSecurity    1\nVNUMs       200000 260000\n"
                      "Builders    Pesto/Thendar~\nRecall      3001\nReset       You hear some strange noises.~\nEnd\n" );
              break;
           case 2:
              fprintf( fp2, "#AREADATA\nName        The Overland Surface Map~\nAuthor      Thendar~\nLevels      1 50\nSecurity    1\nVNUMs       260000 272000\n"
                      "Builders    Thendar~\nRecall      3001\nReset       You hear some strange noises.~\nEnd\n" );
              break;
           case 3:
              fprintf( fp2, "#AREADATA\nName        The Volcanic Tunnels of the Underdark~\nAuthor      Thendar~\nLevels      1 50\nSecurity    1\nVNUMs       280000 290000\n"
                      "Builders    Thendar~\nRecall      3001\nReset       You hear some strange noises.~\nEnd\n" );
              break;
           case 4:
              fprintf( fp2, "#AREADATA\nName        The Chasms of the Underdark~\nAuthor      Thendar~\nLevels      1 50\nSecurity    1\nVNUMs       300000 310000\n"
                      "Builders    Thendar~\nRecall      3001\nReset       You hear some strange noises.~\nEnd\n" );
              break;
           case 5:
              fprintf( fp2, "#AREADATA\nName        The Deep Underdark~\nAuthor      Thendar~\nLevels      1 50\nSecurity    1\nVNUMs       300000 310000\n"
                      "Builders    Thendar~\nRecall      3001\nReset       You hear some strange noises.~\nEnd\n" );
              break;
        }

        fprintf( fp2, "#BASTROOMS\n" );

	while( 1 )
	{
		ch = fgetc( fp );
		if( ch == 0 )
		   break;
                // Need one of these for *each* key type.
                if( key == 1 )
                {
		 switch( ch )
		 {
		  default:
		    printf( "Unrecognized terrain type '%c' found at row %d column %d (vnum %d) - Aborting!\n\r",
                            ch, row, column, vnum );
		    fclose( fp );
		    fprintf( fp2, "#0\n#$\n" );
		    fclose( fp2 );
                    return 3;
                    break;
                  case '\n':
                    row++;
                    column = 0;
                    ext_terrain = 0;
                    terrain = 0;
                    flag = 0;
                    break;
		  case 'W':
		    ext_terrain = 1;
		    terrain = 12;
                    flag = 0;
                    break;
		  case 'k':
		    ext_terrain = 2;
		    terrain = 5;
                    flag = 0;
                    break;
		  case 'i':
		    ext_terrain = 3;
		    terrain = 25;
                    flag = 0;
                    break;
		  case 'L':
		    ext_terrain = 4;
		    terrain = 25;
                    flag = 0;
                    break;
		  case 'S':
		    ext_terrain = 5;
		    terrain = 26;
                    flag = 0;
                    break;
		  case 'B':
		    ext_terrain = 6;
		    terrain = 2;
                    flag = 1048576;
                    break;
		  case 'M':
		    ext_terrain = 7;
		    terrain = 5;
                    flag = 0;
                    break;
		  case 'n':
		    ext_terrain = 8;
		    terrain = 4;
                    flag = 0;
                    break;
		  case 'm':
		    ext_terrain = 9;
		    terrain = 2;
                    flag = 0;
                    break;
		  case 'C':
		    ext_terrain = 10;
		    terrain = 5;
                    flag = 0;
                    break;
		  case 'c':
		    ext_terrain = 11;
		    terrain = 5;
                    flag = 0;
                    break;
		  case 'x':
		    ext_terrain = 12;
		    terrain = 2;
                    flag = 0;
                    break;
		  case 'D':
		    ext_terrain = 13;
		    terrain = 24;
                    flag = 0;
                    break;
		  case 'd':
		    ext_terrain = 14;
		    terrain = 2;
                    flag = 0;
                    break;
		  case 'h':
		    ext_terrain = 15;
		    terrain = 2;
                    flag = 0;
                    break;
		  case 'r':
		    ext_terrain = 16;
		    terrain = 6;
                    flag = 0;
                    break;
		  case 'R':
		    ext_terrain = 17;
		    terrain = 7;
                    flag = 0;
                    break;
		  case 'a':
		    ext_terrain = 18;
		    terrain = 3;
                    flag = 0;
                    break;
		  case 'A':
		    ext_terrain = 19;
		    terrain = 2;
                    flag = 0;
                    break;
		  case '7':
		    ext_terrain = 20;
		    terrain = 27;
                    flag = 0;
                    break;
		  case 'v':
		    ext_terrain = 21;
		    terrain = 6;
                    flag = 0;
                    break;
		  case 'w':
		    ext_terrain = 22;
		    terrain = 6;
                    flag = 0;
                    break;
		  case 'I':
		    ext_terrain = 23;
		    terrain = 2;
                    flag = 0;
                    break;
		  case '3':
		    ext_terrain = 24;
		    terrain = 27;
                    flag = 0;
                    break;
		  case 'g':
		    ext_terrain = 25;
		    terrain = 6;
                    flag = 1048576;
                    break;
		  case 'F':
		    ext_terrain = 26;
		    terrain = 3;
                    flag = 0;
                    break;
		  case 'p':
		    ext_terrain = 27;
		    terrain = 25;
                    flag = 0;
                    break;
		  case 'P':
		    ext_terrain = 28;
		    terrain = 25;
                    flag = 0;
                    break;
		  case '1':
		    ext_terrain = 29;
		    terrain = 5;
                    flag = 0;
                    break;
		  case 'e':
		    ext_terrain = 30;
		    terrain = 2;
                    flag = 0;
                    break;
		  case 'E':
		    ext_terrain = 31;
		    terrain = 3;
                    flag = 0;
                    break;
		  case '5':
		    ext_terrain = 32;
		    terrain = 25;
                    flag = 0;
                    break;
		  case 'b':
		    ext_terrain = 33;
		    terrain = 3;
                    flag = 0;
                    break;
		  case 'Z':
		    ext_terrain = 34;
		    terrain = 2;
                    flag = 0;
                    break;
		  case '9':
		    ext_terrain = 35;
		    terrain = 5;
                    flag = 0;
                    break;
		  case 'z':
		    ext_terrain = 36;
		    terrain = 5;
                    flag = 0;
                    break;
		  case '&':
		    ext_terrain = 37;
		    terrain = 2;
                    flag = 1048576;
                    break;
		  case '^':
		    ext_terrain = 38;
		    terrain = 24;
                    flag = 1048576;
                    break;
		  case '%':
		    ext_terrain = 39;
		    terrain = 2;
                    flag = 1048576;
                    break;
		  case '@':
		    ext_terrain = 40;
		    terrain = 2;
                    flag = 1048576;
                    break;
		  case '#':
		    ext_terrain = 41;
		    terrain = 2;
                    flag = 1048576;
                    break;
		  case ']':
		    ext_terrain = 42;
		    terrain = 25;
                    flag = 1048576;
                    break;
		  case '6':
		    ext_terrain = 43;
		    terrain = 2;
                    flag = 0;
                    break;
		  case '8':
		    ext_terrain = 44;
		    terrain = 4;
                    flag = 0;
                    break;
		  case '!':
		    ext_terrain = 45;
		    terrain = 2;
                    flag = 0;
                    break;
		  case '(':
		    ext_terrain = 46;
		    terrain = 5;
                    flag = 0;
                    break;
		  case '2':
		    ext_terrain = 47;
		    terrain = 27;
                    flag = 0;
                    break;
		  case 'Y':
		    ext_terrain = 48;
		    terrain = 2;
                    flag = 0;
                    break;
		 }
                }
                else if( key == 2 )
                {
                 switch( ch )
                 {
                  default:
                    printf( "Unrecognized terrain type '%c' found at row %d column %d (vnum %d) - Aborting!\n\r",
                            ch, row, column, vnum );
                    fclose( fp );
                    fprintf( fp2, "#0\n#$\n" );
                    fclose( fp2 );
                    return 3;
                    break;
                  case '\n':
                    row++;
                    column = 0;
                    ext_terrain = 0;
                    terrain = 0;
                    flag = 0;
                    break;
                  case 'W':
                    ext_terrain = 49;
                    terrain = 12;
                    flag = 0;
                    break;
                  case 'p':
                    ext_terrain = 50;
                    terrain = 2;
                    flag = 50;
                    break;
                  case 'f':
                    ext_terrain = 51;
                    terrain = 3;
                    flag = 0;
                    break;
                  case 'm':
                    ext_terrain = 52;
                    terrain = 5;
                    flag = 0;
                    break;
                  case 's':
                    ext_terrain = 53;
                    terrain = 26;
                    flag = 0;
                    break;
                  case 't':
                    ext_terrain = 54;
                    terrain = 3;
                    flag = 0;
                    break;
                  case 'c':
                    ext_terrain = 55;
                    terrain = 7;
                    flag = 1048576;
                    break;
                  case 'D':
                    ext_terrain = 56;
                    terrain = 7;
                    flag = 1048576;
                    break;
                  case 'F':
                    ext_terrain = 57;
                    terrain = 3;
                    flag = 0;
                    break;
                  case 'g':
                    ext_terrain = 58;
                    terrain = 3;
                    flag = 1048576;
                    break;
                  case 'r':
                    ext_terrain = 59;
                    terrain = 3;
                    flag = 0;
                    break;
                  case 'R':
                    ext_terrain = 60;
                    terrain = 7;
                    flag = 1048576;
                    break;
                  case 'e':
                    ext_terrain = 61;
                    terrain = 3;
                    flag = 0;
                    break;
                  case 'q':
                    ext_terrain = 62;
                    terrain = 7;
                    flag = 1048576;
                    break;
                  case 'z':
                    ext_terrain = 63;
                    terrain = 3;
                    flag = 0;
                    break;
                  case 'a':
                    ext_terrain = 64;
                    terrain = 7;
                    flag = 1048576;
                    break;
                  case 'x':
                    ext_terrain = 65;
                    terrain = 2;
                    flag = 0;
                    break;
                  case 'Z':
                    ext_terrain = 66;
                    terrain = 3;
                    flag = 0;
                    break;
                  case '1':
                    ext_terrain = 67;
                    terrain = 2;
                    flag = 0;
                    break;
                  case '2':
                    ext_terrain = 68;
                    terrain = 3;
                    flag = 0;
                    break;
                  case '3':
                    ext_terrain = 69;
                    terrain = 6;
                    flag = 0;
                    break;
                  case '4':
                    ext_terrain = 70;
                    terrain = 3;
                    flag = 0;
                    break;
                  case '5':
                    ext_terrain = 71;
                    terrain = 2;
                    flag = 1048576;
                    break;
                  case 'k':
                    ext_terrain = 72;
                    terrain = 2;
                    flag = 0;
                    break;
                  case 'i':
                    ext_terrain = 73;
                    terrain = 6;
                    flag = 1048576;
                    break;
                  case 'o':
                    ext_terrain = 74;
                    terrain = 2;
                    flag = 0;
                    break;
                  case 'b':
                    ext_terrain = 75;
                    terrain = 6;
                    flag = 0;
                    break;
                  case 'u':
                    ext_terrain = 76;
                    terrain = 6;
                    flag = 0;
                    break;
                  case 'y':
                    ext_terrain = 77;
                    terrain = 6;
                    flag = 0;
                    break;
                  case 'h':
                    ext_terrain = 78;
                    terrain = 3;
                    flag = 0;
                    break;
                  case 'H':
                    ext_terrain = 79;
                    terrain = 26;
                    flag = 1048576;
                    break;
                  case 'L':
                    ext_terrain = 80;
                    terrain = 5;
                    flag = 0;
                    break;
                  case 'K':
                    ext_terrain = 81;
                    terrain = 2;
                    flag = 0;
                    break;
                  case 'J':
                    ext_terrain = 82;
                    terrain = 3;
                    flag = 0;
                    break;
                  case 'X':
                    ext_terrain = 83;
                    terrain = 2;
                    flag = 0;
                    break;
                  case 'B':
                    ext_terrain = 84;
                    terrain = 2;
                    flag = 0;
                    break;
                  case 'C':
                    ext_terrain = 85;
                    terrain = 3;
                    flag = 0;
                    break;
                  case 'V':
                    ext_terrain = 86;
                    terrain = 3;
                    flag = 0;
                    break;
                  case '&':
                    ext_terrain = 87;
                    terrain = 3;
                    flag = 0;
                    break;
                  case '!':
                    ext_terrain = 45;
                    terrain = 27;
                    flag = 0;
                    break;
                  case '@':
                    ext_terrain = 88;
                    terrain = 6;
                    flag = 0;
                    break;
                  case 'v':
                    ext_terrain = 89;
                    terrain = 3;
                    flag = 0;
                    break;
                  case '#':
                    ext_terrain = 90;
                    terrain = 2;
                    flag = 1048576;
                    break;
                  case '(':
                    ext_terrain = 91;
                    terrain = 3;
                    flag = 0;
                    break;
                 }
                }
                else if( key == 3)
                {
                 switch( ch )
                 {
                  default:
                    printf( "Unrecognized terrain type '%c' found at row %d column %d (vnum %d) - Aborting!\n\r",
                            ch, row, column, vnum );
                    fclose( fp );
                    fprintf( fp2, "#0\n#$\n" );
                    fclose( fp2 );
                    return 3;
                    break;
                  case '\n':
                    row++;
                    column = 0;
                    ext_terrain = 0;
                    terrain = 0;
                    flag = 0;
                    break;
                  case '.':
                    ext_terrain = 92;
                    terrain = 28;
                    flag = 134250496;
                    passable = FALSE;
                    break;
                  case 'a':
                    ext_terrain = 93;
                    terrain = 14;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'A':
                    ext_terrain = 94;
                    terrain = 15;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'b':
                    ext_terrain = 95;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'B':
                    ext_terrain = 96;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'c':
                    ext_terrain = 97;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'C':
                    ext_terrain = 98;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'd':
                    ext_terrain = 99;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'e':
                    ext_terrain = 100;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case '(':
                    ext_terrain = 101;
                    terrain = 28;
                    flag = 134250496;
                    passable = FALSE;
                    break;
                  case ')':
                    ext_terrain = 102;
                    terrain = 28;
                    flag = 134250496;
                    passable = FALSE;
                    break;
                  case 'f':
                    ext_terrain = 103;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'F':
                    ext_terrain = 104;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'g':
                    ext_terrain = 105;
                    terrain = 17;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'G':
                    ext_terrain = 106;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'h':
                    ext_terrain = 107;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'i':
                    ext_terrain = 108;
                    terrain = 12;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'j':
                    ext_terrain = 109;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'J':
                    ext_terrain = 110;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'k':
                    ext_terrain = 111;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'm':
                    ext_terrain = 112;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'n':
                    ext_terrain = 113;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'o':
                    ext_terrain = 114;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'p':
                    ext_terrain = 115;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case '!':
                    ext_terrain = 45;
                    terrain = 12;
                    flag = 0;
                    passable = TRUE;
                    break;
                 }
                }
                else if( key == 4 )
                {
                 switch( ch )
                 {
                  default:
                    printf( "Unrecognized terrain type '%c' found at row %d column %d (vnum %d) - Aborting!\n\r",
                            ch, row, column, vnum );
                    fclose( fp );
                    fprintf( fp2, "#0\n#$\n" );
                    fclose( fp2 );
                    return 3;
                    break;
                  case '\n':
                    row++;
                    column = 0;
                    ext_terrain = 0;
                    terrain = 0;
                    flag = 0;
                    break;
                  case '.':
                    ext_terrain = 116;
                    terrain = 28;
                    flag = 134250496;
                    passable = FALSE;
                    break;
                  case 'k':
                    ext_terrain = 117;
                    terrain = 13;
                    flag = 1;
                    passable = TRUE;
                    break;
                  case '!':
                    ext_terrain = 45;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'b':
                    ext_terrain = 118;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'c':
                    ext_terrain = 119;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'd':
                    ext_terrain = 120;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'e':
                    ext_terrain = 121;
                    terrain = 13;
                    flag = 128;
                    passable = TRUE;
                    break;
                  case 'f':
                    ext_terrain = 122;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'g':
                    ext_terrain = 123;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'h':
                    ext_terrain = 124;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'i':
                    ext_terrain = 125;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'j':
                    ext_terrain = 126;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'm':
                    ext_terrain = 127;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'n':
                    ext_terrain = 128;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'o':
                    ext_terrain = 129;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case '(':
                    ext_terrain = 130;
                    terrain = 28;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case ')':
                    ext_terrain = 131;
                    terrain = 28;
                    flag = 0;
                    passable = TRUE;
                    break;
                  } 
                }
                else
                {
                 switch( ch )
                 {
                  default:
                    printf( "Unrecognized terrain type '%c' found at row %d column %d (vnum %d) - Aborting!\n\r",
                            ch, row, column, vnum );
                    fclose( fp );
                    fprintf( fp2, "#0\n#$\n" );
                    fclose( fp2 );
                    return 3;
                    break;
                  case '\n':
                    row++;
                    column = 0;
                    ext_terrain = 0;
                    terrain = 0;
                    flag = 0;
                    break;
                  case '.':
                    ext_terrain = 132;
                    terrain = 28;
                    flag = 134250496;
                    passable = FALSE;
                    break;                 
                  case '!':
                    ext_terrain = 45;
                    terrain = 2;
                    flag = 0;
                    break;
                  case 'a':
                    ext_terrain = 133;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'A':
                    ext_terrain = 134;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'b':
                    ext_terrain = 135;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case ')':
                    ext_terrain = 136;
                    terrain = 28;
                    flag = 134250496;
                    passable = FALSE;
                    break;
                  case '(':
                    ext_terrain = 137;
                    terrain = 28;
                    flag = 134250496;
                    passable = FALSE;
                    break;
                  case 'c':
                    ext_terrain = 138;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'd':
                    ext_terrain = 139;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'e':
                    ext_terrain = 140;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'f':
                    ext_terrain = 141;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'g':
                    ext_terrain = 142;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'h':
                    ext_terrain = 143;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'i':
                    ext_terrain = 144;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'j':
                    ext_terrain = 145;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'k':
                    ext_terrain = 146;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'l':
                    ext_terrain = 147;
                    terrain = 18;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'm':
                    ext_terrain = 148;
                    terrain = 13;
                    flag = 384;
                    passable = TRUE;
                    break;
                  case 'o':
                    ext_terrain = 149;
                    terrain = 13;
                    flag = 128;
                    passable = TRUE;
                    break;
                  case 'p':
                    ext_terrain = 150;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'w':
                    ext_terrain = 151;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'u':
                    ext_terrain = 152;
                    terrain = 16;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'v':
                    ext_terrain = 153;
                    terrain = 10;
                    flag = 0;
                    passable = TRUE;
                    break;
                  case 'y':
                    ext_terrain = 154;
                    terrain = 13;
                    flag = 0;
                    passable = TRUE;
                    break;
                 }
                }
                // Handle newlines
                if( ext_terrain == 0 )
                  continue;

	        fprintf( fp2, "#%d\n~\n~\n%d %d %d\n", vnum, ext_terrain, flag, terrain );

                if( passable )
                {
                  if( row > 0 )
                    fprintf( fp2, "D0\n~\n~\n0 0 %d\n", (vnum - width) );
                  if( row < (height - 1) )
                    fprintf( fp2, "D2\n~\n~\n0 0 %d\n", (vnum + width) );
                  if( column > 0 )
                    fprintf( fp2, "D3\n~\n~\n0 0 %d\n", (vnum - 1) );
                  if( column < (width - 1) )
                    fprintf( fp2, "D1\n~\n~\n0 0 %d\n", (vnum + 1) );
                }

	        fprintf( fp2, "S\n" );

	        vnum += 1;
        	column += 1;
	}

        fprintf( fp2, "#0\n#$\n" );

	fclose( fp );
	fclose( fp2 );
	return 0;
}

