#include "UcitavanjeDogadjaja.h"

bool UcitavanjeDogadjaja::prijavljen = 0;
bool UcitavanjeDogadjaja::loadPrijavljen = 0;
QTimer * UcitavanjeDogadjaja::prijavljentimer = NULL;

MDogadjaj* UcitavanjeDogadjaja::dogadjaj = NULL;
UcitavanjeDogadjaja* UcitavanjeDogadjaja::instance = NULL;
int UcitavanjeDogadjaja::brslika = 0;
int UcitavanjeDogadjaja::brprijava = 0;
