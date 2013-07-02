#include <stdio.h>

int main( int argv, char *argc )
{
	FILE *fp;
	FILE *fp2;
	int row;
	int column;
	int vnum;
	char ch;
        int terrain;
        int ext_terrain;
        int width;
        int height;
        int flag;

        width = 330;
        height = 158;
	vnum = 200000;
	column = 0;
	row = 0;

	if( !(fp = fopen( "mount_cont.txt", "r" )))
          return 1;

        if( !(fp2 = fopen( "surfacemap.are", "w" )))
          return 2;

        fprintf( fp2, "#AREADATA\nName        The Mountainous Surface Map~\nAuthor      Pesto~\nLevels      1 50\nSecurity    1\nVNUMs       200000 260000\n"
                      "Builders    Pesto/Thendar~\nRecall      3001\nReset       You hear some strange noises.~\nEnd\n" );

        fprintf( fp2, "#BASTROOMS\n" );

	while( 1 )
	{
		ch = fgetc( fp );
		if( ch == 0 )
		   break;
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
		    terrain = 7;
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
                // Handle newlines
                if( ext_terrain == 0 )
                  continue;

	        fprintf( fp2, "#%d\n~\n~\n%d %d %d\n", vnum, ext_terrain, flag, terrain );

                if( row > 0 )
                  fprintf( fp2, "D0\n~\n~\n0 0 %d\n", (vnum - width) );
                if( row < (height - 1) )
                  fprintf( fp2, "D2\n~\n~\n0 0 %d\n", (vnum + width) );
                if( column > 0 )
                  fprintf( fp2, "D3\n~\n~\n0 0 %d\n", (vnum - 1) );
                if( column < (width - 1) )
                  fprintf( fp2, "D1\n~\n~\n0 0 %d\n", (vnum + 1) );

	        fprintf( fp2, "S\n" );

	        vnum += 1;
        	column += 1;
	}

        fprintf( fp2, "#0\n#$\n" );

	fclose( fp );
	fclose( fp2 );
	return 0;
}

