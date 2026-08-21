//
// Created by 2003SINGER on 2026/2/22.
//

#ifndef DESKTOPPET_PAGE_H
#define DESKTOPPET_PAGE_H

typedef enum {
    MAIN,
    HISTORY,
    INTERACTION
}PAGE;

typedef enum {
    ONE=0,
    TWO=1,
    THREE=2,
    FOUR=3,
    FIVE=4,
}POINTER;

extern PAGE Page;
extern POINTER MainPointer;
extern POINTER InteractionPointer;
extern POINTER HistoryPointer;


#endif //DESKTOPPET_PAGE_H