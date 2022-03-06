/*
FP aritmetika, Pa3cio, UL FRI, ARS 2022
prevedi z: g++ -o FParit FParit.c -O0
zaženi z: ./FParit
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>


#define NESKONCNO 0x7F800000 // zapis neskončnega FP stevila v bitni 32-bitni obliki (E = 0xFF, M = 0)


bool jeNeskoncno(float A) {
    // iz zapisa odstranimo prvi bit predznaka in preverimo ali je neskončno:
    if ((*(int *)&A & 0x7FFFFFFF) == NESKONCNO)
        return true;
    return false;
}

bool jeNaN(float A) {
  // NAN: E = 0xFF, M != 0 
  // izločimo eksponent:
  int exp = (*(int *)&A & 0x7F800000) >> 23;
  // izločimo mantiso:
  int mantissa = *(int *)&A & 0x007FFFFF;
  if (exp == 0xFF && mantissa != 0)
    return true;
  return false;
}

int Predznak(float A) {
  // Vrnimo predznak (0x00000000 ali 0x80000000):
  return (*(int *)&A) & 0x80000000;
}

// V FP nikoli ne smemo početi tega: if (a == b) !!!!
// Funkcija preveri ali sta dve FP stevili 'skoraj' enaki 
// Primerjavo izvajamo na osnovi ULP razdalje med števili
// če je razdalja manjša od maxULPs, potem rečemo, da sta števili enaki
// maxULPs določa, koliko smeta biti narazen v ULP-jih, da ju še smatram za enaki
bool StaSkorajEnakiULP(float A, float B, int maxULPs) {
    
    // 1. Ali je katero izmed števil neskončno? 
    if (jeNeskoncno(A) || jeNeskoncno(B))
        return A == B;  // potem sta enaki, če se ujemata v vseh bitih

    // 2. Ali je katero število NaN?
    if (jeNaN(A) || jeNaN(B))
        return false;   // ker nista števili, potem niti ne moreta biti enaki

    // 3. Preverimo ali se razlikujeta v predznaku:
    if (Predznak(A) != Predznak(B))
        return false; // če nimata enakega predznaka, potem nista enaki

    // 4. Primerjamo velikosti na osnovi ULP razdalje:
    int aInt = *(int *)&A;
    // izločimo le abs. velikost, da ju lažje primerjam
    if (aInt < 0)
        aInt = 0x80000000 - aInt;
    // izločimo le abs. velikost, da ju lažje primerjam
    int bInt = *(int *)&B;
    if (bInt < 0)
        bInt = 0x80000000 - bInt;
    // primerjam:
    int intDiff = abs(aInt - bInt); // odštejem njuni absolutni velikosti, da dobim ULP razdaljo)
    if (intDiff <= maxULPs)
        return true;  // sta zeloo blizu, torej enaki
    return false;     // sta precej narazen, torej neenaki
}

void IzracunajRazlikoULP(float A, float B){
  int aInt = (*((int *)&A));
  int bInt = (*((int *)&B));

  // izločimo le abs. velikost, da ju lažje primerjam
  if (aInt < 0)
      aInt = 0x80000000 - aInt;
  // izločimo le abs. velikost, da ju lažje primerjam
  if (bInt < 0)
      bInt = 0x80000000 - bInt;
  // razlika v ULP:
  int intDiff = abs(aInt - bInt); // odštejem njuni absolutni velikosti, da dobim ULP razdaljo

  printf("Števili %+1.15f in %+1.15f sta narazen za %d ULPjev\n", A, B, intDiff);
}

float sestej(float a, float b){
    return a + b;
}

// Izpiši FP število:
void izpisi(float a){
    printf("%.15f\n", a);
}
void izpisiNapaka(){
    printf("\n?!?!? WTF !?!? Ma ne me jebat!! \n");
}

// Izpiši FP število in njegovo predstavitev v HEX:
void IzpisiFPstevilo(float f, int offset) {
  // obravnavaj podano FP število kot 32 bitni int in dodaj offset:
  (*((int *)&f)) += offset;
  int x1 = (*((int *)&f)) - 1;
  int x2 = (*((int *)&f)) + 1;
  float f1 = (*((float *)&x1)) ; // FP število pred številom f (predhodnik)
  if (f == 0.0) f1 = (*((float *)&x2));

  // izračunamo ulp med predhodnikom in f
  float ulp_1 = fabs(f - f1);

  // izločimo eksponent in mantiso iz f:
  int E = ( ((*((int *)&f)) & 0x7FFFFFFF ) >> 23 );
  int M = ((*((int *)&f)) & 0x007FFFFF);

  // izpišimo vse podatke:
  if (E == 0) {
    // Izpiši denormalizirano ali ničlo:
    if (M == 0) { //ničla
      printf("%+1.15f \t\t 0x%08X, E=0x%02X, M=0x%06X, ničla, ulp=%+1.15g\n", f, *(int *)&f, E, M, ulp_1);
    }
    else { //denormalizirano
      printf("%+1.15f \t\t 0x%08X, E=0x%02X(%d), M=0x%06X, denormalizirano, ulp=%+1.15g \n", f, *(int *)&f, E, E-126, M, ulp_1);
    }
  } 
  else {
    // Izpiši:
    printf("%+1.15f \t\t 0x%08X, E=0x%02X(%d), M=0x%06X, ulp=%+1.15g\n", f, *(int *)&f, E, E-127, M, ulp_1);
  }
  
}


// Izpiši FP število v znanstveni notaciji ter njegovo predstavitev v HEX:
void IzpisiFPsteviloScient(float f, int offset) {
  // obravnavaj podano FP število kot 32 bitni int in dodaj offset:
  (*((int *)&f)) += offset;
  int x1 = (*((int *)&f)) - 1;
  int x2 = (*((int *)&f)) + 1;
  float f1 = (*((float *)&x1)) ; // FP število pred številom f (predhodnik)
  

  // izračunamo ulp med predhodnikom in f
  float ulp_1 = fabs(f - f1);
  

  // izločimo eksponent in mantiso iz f:
  int E = ( ((*((int *)&f)) & 0x7FFFFFFF ) >> 23 );
  int M = ((*((int *)&f)) & 0x007FFFFF);

  if (E == 0) {
    // Izpiši denormalizirano ali ničlo:
    if (M == 0) { //ničla
      printf("%+1.15g \t\t 0x%08X, E=0x%02X, M=0x%06X, ničla, ulp=%+1.15g \n", f, *(int *)&f, E, M, ulp_1);
    }
    else { //denormalizirano
      printf("%+1.15g \t\t 0x%08X, E=0x%02X(%d), M=0x%06X, denormalizirano, ulp=%+1.15g \n", f, *(int *)&f, E, E-126, M, ulp_1);
    }
  } 
  else {
    // Izpiši:
    printf("%+1.15g \t\t 0x%08X, E=0x%02X(%d), M=0x%06X, ulp=%+1.15g\n", f, *(int *)&f, E, E-127, M, ulp_1);
  }
}

void operacijeFP(float f){
    float sum;
    float product;

    sum = 0.0;
    for (int i = 0; i < 100; ++i)
        sum += f;
    product = f * 100;

    printf("\nStokrat seštejemo %1.1f:\n", f);
    printf("vsota = ");
    IzpisiFPsteviloScient(sum, 0);
    


    sum = 0.0;
    for (int i = 0; i < 1000000; ++i)
        sum += f;
    product = f * 1000000;

    printf("\nMiljonkrat seštejemo %1.1f:\n", f);
    printf("vsota = ");
    IzpisiFPsteviloScient(sum, 0);
    /*
    printf("\nZmnožimo 1000000 x %1.1f:\n", f);
    printf("produkt = ");
    IzpisiFPsteviloScient(product, 0);
    */


    printf("\nDesetmiljonkrat seštejemo %1.1f in ga nato prištejemo k velikemu številu a:\n", f);
    float a = 12345678.987654321;
    printf("a = ");
    IzpisiFPstevilo(a, 0);
    sum = 0.0;
    for (int i = 0; i < 10000000; ++i)
        sum += f;
    printf("vsota = ");
    IzpisiFPstevilo(sum, 0);
    a += sum;
    printf("a + vsota = ");
    IzpisiFPstevilo(a, 0);
    printf("a + 1000000 = ");
    IzpisiFPstevilo(13345678.987654321, 0);
    IzracunajRazlikoULP(a, 13345678.987654321);

    

    printf("\nDesetmiljonkrat prištevamo %1.1f k velikemu številu a:\n", f);
    sum = 0.0;
    a = 12345678.987654321;
    printf("a = ");
    IzpisiFPstevilo(a, 0);
    for (int i = 0; i < 10000000; ++i)
        a += f;
    printf("Novi a = ");
    IzpisiFPstevilo(a, 0);
    


    printf("\n!?!? WTF !?!?! Seštevanje v FP ni asociativno!");
}

int main(int argc, char** argv) {
  
    system("clear"); // počisti konzolo
    printf("\n\n\n");
    printf("**************************************************************************************\n");
    printf("*                             Aritmetika v plavajoči vejici                          *\n");
    printf("*                                (c) Pa3cio, Ul FRI, 2022                            *\n");
    printf("**************************************************************************************\n");
    printf("\n");

    printf("0.1 + 0.2 = %.15f \n", sestej(.1, .2));
    if (sestej(.1, .2) != 0.3) {
        izpisiNapaka();
    } 

    printf("\n");
    printf("0.1 : ");
    IzpisiFPstevilo(0.1,0);
    printf("0.2 : ");
    IzpisiFPstevilo(0.2,0);
    printf("0.3 : ");
    IzpisiFPstevilo(0.3,0);
    printf("0.01 : ");
    IzpisiFPstevilo(0.01,0);

    operacijeFP(0.1);



    printf("\n\n\n");

    return 0;
}