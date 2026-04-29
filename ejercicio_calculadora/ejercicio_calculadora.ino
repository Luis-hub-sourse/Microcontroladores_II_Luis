double a;
double b;
float opcion;

void calculadora(double a, double b, float dato);

void calculadora(double a, double b, float dato) {
    String operacion;
    double resultado;
  
    if (dato == 1) {
          operacion = "suma";
          resultado = a + b;
    } 
    else if (dato == 2) {
          operacion = "resta";
          resultado = a - b;
    } 
    else if (dato == 3) {
          operacion = "multiplicación";
          resultado = a * b;
    } 
    else if (dato == 4) {
        operacion = "división";
        if (b != 0) {
          resultado = a / b;
        } else {
            Serial.println("Error: división por cero");
            return;
        }
  }
  Serial.printf("El resultado de la %s es: %f\n", operacion, resultado);
  //Serial.print(operacion);
  //Serial.print(" es: ");
  //Serial.println(resultado);
}

void selecion();

void selecion() {
  Serial.println("1. Sumar");
  Serial.println("2. Restar");
  Serial.println("3. Multiplicar");
  Serial.println("4. Dividir");
  Serial.println("Ingrese una opción: ");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Bienvenido a la Micro-calculadora");
  delay(1000);
  selecion();
}

void loop() {
  
  opcion = Serial.parseFloat();

  if (opcion >= 1 && opcion <= 4) {
    Serial.println("Ingrese el primer número:");
    while (1){
      a = Serial.parseFloat();
      if (a != 0){
        Serial.println(a, 10);
        break;
      }
    }
    Serial.println("Ingrese el segundo número:");
    while (1){
      b = Serial.parseFloat();
      if (b != 0){
        Serial.println(b, 10);
        break;
      }
    }
   calculadora(a, b, opcion);
   selecion();
  } else if ((opcion < 1 || opcion > 4) && opcion != 0){
    Serial.println("Opción inválida.");
    selecion();
  }
}
