int a = 0;
String password = "42737462";
String pasword_ingresada;

void verificar_contrasenna();

void verificar_contrasenna(){

    Serial.print("Ingrese su contraseña: ");

    while (1){
      
      digitalWrite(8, LOW);   
      delay(500);                   
      digitalWrite(8, HIGH);  
      delay(500); 
      
      pasword_ingresada = Serial.readString();
      
      if (pasword_ingresada != 0){
        Serial.println(pasword_ingresada);
        break;
        
      }
    }

    if (password == pasword_ingresada){
      
      Serial.println("La contraseña ingresada es correcta");
      
      while(1){
        
        digitalWrite(8, LOW);
      
      }

    }else if(a == 10){
      
      Serial.println("La sesion fue bloqueada por muchos intentos.");
      
      while(1){
        digitalWrite(8, HIGH);
      }
      
    }else{
      
      a++;
      Serial.println("¡Contraseña incorrecta!");
      
    }
}

void setup() {
  Serial.begin(9600);
  pinMode(8, OUTPUT);
}

void loop() {
  verificar_contrasenna();
}
