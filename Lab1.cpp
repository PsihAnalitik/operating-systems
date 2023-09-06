#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BLOCK_SIZE 1024
#define DATA_SIZE (256 * 1024)
#define NUM_ITERATIONS 10

void fillRandomData(unsigned char* data, int size) {
    for (int i = 0; i < size; i++) {
        data[i] = rand() % 256;  // Заполняем данными от 0 до 255 (случайные байты)
    }
}

void writeTextToFile(const char* fileName, const char* text) {
    // Открываем файл для записи. Если файл не существует, он будет создан. 
    // Если файл существует, его содержимое будет перезаписано.
    FILE* file = fopen(fileName, "a");

    if (file == NULL) {
        // Ошибка открытия файла
        perror("Ошибка открытия файла");
        return;
    }

    // Записываем текст в файл
    fprintf(file, "%s", text);

    // Закрываем файл
    fclose(file);
}

void writeToExternalMemory(const char* externalFileName, const unsigned char* data, int dataSize) {
    FILE* externalFile = fopen(externalFileName, "wb"); // Открываем файл для записи в бинарном режиме

    if (externalFile == NULL) {
        perror("Ошибка открытия файла для записи");
        return;
    }

    int numBlocks = dataSize / BLOCK_SIZE;
    int remainingBytes = dataSize % BLOCK_SIZE;

    for (int i = 0; i < numBlocks; i++) {
        fwrite(&data[i * BLOCK_SIZE], sizeof(unsigned char), BLOCK_SIZE, externalFile);
    }

    if (remainingBytes > 0) {
        fwrite(&data[numBlocks * BLOCK_SIZE], sizeof(unsigned char), remainingBytes, externalFile);
    }

    fclose(externalFile);
}

int main() {
    unsigned char dataArray[DATA_SIZE];
    int blockSize;
    clock_t startTime, endTime;

    // Инициализируем генератор случайных чисел
    srand(time(NULL));

    // Заполняем массив случайными данными
    fillRandomData(dataArray, DATA_SIZE);
    FILE* file = fopen("Analyze_time.txt", "wb");

    fclose(file);
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // Изменяем размер блока в соответствии с характеристикой
        blockSize = rand() % 513 + 512;  // Размер блока от 512 до 1024 байт
        struct timespec startTime, endTime;
        clock_gettime(CLOCK_MONOTONIC, &startTime);

        // Здесь выполняется операция обмена данными поблочно между оперативной и внешней памятью
        writeToExternalMemory("External_Memory.txt", dataArray, DATA_SIZE);

        clock_gettime(CLOCK_MONOTONIC, &endTime);

        double elapsedTime = 1000000000*(endTime.tv_sec - startTime.tv_sec)+(endTime.tv_nsec - startTime.tv_nsec);
        double timeInSeconds = (double)(elapsedTime) / CLOCKS_PER_SEC;
        double transferRate = (double)((double)(blockSize / 1024.0) / timeInSeconds);
        // Вычисляем скорость обмена и выводим результаты
        printf("Размер блока: %d байт\n", blockSize);
        printf("Время обмена: %.2f сек\n", timeInSeconds);
        printf("Скорость обмена: %.2f Кбайт/c\n", transferRate);
        char resultString[256];
        snprintf(resultString, sizeof(resultString), "%d %.2f %.2f \n", blockSize, timeInSeconds, transferRate);
        writeTextToFile("Analyze_time.txt",resultString);

    }

    return 0;
}
