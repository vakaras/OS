Grįžimas iš pertraukimo:

1.  Išsaugome branduolio dėklo adresą.
2.  Įjungiame proceso puslapiavimą.
1.  Nukopijuojame CPUInfo adresu, kur prasideda proceso dėklas.
2.  Iškviečiame funkciją ``switch_process(stack_address)``, kuri nusistato 
    teisingą rsp reikšmę ir padaro grįžimą iš pertraukimo.


Pertraukimo apdorojimas:

3.  Išsaugome CPUInfo Proceso informacijoje.
2.  Atstatome branduolio dėklo adresą.
1.  Įjungiame branduolio puslapiavimą.
4.  Apdorojame pertraukimą.
