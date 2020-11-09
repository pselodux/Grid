int row[8][8] = {
                  { 4,4,4,4,4,3,2,1 },
                  { 4,4,4,4,4,4,4,4 },
                  { 1,1,1,1,1,1,1,1 },
                  { 2,2,2,2,2,2,2,2 },
                  { 4,4,4,4,1,1,1,1 },
                  { 3,3,3,3,2,2,2,2 },
                  { 2,2,2,2,3,3,3,3 },
                  { 1,1,1,1,4,4,4,4 }
                };

int delayTime = 300;


void setup() {

  for (int i=2; i<10; i++){
    pinMode(i, OUTPUT);
  }
  for (int i=14; i<22; i++){
    pinMode(i, OUTPUT);
  }
  for (int i=0; i<8; i++){
    digitalWrite(i+2, HIGH);
    digitalWrite(i+14, LOW);
  }
}

void loop() {

  for(int i=0; i<8; i++){
    drawRow(i);
  }

}

/*
 * Draw a row. Iterates 4 times:
 * 1st time, all intensities above zero are enabled.
 * 2nd time, all under 2 (ie. intensity 1 only) are disabled.
 * 3rd time, all under 3 are disabled.
 * 4th time, all under 4 are disabled. 
 *  
 * Not sure if/how there is a better way to do this. 
 * It's still a bit flickery, but it's the brightest I've been able to get it!
 */
void drawRow(int rowNum){
  // row on
  digitalWrite(rowNum+14, HIGH);

  // columns on
  for(int i=0; i<8; i++){
    if(row[rowNum][i]){
      digitalWrite(i + 2, LOW);
    }
  }
  delayMicroseconds(delayTime);

  for(int i=0; i<8; i++){
    if(row[rowNum][i] < 2){
      digitalWrite(i + 2, HIGH);
    }
  }
  delayMicroseconds(delayTime);

  for(int i=0; i<8; i++){
    if(row[rowNum][i] < 3){
      digitalWrite(i + 2, HIGH);
    }
  }
  delayMicroseconds(delayTime);

  for(int i=0; i<8; i++){
    if(row[rowNum][i] < 4){
      digitalWrite(i + 2, HIGH);
    }
  }
  delayMicroseconds(delayTime);


//  // columns off
//  for(int i=0; i<8; i++){
//    digitalWrite(i + 2, HIGH);
//  }

  // row off
  digitalWrite(rowNum+14, LOW);
}

/*
 * Button input. Decoupled from LEDs, just send on/off signals via MIDI,
 * at 127 velocity.
 */



/*
 * MIDI input. Each note corresponds to a row/column:
 * 0-7: Row 1
 * 8-15: Row 2
 * ...
 * 48-63: Row 8
 * 
 * .. and velocity corresponding to an intensity:
 * 0: off
 * 1-31: intensity 1
 * 32-63: intensity 2
 * 64-95: intensity 3
 * 96-127: intensity 4
 * 
 * Intensity is then added to the relevant row+column position 
 * when a note on is sent, and set to 0 when note off is sent.
 * 
 * Add an "all notes off" function?
 */

 
