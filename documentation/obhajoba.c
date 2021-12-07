=======================OBHAJOBA PROJEKTU IFJ - 10.12.2021=======================
•predstavenie + zadanie
/**
 * Dobrý deň, vážená komisia, ja som Andrei a toto sú moji kolegovia, Zdenek, Richard a Andrei.
 * Sme tím číslo 082 a vybrali sme si variatnu zadania č. 2 - "implementace tabulky symbolu pomoci
 * tabulky s rozptylenymi polozkami".
 */
•štruktúra projektu
/**
 * Našu implementáciu je možné rozdeliť do 4 samostatných modulov: scanner,
 * precedenčná syntaktická analýza, parser, generátor kódu a pomocné dátové štruktúry.
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
 * Takze kdyz po id jde id nebo po prave zavorce jde id, mame nejednoznacnost
 * V jednom pripade narazime na chybu, v jinem na spravny zapis. Resit toto muze
 * pouze parser. Rozsireni PT je v tom, ze se objevi 2 pripady, kdy budeme muset
 * zredukovat zustatek na zasobniku a vratit rizeni parseru, ktery rozhodne, zda slo o chybu
 * nebo spravnou syntaxe.
 *
 * (v prezentaci popis to kratce, uved ve slajdu/ech pripady good/bad, a rozsireni
 * ze redukujeme zustatek a vratime rizeni. Cely text pro vsechny, aby vedeli, jak to funguje.
 * Na schuzce to probereme jeste jednou, omlouvam se za chyby)
 */
•parser
    •komunikácia so všetkým
    •zhora dole, rekurzivný zostup
    •syntaktika i sémantika
    •chyba = koniec
/**
 * Parser je hlavnim elementem prekladace, protoze komunikeje se vsemi jinymi elementy
 * a ridi celou funkcnost prekladace. (Parser je spusten z mainu. - zbytecna informace, ale
 * necham). Syntakticka analyza se provadi shora dolu metodou rekurzivniho sestupu. Kazdy nonterminal
 * ma urcitou funkci, kde podle LL tabulky se spracovava jedno urcite pravidlo. Neni vsetko
 * je implementovano pomoci rekurzi, protoze mi, jako vyvojaru prislo jednoduchym volat funkci v ramcich
 * celeho pravidla.
 *
 * Tedka o semnatice.
 * 1. Semanticke chyby pro nekompatibilitu typu prirazeni, navratovych hodnot funkci a predanych
 * argumentu do funkci se detekuji pomoci 2 poli (leva strana typu a prava strana typu). Po zpracovani
 * urciteho pravidla se provasi porovnani podle semnatiky jazyka IFJ21.
 *
 * 2. Kompatibilita typu ve vyrazech se detekuje stejnym zpusobam, ale je pouziity dve charove promenne.
 *
 * 3. Semanticke chyby pro nedefinici, redefinice a tak dal se detekuji pomoci tabulek symbolu.
 * Pro jmena funkci mame globalni tabulku symbolu, a pro jmena promennych je vytvoren zasobnik
 * tabulek symbolu. K takovemu reseni jsme prisli kvuli nutnosti praci s bloky a nutnost videt promenne s
 * predchozich bloku.
 *
 */
•generátor
    •generuje 3AC??
    •Postfix z expression
    •typy operatorov
    •tlač na konci
/**
 *  Generovanie cieloveho kodu IFJcode21 je implementovany ako samostatny modul ktory je riadeny syntaxou. To znamena ze
 *  komponenty modulu su volane v parseri na základe pravidiel LL-Gramatiky. Cielovy kod sa generuje priamo bez tvorby
 *  3adresneho kodu, ktory sme nemuseli vytvarat na zaklade neoptimalizacie cieloveho kodu.
 *
 *  Na zaistenie vypisania cieloveho kodu len za podmienky bezchybnej analyzy zapisujeme cielovy kod do 2 textovych
 *  blokov -  definicie funkcii a ich volanie .  Tieto 2 textove bloky po uspesnej analyze skonkatenujeme a
 *  posleme na standartny vystup.
 *
 *  Generovani kodu pro vyrazy je volane ihned po jejich redukci, v prubehu ktere je vyraz zapsan do  obojstranneho radu
 *  (to znamena stack spojeny s radom - frontou) v postfixovom formate. Jednotlive elementy v rade nesu vsetky potrebne
 *  informacie na generovanie vyrazu - typ operatora, meno premennej ci hodnotu kontanty. Generator generuje instrukcie
 *  kodu Ifjcode21 ktore vyuzivaju len zasobnik. To znamena ze hodnoty, medzivysledky a nasledne vysledok vyrazu su
 *  ulozene na zasobnik.
 *
 *  Deklaracie premennych a ich mozny konflikt nazvov (na zaklade vyskytu toho isteho mena v roznych scopoch)
 *  (?? ramcoch po slovensky??) sme implementovali vdaka obojsmernemu radu v ktorom sa uklada adresa elementu tabulky
 *  symbolov s prislusnym identifikatorom ktora obsahuje unikatne cislo premennej.
 *
 *  Volani funkcii je zajisteno vygenerovanim kodu ktory preda funkcii argumenty pomocou docasneho ramcu, nasledne
 *  je vygenerovan kod pro zavolani funkce. Pro spravnou funkcnost volane funkce je ihned na zacatku generovan kod,
 *  ktory z docasneho ramca vytvori lokalni ramec funkce a pro vsechny argumenty ktere byli funkci predane vytvori promenne se
 *  unikatnym nazvom podle nazvu parametru funkce. Nasledne se generuje kod tela funkce.
 *
 *  Pre generovanie podmienok a cyklov pouzivame navestia ktore su taktiez reprezentovane unikatnym cislom a
 *  identifikatorom funkcie kde sa nachadzaju. Na zabranenie redeklaracie promennych zapisuje kod cyklu do 2 roznych blokov.
 *  Vyskytnute deklaracie zapisujeme nadalej do bloku definicii funkcii no zvysny kod tela cyklu zapisujeme do 3.
 *  pomocneho textoveho bloku.  Nasledne po vygerovani celeho cyklu tieto 2 bloky skonkatenujeme.
 */

•prostredie
/**
 * Programovali sme v prostredí Clionu s aktuálnou verziou gcc na linux mint a ubuntu
 * a používali sme verzovací systém git spolu s portálom GitHub.
 * Jeden tryhard to cele psal ve Vim, ale to je pouze pro velmi specificke studenty.
 */
•komunikácia
    •Discord
    •Schôdzky v škole - multiple for two and more hours
/**
 * Slack za mne je pice, pokud nekoupis professional edition. (prosim nepis toto do prezentace)
 *
 * Pre komunikáciu sme používali výhradne iba discord server ktorý funguje na
 * rovnakom princípe ako slack ktorý používajú profesionálne firmy a tech.
 * giganti ako apple či microsoft. Na danom servery sme mali vytvorené komunikačné vlákna
 * kde boli založené "T0D0" listy, bug listy alebo bežná komunikácia či voice chaty s
 * možnosťou zdielať obrazovku vďaka čomu sme mohli riešiť mnoho problémov
 * digitálne a neohrozovať sa covidákom smrťákom. Avšak aj napriek
 * prvotriednej digitálne komunikácií sme sa snažili mať čo najviac
 * osobných stretnútí kde sa dá toho vždy vysvetliť najviac.
 */
•zhrnutie tímovej práce
    •Som rád že sme začali tak skoro
/**
 * (Scanner byl pripraven uz 1.10.2021 haha )
 *
 * Ihneď pri skladaní tímu sme všetci rozumeli že sa
 * očakáva pravidelná a skorá práca na projekte čo sa nám nakoniec aj podarilo.
 * Skušobné odovzdanie bolo ohodnotené 75%-ami a slúžilo ako veľmi
 * dobrý benchmark toho, čo treba ešte zlepšiť. ( Po pokusnom odovzdaní nám teda
 * už neostalo veľa práce, iba vypracovať dokumentáciu, opraviť bugy kedtak dopracovať
 * zabudnutú či zle pochopenú funkčnosť ako napríklad podporu pre nil hodnoty a spisovať testy.
 * Za danú skúsenosť so správnym rozvrhovaním času práce by som sa mojemu tímu rád
 * poďakoval. TOTO nepis do prezentace)
 */
•implementačné nedostatky
/**
 * Mame spoustu, ale o tom rikat nebudeme *emot so slnečnými okuliarmi*
 */
