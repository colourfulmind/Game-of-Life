#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define G_HEIGHT 25
#define G_WIDTH 80

void dead_cell(int i, int j);
void living_cell(int i, int j);
void rendering_map(char **arr, int *check);
void change_array(char **arr, int *check);
void tmp_array(char **tmp_arr, char **arr, int i, int j);
char simple_cell_processing(char **arr);
void clear_screen();
void game(char **arr);
char **create_array(int row, int col);
void free_array(char **array);
void output(char **arr, int row, int col);
void input(char **arr);
int compare(char **arr, char **buffer);
void scan_speed(int *speed, int *check);

int main() {
    char **arr = create_array(G_HEIGHT, G_WIDTH);
    input(arr);
    freopen("/dev/tty", "r", stdin);
    game(arr);
    free_array(arr);
    return (0);
}

void game(char **arr) {
    int speed = 1;
    int check = 0;
    int generation = 1;
    while (check != 1) {
        clear_screen();
        printf("Generation: %d\n", generation);
        rendering_map(arr, &check);
        change_array(arr, &check);
        scan_speed(&speed, &check);
        usleep(1000000 / speed);
        generation++;
    }
}

void input(char **arr) {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 80; j++) {
            scanf("%c", &arr[i][j]);
        }
    }
}

int kbhit() {
    int bytes_ready;
    ioctl(0, FIONREAD, &bytes_ready);
    // 0 это ввод и 1 это вывод
    return bytes_ready;
}

void scan_speed(int *speed, int *check) {
    // отключение буферизации
    system("/bin/stty raw");
    char c;
    if (kbhit()) {
        c = getchar();
        if (c == '1') {
            *speed = 1;
        }
        if (c == '2') {
            *speed = 2;
        }
        if (c == '3') {
            *speed = 3;
        }
        if (c == '4') {
            *speed = 4;
        }
        if (c == '5') {
            *speed = 5;
        }
        if (c == '6') {
            *speed = 8;
        }
        if (c == 'q') {
            *check = 1;
        }
    }
    system("/bin/stty cooked");
}

void change_array(char **arr, int *check) {
    char **buffer = create_array(G_HEIGHT, G_WIDTH);
    char **tmp_arr = NULL;
    for (int i = 0; i < G_HEIGHT; i++) {
        for (int j = 0; j < G_WIDTH; j++) {
            // объявление временного двумерного массива
            // и вызов функции для записи данных в массив
            tmp_arr = create_array(3, 3);
            tmp_array(tmp_arr, arr, i, j);
            buffer[i][j] = simple_cell_processing(tmp_arr);
            free_array(tmp_arr);
        }
    }
    *check = compare(arr, buffer);
    for (int i = 0; i < G_HEIGHT; i++) {
        for (int j = 0; j < G_WIDTH; j++) {
            arr[i][j] = buffer[i][j];
        }
    }
    free_array(buffer);
}

void tmp_array(char **tmp_arr, char **arr, int i, int j) {
    if (i == 0) {
        i += G_HEIGHT;
    }
    if (j == 0) {
        j += G_WIDTH;
    }
    tmp_arr[0][0] = arr[(i - 1) % 25][(j - 1) % 80];
    tmp_arr[0][1] = arr[(i - 1) % 25][j % 80];
    tmp_arr[0][2] = arr[(i - 1) % 25][(j + 1) % 80];
    tmp_arr[1][0] = arr[i % 25][(j - 1) % 80];
    tmp_arr[1][1] = arr[i % 25][j % 80];
    tmp_arr[1][2] = arr[i % 25][(j + 1) % 80];
    tmp_arr[2][0] = arr[(i + 1) % 25][(j - 1) % 80];
    tmp_arr[2][1] = arr[(i + 1) % 25][j % 80];
    tmp_arr[2][2] = arr[(i + 1) % 25][(j + 1) % 80];
}

char simple_cell_processing(char **arr) {
    char status;
    int counter = 0;  // счетчик живых клеток вокруг
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (arr[i][j] == '1') {
                counter++;
            }
            if (arr[i][j] == '1' && i == 1 && j == 1) {
                counter--;
            }
        }
    }
    if (arr[1][1] == '1') {
        if (counter == 2 || counter == 3) {
            status = '1';
        } else {
            status = '0';
        }
    } else {
        if (counter == 3) {
            status = '1';
        } else {
            status = '0';
        }
    }
    return status;
}

void rendering_map(char **arr, int *check) {
    if (*check != 1) {
        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 80; j++) {
                (arr[i][j] == '1') ? living_cell(i, j) : dead_cell(i, j);
            }
        }
        printf("\n");
    }
}

void dead_cell(int i, int j) {
    if (j == 79 && i != 24) {
        printf("░\n");
    } else {
        printf("░");
    }
}

void living_cell(int i, int j) {
    if (j == 79 && i != 24) {
        printf("▓\n");
    } else {
        printf("▓");
    }
}

void clear_screen() {
    printf("\e[1;1H\e[2J");
}

char **create_array(int row, int col) {
    char **arr = (char **)malloc(row * col * sizeof(char) + row * sizeof(char *));
    char *ptr = (char *) (arr + row);
    for (int i = 0; i < row; ++i) {
        arr[i] = ptr + col * i;
    }
    return (arr);
}

void free_array(char **array) {
    if (array) {
        free(array);
    }
}

int compare(char **arr, char **buffer) {
    char **check = create_array(G_HEIGHT, G_WIDTH);
    for (int i = 0; i < G_HEIGHT; i++) {
        for (int j = 0; j < G_WIDTH; j++) {
            if (arr[i][j] == buffer[i][j]) {
                check[i][j] = 1;
            } else {
                check[i][j] = 0;
            }
        }
    }
    for (int i = 0; i < G_HEIGHT; i++) {
        for (int j = 0; j < G_WIDTH; j++) {
            if (check[i][j] == 0) {
                free_array(check);
                return 0;
            }
        }
    }
    free_array(check);
    return 1;
}
