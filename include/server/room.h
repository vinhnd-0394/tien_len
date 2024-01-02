#if !defined(ROOM_H_)
#define ROOM_H_
#include "session.h"
#include "index.h"
typedef struct _Room
{
    char name[127];
    int id;
    int maxUser;
    int curUser;
    Session users[4];
    Player players[4];
    bool isPlaying;
    int lastTurn;
} *Room;
static Dllist rooms = NULL;
Room newRoom(char *name, int maxUser);
void addRoom(Room room);
void freeRoom(Room room);
void freeRoomList();
Room findRoom(int id);
Room findRoomByName(char *name);
int findRoomsByNamePrefix(char *prefix, Room result[100]);
Room findRoomByUser(char *username);
bool joinRoom(Room room, Session session);
bool leaveRoom(Room room, Session session);
bool isRoomFull(Room room);
int getRoomCount();
int randRoomId();

int randRoomId()
{
    int id = 0;
    while (true)
    {
        id = rand() % 10000;
        if (findRoom(id) == NULL)
        {
            break;
        }
    }
    return id;
}
Room newRoom(char *name, int maxUser)
{
    if (!rooms)
    {
        rooms = new_dllist();
    }

    if (maxUser > 4 || maxUser < 1)
    {
        return NULL;
    }

    int id = randRoomId();
    Room room = (Room)malloc(sizeof(struct _Room));
    *room = (struct _Room){
        .name = {0},
        .id = id,
        .maxUser = maxUser,
        .curUser = 0,
        .users = {0},
        .players = {0},
        .isPlaying = false,
        .lastTurn = -1,
    };
    strcpy(room->name, name);
    addRoom(room);
    return room;
}
void addRoom(Room room)
{
    dll_append(rooms, new_jval_v(room));
}
void freeRoom(Room room)
{
    Dllist ptr;
    dll_traverse(ptr, rooms)
    {
        if (jval_v(ptr->val) == room)
        {
            dll_delete_node(ptr);
            break;
        }
    }
    free(room);
}
void freeRoomList()
{
    Dllist ptr;
    dll_traverse(ptr, rooms)
    {
        freeRoom((Room)jval_v(ptr->val));
    }
    free_dllist(rooms);
}

Room findRoom(int id)
{
    if (!rooms)
    {
        return NULL;
    }
    Dllist ptr;
    dll_traverse(ptr, rooms)
    {
        Room room = (Room)jval_v(ptr->val);
        if (room->id == id)
        {
            return room;
        }
    }
    return NULL;
}

Room findRoomByName(char *name)
{
    if (!rooms)
    {
        return NULL;
    }
    Dllist ptr;
    dll_traverse(ptr, rooms)
    {
        Room room = (Room)jval_v(ptr->val);
        if (strcmp(room->name, name) == 0)
        {
            return room;
        }
    }
    return NULL;
}
int findRoomsByNamePrefix(char *prefix, Room result[100])
{
    if (!rooms)
    {
        return NULL;
    }
    int count = 0;
    Dllist ptr;
    dll_traverse(ptr, rooms)
    {
        Room room = (Room)jval_v(ptr->val);
        if (strstr(room->name, prefix) != NULL)
        {
            result[count++] = room;
        }
    }
    return count;
}
Room findRoomByUser(char *username)
{
    if (!rooms)
    {
        return NULL;
    }
    Dllist ptr;
    dll_traverse(ptr, rooms)
    {
        Room room = (Room)jval_v(ptr->val);
        for (int i = 0; i < room->curUser; i++)
        {
            if (strcmp(room->users[i]->username, username) == 0)
            {
                return room;
            }
        }
    }
    return NULL;
}

bool joinRoom(Room room, Session session)
{
    printf("current user in room: %d\n", room->curUser);
    if (room->curUser >= room->maxUser || room->isPlaying)
    {
        return false;
    }
    room->users[room->curUser] = session;
    strcpy(room->players[room->curUser++].name, session->username);
    // for testing
    exportRoom();
    return true;
}

bool leaveRoom(Room room, Session session)
{
    for (int i = 0; i < room->curUser; i++)
    {
        if (room->users[i] == session)
        {
            for (int j = i; j < room->curUser - 1; j++)
            {
                room->users[j] = room->users[j + 1];
                room->players[j] = room->players[j + 1];
            }
            room->curUser--;
            // note: Sau khi phòng trống thì phải xóa phòng đó đi
            // for testing
            if (room->curUser == 0)
            {
                freeRoom(room);
            }
            exportRoom();
            return true;
        }
    }
    return false;
}

bool isRoomFull(Room room)
{
    return room->curUser >= room->maxUser ? true : false;
}

int getRoomCount()
{
    int count = 0;
    Dllist ptr;
    dll_traverse(ptr, rooms)
    {
        count++;
    }
    return count;
}
void exportRoom()
{
    Dllist ptr;
    FILE *f = fopen("rooms.txt", "a+");
    dll_traverse(ptr, rooms)
    {
        Room room = (Room)jval_v(ptr->val);
        fprintf(f, "%s \t%s \t%s \t%s\n", "ID", "Room name", "Max", "curPlayer");
        fprintf(f, "%d \t%s \t%d \t\t%d\n", room->id, room->name, room->maxUser, room->curUser);
        fprintf(f, "%s \t%s \n", "Name", "Card in hand");
        for (int i = 0; i < room->curUser; i++)
        {
            fprintf(f, "%s \t\t%d \n", room->users[i]->username, room->players[i].cardSize);
        }
        fprintf(f, "------------------------\n");
    }
    fclose(f);
}
#endif // ROOM_H_
