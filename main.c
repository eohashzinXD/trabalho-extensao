#include "structs.h"


int carregarDados(Dados *a, const char *filename, int *count){
    Dados unico;
    char linha[256], string_data[11], nome[20], string_litros[5];
    char *caractere_nao_convertido;
    char dia[3], mes[3], ano[5];

    //printf("%s %d\n",filename, *count);

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

    fwrite(a, sizeof(Dados), *count, file);
    
    fclose(file);
    printf("Lote inserido!");
}

void binSumCsv() {
	int i;
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
    for (i = 0; i < 12; i++) {
        fprintf(fileCsv, "%d,%.1f\n", i + 1, somaMes[i]);
    }

    fclose(fileBin);
    fclose(fileCsv);

    printf("Sumatorio feito com Sucesso!");
}

void binToCsv(Dados *a, int *count){
    FILE *fileBin = fopen("reg.bin", "rb");
    if (!fileBin)
    {
        printf("Erro ao ler o arquivo Bin!");
        return -2;
    }

    FILE *fileCsv = fopen("listagem.csv", "w");
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

    printf("Listagem feito com Sucesso!");
}

char* recupera_nome_arquivo_fisico_txt(int argc, char *argv[]){    
    //printf("%d %s %s\n",argc, argv[0], argv[1]);
    if (argc != 2) return NULL;
    return argv[1];
}

int excluirLote(const char *nome) {
    FILE *fileBin = fopen("reg.bin", "rb");
    if (!fileBin) {
        printf("Erro ao abrir o arquivo Bin!\n");
        return -1;
    }

    FILE *tempFile = fopen("temp.bin", "wb");
    if (!tempFile) {
        printf("Erro ao abrir o arquivo temporário!\n");
        fclose(fileBin);
        return -1;
    }
    Dados temp;
    int achou = 0;

    while (fread(&temp, sizeof(Dados), 1, fileBin) == 1) {
        if (strstr(temp.nome, nome) == NULL) {
            fwrite(&temp, sizeof(Dados), 1, tempFile);
        } else {
            achou = 1;
        }
    }
    fclose(fileBin);
    fclose(tempFile);

    if (achou == 1) {
        remove("reg.bin"); // Remove o arquivo original
        rename("temp.bin", "reg.bin"); // Renomeia o arquivo temporário para o nome original
        printf("Lote excluído com sucesso!\n");
    } else {
        remove("temp.bin"); // Remove o arquivo temporário, pois o dado não foi encontrado
        printf("Lote não encontrado!\n");
        return -1; // Dado não encontrado
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    setlocale(LC_ALL, "Portuguese");
    int count= 0;
    Dados a[100];
    printf("Arquivo selecionado: %s\n",argv[1]);
    char *nomeArq = recupera_nome_arquivo_fisico_txt(argc, argv);
    int op = 0;
    carregarDados(&a, nomeArq, &count);

    do
    {
        char nome[200];
        printf("\n");
        printf(" 1 - Inserir lote  \n");
        printf(" 2 - Eliminar lote \n");
        printf(" 3 - Somatorio mensal (csv) \n");
        printf(" 4 - Listagem (csv) \n");
        printf(" 5 - Encerrar  \n");  
        printf("\n");
        op=input_d("Digite uma opção: [1-5]:");

        switch (op)
        {
        case 1:
            array2Bin(&count, &a);
            break;
        case 2:
            fflush(stdin);
            printf("Digite o nome do lote que deseja excluir: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';
            excluirLote(nome);
            break;
        case 3:
            binSumCsv();
            break;
        case 4:
            binToCsv(&a, count);
            break;
        case 5: 
            printf("Saindo...!");
            return;
        
        default:
        printf("Opção Invalida!");
            break;
        }
    } while (op != 5);
    

    // carregarDados(&a, nomeArq, &count);
    // array2Bin(count, &a);
    // binToCsv(&a, count);
    // binSumCsv();

    return 0;
}
