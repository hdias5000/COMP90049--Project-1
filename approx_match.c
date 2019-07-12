/// Project 1 - Lexical Normalisation for Twitter Data
/// Hasitha Dias - 789929
/// This file contains the implementations for approximate string matching
/// methods.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

struct LinkedList{
    char* word;
    char* soundex;
    struct LinkedList *next;
 };

typedef struct LinkedList *node; //Define node as pointer of data type struct LinkedList

struct StrList{
    int score;
    char* value;
    struct StrList *next;
 };

typedef struct StrList *matchlist;

// Source: "https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C"
int levenshtein(char *s1, char *s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int column[s1len+1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return(column[s1len]);
}

char* removezeros(char *trunc){
    char* code;
    code = malloc(sizeof(char)*(strlen(trunc)+1));
    strcpy(code,trunc);
    int i,j;
    for (i = 0; code[i]!='\0'; i++) {
        while (code[i]=='0') {
            for (j = i; code[j]!='\0'; j++) {
                code[j] = code[j+1];
            }
            code[j] = '\0';
        }
    }
    return code;
}

// Source: "https://github.com/eokeeffe/C-code/blob/master/C-FAQ/soundex.c"
void soundex(char* name,char* sound){
  char   * in = name,
         * out = sound;
  char   prev = '\0';
  int    is_first = 1;
  char   code;

  while (*in != '\0' && out - sound < 4)
  {
    switch(tolower(*in))
    {
      case 'b':
      case 'f':
      case 'p':
      case 'v':
        code = '1';
      break;

      case 'c':
      case 'g':
      case 'j':
      case 'k':
      case 'q':
      case 's':
      case 'x':
      case 'z':
        code = '2';
      break;

      case 'd':
      case 't':
        code = '3';
      break;

      case 'l':
        code = '4';
      break;

      case 'm':
      case 'n':
        code = '5';
      break;

      case 'r':
        code = '6';
      break;

      /* includes "aehiouwy", punctuation, numbers and control chars */
      default:
        code = 0;
      break;
    }

    if (is_first)
    {
      *out++ = toupper(*in);
      is_first = 0;
    }
    else if (code && prev != code)
      *out++ = code;
    prev = code;

    in++;
  }

  while (out - sound < 4){
    *out++='0';
}

  *out = '\0';
}

//creates dictionary nodes
node createNode(){
    node temp; // declare a node
    temp = (node)malloc(sizeof(struct LinkedList)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new node
}

//adds dictionary nodes
node addNode(node head, char* value){
    node temp,p;// declare two nodes temp and p
    temp = createNode();//createNode will return a new node with data = value and next pointing to NULL.
    temp->word = malloc(sizeof(char)*(strlen(value)+1));

    char sound[10];
    soundex(value,sound);
    char* soundcode = removezeros(sound);
    strcpy(temp->word,value);
    temp->soundex = malloc(sizeof(char)*(strlen(soundcode)+1));

    strcpy(temp->soundex,soundcode);
    free(soundcode);

    if(head == NULL){
        head = temp;     //when linked list is empty
    }
    else{
        p  = head;//assign head to p
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }

    return head;
}

//creates matches nodes
matchlist createNodeM(){
    matchlist temp; // declare a node
    temp = (matchlist)malloc(sizeof(struct StrList)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new node
}

//adds matches nodes
matchlist addNodeM(matchlist head,int score, char* value){
    matchlist temp,p;// declare two nodes temp and p
    temp = createNodeM();//createNode will return a new node with data = value and next pointing to NULL.
    temp->value = malloc(sizeof(char)*(strlen(value)+1));
    strcpy(temp->value,value);
    temp->score = score;

    if(head == NULL){
        head = temp;     //when linked list is empty
    }
    else{
        p  = head;//assign head to p
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }

    return head;
}



int main(int argc, char const *argv[]) {
    clock_t t;
    t = clock();

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("./dict.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    node dictionary = NULL;
    node current;
    while((read = getline(&line, &len, fp)) != -1){
        line[strlen(line)-2] = '\0';
        dictionary = addNode(dictionary,line);
    }

    fclose(fp);

    FILE *f = fopen("./file.json", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    FILE *misspell;
    char * word = NULL;
    len = 0;
    misspell = fopen("./misspell.txt", "r");
    if (misspell == NULL){
        printf("error\n" );
        exit(EXIT_FAILURE);}
    int count = 0;
    fprintf(f, "[");

    //each token
    while(getline(&word, &len, misspell) != -1){
        word[strlen(word)-2] = '\0';
        matchlist matches = NULL; // declare a node
        matchlist soundex_matches = NULL;

        int lowest = INT_MAX;
        int soundex_lowest = INT_MAX;
        node be = dictionary;
        //possible matches
        while (be!=NULL) {
            char soundcode[10];
            soundex(word,soundcode);
            char* sound = removezeros(soundcode);

            int distance = levenshtein(word,be->word);
            int sound_dist = levenshtein(sound,be->soundex);
            free(sound);
            //finds edit distance matches
            if (distance==lowest) {
                matches = addNodeM(matches,distance,be->word);

            }else if (distance<lowest) {
                lowest = distance;
                matches = createNodeM(); // allocate memory using malloc()
                matches->score = distance;
                matches->value = malloc(sizeof(char)*(strlen(be->word)+1));
                strcpy(matches->value,be->word);
            }
            //finds soundex matches
            if (sound_dist==soundex_lowest) {
                soundex_matches = addNodeM(soundex_matches,sound_dist,be->word);

            }else if (sound_dist<soundex_lowest) {
                soundex_lowest = sound_dist;
                soundex_matches = createNodeM(); // allocate memory using malloc()
                soundex_matches->score = sound_dist;
                soundex_matches->value = malloc(sizeof(char)*(strlen(be->word)+1));
                strcpy(soundex_matches->value,be->word);
            }
            node previ = be;
            be = be->next;
        }

        if (count!=0) {
            fprintf(f, ",\n");
        }

        //prints edit distance matches
        fprintf(f, "{\"%s\":{\"edit\":[",word );
        matchlist currentmatch = matches;
        matchlist previous;
        while (currentmatch!=NULL){
            if (currentmatch->next!=NULL) {
                fprintf(f, "{\"score\":%d,\"match\":\"%s\"},",currentmatch->score,currentmatch->value);
            }else {
                fprintf(f, "{\"score\":%d,\"match\":\"%s\"}",currentmatch->score,currentmatch->value);
            }
            previous = currentmatch;
            currentmatch = currentmatch->next;

            free(previous->value);
            free(previous);
        }
        fprintf(f, "]");

        //prints soundex matches
        fprintf(f, ",\"soundex\":[");
        matchlist soundexmatch = soundex_matches;
        matchlist prev;
        while (soundexmatch!=NULL){
            if (soundexmatch->next!=NULL) {
                fprintf(f, "{\"score\":%d,\"match\":\"%s\"},",soundexmatch->score,soundexmatch->value);
            }else {
                fprintf(f, "{\"score\":%d,\"match\":\"%s\"}",soundexmatch->score,soundexmatch->value);
            }
            prev = soundexmatch;
            soundexmatch = soundexmatch->next;

            free(prev->value);
            free(prev);

        }
        fprintf(f, "]");


        fprintf(f, "}}");
        count++;
    }
    fprintf(f, "]" );
    fclose(f);

    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds

    printf("took %f seconds to execute \n", time_taken);
    return 0;
}
