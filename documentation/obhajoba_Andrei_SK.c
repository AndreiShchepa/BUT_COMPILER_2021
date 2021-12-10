=======================OBHAJOBA PROJEKTU IFJ - 10.12.2021=======================
•predstavenie + zadanie
/**
 * Dobrý deň, vážená komisia, ja som Richard Gajdošík a toto sú moji kolegovia, Andrei Ščapaniak,
 * Zdeněk Lapše a Andrej Bínovský.
 * Sme tím číslo 082 a vybrali sme si variatnu zadania č. 2 - "implementace tabulky symbolov pomocou
 * tabulky s rozptylenymi položkami".
 * Temou dnešnej prezentácie je "Implementacia prekladača jazyka IFJ21"
 */
##################### scroll slide #########################/**
Našu implementáciu je možné rozdeliť do 5 samostatných modulov:
1. Lexikálna analýza,
2. Syntakticka analyza
3. precedencni syntakticka analyza
4. semanticka analyza
5. generovani koda
Postupne prejdem každý modul a vysvetlím jeho implementáciu */
##################### scroll slide #########################
/**
Prvým modulem je lexikálny analyzátor, který je implementovaný pomocou deterministickeho konecneho automatu.
Tento modul je volany funkciou get_next_token() a jeho hlavnim cielem je:
1. nacitat token zo standartneho vstupu
2. urcit jeho typ na zaklade koncoveho stavu automatu
3. vyplnit potrebne atributy tokenu pro dalsiu pracu s nim
 */
##################### scroll slide #########################
/**
Syntakticky analyzator je hlavnym elementom prekladaca, protoze riadi všetky ostatne moduly.
Funguje metodou rekurzivniho zostupu na zaklade LL-gramatiky.
Po zavolani funkcie get_next_token() dostane od lexiklaneho analyzatoru token.
Na zaklade typu tokenu môžu nastať 3 akcie:
1. Riadenie sa preda precedencnemu syntaktickemu analyzatoru pre zpracovanie vyrazu
2. Pre dany token se zavola urcita semanticka akcia
3. Vygeneruje sa cast cieloveho kodu
Po prevedeni aspon jednej z tychto akcii se odsimuluje pridanie listu do derivacneho stromu.
Tato akce je riadena LL-gramatikou, v ktorej mame 49 pravidel.
Terat rozebereme podrobnejšie kazdu z troch akcii pre tokeny, ktore je vidiet na obrazku a boli predtým zmienené.*/
##################### scroll slide #########################
/**
Prvou z nich je predanie riadenia precedencnemu syntaktickemu analyzatoru.
Sluzi pre spracovanie vyrazu zdola hore na zaklade precedencnej tabulky.
Kvoli specifikacii jazyka IFJ21 jednotlive tokeny a prikazy mozu byt zapisane hned po sobe bez urcitych oddelovacu.
Tym padom náš tym narazil na dva pripady, ktore boli riesene jednoduchym rozsirenim precedencnej tabulky.
Tieto dva pripady je videt na slajde v tabulke.
Rozsirenim je pridanie 5. akcie do precedencnej tabulky, ktera bez ukladania tokenu do zasobniku postupne zredukuje a ukonci zpracovanie vyrazu.
Potom syntakticky analyzator vie vyriesit tieto 2 nedeterminismy.
Cely zpracovany vyraz se postupne zapisuje v postfix formate a potom je predany generatoru kodu.
 */
##################### scroll slide #########################
/**
Jednou z akcii nad tokenom je semanticka analyza.
Jej hlavnym modulom je tabulka symbolov.
Podle cisla timu sme mali variantu cislo 2 a proto sme implementovali tabulku symbolov pomocou tabulky s roptylenymi polozkami.
Na slajde je vidiet viacero tabuliek symbolov a to kvoli specifikacii prace s identifikatormi.
Mena funkcii su ulozene do takzvanej globalnej tabulky symbolov, kde se nachadza do konce prekladu.
Mena premennych sa ukladaju do pola tabuliek symbolov kvoli vnorenym ramcom a moznosti deklaracie premennych rovnakého mena v roznych ramcoch.
Po vyskoceni z ramca je tabulka symbolov odstranena z pola.
 */
##################### scroll slide #########################
/**
Poslednym modulom v prekladaci je generator kodu.
1. Implementacia.
Kvoli sekvencnemu priechodu cieloveho kodu jeho generovanie prebieha do dvoch blokov, ktore sa potom konkatenuju.
V prom bloku je napisane volanie vsetkych funkcii, v inom ich implementacia, do kterych skace program v pripade volania.

2. Deklaracie premennych.
Deklaracie premennych a ich konflikt v pripade while cyklu sme vyresili rozdelenim bloku pre funkcie.
Do jedneho bloku se zapisuju vsetky deklaracie premennych.
S tymto blokom potom bude spojena inplementacia celej funkcie.
Redeklaracia premennych v cielovom kode je vyriesena pomocou vytvorenia unikatneho mena pre kazdu premennu:
(_ - podtrzitko) menoFunkcie_menoPremennej_cisloRamca.

3. Funkcie.
Volani funkcii je zaistene vygenerovanim kodu ktory preda funkcii argumenty pomocou docasneho ramca.
Nasledne je vygenerovany kod pre zavolanie funkce.
Pre spravnu funkcnost volania funkcie je na zaciatku generovany kod, ktory z docasneho ramca vytvori lokalny ramec funkcie
a pre vsetky argumenty ktore boli predane funkcia vytvori premennu s unikatnym nazvom.
Nasledne se generuje kod tela funkcie.

4. Vyrazy
Generovanie vyrazu je implementovane nasledovne.
Generatoru kodu pride vyraz v postfixovem formate, elemnty ktoreho obsahuju potrebne atributy.
Hodnoty, medzivysledky a následne vysledek vyrazu se ukladaju na zasobnik.

5. Podmienky a cykly.
Pre generovanie podmienok a cyklu pouzivame navestie, ktore maju unikatne mená reprezentovane cislem ramca a menom funkcie kde se nachazeju.
*/
###################### scroll slide #########################
/**
Tymto obrazkem, ktery reprezentuje nas cely prekladac, by som ukoncil tuto prezentaciu.
Dakujem vam za pozornosť.
Pokiaľ máte otázky na čokoľvek veľmi radi ich vám zodpovieme.
*/