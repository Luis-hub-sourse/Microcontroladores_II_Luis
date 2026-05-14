void cuadrado_espacios();

void cuadrado_espacios(){
  for(int i = 0; i < 10; i++){
    
    Serial.print("*");
    
  }
  
  for(int s = 0; s < 8; s++){
    
    Serial.print("\n*");
    
    for(int c = 0; c < 8; c++){
      
      Serial.print(" ");
      
    }
    Serial.print("*");
  }
  
  Serial.println("");
  
  for(int e = 0; e < 10; e++){
    
    Serial.print("*");
    
  }  
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // cuadrado_espacios();
  
  for(int i = 0; i < 10; i++) {

    for(int r = 0; r < 10; r++) {
      Serial.print("*");
    }

    Serial.println();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
