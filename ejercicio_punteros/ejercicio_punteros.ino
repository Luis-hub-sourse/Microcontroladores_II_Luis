void setup() {
  Serial.begin(115200);

}

void loop() {
    int a = 10;
    char b = 'X';
    float c = 3.14;
    unsigned int d = 20;
    long int e = 100000;
    double f = 2.71828;

    // Punteros a cada variable
    int *pa = &a;
    char *pb = &b;
    float *pc = &c;
    unsigned int *pd = &d;
    long int *pe = &e;
    double *pf = &f;

    // Mostrar tamaños
    Serial.printf("Tamanio de int: %zu bytes\n", sizeof(int));
    Serial.printf("Tamanio de char: %zu bytes\n", sizeof(char));
    Serial.printf("Tamanio de float: %zu bytes\n", sizeof(float));
    Serial.printf("Tamanio de unsigned int: %zu bytes\n", sizeof(unsigned int));
    Serial.printf("Tamanio de long int: %zu bytes\n", sizeof(long int));
    Serial.printf("Tamanio de double: %zu bytes\n\n", sizeof(double));

    // Mostrar direcciones de memoria
    Serial.printf("Direccion de int: %p\n", (void*)pa);
    Serial.printf("Direccion de char: %p\n", (void*)pb);
    Serial.printf("Direccion de float: %p\n", (void*)pc);
    Serial.printf("Direccion de unsigned int: %p\n", (void*)pd);
    Serial.printf("Direccion de long int: %p\n", (void*)pe);
    Serial.printf("Direccion de double: %p\n", (void*)pf);

  delay(3000);
}
