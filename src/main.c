#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include "date.h"
#include "lists.h"
#include "files.h"
#include "additional_task.h"

#include "tui.h"

void print (list_elem_t * e, wchar_t * text)
{
    setlocale(LC_ALL, "");

    wchar_t date_wcs[1024];
    date2wcs(&e->data.date, date_wcs);
    swprintf(text, 2048, L"%lu%c%lu%c%ls%c%ls%c%ls%c%ls%c%.3Lf%c%.2Lf%c%ls",
            0, CSV_DELIMITER,
            e->data.cage_num, CSV_DELIMITER,
            e->data.animal.name, CSV_DELIMITER,
            e->data.animal.breed, CSV_DELIMITER,
            e->data.animal.areal, CSV_DELIMITER,
            e->data.products.type, CSV_DELIMITER,
            e->data.products.weight, CSV_DELIMITER,
            e->data.products.cost, CSV_DELIMITER,
            date_wcs
        );
}



int main ()
{
    const menu_t MAIN_MENU = {
        L"     Просто выйти      ",
        L" Выйти с большой буквы ",
        L"   Выйти через Ctrl+C  ",
        NULL
    };

    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, 1);

    // tui_draw_popup_text_message(L"Текстовое окно", L"merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp merp");
    //tui_draw_vmenu(stdscr, 0, 0, 1, MAIN_MENU, 0);
    
    tui_draw_popup_select (L"Выбор выхода из программы", L"Как предпочитаете выйти из этой моноформной программы? Всё остальное - это merp merp merp merp merp merp merp merp ....", MAIN_MENU);

    endwin();
    return 0;
}