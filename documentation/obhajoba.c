=======================OBHAJOBA PROJEKTU IFJ - x.xx.2021=======================
•predstavenie + zadanie
/**
 * Dobrý deň, vážená komisia, ja som André a toto sú moji kolegovia, Andrei, Andrej a Andrey.
 * Sme tím číslo xyz a vybrali sme si variatnu zadania č. 1 - "meno implementace".
 */
•štruktúra projektu
/**
 * Našu implementáciu je možné rozdeliť do 5 samostatných modulov: scanner, 
 * precedenčná syntaktická analýza, parser, generátor kódu a pomocné dátové štruktúry.
 * Analýzu vykonávame v jedinom priechode vstupu.
 */
•scanner
    •get_next_token(&token); - KA
/**
 * Scanner slúži pre lexikálnu analýzu a v jeho rozhraní je zadefinovaná čásť kodu pod menom
 * NEXT_TOKEN() v ktorom voláme funkciu get_next_token(&token). Táto funkcia zo vstupu
 * načíta jeden lexém pomocou konečnáho automatu, určí jeho typ, keyword, hodnotu 
 * alebo meno premennej, či obsah stringu a uloží do globálneho token cez pointer ktorý ukazuje
 * na štruktúru s hodnotami tokenu.
 */
•precedenčná syntaktická analýza
    •zdola hore, výrazy
    •expression();
    •precedenčná tabuľka
/**
 * Precedenčná syntaktická analýza je modul ktorý zaisťuje spracovanie výrazov metódou zdola hore.
 * Vo svojom rozhraní obsahuje expression(), ktorú volá parser, ked chce precedenčnej analýze predať
 * riadenie vo chvíli, kedy očakáva výraz. Postupne spracováva tokeny, ktoré volá skrz časť kodu
 * definovanú pod meno GET_TOKEN v ktorej sa nachádza funkcie get_next_token a pomocou precedenčnej 
 * tabuľky symbolov určuje precedenciu dané výrazy redukuje podľa pravidiel.
 * Pokiaľ rozhranie expression narazí na dva po sebe idúce ***?neterminály?*** predá riadenie späť
 * parseru ktorý zaručí kontrolu či sa jedná o koniec výrazu alebo chybný zápis výrazu s dvomi
 * **?i?** hneď po sebe. 
 */
•parser
    •komunikácia so všetkým
    •zhora dole, rekurzivný zostup?
    •syntaktika i sémantika
    •chyba = koniec
/**
 * Parser je mozgom programu, pretože komunikuje so a riadi všetky ostatné moduly.
 * TODO: opisať funkčnosť parseru
 */
•generátor
    •generuje 3AC??
    •Postfix z expression
    •typy operatorov
    •tlač na konci
/**
 * TODO: opisat funkčnosť gencode
 */
•prostredie
/**
 * Programovali jsme v prostredí Clionu s aktuálnou verziou gcc na linux mint a ubuntu
 * a používali sme verzovací systém git spolu s portálom GitHub. 
 */
•komunikácia
    •Discord
    •Schôdzky v škole - multiple for two and more hours
/**
 * Pre komunikáciu sme používali výhradne iba discord server ktorý funguje na
 * rovnakom princípe ako slack ktorý používajú profesionálne firmy a tech.
 * giganti ako apple či microsoft. Na danom servery sme mali vytvorené komunikačné vlákna
 * kde boli založené TODO listy, bug listy alebo bežná komunikácia či voice chaty s
 * možnosťou zdielať obrazovku vďaka čomu sme mohli riešiť mnoho problémov 
 * digitálne a neohrozovať sa covidákom smrťákom. Avšak aj napriek 
 * prvotriednej digitálne komunikácií sme sa snažili mať čo najviac
 * osobných stretnútí kde sa dá toho vždy vysvetliť najviac.
 */ 
•zhrnutie tímovej práce
    •Som rád že sme začali tak skoro
/**
 * Ihneď pri skladaní tímu sme všetci rozumeli že sa 
 * očakáva pravidelná a skorá práca na projekte čo sa nám nakoniec aj podarilo.
 * Skušobné odovzdanie bolo ohodnotené 75%-ami a slúžilo ako veľmi
 * dobrý benchmark toho, čo treba ešte zlepšiť. Po pokusnom odovzdaní nám teda
 * už neostalo veľa práce, iba vypracovať dokumentáciu, opraviť bugy kedtak dopracovať
 * zabudnutú či zle pochopenú funkčnosť ako napríklad podporu pre nil hodnoty a spisovať testy.
 * Za danú skúsenosť so správnym rozvrhovaním času práce by som sa mojemu tímu rád
 * poďakoval.
 */ 
•implementačné nedostatky
/**
 * Máme nejaké? Jasné že nie *emot so slnečnými okuliarmi*
 */ 
