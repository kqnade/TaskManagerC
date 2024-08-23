#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 10
#define TASK_FILE "tasks.txt"
#define MAX_TASK_LENGTH 50
#define INPUT_WINDOW_HEIGHT 5
#define INPUT_WINDOW_WIDTH 50

// タスクをファイルに保存
void save_tasks(char tasks[MAX_TASKS][MAX_TASK_LENGTH], int task_count) {
    FILE *file = fopen(TASK_FILE, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    for (int i = 0; i < task_count; ++i) {
        fprintf(file, "%s\n", tasks[i]);
    }

    fclose(file);
}

// ファイルからタスクを読み込み
int load_tasks(char tasks[MAX_TASKS][MAX_TASK_LENGTH]) {
    FILE *file = fopen(TASK_FILE, "r");
    if (file == NULL) {
        return 0; // ファイルが存在しない場合は0タスク
    }

    int task_count = 0;
    while (fgets(tasks[task_count], MAX_TASK_LENGTH, file) != NULL && task_count < MAX_TASKS) {
        tasks[task_count][strcspn(tasks[task_count], "\n")] = '\0'; // 改行文字を削除
        task_count++;
    }

    fclose(file);
    return task_count;
}

// メインウィンドウの縁取りを描画
void draw_border() {
    int height, width;
    getmaxyx(stdscr, height, width);
    box(stdscr, 0, 0);
    mvprintw(0, (width - 12) / 2, "Task Manager");
}

// タスクを描画
void draw_tasks(char tasks[MAX_TASKS][MAX_TASK_LENGTH], int task_count, int selected_task) {
    for (int i = 0; i < task_count; ++i) {
        if (i == selected_task) {
            attron(A_REVERSE); // 選択されたタスクの反転表示
        }
        // タスクの最初の文字が正しく表示されるように修正
        mvprintw(i + 1, 2, "[%s] %s", tasks[i][0] == 'X' ? "X" : " ", tasks[i] + (tasks[i][0] == 'X' ? 2 : 0));
        if (i == selected_task) {
            attroff(A_REVERSE);
        }
    }
}

// フローティングウィンドウでタスクの入力を処理
void get_task_input(char *new_task) {
    WINDOW *input_win;
    int ch, i = 0;

    // フローティングウィンドウを作成
    input_win = newwin(INPUT_WINDOW_HEIGHT, INPUT_WINDOW_WIDTH, LINES / 2 - INPUT_WINDOW_HEIGHT / 2, COLS / 2 - INPUT_WINDOW_WIDTH / 2);
    box(input_win, 0, 0); // 縁取りを描画
    mvwprintw(input_win, 1, 1, "Enter new task: ");
    wrefresh(input_win);

    curs_set(1); // カーソルを表示
    echo();
    while ((ch = wgetch(input_win)) != '\n') {
        switch (ch) {
            case KEY_BACKSPACE:
            case 127: // Backspace key (might be 127 on some terminals)
                if (i > 0) {
                    new_task[--i] = '\0';
                    mvwprintw(input_win, 1, 1, "Enter new task: %s", new_task);
                    wclrtoeol(input_win); // Clear the rest of the line
                    wrefresh(input_win);
                }
                break;
            default:
                if (i < MAX_TASK_LENGTH - 1 && (ch >= 32 && ch <= 126)) { // Printable characters
                    new_task[i++] = ch;
                    new_task[i] = '\0';
                    mvwprintw(input_win, 1, 1, "Enter new task: %s", new_task);
                    wclrtoeol(input_win); // Clear the rest of the line
                    wrefresh(input_win);
                }
                break;
        }
    }
    noecho();
    curs_set(0); // カーソルを非表示
    delwin(input_win);
}

int main() {
    char tasks[MAX_TASKS][MAX_TASK_LENGTH];
    int task_count = load_tasks(tasks);
    int selected_task = 0;
    int ch;

    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    while (1) {
        clear();
        draw_border();
        draw_tasks(tasks, task_count, selected_task);
        refresh();

        ch = getch();
        switch (ch) {
            case 'q':
                save_tasks(tasks, task_count);
                endwin();
                return 0;

            case 'j': // 次のタスクに移動
                if (selected_task < task_count - 1) selected_task++;
                break;

            case 'k': // 前のタスクに移動
                if (selected_task > 0) selected_task--;
                break;

            case 'a': // タスクの追加
                if (task_count < MAX_TASKS) {
                    char new_task[MAX_TASK_LENGTH] = {0};
                    get_task_input(new_task);
                    snprintf(tasks[task_count], MAX_TASK_LENGTH, "%s", new_task);
                    task_count++;
                    clear();
                }
                break;

            case 'd': // タスクの削除
                if (task_count > 0) {
                    for (int i = selected_task; i < task_count - 1; i++) {
                        strcpy(tasks[i], tasks[i + 1]);
                    }
                    task_count--;
                    if (selected_task >= task_count) selected_task = task_count - 1;
                }
                break;

            case 'x': // タスクの完了マーク
                if (task_count > 0) {
                    if (tasks[selected_task][0] == 'X') {
                        memmove(tasks[selected_task], tasks[selected_task] + 2, strlen(tasks[selected_task] + 2) + 1);
                    } else {
                        memmove(tasks[selected_task] + 2, tasks[selected_task], strlen(tasks[selected_task]) + 1);
                        tasks[selected_task][0] = 'X';
                    }
                }
                break;

            default:
                break;
        }
    }
}

