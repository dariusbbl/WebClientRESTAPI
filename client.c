#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <stdbool.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
#include "parson.c"
#define HOST "34.246.184.49"
#define PORT 8080
#define REGISTER "/api/v1/tema/auth/register"
#define TYPECON "application/json"
#define LOGIN "/api/v1/tema/auth/login"
#define ACCESS "/api/v1/tema/library/access"
#define BOOKS "/api/v1/tema/library/books"
#define LOGOUT "/api/v1/tema/auth/logout"
int main()
{
    char buffer[BUFLEN], username[BUFLEN], password[BUFLEN];
    int sockfd;
    char *message, *token, *response;;
    char **cookies;

    while (1)
    {
        // citeste de la tastatura
        memset(buffer, 0, BUFLEN);
        fgets(buffer, BUFLEN, stdin);
        sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

        // daca s-a primit comanda register
        if (strcmp(buffer, "register\n") == 0)
        {
            // citeste username-ul si parola
            printf("username=");
            memset(username, 0, BUFLEN);
            fgets(username, BUFLEN, stdin);
            username[strlen(username) - 1] = '\0';
            
            printf("password=");
            memset(password, 0, BUFLEN);
            fgets(password, BUFLEN, stdin);
            password[strlen(password) - 1] = '\0';

            int i = 0, ok = 0;

            // Verificare pentru username și password sa nu contina spatiu
            while (username[i] != '\0' || password[i] != '\0') {
                if ((i < strlen(username) && username[i] == ' ' ) || (i < strlen(password) && password[i] == ' ')) {
                    ok = 1;
                    break;
                }
                i++;
            }
            if (ok) {
                printf("Eroare! Username sau parola contine spatiu\n");
                continue; 
            }

            // creeaza un obiect JSON cu username-ul si parola
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
  
            // serializeaza obiectul JSON
            char *serialized_string = NULL;
            serialized_string = json_serialize_to_string_pretty(root_value);
            message = compute_post_request(HOST, REGISTER, TYPECON, serialized_string, NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            char *JSONrasp = basic_extract_json_response(response);
            // afiseaza mesajul de eroare sau mesajul de succes
            if (JSONrasp != NULL)
            {
                JSON_Value *root_value = json_parse_string(JSONrasp);
                JSON_Object *root_object = json_value_get_object(root_value);
                const char *error_message = json_object_get_string(root_object, "error");
                printf("Eroare! %s\n", error_message);
                json_value_free(root_value);
            }
            else
            {
                printf("200 - OK - Utilizator inregistrat cu succes!\n");
            }

            json_free_serialized_string(serialized_string);
            json_value_free(root_value);
        }
        // daca s-a primit comanda login
        else if (strcmp(buffer, "login\n") == 0)
        {
            char *serialized_string = NULL;
            const char *error_message = NULL;
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            // verifica daca utilizatorul este deja autentificat
            if (cookies != NULL)
                printf("Esti deja autentificat!\n");
            
            // citeste username-ul si parola
            printf("username=");
            memset(username, 0, BUFLEN);
            fgets(username, BUFLEN, stdin);
            username[strlen(username) - 1] = '\0';
            
            printf("password=");
            memset(password, 0, BUFLEN);
            fgets(password, BUFLEN, stdin);
            password[strlen(password) - 1] = '\0';
            
            int i = 0, ok = 0;

            // Verificare pentru username și password sa nu contina spatiu
            while (username[i] != '\0' || password[i] != '\0') {
                if ((i < strlen(username) && username[i] == ' ' ) || (i < strlen(password) && password[i] == ' ')) {
                    ok = 1;
                    break;
                }
                i++;
            }
            if (ok) {
                printf("Eroare! Username sau parola contine spatiu\n");
                continue; 
            }
            // creeaza un obiect JSON cu username-ul si parola
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            // serializeaza obiectul JSON, trimite mesajul la server si primeste raspunsul
            serialized_string = json_serialize_to_string_pretty(root_value);
            message = compute_post_request(HOST, LOGIN, TYPECON, serialized_string, NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            char *JSONrasp = basic_extract_json_response(response);

             if (JSONrasp != NULL)  // daca s-a primit un raspuns de la server
            {
                JSON_Value *error_root_value = json_parse_string(JSONrasp);
                JSON_Object *error_root_object = json_value_get_object(error_root_value);
                error_message = json_object_get_string(error_root_object, "error");
                json_value_free(error_root_value);
            }
            // afiseaza mesajul de eroare sau mesajul de succes
            if (JSONrasp == NULL) {
                fprintf(stdout, "Succes! Utilizatorul a fost logat cu succes\n");
                char *aux = strstr(response, "Set-Cookie:");
                if (aux != NULL) {
                    char *start = aux + strlen("Set-Cookie:");
                    char *end = strchr(start, ';');
    
                    if (end == NULL) {
                        end = start + strlen(start); 
                    }

                    size_t cookie_length = end - start;
                    cookies = (char **)malloc(sizeof(char *));
                    cookies[0] = (char *)malloc((cookie_length + 1) * sizeof(char));
                    strncpy(cookies[0], start, cookie_length);
                    cookies[0][cookie_length] = '\0';
                }
            }
            if (error_message != NULL)
            {
                printf("Eroare! %s\n", error_message);
            }
            json_free_serialized_string(serialized_string);
            json_value_free(root_value);
        }
        // daca s-a primit comanda enter_library
        else if (strcmp(buffer, "enter_library\n") == 0) {
            message = compute_get_request(HOST, ACCESS, NULL, cookies, 1, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            char *JSONrasp = basic_extract_json_response(response);

            if (JSONrasp != NULL) {
                JSON_Value *root_value = json_parse_string(JSONrasp);
                if (root_value != NULL) { // daca s-a primit un raspuns de la server
                    JSON_Object *root_object = json_value_get_object(root_value); // extrage obiectul JSON
                    const char *error_message = json_object_get_string(root_object, "error"); // extrage mesajul de eroare

                    if (error_message != NULL) {
                        printf("Eroare! %s\n", error_message);
                    } else {
                        const char *token_value = json_object_get_string(root_object, "token"); // extrage token-ul
                        if (token_value != NULL) {
                            size_t token_length = strlen(token_value); // calculeaza lungimea token-ului
                            token = malloc((token_length + 1) * sizeof(char)); // aloca memorie pentru token
                            if (token != NULL) {
                                strncpy(token, token_value, token_length);
                                token[token_length] = '\0';
                                printf("Succes! Utilizatorul are acces la biblioteca\n");
                            } else {
                                printf("Nu ai acces la biblioteca!\n");
                            }
                        } 
                    }

                    json_value_free(root_value);
                }
            }
        }
        // daca s-a primit comanda get_books
        else if (strcmp(buffer, "get_books\n") == 0)
        {
            // trimite mesajul la server
            message = compute_get_request(HOST, BOOKS, TYPECON, cookies, 1, token);
            send_to_server(sockfd, message);
            // primeste raspunsul de la server
            response = receive_from_server(sockfd);
            // daca raspunsul contine "[]", inseamna ca nu exista carti in biblioteca
            bool noBooks = (strstr(response, "[]") != NULL);
            if (noBooks)
            {
                printf("Nu exista carti in biblioteca!\n");
            }
            // daca avem carti in biblioteca
            if(!noBooks)
            {
                char *JSONrasp = basic_extract_json_response(response);
                char *error = strstr(response, "error");
                if (error == NULL)
                {
                    JSON_Value *root_value = json_parse_string(JSONrasp - 1);
                    JSON_Array *books_array = json_value_get_array(root_value);
                    int books_count = json_array_get_count(books_array);

                    if (books_count == 0)
                    {
                        printf("Nu exista carti in biblioteca!\n");
                    }
                    else
                    {
                        // afiseaza cartile din biblioteca
                        for (int i = 0; i < books_count; i++)
                        {
                            JSON_Object *book_object = json_array_get_object(books_array, i);
                            int id = json_object_get_number(book_object, "id");
                            const char *title = json_object_get_string(book_object, "title");
                            printf("id: %d, title: %s\n", id, title);
                        }
                    }

                    json_value_free(root_value);
                }
                if (error != NULL)
                {
                    JSON_Value *root_value = json_parse_string(JSONrasp);
                    JSON_Object *root_object = json_value_get_object(root_value);
                    const char *error_message = json_object_get_string(root_object, "error");
                    printf("Eroare! %s\n", error_message);
                    json_value_free(root_value);
                }
            }
        }
        // daca s-a primit comanda get_book
        else if (strcmp(buffer, "get_book\n") == 0)
        {
            int book_id;

            printf("id=");
            if (scanf("%d", &book_id) != 1 || book_id < 0)
            {
                printf("Cartea cu id=%d nu exista!\n", book_id);
                while (getchar() != '\n'); 
                continue; 
            }
            // creeaza calea catre cartea cu id-ul book_id
            char book_path[BUFLEN];
            snprintf(book_path, sizeof(book_path), "%s/%d", BOOKS, book_id);
            // trimite mesajul la server
            message = compute_get_request(HOST, book_path, TYPECON, cookies, 1, token);
            send_to_server(sockfd, message);
            // primeste raspunsul de la server
            response = receive_from_server(sockfd);
            printf("%s\n", response);
        }
        // daca s-a primit comanda add_book
        else if (strcmp(buffer, "add_book\n") == 0)
        {
            // declara variabilele pentru a citi datele cartii
            char title[BUFLEN], author[BUFLEN], genre[BUFLEN], publisher[BUFLEN], page_count[BUFLEN];
            // citeste datele cartii
            printf("title=");
            memset(title, 0, BUFLEN);
            fgets(title, BUFLEN, stdin);
            title[strlen(title) - 1] = '\0';

            printf("author=");
            memset(author, 0, BUFLEN);
            fgets(author, BUFLEN, stdin);
            author[strlen(author) - 1] = '\0';

            printf("genre=");
            memset(genre, 0, BUFLEN);
            fgets(genre, BUFLEN, stdin);
            genre[strlen(genre) - 1] = '\0';

            printf("publisher=");
            memset(publisher, 0, BUFLEN);
            fgets(publisher, BUFLEN, stdin);
            publisher[strlen(publisher) - 1] = '\0';

            printf("page_count=");
            memset(page_count, 0, BUFLEN);
            fgets(page_count, BUFLEN, stdin);
            page_count[strcspn(page_count, "\n")] = '\0';

            //daca vreunul dintre campuri este gol, nu mai creez cartea
            if (strlen(title) == 0 || strlen(author) == 0 || strlen(genre) == 0 || strlen(publisher) == 0 || strlen(page_count) == 0)
            {
                printf("Eroare! Campuri invalide!\n");
                continue;
            }
            
            // verificare daca numarul de pagini este un numar
            int count_page;
            int i = 0, ok = 1;
            count_page = atoi(page_count);

            while (page_count[i] != '\0')
            {
                if (!isdigit(page_count[i]))
                {
                    printf("Eroare! Tip de date incorect pentru numarul de pagini\n");
                    ok = 0;
                    break; 
                }
                i++;
            }
            if (ok == 0)
            {
                continue;
            }

            // creeaza un obiect JSON cu datele cartii
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            json_object_set_string(root_object, "title", title);
            json_object_set_string(root_object, "author", author);
            json_object_set_string(root_object, "genre", genre);
            json_object_set_string(root_object, "publisher", publisher);
            json_object_set_number(root_object, "page_count", count_page);

            // serializeaza obiectul JSON
            char *serialized_string = NULL;
            serialized_string = json_serialize_to_string_pretty(root_value);
            message = compute_post_request(HOST, BOOKS, TYPECON, serialized_string, cookies, 1, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            char *JSONrasp = basic_extract_json_response(response);

            if (JSONrasp != NULL)
            {
                JSON_Value *root_value = json_parse_string(JSONrasp);
                if (root_value != NULL)
                {
                    const char *error_message = json_object_get_string(json_value_get_object(root_value), "error");
                    if (error_message != NULL)
                    {
                        printf("%s\n", error_message); 
                    }
                    json_value_free(root_value); 
                }
            }

            if (strstr(response, "error") != NULL)
            {
                printf("Nu ai acces la biblioteca!\n");
            }
            if (strstr(response, "429") == NULL)
            {
                printf("Succes! Cartea a fost adaugata cu succes!\n");
            }
            else
            {
                printf("Eroare! Cartea nu a fost adaugata!\n");
            }
            json_free_serialized_string(serialized_string);
        }
        // daca s-a primit comanda delete_book
        else if (strcmp(buffer, "delete_book\n") == 0)
        {
            char id[BUFLEN];
            printf("id=");
            // citeste id-ul cartii
            if (fgets(id, BUFLEN, stdin) == NULL)
            {
                printf("Eroare la citirea ID-ului cartii.\n");
                continue;
            }
            id[strcspn(id, "\n")] = '\0';
            int ok = 0;
            // verifica daca id-ul este un numar
            for (int i = 0; id[i] != '\0'; i++)
            {
                if (!isdigit(id[i]))
                {
                    printf("Eroare! Introdu un id valid!\n");
                    ok = 1;
                    break;
                }
                i++;
            }
            if (ok)
            {
                continue;
            }
            // transforma id-ul in numar
            int nr_id = atoi(id);
            char urlbook[BUFLEN];
            snprintf(urlbook, sizeof(urlbook), "%s/%d", BOOKS, nr_id);
            // trimite mesajul la server
            message = compute_delete_request(HOST, urlbook, TYPECON, cookies, 1, token);
            send_to_server(sockfd, message);
            // primeste raspunsul de la server
            response = receive_from_server(sockfd);
            char *JSONrasp = basic_extract_json_response(response);

            if (JSONrasp != NULL)
            {
                if (strstr(response, "error") != NULL)
                {
                    JSON_Value *root_value = json_parse_string(JSONrasp);
                    if (root_value != NULL)
                    {
                        JSON_Object *root_object = json_value_get_object(root_value);
                        if (root_object != NULL)
                        {
                            const char *error_message = json_object_get_string(root_object, "error");
                            if (error_message != NULL)
                            {
                                printf("Eroare! %s\n", error_message);
                            }
                        }
                        json_value_free(root_value); 
                    }
                }
            }
             if (JSONrasp == NULL)
            {
                printf("Succes! Cartea cu id %d a fost stearsa cu succes\n", nr_id);
            }
        }
        // daca s-a primit comanda logout
        else if (strcmp(buffer, "logout\n") == 0)
        {
            message = compute_get_request(HOST, LOGOUT, TYPECON, cookies, 1, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            char *JSONrasp = basic_extract_json_response(response);
    
            if (JSONrasp == NULL)
            {
                printf("Succes! Utilizatorul s-a delogat cu succes!\n");
                free(cookies[0]);
                free(cookies);
                free(token);
                token = NULL;
                cookies = NULL;
            }
            else
            {
                JSON_Value *root_value = json_parse_string(JSONrasp);
                if (root_value != NULL)
                {
                    JSON_Object *root_object = json_value_get_object(root_value);
                    const char *error_message = json_object_get_string(root_object, "error");
                    if (error_message != NULL)
                    {
                        printf("Eroare! %s\n", error_message);
                    }
                    json_value_free(root_value);
                }
            }
        }
        // daca s-a primit comanda exit
        else if (strcmp(buffer, "exit\n") == 0)
        {
            // elibereaza memoria alocata
            if (token != NULL)
            {
                free(token);
                token = NULL;
            }
            if (cookies != NULL)
            {
                free(cookies[0]);
                free(cookies);
                cookies = NULL;
            }
            break;
        }
        // daca s-a primit o comanda invalida
        else
        {
            printf("Eroare! Comanda invalida!\n");
        }
        // se inchide conexiunea
        close_connection(sockfd);
    }
    // se inchide conexiunea
    close_connection(sockfd);
    return 0;
}