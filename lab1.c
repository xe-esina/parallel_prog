#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#define A 3360

void generate(int N, unsigned int seed, double* M1, double* M2) {
    // Cлучайные вещественные числа, имеющие равномерный закон распределения в диапазоне от 1 до A (включительно)
    for (int i = 0; i < N; i++)
        M1[i] = 1 + (rand_r(&seed) / (double) RAND_MAX) * (A - 1);

    // Аналогично сформировать массив М2 размерностью N/2 со случайными вещественными числами в диапазоне от А до 10*А
    for (int i = 0; i < N/2; i++)
        M2[i] = A + (rand_r(&seed) / (double) RAND_MAX) * (10 * A - A);
}

void map(int N, double* M1, double* M2) {
    // В массиве М1 к каждому элементу применить операцию гиперболический тангенс с последующим уменьшением на 1
    for (int i = 0; i < N ; i++)
        M1[i] = tanh(M1[i]) - 1;

    // Затем в массиве М2 каждый элемент поочерёдно сложить с предыдущим, 
    // а к результату сложения применить операцию квадратный корень после умножения на e
    double M2_copy[N/2];
    for (int i = 0; i < N/2; i++)
        M2_copy[i] = M2[i];

    M2[0] = sqrt(M2[0] * M_E);
    for (int i = 1; i < N/2; i++)
        M2[i] = sqrt((M2[i] + M2_copy[i-1]) * M_E);
}

void merge(int N, double * M1, double * M2) {
    // В массивах М1 и М2 ко всем элементами с одинаковыми индексами 
    // попарно применить операцию модуль разности, результат сохранить в M2.
    for (int i = 0; i < N/2; i++)
        M2[i] = fabs(M2[i] - M1[i]);
}

// Релализация heapsort из статьи https://habr.com/ru/company/otus/blog/460087/
void swap(double *a, double *b) {
    double t = *a;
    *a = *b;
    *b = t;
}

void heapify(double *arr, int n, int i) {
    int largest = i;   
    // Инициализируем наибольший элемент как корень
    int l = 2*i + 1; // левый = 2*i + 1
    int r = 2*i + 2; // правый = 2*i + 2

    // Если левый дочерний элемент больше корня
    if (l < n && arr[l] > arr[largest])
        largest = l;

    // Если правый дочерний элемент больше, чем самый большой элемент на данный момент
    if (r < n && arr[r] > arr[largest])
        largest = r;

    // Если самый большой элемент не корень
    if (largest != i) {
        swap(&arr[i], &arr[largest]);

        // Рекурсивно преобразуем в двоичную кучу затронутое поддерево
        heapify(arr, n, largest);
    }
}

void heap_sort(double *arr, int n) {
  // Построение кучи (перегруппируем массив)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

   // Один за другим извлекаем элементы из кучи
    for (int i=n-1; i>=0; i--) {
        // Перемещаем текущий корень в конец
        swap(&arr[0], &arr[i]);

        // вызываем процедуру heapify на уменьшенной куче
        heapify(arr, i, 0);
    }
}

double reduce(double * M2, int N) {
    // Рассчитать сумму синусов тех элементов массива М2, 
    // которые при делении на минимальный ненулевой элемент массива М2 
    // дают чётное число (при определении чётности учитывать только целую часть числа)
    double min = M2[0];
    for (int i = 1; i < N/2; i++)
        if (M2[i] != 0 && M2[i] < min)
            min = M2[i];
    
    double X = 0;
    for (int i = 0; i < N/2; i++)
        if (((int) (M2[i] / min)) % 2 == 0) 
            X += sin(M2[i]);
        
    return X;
}

void print(int N, double * M1, double *M2){
    for (int i = 0; i<N; i++) {
        printf("%f ",M1[i]);
    }
    printf("\n");
    for (int i = 0; i<N/2; i++) {
        printf("%f ",M2[i]);
    }
    printf("\n");
}


int main (int argc, char* argv[]) {
    // N равно первому параметру командной строки
    int N = atoi(argv[1]);
    
    // Запомнить текущее время T1
    // timeval хранит секунды в поле tv_sec и микросекунды в поле tv_usec
    struct timeval t1;
    gettimeofday(&t1, NULL);

    // 50 экспериментов
    for (int i = 0; i < 50; i++) {
        double M1[N];
        double M2[N/2];
        
        // Инициализировать случайное значение ГСЧ
        srand(i);
        
        // Заполнить массив исходных данных размером N
        generate(N, i, M1, M2);
        
        // Решить поставленную задачу, заполнить массив с результатами
        map(N, M1, M2);
        merge(N, M1, M2);
               
        // Отсортировать массив с результатами указанным методом и посчитать агрегирующую функцию
        heap_sort(M2, N/2);        
        printf("%f\n", reduce(M2, N));
    }
    
    // Запомнить текущее время T2
    struct timeval t2;
    gettimeofday(&t2, NULL);

    // Считаем, сколько программа работала в мс и выводим
    long delta_ms = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000;
    printf("\nN=%d. Milliseconds passed: %ld\n", N, delta_ms);
    
    return 0;
}

