#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getbtns( int btn ){
    switch (btn){
        case 1:
            return ((PORTF & 0b0010) >> 1);
        case 2:
            return ((PORTD & 0b00000000000100000) >> 5);
        case 3:
            return ((PORTD & 0b00000000001000000) >> 6);
        case 4:
            return ((PORTD & 0b00000000010000000) >> 7);
    }
}
