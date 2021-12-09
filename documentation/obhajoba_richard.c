=======================OBHAJOBA PROJEKTU IFJ - 10.12.2021=======================
•predstavenie + zadanie
/**
 * Dobrý deň, vážená komisia, ja som Richard a toto sú moji kolegovia, Andrei, Zdeněk a Andrej.
 * Sme tím číslo 082 a vybrali sme si variatnu zadania č. 2 - "implementace tabulky symbolov pomocou
 * tabulky s rozptylenymi položkami".
 */
•štruktúra projektu
/**
 * Našu implementáciu je možné rozdeliť do 4 samostatných modulov: scanner,
 * precedenčná syntaktická analýza, parser, generátor kódu. Tieto moduly využívajú pomocné dátové štruktúry.
 * Analýzu vykonávame v jedinom priechode vstupu.
 */
•scanner
•get_next_token(&token); - DKA
/**
 * Scanner slúži pre lexikálnu analýzu. Pomoci funkci get_next_token() parser nacte ze standartniho
 * vstupu token pomocou deterministickeho konečnáho automatu. Na zaklade konecneho stavu bude urcen jeho typ.
 * Typ je nutny pro dalsi praci s timti tokenem a take pro urceni nasledujicich informaci:
 * 1. zda token je klicovem slovem;
 * 2. pokud jde o cislo, bude proveden prevod do odpovidajici promenne;
 */

•parser
•komunikácia so všetkým
•zhora dole, rekurzivný zostup
•syntaktika i sémantika
•chyba = koniec
/**
 * Parser je hlavnim elementem prekladace, protoze komunikeje se vsemi jinymi elementy
 * a ridi celou funkcnost prekladace. Syntakticka analyza se provadi shora dolu metodou rekurzivniho sestupu. Kazdy nonterminal
 * ma urcitou funkci, kde podle LL tabulky se spracovava jedno urcite pravidlo. Neni vsetko
 * implementovano pomoci rekurzi, protoze nam prišlo byt jednoduchšie volat funkci v ramcich
 * celeho pravidla.
 *
 * 1. Semanticke chyby pro nekompatibilitu typu prirazeni, navratovych hodnot funkci a predanych
 * argumentu do funkci se detekuji pomoci 2 poli (leva strana typu a prava strana typu). Po zpracovani
 * urciteho pravidla se provadi porovnani podle semnatiky jazyka IFJ21.
 *
 * 2. Kompatibilita typu ve vyrazech se detekuje stejnym zpusobam, ale jsou pouzity dve charove promenne.
 */


•precedenčná syntaktická analýza
•zdola hore, výrazy
•expression();
•precedenčná tabuľka
/**
 * Precedenčná syntaktická analýza je modul ktorý zaisťuje spracovanie výrazov metódou zdola hore.
 * Vo svojom rozhraní obsahuje expression(), ktorú volá parser, ked chce precedenčnej analýze predať
 * riadenie vo chvíli, kedy očakáva výraz. Postupne spracováva tokeny a pomocou precedenčnej
 * tabuľky symbolov určuje precedenciu a dané výrazy redukuje podľa pravidiel.
 * Kvuli specifikaci jazyka IFJ21 jednotlive tokeny a prikazy muzou byt zapsane hned po sobe
 * bez urcitych oddelovacu. Tim padem nas tym narazil na dva pripady, ktere jsme vyresili jednoduchym
 * rozsirenim precedencni tabulky.
 *
 * (vsechne promenne jsou driv deklarovane)
 * 1. a = 2 + b c = 2 + a - spravna syntaxe
 * 2. a = (2 + b) c = 2 + a - spravna syntaxe
 * !ALE!
 * 1. a = 2 + b c  - spatna syntaxe
 * 2. a = (2 + b)c - spatna syntaxe
 *
 * Takze kdyz po identifikator jde iddentifikator nebo po prave zavorce jde identifikator,
 * tak mame nejednoznacnost.
 * V jednom pripade narazime na chybu, v jinem na spravny zapis. Resit toto muze
 * pouze parser. Rozsireni PT je v tom, ze se muzou objevit 2 pripady, kdy budeme muset
 * zredukovat zustatek na zasobniku a vratit rizeni parseru, ktery rozhodne, zda slo o chybu
 * nebo spravnou syntax.
 */


•sémantické akce
/**
 * Semanticke chyby pro nedefinici a redefinice se detekuji pomoci tabulek symbolu.
 * Pro jmena funkci mame globalni tabulku symbolu, a pro jmena promennych je vytvoren zasobnik
 * tabulek symbolu. K takovemu reseni jsme prisli kvuli nutnosti praci s bloky a nutnost videt promenne z
 * predchozich bloku.
 *
 */

•generátor
•Postfix z expression
•typy operatorov
•tlač na konci
/**
 *  Generovanie cieloveho kodu IFJcode21 je imolementované a volané v parseri na základe pravidiel LL-Gramatiky.
 *  Nakolik nedelame optimalizaci kodu neaplikovali sme reseni 3AC.
 *  Na zaistenie vypisania cieloveho kodu len za podmienky bezchybnej analyzy zapisujeme cielovy kod do 2 textovych
 *  blokov -  definicie funkcii a ich volanie .  Tieto 2 textove bloky po uspesnej analyze skonkatenujeme a
 *  vypíšeme na standartny vystup.
 *
 *  Deklaracie premennych a ich mozny konflikt nazvov sme vyriešili vdaka obojsmernemu radu v ktorom sa uklada adresa elementu tabulky
 *  symbolov s prislusnym identifikatorom ktora obsahuje unikatne cislo premennej.
 *
 *  Volani funkcii je zajisteno vygenerovanim kodu ktory preda funkcii argumenty pomocou docasneho ramcu, nasledne
 *  je vygenerovan kod pro zavolani funkce. Pro spravnou funkcnost volane funkce je ihned na zacatku generovan kod,
 *  ktory z docasneho ramca vytvori lokalni ramec funkce a pro vsechny argumenty ktere byli funkci predane vytvori promenne se
 *  unikatnym nazvom podle nazvu parametru funkce. Nasledne se generuje kod tela funkce.
 *
 *  Generovani kodu pro vyrazy je volane ihned po jejich redukci, v prubehu ktere je vyraz zapsan do obojstranneho radu
 *  v postfixovom formate. Jednotlive elementy v rade nesu vsetky potrebne
 *  informacie na generovanie vyrazu - typ operatora, meno premennej ci hodnotu kontanty. Hodnoty, medzivysledky a nasledne vysledok vyrazu su
 *  ulozene na zasobnik.
 *
 *  Pre generovanie podmienok a cyklov pouzivame navestia ktore su taktiez reprezentovane unikatnym cislom a
 *  identifikatorom funkcie kde sa nachadzaju. Na zabranenie redeklaracie promennych zapisuje kod cyklu do 2 roznych blokov.
 *  Vyskytnute deklaracie zapisujeme nadalej do bloku definicii funkcii no zvysny kod tela cyklu zapisujeme do 3.
 *  pomocneho textoveho bloku.  Nasledne po vygerovani celeho cyklu tieto 2 bloky skonkatenujeme.
 */

