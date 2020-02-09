//Fatih Aydin. All rights reserved. Arduino Scientific Calculator.

//importing libraries
#include <Keypad.h>   
#include <StackArray.h>
#include <LiquidCrystal.h>

const byte SATIR = 4;         //number of keypad rows
const byte SUTUN= 4;          //number of keypad cols
char keys[SATIR][SUTUN] = {   //key value matrix 4x4
{'1','2','3','+'},
{'4','5','6','-'},
{'7','8','9','*'},
{'C','0','=','/'}
};

byte rowPins[SATIR] = { 13, 10, 9, 8 };     //setting pins and key mapping
byte colPins[SUTUN] = { 7, 6, 1, 0 };
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, SATIR, SUTUN);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);      //setting lcd pins
  
int SIZE = 20;            //size of the stack array                        
char stack[20];           //stack array
int top = -1;             //top pointer for stack. Stack is empty so top equals -1.
String infixString = "";  //our infix string - we will be send data as a parameter
String temp = "";
int operatorCounter = 0;

StackArray <float> resultStack;
      
void setup() {
  lcd.begin(16, 2);       //we use 16x2 lcd
  lcd.print("fatay computers");    
  delay(2400);            //opening(intro) time
  lcd.clear();            //clear lcd
  lcd.setCursor(0, 0);    //setting cursor to top and left.
}

void loop(){

  //get value from keypad.
  char key = kpd.getKey();

  if (key != NO_KEY){
    if(key == '=') {
      infixString += ","+temp+",";
      String postOut  = InfixToPostFix(infixString);
      float result   = goToResult(postOut);
      delay (100);
      lcd.setCursor(0,1);
      lcd.print(result);
      lcd.setCursor(0,0);
      delay (100);
      
    } else {
      
      if(!is_operator(key)) {
        temp += key;
      }else{
        infixString += ","+temp+","+key;
        temp = "";
        operatorCounter++;
      }
      
      lcd.print(key);
      delay(100);
    }
  }
  
}

///////////////////////////////////-STACK CODES-//////////////////////////////////////

void push(char item) {
  if(top >= SIZE-1) {
    lcd.print("Stack Overflow.");
  }
  else {
    top = top+1;
    stack[top] = item;
  }
}

char pop() {
  char item ;

  if(top <0) {
    lcd.print("Stack under-flow");
  } else {
    item = stack[top];
    top = top-1;
    return(item);
  }
}

boolean isEmpty() {
  if (top == -1) {return true;} else {return false;}
}

char peek() {
  char x;
  if (top < 0) { 
    //lcd.clear();
    //lcd.print("Stack is empty!");
  } else {
    x = stack[top];
    return x;  
  }

}

///////////////////////////////-STACK CODES END-/////////////////////////////////////

boolean is_operator(char symbol) {
  if(symbol == '^' || symbol == '*' || symbol == '/' || symbol == '+' || symbol =='-')
  {
    return true;
  }
  else
  {
    return false;
  }
}


//This function used for finding precedence value of the operations.
int prec(char c) { 
    if(c == '^') 
    return 3; 
    else if(c == '*' || c == '/') 
    return 2; 
    else if(c == '+' || c == '-') 
    return 1; 
    else
    return -1; 
} 

//Converting infix notation to postfix notation
String InfixToPostFix(String infix) {

  String postfix = "";
  
  for(int i=0; i<infix.length(); i++) {
    
    char data = infix.charAt(i);              //Scanning each char of the infix string
    boolean isOperator = is_operator(data);   //Which category? => Operand or Operator
    
    if(!isOperator){ 
      // If the scanned character is an operand, output it.
      postfix += data;
    } else if (data == '(') {
      // If the data is '(' push it to stack.
      push('(');
    } else if(data == ')') {
      //If the scanned character is an ‘)’, pop and to output string from the stack until an ‘(‘ is encountered. 
     while(!isEmpty() && peek() != '('){
        char add = pop();
        postfix += add;
      }
      if(peek() == '(') {
        char add = pop(); 
      }
    } else {
       //If an operator is scanned 
       while(!isEmpty() && prec(data) < prec(peek())) {
           char add = pop();
           postfix+=add;
       }

       push(data);
    }
   
  }

  //Pop all the remaining elements from the stack 
  while(!isEmpty()) {
    char add = pop();
    postfix += add;
  }

  //return the result value
  return postfix;


}

float goToResult(String Postfix) {
  const int operandCounter = Postfix.length();
  String operandArray[operandCounter];

  int k = 0;
  int cc = 0;
  String tmp="";
  for(int i=0; i<Postfix.length(); i++) {
    char sc = (char) Postfix.charAt(i);
    if (is_operator(sc)) {
      operandArray[k] = sc;
      k++;
    }
    else if(sc == ',' && cc%2 == 0) {
       cc++;
    } else if(cc%2==1 && sc != ',' && !is_operator(sc)) {
       tmp += sc; 
    } else if(cc%2 == 1 && sc == ',') {
       operandArray[k] = tmp;
       tmp = "";
       cc++;
       k++;
    } 
   
  } 

  float res = 0;
  for (int i=0; i<operandCounter; i++) {
    char k = operandArray[i].charAt(0);
    if(k >= '0' && k <= '9') {
      float data = (float) operandArray[i].toInt();
      resultStack.push(data);
    } else if(is_operator(k)){
      float one = resultStack.pop();
      float two = resultStack.pop();
      
      if(k=='+') {
         float t = one + two;
         resultStack.push(t);
      } else if (k=='-') {
         float t = two - one;
         resultStack.push(t);
      } else if (k=='*') {
         float t = one * two;
         resultStack.push(t);
      } else if (k=='/') {
         float t = two / one;
         resultStack.push(t);
      } else {}
      
    } else {
      //do nothing  
    }


    
  }
  res = resultStack.pop();
  return res;
  
}
