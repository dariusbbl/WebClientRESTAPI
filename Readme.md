Pentru realizarea temei, m-am folosit de fisierele laboratului 9 ce erau deja implementate (mare parte din ele, restul au fost rezolvate la laborator) si parsarea JSON din link-urile temei. 
Implementarea propriu-zisa este facuta in fisierul client.c :
Parsarea si folosirea datelor JSON sunt efectuate folosind biblioteca 'parson', pentru o mai facila creare, serializare si deserializare a obiectelor. Pentru crearea si initializarea acestora am folosit 'json_value_init_object', pentru adaugarea datelor 'json_object_set..' pentru text si numere, am transformat obiectele JSON in string folosinf 'json_serialize_to_string_pretty' pentru a-l trimite ulterior catre server, am extras datele in obiectele JSON create prin 'json_parse_string' si am eliberat reursele dupa procesarea datelor cu 'json_value_free'.
Tema reprezinta o interfata de gestionare a utilizatorilor si cartilor prin serviciul web de la adresa "34.246.184.49", avand portul "8080", unde utilizatorii pot efectua diverse comenzi:
    -> register: este mai intai solicitat utilizatorului sa isi creeze un username si o parola ce sunt date de la tastatura folosind 'fgets', ce ulterior sunt verificate daca acestea contin spatii (lucru interzis in cerinta temei; contul va fi inregistrat doar in cazul in care aceasta cerinta este respectata, altfel se afiseaza mesaj de eroare si se reia procesul). Ulterior, se creeaza obiectele JSON , iar prin functia definita in "requests.c", 'compute_post_request', construiesc cererea catre server pe care o trimit prin 'send_to_server'. Urmeaza pocesarea raspunsului primit de la server ('recieve_from_server') ce verifica daca exista mesaje de eroare si se afiseaza rezultatul.

    -> login: similar cu register, se introduc username-ul si parola, programul verificand si daca utilizatorul nu este deja autentificat (daca exista cookie-uri salvate). Se creeaza obiectele JSON de username si parola si se trimite cererea de login catre server si se proceseaza raspunsul pentru a extrage cookie-urile si a afisa rezultatul autentificarii.

    -> enter_library:  se trimite o cerere catre server pentru a accesa biblioteca. Daca accesul este permis, raspunsul de la server contine include un token de acces care este salvat pentru a fi folosit în cererile ulterioare.

    -> get_books: se trimite o cerere pentru a obtine lista de carti, incluzand cookie-ul si token-ul. Se verifica daca lista de carti este goala si se afiseaza mesajul corespunzator. Altfel, se afiseaza fiecare carte cu detaliile aferente.

    -> get_book: se trimite o cerere de vizualizare a unei anumite carti, iar utilziatorului ii este solicitata introducerea id-ului acesteia. Se verifica daca numarul introdus este valid si programul construieste URL-ul pentru cererea catre server, atasand id-ul cartii la 'BOOKS'. Se foloseste functia 'compute_get_request' pentru a creea cererea corespunzatoare URL-ului specific cartii ce este transmisa catre server si procesat raspunsul. Daca nu s-a intalnit mesaj de eroare, se deserializeaza si se afiseaza informatiile despre carte.

    -> add_book: se solicita informatii despre noua carte pe care dorim sa o introducem: titlu, autor, gen, editura si numar de pagini, ce vor fi ulterior procesate in obiect de tip JSON. Se verifica daca datele introduse sunt valide si se afiseaza rezultatul de succes/eroare al comenzii.

    -> delete_book: se cere utilizatorului sa introduca id-ul cartii pe care doreste sa o elimine si in caz ca acesta este valid, se trimite o cerere catre server si se proceseaza raspunsul + se afiseaza statusul comenzii.

    -> logout: se trimite o cerere catre server pentru a incheia sesiunea, iar cookie-urile si token-ul sunt eliberate si se afiseaza confirmare.

    -> exit: se termina de executat programul