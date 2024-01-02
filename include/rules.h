#if !defined(_GAME_RULES_H_)
#define _GAME_RULES_H_
#include "tienlen.h"
#include <stdbool.h>
typedef enum bai_danh_xuong
{
    BaiLoi,
    Don,
    Doi,
    BoBa,
    Sanh,
    BaDoiThong,
    TuQuy,
    BonDoiThong

} BaiDanhXuong;
typedef struct _cards
{
    BaiDanhXuong NameOfGroupCards; // bài đánh xuống loại gì
    int NumberOfGroupCards; // số bài đánh xuống
    Card cards[CARD_SIZE / MAX_PLAYER];
} InforCards;
int valueOfCard(Card card);
InforCards xepBai(InforCards cards);
static bool checkDon(InforCards cards);
static bool checkDoi(InforCards cards);
static bool checkBoBa(InforCards cards);
static bool checkSanh(InforCards cards);
static bool checkBaDoiThong(InforCards cards);
static bool checkTuQuy(InforCards cards);
static bool checkBonDoiThong(InforCards cards);
BaiDanhXuong checkBaiDanhXuong(InforCards cards);
bool checkValid(InforCards previous, InforCards present);
// Các hàm kiểm tra bài:
// B1: Xếp bài - xepBai()
// B2: Kiểm tra bài xem bài đó thuộc loại bài nào hay bài lỗi - checkBaiDanhXuong()
// B3: Kiểm tra bài đó có hợp lệ hay không - checkValid()

int valueOfCard(Card card)
{
    return card.value * 4 + card.suit;
}
InforCards xepBai(InforCards cards)
{
    int sll = cards.NumberOfGroupCards;
    for (int i = 0; i < sll; i++)
    {
        for (int j = 0; j < sll - 1; j++)
        {
            if (valueOfCard(cards.cards[j]) > valueOfCard(cards.cards[j + 1]))
            {
                Card tmpCard = cards.cards[j];
                cards.cards[j] = cards.cards[j + 1];
                cards.cards[j + 1] = tmpCard;
            }
        }
    }
    return cards;
}
BaiDanhXuong checkBaiDanhXuong(InforCards cards)
{
    if (checkDon(cards))
    {
        return Don;
    }
    if (checkDoi(cards))
    {
        return Doi;
    }
    if (checkBoBa(cards))
    {
        return BoBa;
    }
    if (checkSanh(cards))
    {
        return Sanh;
    }
    if (checkBaDoiThong(cards))
    {
        return BaDoiThong;
    }
    if (checkTuQuy(cards))
    {
        return TuQuy;
    }
    if (checkBonDoiThong(cards))
    {
        return BonDoiThong;
    }
    return BaiLoi;
}
bool checkDon(InforCards cards)
{
    return cards.NumberOfGroupCards == 1;
}
bool checkDoi(InforCards cards)
{
    if (cards.NumberOfGroupCards != 2)
    {
        return false;
    }
    return cards.cards[0].value == cards.cards[1].value;
}
bool checkBoBa(InforCards cards)
{
    if (cards.NumberOfGroupCards != 3)
    {
        return false;
    }
    return cards.cards[0].value == cards.cards[1].value && cards.cards[1].value == cards.cards[2].value;
}
bool checkSanh(InforCards cards)
{
    if (cards.NumberOfGroupCards <= 2)
        return false;

    if (cards.cards[cards.NumberOfGroupCards - 1].value == TWO)
    {
        return false;
    }

    for (int i = 0; i < cards.NumberOfGroupCards - 1; i++)
    {
        if (cards.cards[i].value != cards.cards[i + 1].value - 1)
        {
            return false;
        }
    }
    return true;
}
bool checkBaDoiThong(InforCards cards)
{
    if (cards.NumberOfGroupCards != 6)
    {
        return false;
    }
    if (cards.cards[5].value == TWO)
    {
        return false;
    }

    for (int i = 0; i < 5; i += 2)
    {
        if (cards.cards[i].value != cards.cards[i + 1].value)
        {
            return false;
        }
    }

    if (cards.cards[1].value != cards.cards[2].value - 1 || cards.cards[3].value != cards.cards[4].value - 1)
    {
        return false;
    }
    return true;
}
bool checkTuQuy(InforCards cards)
{
    if (cards.NumberOfGroupCards != 4)
    {
        return false;
    }
    CardValue value = cards.cards[0].value;
    for (int i = 1; i < 4; i++)
    {
        if (cards.cards[i].value != value)
        {
            return false;
        }
    }
    return true;
}
bool checkBonDoiThong(InforCards cards)
{
    if (cards.NumberOfGroupCards != 8)
    {
        return false;
    }
    if (cards.cards[7].value == TWO)
    {
        return false;
    }

    // check đôi
    for (int i = 0; i < 7; i += 2)
    {
        if (cards.cards[i].value != cards.cards[i + 1].value)
        {
            return false;
        }
    }

    // check liên tiếp
    for (int i = 1; i < 8; i += 2)
    {
        if (cards.cards[i].value != cards.cards[i + 1].value - 1)
        {
            return false;
        }
    }

    return true;
}

bool checkValid(InforCards prev, InforCards cur)
{
    BaiDanhXuong prevName = prev.NameOfGroupCards, curName = cur.NameOfGroupCards;
    int prevNum = prev.NumberOfGroupCards, curNum = cur.NumberOfGroupCards;
    Card *prevCard = prev.cards, *curCard = cur.cards;
    // Lượt đầu tiên,hoặc lượt mới(sau khi tất cả người chơi còn lại bỏ lượt)
    if (prevNum == 0 && curName != BaiLoi // TODO: && curName == Don(Có bug ở đây => Lượt đầu nên là đánh đơn, còn các lượt mới có thể đánh bất kì)
    )
    {
        return true;
    }
    // bài lỗi
    if (prevName == BaiLoi || curName == BaiLoi)
    {
        return false;
    }
    //  cùng bộ
    if (prevName == curName)
    {
        if (prevName != Sanh)
        {
            return valueOfCard(prevCard[prevNum - 1]) < valueOfCard(curCard[curNum - 1]);
        }
        return prevNum <= curNum && valueOfCard(prevCard[prevNum - 1]) <= valueOfCard(curCard[curNum - 1]) && prevCard[0].value <= curCard[0].value;
    }
    // khác bộ:
    switch (prevName)
    {
    case Don:
        if (prevCard[0].value != TWO)
            return false;
        return curName == BaDoiThong || curName == BonDoiThong || curName == TuQuy;
        break;
    case Doi:
        if (prevCard[0].value != TWO)
            return curName == BonDoiThong;
        return curName == TuQuy || curName == BonDoiThong;
        break;
    case BaDoiThong:
        return curName == BonDoiThong;
        break;
    default:
        return false;
        break;
    }
}

#endif // _GAME_RULES_H_
