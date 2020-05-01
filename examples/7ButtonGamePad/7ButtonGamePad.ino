#include <BleGamepad.h> 


BleGamepad bleGamepad;

int previousButton1State = HIGH;
int previousButton2State = HIGH;
int previousButton3State = HIGH;
int previousButton4State = HIGH;
int previousButton5State = HIGH;
int previousButton6State = HIGH;
int previousButton7State = HIGH;
int previous_x_axis_State = 0;
int previous_y_axis_State = 0;


const int topButton = 27;
const int leftButton = 32;
const int lowButton = 15;
const int rightButton = 33;
const int startButton = 14;
const int selectButton = SCL;
const int x_axis = A2;
const int y_axis = A3;
const int joystickClick = SDA;

const int buttons[] = { topButton, leftButton, lowButton, rightButton, startButton, selectButton, joystickClick };


void setup() {

  for (int i = 0; i < 7; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }
  
  Serial.begin(115200);
  bleGamepad.begin();
}

void loop() {
  if(bleGamepad.isConnected()) {

    int currentButton1State = digitalRead(topButton);
    int currentButton2State = digitalRead(leftButton);
    int currentButton3State = digitalRead(lowButton);
    int currentButton4State = digitalRead(rightButton);
    int currentButton5State = digitalRead(joystickClick);
    int currentButton6State = digitalRead(startButton);
    int currentButton7State = digitalRead(selectButton);

    // Mapping the potentiometer readings from the joystick to a desireable range
    int current_x_axis_State = map(analogRead(x_axis), 0, 4095, -127, 127); 
    
    // The mapping is backwards here because\
    the readings would produce an inverted y axis by default, which annoys me. If thats your thing, feel free to flip the negative signs.
    int current_y_axis_State = map(analogRead(y_axis), 0, 4095, 127, -127); 


    buttonPress(previousButton1State,currentButton1State, BUTTON_1);
    previousButton1State = currentButton1State;
    
    buttonPress(previousButton2State,currentButton2State, BUTTON_2);
    previousButton2State = currentButton2State;
    
    buttonPress(previousButton3State,currentButton3State, BUTTON_3);
    previousButton3State = currentButton3State;

    buttonPress(previousButton4State,currentButton4State, BUTTON_4);
    previousButton4State = currentButton4State;

    buttonPress(previousButton5State,currentButton5State, BUTTON_5);
    previousButton5State = currentButton5State;
    

    buttonPress(previousButton6State,currentButton6State, BUTTON_6);
    previousButton6State = currentButton6State;


    buttonPress(previousButton7State,currentButton7State, BUTTON_7);
    previousButton7State = currentButton7State;


    
    if (current_x_axis_State != previous_x_axis_State || current_y_axis_State != previous_y_axis_State) // checking 
    {

      bleGamepad.setAxes(current_x_axis_State, current_y_axis_State);
      
    }
    previous_x_axis_State = current_x_axis_State;
    previous_y_axis_State = current_y_axis_State;
    
    // This mild delay gives the Huzzah some time to breathe.\
    It also makes the joystick feel nicer and a bit more natural. Too many updates too\
    fast would feel way too snappy. 
    
    delay(10); 
     
  }
}

// Helper function to complete a buttonPress
void buttonPress(int previousButtonState,int currentButtonState, int button) {
  
    if (currentButtonState != previousButtonState)
    {
      if(currentButtonState == LOW)
      {
        bleGamepad.press(button);
      }
      else
      {
        bleGamepad.release(button);
      }
    }
}
