Dobry den, vazene cleny komise, jsem A S, vedouci tymu cislo XX.
Dovolte mi prosim predstavit sve kolegy A B, Z L a R G.
Tema dnesni prezentace je "Implementace prekladace jazyka IFJ21".

##################### scroll slide #########################

Nasi implementaci lze rozdelit do 5 samostatnych modulu:
1. lexikalni analyza,
2. syntakticka analyza
3. precedencni syntakticka analyza
4. semanticka analyza
5. generovani koda
S techto casti se sestava prezentace, obsah ktere muzete videt pred sebou.
Postupne projdu pres kazdy samostatny modul prekladace a strucne povim jejich implementaci.

##################### scroll slide #########################

Prvnim modulem je lexikalni analyzator, ktery je implementovan pomoci deterministickeho konecneho automatu.
Tento modul je volan funkci get_next_token() a jeho hlavnim cilem je:
1. nacist token ze standartniho vstupu
2. urcit jeho typ na zaklade koncoveho stavu automatu
3. vyplnit potrebne atributy tokenu pro dalsi praci s nim

#################### scroll slide ##########################

Dale jde syntakticky analyzator a je hlavnim modulem prekladace, protoze ridi vsechny ostatni moduly.
Funguje metodou rekurzivniho sestupu na zaklade LL-gramatiky.
Po zavolani funkci get_next_token() dostane od lexiklaniho analyzatoru token.
Na zaklade typu tokenu se muzou provest 3 akci:
1. Rizeni se preda precedencnimu syntaktickemu analyzatoru pro zpracovani vyrazu
2. Pro dany token se zavola urcita semanticka akce
3. Vygeneruje se cast ciloveho kodu
Po provedeni aspon jedne z techto akci se odsimuluje pridani listu do derivacniho stromu.
Tato akce je rizena LL-gramatikou, ve ktere mame 49 pravidel.
Ted rozebereme podrobneji kazdou ze tri akci pro tokeny, ktere je videt na obrazku a byly zminene drive.

#################### scroll slide ##########################

Prvni z nich je predani rizeni precedencnimu syntaktickemu analyzatoru.
Slouzi pro zpracovani vyrazu zdolu nahoru na zaklade precedencni tabulky.
Kvuli specifikaci jazyka IFJ21 jednotlive tokeny a prikazy muzou byt zapsane hned po sobe bez urcitych oddelovacu.
Tim padem nas tym narazil na dva pripady, ktere jsme vyresili jednoduchym rozsirenim precedencni tabulky.
Tyto dva pripady lze videt na slajdu v tabulce.
Rozsirenim je pridani 5. akce do precedencni tabulky, ktera bez ukladani tokenu do zasobniku postupne ho zredukuje a ukonci zpracovani vyrazu.
Pak syntakticky analyzator umi vyresit tyto 2 nedeterminismy.
Cely zpracovany vyraz se postupne zapisuje v postfiz formatu a pak je predan generatoru kodu.

#################### scroll slide ##########################

Jednou z akci nad tokenem je semanticka analyza.
Jeji hlavnim modulem je tabulka symbolu.
Podle cisla tymu jsme meli variantu cislo 2 a proto jsme implementovali tabulku symbolu pomoci tabulky s roptylenymi polozkami.
Na slajdu je videt vice tabulek symbolu a to kvuli specifikaci prace s identifikatory.
Jmena funkci jsou ulozena do takzvane globalni tabulky symbolu, kde se nachazi do konce prekladu.
Jmena promennych se ukladaji do pole tabulek symbolu kvuli vnorenym ramcum a moznosti deklaraci promennych stejneho jmena v ruznych ramcich.
Po vyskoceni z ramce je tabulka symbolu odstranena z pole.

################### scroll slide ###########################

Poslednim modulem v prekladace je generator kodu.
1. Implementace.
Kvuli sekvencnimu pruchodu ciloveho kodu jeho generovani probiha do dvou bloku, ktere se pak konkatenuji.
V prvnim bloku je napsane volani vsech funkci, v jinem jejich implementace, do kterych skace program v pripade volani.

2. Deklarace promennych.
Deklarace promennych a jejich konflikt v pripade while cyklu jsme vyresili rozdelenim bloku pro funkci.
Do jednoho bloku se zapisuji vsechny deklarace promennych.
S timto blokem pak bude spojena inplemenace cele funcke.
Redeklarace promennych v cilovem kodu je veresena pomoci vytvoreni unikatniho jmena pro kazdou promenne:
(_ - podtrzitko) jmenoFunkce_jmenoPromenne_cisloRamce.

3. Funkce.
Volani funkci je zajisteno vygenerovanim kodu ktery preda funkci argumenty pomoci docasneho ramce.
Nasledne je vygenerovan kod pro zavolani funkce.
Pro spravnou funkcnost volani funkce je na zacatku generovan kod, ktery z docasneho ramce vytvori lokalni ramec funkce
a pro vsechny argumenty ktere byly predane funkce vytvori promenne s unikatnim nazvem.
Nasledne se generuje kod tela funkce.

4. Vyrazy
Generovni vyrazu je implementovane nasledovne.
Generatoru kodu prijde vyraz v postfixovem formatu, elemnty ktereho obsahuji potrebne atributy.
Hodnoty, mezivysledky a nasledne vysledek vyrazu se ukladaji na zasobnik.

5. Podminky a cykly.
Pre generovanie podminek a cyklu pouzivame navesti, ktere maji unikatni jmena reprezentovana cislem ramce a jmenem funkce kde se nachazeji.

###################### scroll slide #########################

Timto obrazkem, ktery reprezentuje nas cely prekladac, bych ukoncil tuto prezentaci.
Dekuji Vam za pozornost.
Pokud mate otazky k cemukoliv, radi na ne odpovime.

