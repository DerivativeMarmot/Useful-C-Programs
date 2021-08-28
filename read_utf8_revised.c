/*
Author: DerivativeMarmot
CreateDate: 2021/8/26 20:37
LastModifiedDate: 2021/8/27 19:38
Version: 1.0.0
Desc: read files coded with utf-8
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <math.h>

char *intDec2strBin(int intDec)
{
    char *strbin = (char *)malloc(sizeof(char) * 9);
    *(strbin + 8) = '\0';
    char *sbp = strbin + 7;
    while (intDec > 0)
    {
        *sbp = intDec % 2 + 48;
        intDec /= 2;
        sbp--;
    }
    while (sbp >= strbin)
    {
        *sbp-- = '0';
    }
    return strbin;
}

int strBin2intDec(char *strbin)
{
    char *sbp = strbin;
    int power = strlen(strbin) - 1;
    int dec = 0;
    while (*sbp != '\0')
    {
        if (*sbp - 48)
        {
            dec += (1 << power);
        }
        sbp++;
        power--;
    }
    return dec;
}

int *utf8ToUnicode(int *hbp, int *hbp_end)
{
    
    int bytech_COUNTER = 1;
    int *bytech = (int *)malloc(sizeof(int));
    while (hbp < hbp_end)
    {
        char *strbinStart = intDec2strBin(*hbp++);
        char *sbSp = strbinStart;
        int BYTE_COUNTER = 0;
        while (*sbSp == '1')
        {
            BYTE_COUNTER++;
            sbSp++;
        }
        while (*sbSp == '0')
            sbSp++; // clear '0' in the first byte
        if (BYTE_COUNTER == 0)
            BYTE_COUNTER = 1;

        char utf8group[BYTE_COUNTER][9]; // 9 refers to 8 bits + '\0'
        strcpy(utf8group[0], strbinStart);
        for (int i = 1; i < BYTE_COUNTER; i++)
        {
            char *strbinCur = intDec2strBin(*hbp++);
            strcpy(utf8group[i], strbinCur);
            free(strbinCur); // sometimes crashes the program. unkown reason
        }

        char *struni = (char *)malloc(sizeof(char) * BYTE_COUNTER * 8);
        strcpy(struni, sbSp);
        for (int i = 1; i < BYTE_COUNTER; i++)
        {
            strcat(struni, *(utf8group + i) + 2);
        }
        int len_struni = strlen(struni);
        struni = realloc(struni, sizeof(char) * (len_struni + 1));
        *(struni + len_struni) = '\0';

        bytech_COUNTER++;
        bytech = realloc(bytech, sizeof(int) * bytech_COUNTER);
        bytech[bytech_COUNTER - 1] = strBin2intDec(struni);

        free(strbinStart); // sometimes crashes the program. unkown reason
        free(struni); // sometimes crashes the program. unkown reason
    }
    bytech[0] = bytech_COUNTER;
    return bytech;
}
/****************************************************/
void read3()
{
    setlocale(LC_ALL, ""); //不知道为什么要这个，但是没有的话就用不了
    FILE *fp = NULL;
    if ((fp = fopen("delivery.txt", "r")) == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0, SEEK_END);
    int filesize = ftell(fp);
    if (filesize == 0)
    {
        perror("No Content in file");
        exit(EXIT_FAILURE);
    }
    rewind(fp);

    char *buffer = malloc(filesize);
    int *hex_buffer = malloc(filesize);
    fread(buffer, filesize, 1, fp);
    char *bp = buffer;
    int *hbp = hex_buffer;
    char *bp_end = bp + filesize;
    int *hbp_end = hbp + filesize;

    while (bp < bp_end)
    {
        *hbp = 0xff & *bp;
        //printf("%d ", *hbp);
        bp++, hbp++;
    }

    hbp = hex_buffer;
    int *bytech = utf8ToUnicode(hbp, hbp_end);
    wchar_t *byte_arr = malloc((bytech[0]) * sizeof(wchar_t));
    for (int i = 1; i < bytech[0]; i++)
    {
        //printf("byte= %d\n", bytech[i]);
        byte_arr[i - 1] = bytech[i];
    }
    byte_arr[bytech[0] - 1] = 0;
    wprintf(L"%ls\n", byte_arr);

    free(bytech); // sometimes crashes the program. unkown reason
    free(byte_arr); // sometimes crashes the program. unkown reason

    //fclose(fp) //call fclose() 程序会卡一下然后强制退出。原因不明
}

int main(void)
{
    read3();
    return 0;
}