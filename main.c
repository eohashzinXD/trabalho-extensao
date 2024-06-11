#include "structs.h"

int carregarDados(Dados *a, const char *filename, int *count){
    Dados unico;
    char linha[256], string_data[11], nome[20], string_litros[5];
    char *caractere_nao_convertido;
    char dia[3], mes[3], ano[5];

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Erro ao abrir o arquivo!");
        return 1;
    }

    while (fgets(linha, sizeof(linha), file))
    {
        sscanf(linha, "%s %s", string_data, string_litros);

        sscanf(string_data, "%2[0-9]/%2[0-9]/%4[0-9]", dia, mes, ano);

        //printf("Data: %s/%s/%s\n", dia, mes, ano);

        unico.qtd = strtod(string_litros, &caractere_nao_convertido);
        unico.dia = strtol(dia, &caractere_nao_convertido, 10);
        unico.mes = strtol(mes, &caractere_nao_convertido, 10);
        unico.ano = strtol(ano, &caractere_nao_convertido, 10);
        snprintf(unico.nome, sizeof(unico.nome), "%s", filename);
        unico.nome[20] = '\0';
        unico.nome[2] = '_';

        //printf("Reg: %d/%d/%d %.1lf %s\n", unico.dia, unico.mes, unico.ano,unico.qtd, unico.nome);

        a[*count] = unico;

        (*count)++;
    }
    fclose(file);
    return count;
    
}

void array2Bin(int *count, Dados *a){
    FILE *file = fopen("reg.bin", "ab+");
    if (!file)
    {
        printf("Erro ao criar o arquivo!");
        return -2;
    }

    fwrite(a, sizeof(Dados), count, file);
    
    fclose(file);
}

void binSumCsv() {
    FILE *fileBin = fopen("reg.bin", "rb");
    if (!fileBin) {
        printf("Erro ao ler o arquivo Bin!");
        return;
    }

    FILE *fileCsv = fopen("somatorio.csv", "w");
    if (!fileCsv) {
        printf("Erro ao abrir o arquivo Csv");
        fclose(fileBin);
        return;
    }

    double somaMes[12] = {0};
    Dados temp;

    while (fread(&temp, sizeof(Dados), 1, fileBin) == 1) {
        somaMes[temp.mes - 1] += temp.qtd;
    }

    fprintf(fileCsv, "Mes,Soma de Litros\n");
    for (int i = 0; i < 12; i++) {
        fprintf(fileCsv, "%d,%.1f\n", i + 1, somaMes[i]);
    }

    fclose(fileBin);
    fclose(fileCsv);
}

void binToCsv(Dados *a, int *count){
    FILE *fileBin = fopen("reg.bin", "rb");
    if (!fileBin)
    {
        printf("Erro ao ler o arquivo Bin!");
        return -2;
    }

    FILE *fileCsv = fopen("listagem.csv", "a+");
    if (!fileCsv)
    {
        printf("Erro ao abrir o arquivor Csv");
        return -2;
    }

    fprintf(fileCsv, "Data, Litros, Arquivo\n");


    double qtdTotal = 0.0;
    Dados temp;

    while (fread(&temp, sizeof(Dados), 1, fileBin) == 1)
    {
        fprintf(fileCsv, "%d/%d/%d,%.1f,%s\n", temp.dia, temp.mes, temp.ano, temp.qtd, temp.nome);
    }
    
    fclose(fileBin);
    fclose(fileCsv);
}

char* recupera_nome_arquivo_fisico_txt(int argc, char *argv[]){    
    if (argc != 2) return NULL;
    return argv[1];
}

int main(int argc, char const *argv[])
{
    int count= 0;
    Dados a[100];
    char *nomeArq = recupera_nome_arquivo_fisico_txt(argc, argv);
    int op = 0;
    
    carregarDados(&a, nomeArq, &count);
    array2Bin(count, &a);
    binToCsv(&a, count);
    binSumCsv();

    return 0;
}
