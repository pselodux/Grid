/* 
 * Row data:
 * 4 = brightest
 * 1 = dimmest
 * 0 = off
 * Note brightness 2/3 aren't very useful as they aren't much different
 * to brightness 4. There's probably some way to make it better, but
 * this is the best I can do at the moment. I'm not very experienced
 * with PWM rates!
*/
int row[8][8];

// delay between each refresh
int delayTime = 200;

// set these up so that holding the button only triggers the target
// once, instead of millions of times..
bool canNoteOn[8][8];
bool canNoteOff[8][8];

// debounce timer
float timer[8][8];

void setup() {
  Serial.begin(38400);

  usbMIDI.setHandleNoteOff(ReceiveMIDINoteOff);
  usbMIDI.setHandleNoteOn(ReceiveMIDINoteOn);

  /*
   * I suppose this section could be streamlined a bit / made easier for adaptation
   * by defining the pinouts as variables outside the setup() function. Something
   * to be added in the future..
  */

  for (int i=0; i<8; i++){
    for (int j=0; j<8; j++){
      canNoteOn[i][j] = true;
      canNoteOff[i][j] = false;
      timer[i][j] = 1.0;
      row[i][j] = 0;
    }
  }

  // set up LED rows and columns
  for (int i=0; i<8; i++){
    pinMode(i+2, OUTPUT);
    pinMode(i+14, OUTPUT);
    digitalWrite(i+2, HIGH);
    digitalWrite(i+14, LOW);
  }

  // set up button columns
  for (int i=24; i<32; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  // set up button rows
  for (int i=32; i<40; i++){
    pinMode(i, INPUT);
  }
}

void loop() {
  
  for(int i=0; i<8; i++){
    // read MIDI input (in the for loop because it's faster)
    usbMIDI.read(1);

    // display active LEDs
    drawRow(i);
    
    // read button presses
    readCol(i);
  }

}


/*
 * Read button presses. 
 * 
 * TODO: Send MIDI!
 */
void readCol(int colNum){
  // enable column
  digitalWrite(colNum+24, LOW);
  // iterate through rows and turn off so they don't dummy read
  for (int i=0; i<8; i++){
//    pinMode(i+32, OUTPUT);
    digitalWrite(i+32, HIGH);
  }
  // iterate through rows and read
  for (int i=0; i<8; i++){
    pinMode(i+32,INPUT);

    if(digitalRead(i+32) == LOW){
      // set debounce timer
      timer[i][colNum] = 1.0;
      
      if(canNoteOn[i][colNum]){
        // send note on
        SendMIDINoteOn(colNum + i*8);
        
        // temporary lighting up just to show feedback
//        row[i][colNum] = 4;
        
        // debug stuff
//        Serial.print("note on ");
//        Serial.print(i);
//        Serial.print(", ");
//        Serial.println(colNum);
        
        canNoteOn[i][colNum] = false;
        canNoteOff[i][colNum] = true;
      }

    }

    
    /* 
     * VERY SIMPLE DEBOUNCE CODE
     * 
     * This starts a (very short) timer which allows note on when
     * it hits zero. Button presses are ignored in between.
     */
     
    if (timer[i][colNum] > 0 && !canNoteOn[i][colNum]){
      timer[i][colNum] -= 0.2;
    }
    if (timer[i][colNum] < 0.6 && canNoteOff[i][colNum]){
      // send note off
      SendMIDINoteOff(colNum + i*8);
        
      // temporary dimming of LED just to show feedback
//      row[i][colNum] = 0;
      
      // debug stuff
//      Serial.print("note off ");
//      Serial.print(i);
//      Serial.print(", ");
//      Serial.println(colNum);
      
      canNoteOff[i][colNum] = false;
    }
    if (timer[i][colNum] <= 0 && !canNoteOff[i][colNum]){
      canNoteOn[i][colNum] = true;
    }
  }
  // disable column
  digitalWrite(colNum+24, HIGH);
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

  // columns on:
  // First, check if any contain a value above zero,
  // and turn them on if so.
  for(int i=0; i<8; i++){
    if(row[rowNum][i] > 0){
      digitalWrite(i + 2, LOW);
    } else {
      digitalWrite(i + 2, HIGH);
    }
  }
  delayMicroseconds(delayTime);

  // After a delay, check for any with value 1, and
  // turn them off.
  for(int i=0; i<8; i++){
    if(row[rowNum][i] < 2){
      digitalWrite(i + 2, HIGH);
    }
  }
  delayMicroseconds(delayTime);

  // Another delay, turn off/keep off if 2
  for(int i=0; i<8; i++){
    if(row[rowNum][i] < 3){
      digitalWrite(i + 2, HIGH);
    }
  }
  delayMicroseconds(delayTime);
  
  // As above, but for 3.
  for(int i=0; i<8; i++){
    if(row[rowNum][i] < 4){
      digitalWrite(i + 2, HIGH);
    }
  }
  delayMicroseconds(delayTime);

  // Value 4 will stay on until the next row is shown.

  // columns off
  // (not needed anymore as it's dealt with in the >0 condition above)
  // for(int i=0; i<8; i++){
  //   digitalWrite(i + 2, HIGH);
  // }

  // row off
  digitalWrite(rowNum+14, LOW);
}

/*
 * MIDI output.
 */

void SendMIDINoteOn(int noteNum){
  usbMIDI.sendNoteOn(noteNum, 127, 1);
}

void SendMIDINoteOff(int noteNum){
  usbMIDI.sendNoteOff(noteNum, 127, 1);
}

/*
 * MIDI input. Each note corresponds to a row/column:
 * 0-7: Row 1
 * 8-15: Row 2
 * ...
 * 48-63: Row 8
 * 
 * .. and velocity corresponding to an intensity:
 * 0: off
 * 1-63: intensity 1
 * 64-127: intensity 4
 * 
 * (I haven't implemented other intensities yet, as 2-3 don't
 * look different enough from 4).
 * 
 * Intensity is then added to the relevant row+column position 
 * when a note on is sent, and set to 0 when note off is sent.
 * 
 * TODO: Add an "all notes off" function?
 */

void ReceiveMIDINoteOn(byte channel, byte note, byte velocity){
  int noteCol = note % 8;
  int noteRow = trunc(note / 8);
  if(velocity > 63){
    row[noteRow][noteCol] = 4;
  } else {
    row[noteRow][noteCol] = 1;
  }
}

void ReceiveMIDINoteOff(byte channel, byte note, byte velocity){
  int noteCol = note % 8;
  int noteRow = trunc(note / 8);
  row[noteRow][noteCol] = 0;
} 
