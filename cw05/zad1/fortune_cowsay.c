#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 2048
char fortune_buff[BUFF_SIZE] = "";

int main(int argc, char** argv) {
    FILE* fortune_output = popen("fortune", "r");
    size_t size = fread(fortune_buff, sizeof(char), BUFF_SIZE, fortune_output);
    fortune_buff[size] = 0;
    pclose(fortune_output);

    FILE* cowsay_input = popen("cowsay", "w");
    fwrite(fortune_buff, sizeof(char), strlen(fortune_buff), cowsay_input);
    pclose(cowsay_input);
}
