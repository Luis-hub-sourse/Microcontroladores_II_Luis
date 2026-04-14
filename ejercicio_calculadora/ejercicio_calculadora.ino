int a;
int b;

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Bienvenido a la Micro-calculadora");
}

void loop() {
  Serial.println("Ingrese una opción:");
  Serial.println("1. Sumar");
  Serial.println("2. Restar");
  Serial.println("3. Multiplicar");
  Serial.println("4. Dividir");

  while (Serial.available() == 0) {} // esperar entrada
  int opcion = Serial.parseInt();

  if (opcion >= 1 && opcion <= 4) {

    Serial.println("Ingrese el primer número:");
    delay(1500);
    while (Serial.available() == 0) {}
    a = Serial.parseInt();
    delay(1500);
    Serial.println("Ingrese el segundo número:");
    while (Serial.available() == 0) {}
    b = Serial.parseInt();

    calculadora(a, b, opcion);

  } else {
    Serial.println("Opción inválida.");
  }

  delay(2000);
}

void calculadora(int a, int b, int dato) {
  String operacion;
  int resultado = 0;

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
