/*James Arseneau, Micheal Mylett */
void printString(char*);
void readString(char*);
void readSector(char*, int);
void handleInterrupt21(int ax, int bx, int cx, int dx);
int interrupt(int, int, int, int, int);


int main(){


/* Step 1 Test */
char buffer[13312]; /*this is the maximum size of a file*/
int sectorsRead;
makeInterrupt21();
interrupt(0x21, 3, "messag", buffer, &sectorsRead); /*read the file into buffer*/
if (sectorsRead>0) {
    interrupt(0x21, 0, buffer, 0, 0); /*print out the file*/
} else {
    interrupt(0x21, 0, "messag not found\r\n", 0, 0); /*no sectors read? then print an error*/
}
while(1);

}

/*Step 1*/
void printString(char* chars) {
    int i = 0;
    while (chars[i] != 0x0) { /*continues if the current char is not empty*/
        interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);/*prints current char to screen*/
        i++;
    }
}

/*Step 2*/
void readString(char* array){
    int i =0;
    char c; /* current key that is being pressed*/
    while(1){
        c= interrupt(0x16,0,0,0,0);
        if (c==0xd){   /* If enter button is pressed last 3 elements in the string will be the ASCII of Enter key, line feed and End of String values*/
            array[i]= 0xd; /*AscII of Enter key   */
            i++;
            array[i] = 0xa; /* line feed */
            i++;
            array[i]=0x0; /* end of string*/

            /*prints characters to the screen*/
            interrupt(0x10, 0xe*256+0xd, 0, 0, 0);
            interrupt(0x10, 0xe*256+0xa, 0, 0, 0);
           break;

        }else if(c== 0x8){/* if backspace key is pressed*/
            if(i >0){
                i--;
                interrupt(0x10, 0xe*256+0x8, 0, 0, 0);/*moves cursor to the left*/
               interrupt(0x10, 0xe*256+' ', 0, 0, 0); /*Displays an empty character*/
                interrupt(0x10, 0xe*256+0x8, 0, 0, 0); /*moves cursor to the left again*/
            }
        }else{ /*prints current char if that char isnt a backspace or enter key*/
            array[i]=c;
            i++;
            interrupt(0x10,0xe*256+c, 0,0,0);
        }
    }
}

/*Step 3 */
void readSector(char* buffer, int sector){
    int relative_sec= sector+1;

    /*used formula  AX=AH*256+AL, CX=CH*256+CL and DX=DH*256+DL where AH=2, AL=1, Bx=buffer,
      CH=0, DH=0, CL is sector+1, DL=0x80   */

    interrupt(0x13,2*256+1,buffer, 0*256+relative_sec, 0*256+0x80);

}

/* Start of Project C */

void readFile(char* filename, char* buffer, int* sectorsRead) {
    char dir[512];
    readSector(dir, 2);
    int fileFound = 0;

    int fileentry;
    for (fileentry = 0; fileentry < 512; fileentry+=32) {
        if (filename[0]==dir[fileentry+0] && filename[1]==dir[fileentry+1] && filename[2]==dir[fileentry+2] && filename[3]==dir[fileentry+3] && filename[4]==dir[fileentry+4] && filename[5]==dir[fileentry+5]) {
            fileFound = 1;

            int fileSector;
            for (fileSector = 6; fileSector < fileentry + 32; fileSector++) {
                /* iterate through every number listed after the file name that isn't 00 and add to buffer */
                if (dir[fileentry+fileSector] == 00) {
                    break;
                } else { /* does this if the directory number is not 00 */
                    readSector(buffer, dir[fileentry+fileSector]);
                    buffer += 512;
                    sectorsRead += 1;
                }
            }
            break;
        }
    }
    if (fileFound == 0) {
        sectorsRead = 0;
        /*return;*/
    }
    return;
}

/* End of Project C */

/*step4 and 5 */
void handleInterrupt21(int ax, int bx, int cx, int dx){


//char* bxString= (char*)bx; /* (char*)bx converts the int value of bx into a char*/

if(ax==0){

    printString((char*) bx);
    }
else if(ax==1){
    readString((char*) bx);
    }
else if(ax==2){
    readSector((char*) bx, cx);
    }
else if(ax==3){
    readFile((char*) bx, (char*) cx, (int*) dx);
}
else if(ax>=4){
    printString("Error: ax cannot be greater than 3");
    }
}
