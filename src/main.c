#include "main.h"

int main ()
{
    /* Включить нужную локализацию */
    setlocale(LC_ALL, "");

    /* Задать начальные настройки ncurses */
    initscr();
    raw();
    noecho();
    keypad(stdscr, 1);

    /* Начало выполнения основной программы */
    const int LAST_CURS_STATE = curs_set (0);
    wchar_t tmp_wcs[1025];                                    // Переменная для хранения временных строк
    mkey_t selected_key = 0;
    list_obj_t * main_list = lists_CreateNewListObject (),    // Список, содержащий все данные
               * save_list = NULL,                            // Список, который нужно сохранить в файл
               * selected_table_list = main_list,             // Список, который нужно отображить в таблице
               * search_results_list = lists_CreateNewListObject ();  // Список с результатами поиска
    int curr_selected_page = 1,
        curr_selected_table_row = 1,
        drawed_rows = 0;

    draw_win_background (stdscr);
    refresh ();
    
    while (
        (selected_key = tui_draw_popup_select (L"Программа для курсового проекта", L"Добро пожаловать в программу, созданную специально для курсового проекта.\nВыберите действие.", MAIN_MENU))
        != 3
    )
    {
        

        switch (selected_key)
        {
            // Выбрано "Работа с программой"
            case 1:
                {
                    /* Создать окна и панели для главного окна */

                    // Само окно
                    WINDOW * win_main = newwin (LINES - 2, COLS - 4, 1, 2);
                    PANEL  * panel_main = new_panel (win_main);
                    
                    update_panels ();

                    // Таблица
                    WINDOW * win_table = derwin (win_main, getmaxy(win_main) - (3 + 1 + 1 + 6 + 3 + 1 + 1), getmaxx (win_main) - 4, 4, 2);
                    PANEL  * panel_table = new_panel (win_table);

                    /* Отрисовать всё и вся */
                    tui_draw_popup_header (win_main, L"Таблица данных");

                    /* Отрисовать подсказки ввода */
                    mvwprintw (win_main, getmaxy(win_main) - 12, 1, "Действия над таблицей:                    ");
                    mvwprintw (win_main, getmaxy(win_main) - 11, 1, "  (← →) Выбор страницы данных.            ");
                    mvwprintw (win_main, getmaxy(win_main) - 10, 1, "  (↑ ↓) Выбор строки в таблице.           ");
                    mvwprintw (win_main, getmaxy(win_main) - 9,  1, "Действия над данными:                     ");
                    mvwprintw (win_main, getmaxy(win_main) - 8,  1, "  (P) Добавить данные.                    ");
                    mvwprintw (win_main, getmaxy(win_main) - 7,  1, "  (W) Сохранить в файл/Загрузить из файла.");
                    mvwprintw (win_main, getmaxy(win_main) - 6,  1, "  (R) Просмотреть подробную информаци.    ");
                    mvwprintw (win_main, getmaxy(win_main) - 5,  1, "  (G) Редактировать элемент.              ");
                    mvwprintw (win_main, getmaxy(win_main) - 4,  1, "  (F) Поиск.                              ");
                    mvwprintw (win_main, getmaxy(win_main) - 3,  1, "  (T) Задание по варианту.                ");
                    mvwprintw (win_main, getmaxy(win_main) - 2,  1, "  (E) Выйти из текущего окна.             ");

                    selected_key = MKEY_NULL;
                    do
                    {
                        switch (selected_key)
                        {
                            // Действие "Добавить элемент" 
                            case L'P': case L'p':
                                {
                                    list_data_t  d;
                                    switch (tui_popup_edit_element_data (L"Данные элемента", &d, 1))
                                    {
                                        case TUI_ADD_ELEMENT_FLAG | TUI_AT_BEGIN_FLAG:
                                            lists_InsertAsListsHead (main_list, lists_CreateNewElement(&d));
                                            break;
                                        
                                        case TUI_ADD_ELEMENT_FLAG | TUI_AT_END_FLAG:
                                            lists_InsertAsListsTail (main_list, lists_CreateNewElement(&d));
                                            break;
                                    }

                                }
                                break;
                            
                            // Действие "Подробная информация"
                            case L'R': case L'r':
                                {
                                    length_t position = (curr_selected_page - 1) * (getmaxy(win_table) - 5) + curr_selected_table_row;
                                    list_elem_t * element = lists_SearchElementByField (selected_table_list, LIST_POSITION, (void *) &position);

                                    if (element)
                                        tui_popup_show_only_element (L"Информация о вольере", element);
                                    else
                                        tui_draw_popup_text_message (L"ОШИБКА", L"ОШИБКА: не удалось найти элемент.\nСкорее всего, это произошло потому, что отсутствуют данные.");
                                
                                    update_panels ();
                                }
                                break;
                            
                            // Действие "Редактировать элемент"
                            case L'G': case L'g':
                                {
                                    length_t position = (curr_selected_page - 1) * (getmaxy(win_table) - 5) + curr_selected_table_row;
                                    list_elem_t * element = lists_SearchElementByField (selected_table_list, LIST_POSITION, (void *) &position);
                                    int correct_code = 1;

                                    if (element)
                                    {
                                        while (!correct_code)
                                        {
                                            
                                        }
                                        

                                        tui_popup_edit_element_data (L"Редактировать запись", &element->data, 0);
                                    }
                                    else
                                        tui_draw_popup_text_message (L"ОШИБКА", L"ОШИБКА: не удалось найти элемент.\nСкорее всего, это произошло потому, что отсутствуют данные.");
                                
                                    update_panels ();
                                }
                                break;
                            
                            // Действие "Поиск"
                            case L'F': case L'f':
                                {
                                    if (selected_table_list->_length_ < 1)
                                    {
                                        tui_draw_popup_text_message (
                                            L"ОШИБКА",
                                            L"Для начала заполните список."
                                        );
                                        break;
                                    }

                                    selected_key = tui_draw_popup_select (
                                        L"Выбор поля для поиска",
                                        L"Выберите поле для поиска.\nПосле чего необходимо будет ввести конкретное значений.",
                                        SELECT_SEARCH_FIELD_MENU
                                    );

                                    void * universal_ptr = NULL;
                                    list_id_t tmp_id = 0;
                                    length_t tmp_int = 0;
                                    weight_t tmp_weight  = 0.0;
                                    cost_t tmp_cost  = 0.0;
                                    udate_t tmp_date = {.D = 1, .M = 9, .Y = 2021};
                                    list_elem_t * tmp_element_ptr = NULL;

                                    switch (selected_key)
                                    {
                                        case LIST_POSITION:
                                            tui_draw_popup_form (
                                                L"Поиск по позиции",
                                                L"Введите требуемый номер позиции.",
                                                L"POS",
                                                tmp_wcs,
                                                POSITION_DGT_CNT,
                                                VMASK_DIGITS
                                            );
                                            tmp_int = wcs2int (tmp_wcs);
                                            universal_ptr = &tmp_int;
                                            break;
                                        
                                        case LIST_ID:
                                            tui_draw_popup_form (
                                                L"Поиск по ID",
                                                L"",
                                                L"ID",
                                                tmp_wcs,
                                                POSITION_DGT_CNT,
                                                VMASK_DIGITS
                                            );
                                            tmp_id = wcs2int (tmp_wcs);
                                            universal_ptr = &tmp_id;
                                            break;
                                        
                                        case LIST_CAGE_NUMBER:
                                            tui_draw_popup_form (
                                                L"Поиск по номеру вольера",
                                                L"",
                                                L"вольер",
                                                tmp_wcs,
                                                POSITION_DGT_CNT,
                                                VMASK_DIGITS
                                            );
                                            tmp_int = wcs2int (tmp_wcs);
                                            universal_ptr = &tmp_int;
                                            break;
                                        
                                        case LIST_ANIMAL_AREAL:
                                            tui_draw_popup_form (
                                                L"Поиск по ареалу",
                                                L"Введите ареал обитания животного.",
                                                L"ареал",
                                                tmp_wcs,
                                                POSITION_DGT_CNT,
                                                VMASK_DIGITS
                                            );
                                            universal_ptr = tmp_wcs;
                                            break;
                                        
                                        case LIST_ANIMAL_BREED:
                                            tui_draw_popup_form (
                                                L"Поиск по породе",
                                                L"Введите породу животного.",
                                                L"порода",
                                                tmp_wcs,
                                                POSITION_DGT_CNT,
                                                VMASK_DIGITS
                                            );
                                            universal_ptr = tmp_wcs;
                                            break;
                                        
                                        case LIST_ANIMAL_NAME:
                                            tui_draw_popup_form (
                                                L"Поиск по имени",
                                                L"Введите имя животного.",
                                                L"имя",
                                                tmp_wcs,
                                                POSITION_DGT_CNT,
                                                VMASK_DIGITS
                                            );
                                            universal_ptr = tmp_wcs;
                                            break;
                                        
                                        case LIST_PRODUCT_TYPE:
                                            tui_draw_popup_form (
                                                L"Поиск по типу",
                                                L"Введите тип продукта.",
                                                L"тип",
                                                tmp_wcs,
                                                POSITION_DGT_CNT,
                                                VMASK_DIGITS
                                            );
                                            universal_ptr = tmp_wcs;
                                            break;
                                        
                                        case LIST_PRODUCT_WEIGHT:
                                            tui_draw_popup_form (
                                                L"Поиск по весу",
                                                L"Введите вес.",
                                                L"вес",
                                                tmp_wcs,
                                                POSITION_DGT_CNT,
                                                VMASK_DIGITS
                                            );
                                            tmp_weight = wcs2float (tmp_wcs);
                                            universal_ptr = &tmp_weight;
                                            break;
                                        
                                        case LIST_PRODUCT_COST:
                                            tui_draw_popup_form (
                                                L"Посик по стоимоти",
                                                L"Введите стоимость.",
                                                L"стоимос",
                                                tmp_wcs,
                                                POSITION_DGT_CNT,
                                                VMASK_DIGITS
                                            );
                                            tmp_cost = wcs2float (tmp_wcs);
                                            universal_ptr = &tmp_cost;
                                            break;
                                        
                                        case LIST_DATE:
                                            tui_draw_popup_date (
                                                L"Поиск по дате",
                                                L"Введите дату.",
                                                &tmp_date
                                            );
                                            universal_ptr = &tmp_date;
                                            break;
                                        
                                        default:
                                            tui_draw_popup_text_message (
                                                L"Предупреждение",
                                                L"Поиск данных был отменён пользователем."
                                            );
                                            break;
                                    }

                                    if (selected_key > LIST_DATE)
                                        break;

                                    /* Найти все совпадения */
                                    tmp_element_ptr = selected_table_list->head;
                                    while (tmp_element_ptr = lists_SearchElementByFieldFromThisElement (tmp_element_ptr, selected_key, universal_ptr))
                                    {
                                        lists_InsertAsListsTail (search_results_list, tmp_element_ptr);
                                        tmp_element_ptr = tmp_element_ptr->next;
                                    }
                                    selected_table_list = search_results_list;
                                }
                                break;
                            
                            // Действие "Задание по варианту"
                            case L'T': case L't':
                                break;
                            
                            // Изменение выбранной строки
                            case MKEY_ARROW_DOWN:
                                if (curr_selected_table_row < drawed_rows)
                                    curr_selected_table_row++;
                                break;
                            case MKEY_ARROW_UP:
                                if (curr_selected_table_row > 1)
                                    curr_selected_table_row--;
                                break;
                            
                            // Изменение текущей страницы
                            case MKEY_ARROW_LEFT:
                                if (curr_selected_page > 1)
                                    curr_selected_page--;
                                break;
                            case MKEY_ARROW_RIGHT:
                                if (curr_selected_page < main_list->_length_ / (getmaxy(win_table) - 5) + !!(main_list->_length_ % (getmaxy(win_table) - 5)))
                                    curr_selected_page++;
                                break;
                            
                            // Действие "Работа с файлами"
                            case L'W': case L'w':
                                // Выбор действия на файлами
                                selected_key = tui_draw_popup_select (
                                                    L"Сохранить/Загрузить",
                                                    L"Вы хотите ЗАГРУЗИТЬ данные из файла или СОХРАНИТЬ данные в файл?",
                                                    SAVE_OR_LOAD_MENU
                                                );

                                switch (selected_key)
                                {
                                    // Загрузить данные
                                    case 1:
                                        /* Выбрать тип загружаемого файла */
                                        selected_key = tui_draw_popup_select (
                                                    L"Загрузить данные",
                                                    L"Выберите тип файла, в котором хранятся данные:\n* .bin -> Специальный формат для хранения данных в бинарном формате. Только эта и никакая другая программа может расшифровать сохранённые данные.\n*  .csv -> Текстовый формат хранения данных. Таким образом вы можете внести данные как из Текстового редактора, так и импортировать из других программ, например, MS Excel.",
                                                    FILE_FORMATS_MENU
                                                );
                                        
                                        if (selected_key == 3)
                                        {
                                            tui_draw_popup_text_message (
                                                L"Предупреждение",
                                                L"Загрузка данных из файла отменена пользователем."
                                            );
                                            break;
                                        }
                                        
                                        /* Обнулить значение tmp_wcs и Запросить имя загружаемого файла файла */
                                        memset (tmp_wcs, 0, sizeof(tmp_wcs));
                                        tui_draw_popup_form (
                                            L"Имя файла",
                                            L"Осталось ввести имя файла.\nПримечание: обычно все файлы сохранений хранятся в папке \"data\".",
                                            L" Введите имя файла",
                                            tmp_wcs,
                                            FILENAME_MAX_LENGTH,
                                            VMASK_ANY_CHAR | VMASK_DIGITS | VMASK_PUNCTS
                                        );

                                        /* Проверить на существование файл */
                                        if (is_exist_wcs (tmp_wcs))
                                        {
                                            tui_draw_popup_text_message (
                                                L"ОШИБКА!",
                                                L"Невозможно получить доступ к файлу.\nВозможно файл не существует или его нельзя прочитать.\nПовторите ввод позже..."
                                            );
                                            break;
                                        }

                                        /* Всё ок. Очистить данные в старых списках и загрузить новые данные */
                                        lists_CleanListObject (main_list);
                                        lists_CleanListObject (search_results_list);
                                        switch (selected_key)
                                        {
                                            case 1:
                                                read_from_bin (main_list, tmp_wcs);
                                                break;
                                            
                                            case 2:
                                                read_from_csv (main_list, tmp_wcs);
                                                break;
                                        }

                                        tui_draw_popup_text_message (
                                            L"УСПЕХ!",
                                            L"Все данные были считаны."
                                        );                                        

                                        break;
                                    
                                    // Сохранить данные
                                    case 2:
                                        /* Какие данные нужно сохранить */
                                        selected_key = tui_draw_popup_select (
                                                            L"Сохранить ...",
                                                            L"Выберите список, который хотите сохранить.",
                                                            LIST_TYPES_MENU
                                                        );
                                        /* Выбрать список для сохранения и проверить список на наличие данных */
                                        save_list = (selected_key == 1) ? (main_list) : (search_results_list);
                                        if (save_list->_length_ < 1)
                                        {
                                            tui_draw_popup_text_message (
                                                L"ОШИБКА!",
                                                L"Похоже, что данные отсутствуют, а значит нечего сохранять.\nДля начала добавьте какие-то данные."
                                            );
                                            break;
                                        }
                                        
                                        /*Всё ок. Выбрать тип сохраняемого файла */
                                        selected_key = tui_draw_popup_select (
                                                    (save_list == main_list) ? L"Сохранить все данные как ..." : L"Сохранить результаты поиска как ...",
                                                    L"Выберите тип файла для сохранения:\n* .bin -> Специальный формат для хранения данных в бинарном формате. Злоумышленнику не сможет скомпрометировать данные.\n*  .csv -> Текстовый формат хранения данных. Возможность как редактирования Текстовыми редакторами, так и экспорта в другие программы, например, MS Excel.",
                                                    FILE_FORMATS_MENU
                                                );
                                        
                                        /* Обнулить значение tmp_wcs и Запросить имя файла для сохранения */
                                        memset (tmp_wcs, 0, sizeof(tmp_wcs));
                                        tui_draw_popup_form (
                                            L"Имя файла",
                                            L"Осталось ввести имя файла.\nПримечание: по умолчанию все файлы сохранений хранятся в папке \"data\".",
                                            L" Введите имя файла",
                                            tmp_wcs,
                                            FILENAME_MAX_LENGTH,
                                            VMASK_ANY_CHAR | VMASK_DIGITS
                                        );
                                        
                                        switch (selected_key)
                                        {
                                            // Сохранить как бинарный файл
                                            case 1:
                                                save_as_bin (save_list, tmp_wcs);
                                                break;
                                            // Сохранить как текстовый файл
                                            case 2:
                                                save_as_csv (save_list, tmp_wcs);
                                                break;
                                        }

                                        tui_draw_popup_text_message (
                                            L"УСПЕХ!",
                                            L"Все данные были сохранены."
                                        );  

                                        break;
                                }

                                break;
                        }

                        wclear (win_table);
                        drawed_rows = tui_draw_table_in_window (win_table, *selected_table_list, curr_selected_page, curr_selected_table_row);

                        update_panels ();
                        doupdate ();
                    }
                    while ((selected_key = getwchar()) != L'E' && selected_key != L'e');
                    
                    /* Удалить панели и окна */
                    del_panel (panel_table);
                    del_panel (panel_main);
                    delwin (win_table);
                    delwin (win_main);
                }
                break;
            
            // Выбрано "О программе"
            case 2:
                tui_draw_popup_text_message (
                    L"О программе",
                    L"Программа была написана для курсового проекта для предмета \"АиП\" (3 семестр).\n* Создатель: Филозоп Алексей.\n* Исходный код: github.com/Cory7666/3semester_AaP_Coursework_new.git"
                );
                break;
        }
    }

    /* Завершить выполнение программы */
    lists_DeleteListObject (&main_list);

    curs_set (LAST_CURS_STATE);
    endwin();
    return 0;
}

/********************************
 * Блок дополнительных функций. *
 ********************************/

void draw_win_background (WINDOW * win)
{
    wmove (win, 0, 0);
    for (size_t i = 0; i < (int) (getmaxx(win) * getmaxy(win) / 6); i++)
    {
        wprintw (win, "*@|@*-");
    }
}