#ifndef PLAYER_H_
#define PLAYER_H_
typedef struct _player
{
    char name[20]; // player's name
    int cardSize; // 0-13 number of cards in player's hand
    char point;   // player's point
} Player;
#endif // PLAYER_H_
