#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "resourcestack.h"

#define MAX_KEY_SIZE 100

struct bufferedStream{
    FILE* file;
    int bufferSize;
    char *buffer;
    int bytesAvailable;
};
struct keyPair{
    char key[MAX_KEY_SIZE];
    char replaceString[MAX_KEY_SIZE];
};

void closeFile(void* f){//JUST to make function pointer compatible with resourse stack
    fclose((FILE *)f);
}
void fillBuffer(struct bufferedStream*);
int bufferContains(struct bufferedStream*, char*);
char getNextByte(struct bufferedStream*);

int main(){
    INTIALIZE_RESOURCE_MANAGER;

    //Getting keys
    FILE* keyFile = fopen("keys.txt", "r");
    PUSH_RESOURCE_MESSAGE(keyFile, closeFile, "failed to open keys file");

    int keyCount;
    fscanf(keyFile, "%d\n", &keyCount);

    struct keyPair *keyPairArray = malloc(sizeof(struct keyPair) * keyCount);
    PUSH_RESOURCE_MESSAGE(keyPairArray, free, "failed to initialize keyPairArray");

    for (int i = 0; i< keyCount; i++){
        struct keyPair *currentKeyPair = keyPairArray + i;
        int skipReplace = 0;
        char c;
        c = getc(keyFile);
        while(c=='\n' || c==' ' || c=='\r' || c=='\t'){
            c = getc(keyFile);
        }
        for (int j = 0;; c = getc(keyFile)){
            if (j == MAX_RESOURCE_COUNT){
                R_EXIT_FAILURE_MESSAGE("key too long");
            }
            if (c== ' '){
                currentKeyPair->key[j] = '\0';
                break;
            }else if (c=='\r' || c=='\n' || c == EOF){
                currentKeyPair->key[j] = '\0';
                currentKeyPair->replaceString[0] = '\0';
                skipReplace = 1;
                break;
            }else{
                currentKeyPair->key[j] = c;
                j++;
            }
            
        }
        if (skipReplace) continue;
        c = getc(keyFile);
        while(c=='\n' || c==' ' || c=='\r' || c=='\t'){
            c = getc(keyFile);
        }
        for(int j = 0; ; c= getc(keyFile)){
            if (j == MAX_RESOURCE_COUNT){
                R_EXIT_FAILURE_MESSAGE("replace string too long");
            }
            if (c=='\r' || c=='\n' || c == EOF){
                currentKeyPair->replaceString[j] = '\0';
                break;
            }else{
                currentKeyPair->replaceString[j] = c;
                j ++;
            }
        }
    }
    FREE_RESOURCE(keyFile);

    for (int i = 0; i<keyCount; i++){
        printf("'%s': '%s'\n", keyPairArray[i].key, keyPairArray[i].replaceString);
    }




    //Performing replacement
    struct bufferedStream* bs = malloc(sizeof(struct bufferedStream));
    PUSH_RESOURCE_MESSAGE(bs, free, "unable to allocate byteStream");
    
    bs-> file = fopen("input", "r");
    PUSH_RESOURCE_MESSAGE(bs->file, closeFile, "unable to open input file");

    bs-> bufferSize = MAX_KEY_SIZE + 1;
    bs-> buffer = malloc(sizeof(char) * bs->bufferSize);
    PUSH_RESOURCE_MESSAGE(bs->buffer, free, "unable to allocate bytestream buffer")

    bs->bytesAvailable = 0;
    

    

    fillBuffer(bs);
    FILE *output = fopen("output", "w");
    PUSH_RESOURCE_MESSAGE(output, closeFile, "failed to open output file");
    while(bs->bytesAvailable){
        int replaceMade = 0;
        for (int i = 0; i<keyCount; i++){
            if (bufferContains(bs, keyPairArray[i].key)){
                fprintf(output, keyPairArray[i].replaceString);
                replaceMade = 1;
                break;
            }
        }
        if (replaceMade) continue;
        char c = getNextByte(bs);
        fprintf(output, "%c", c);
    }

    R_EXIT_SUCCESS
}

void fillBuffer(struct bufferedStream* bs){
    while(!feof(bs->file) && (bs->bytesAvailable < bs->bufferSize)){
        char c = getc(bs->file);
        if (c == EOF) break;
        bs->buffer[bs->bytesAvailable] = c;
        bs->bytesAvailable++;
    }
}

int bufferContains(struct bufferedStream* bs, char* str){
    int i = 0;
    while(1){
        if (str[i] == '\0') break;
        if (bs->bytesAvailable <= i) return 0;
        if (bs->buffer[i] != str[i]) return 0;
        i++;
    }
    for (int j = i; j < bs->bytesAvailable; j++){
        bs->buffer[j-i] = bs->buffer[j];
    }
    bs->bytesAvailable -= i;
    fillBuffer(bs);
    return 1;
}

char getNextByte(struct bufferedStream*bs){
    char c = bs->buffer[0];
    for (int i = 0; i<bs->bytesAvailable-1; i++){
        bs->buffer[i] = bs->buffer[i+1];
    }
    bs->bytesAvailable--;
    fillBuffer(bs);
    return c;
}