int a;
int b;
int opcion;

void calculadora(int a, int b, int dato);

void calculadora(int a, int b, int dato) {
  String operacion;
  int resultado;

  if (dato == 1) {
    operacion = "suma";
    resultado = a + b;
  } 
  else if (dato == 2) {
    operacion = "resta";
    resultado = a - b;
  } 
  else if (dato == 3) {
    operacion = "multiplicar";
    resultado = a * b;
  } 
  else if (dato == 4) {
    operacion = "division";
    if (b != 0) {
      resultado = a / b;
    } else {
      Serial.println("Error: división por cero");
      return;
    }
  }

  Serial.printf("El resultado de la %s es: %d\n", operacion, resultado);
  //Serial.print(operacion);
  //Serial.print(" es: ");
  //Serial.println(resultado);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Bienvenido a la Micro-calculadora");
}

void loop() {
  Serial.println("1. Sumar");
  Serial.println("2. Restar");
  Serial.println("3. Multiplicar");
  Serial.println("4. Dividir");
  Serial.println("Ingrese una opción: ");

  while (1) {
  opcion = Serial.parseInt();

  if (opcion >= 1 && opcion <= 4) {
    Serial.println("Ingrese el primer número:");
    Serial.read();
    while (1){
      a = Serial.parseInt();
      if (a != 0){
        Serial.println(a);
        break;
      }
    }
    Serial.println("Ingrese el segundo número:");
    while (1){
      b = Serial.parseInt();
      if (b != 0){
        Serial.println(b);
        break;
      }
    }
    break;
  } else if (opcion < 1 && opcion > 4){
    Serial.println("Opción inválida.");
    Serial.read();
  }
  }
  calculadora(a, b, opcion);
  delay(2000);
}
