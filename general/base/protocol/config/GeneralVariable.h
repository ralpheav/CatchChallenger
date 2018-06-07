#ifndef CATCHCHALLENGER_GENERALVARIABLE_H
#define CATCHCHALLENGER_GENERALVARIABLE_H

    #define CATCHCHALLENGER_SHA224HASH_SIZE             28

    #ifndef CATCHCHALLENGER_RELEASE
        #define CATCHCHALLENGER_EXTRA_CHECK
    #endif

    #define CATCHCHALLENGER_TOKENSIZE_CONNECTGAMESERVER 32
    #define CATCHCHALLENGER_MONSTER_LEVEL_MAX           100
    #define CATCHCHALLENGER_MONSTER_WILD_SKILL_NUMBER   5
    #define CATCHCHALLENGER_MIN_RANDOM_TO_FIGHT         7
    #define CATCHCHALLENGER_MIN_PACKET_SIZE             128

    #ifdef CATCHCHALLENGER_SERVER_DATAPACK_ONLYBYMIRROR
        #define CATCHCHALLENGER_MAX_PACKET_SIZE  256 * 1024
        #define CATCHCHALLENGER_MAX_FILE_SIZE    16 * 1024
    #else
        #define CATCHCHALLENGER_MAX_PACKET_SIZE  1024 * 1024
        #define CATCHCHALLENGER_MAX_FILE_SIZE    8 * 1024 * 1024
    #endif

    //Here to ProtocolParsingGeneral
    #define TOKEN_SIZE_FOR_MASTERAUTH              32
    #define TOKEN_SIZE_FOR_CLIENT_AUTH_AT_CONNECT  16
    #define CATCHCHALLENGER_MAXPROTOCOLQUERY       16
    #define RANDOM_FLOAT_PART_DIVIDER              10000

    #define CATCHCHALLENGER_EXTENSION_ALLOWED      "tmx;xml;tsx;js;png;jpg;gif;ogg;opus"
    #define CATCHCHALLENGER_CHECK_MAINDATAPACKCODE "^[a-z0-9]+$" //can't be "-" because the separator is used for http download
    #define CATCHCHALLENGER_CHECK_SUBDATAPACKCODE  "^[a-z0-9]+$" //can't be "-" because the separator is used for http download
    #define DATAPACK_FILE_REGEX                    "^[0-9a-z_./-]*[0-9a-z]\\.[a-z]{2,4}$" //Not work: "^[-0-9/a-z\\._]*[0-9a-z]\\.[a-z]{2,4}$", Partial work: "^[-0-9a-z\\._/]*[0-9a-z]\\.[a-z]{2,4}$", Work: "^[0-9a-z/.-]*[0-9a-z]\\.[a-z]{2,4}$"
    #define DATAPACK_BASE_PATH_MAPBASE             "map/"
    #define DATAPACK_BASE_PATH_MAPMAIN             "map/main/"
    #define DATAPACK_BASE_PATH_MAPSUB1             "map/main/"
    #define DATAPACK_BASE_PATH_MAPSUB2             "/sub/"
    #define DATAPACK_BASE_PATH_ZONE1               "map/main/"
    #define DATAPACK_BASE_PATH_ZONE2               "/zone/"
    #define DATAPACK_BASE_PATH_SKIN                "skin/fighter/"
    #define DATAPACK_BASE_PATH_SKINBASE            "skin/"
    #define DATAPACK_BASE_PATH_ANIMATION           "animation/"
    #define DATAPACK_BASE_PATH_ITEM                "items/"
    #define DATAPACK_BASE_PATH_INDUSTRIESBASE      "industries/"
    #define DATAPACK_BASE_PATH_INDUSTRIESSPEC1     "map/main/"
    #define DATAPACK_BASE_PATH_INDUSTRIESSPEC2     "/industries/"
    #define DATAPACK_BASE_PATH_PLANTS              "plants/"
    #define DATAPACK_BASE_PATH_MONSTERS            "monsters/"
    #define DATAPACK_BASE_PATH_BUFF                "monsters/buff/"
    #define DATAPACK_BASE_PATH_BUFFICON            "monsters/buff/"
    #define DATAPACK_BASE_PATH_SKILL               "monsters/skill/"
    #define DATAPACK_BASE_PATH_SKILLANIMATION      "monsters/skill-animation/"
    #define DATAPACK_BASE_PATH_FIGHT1              "map/main/"
    #define DATAPACK_BASE_PATH_FIGHT2              "/fight/"
    #define DATAPACK_BASE_PATH_PLAYERBASE          "player/"
    #define DATAPACK_BASE_PATH_PLAYERSPEC          "map/main/"
    #define DATAPACK_BASE_PATH_QUESTS1             "map/main/"
    #define DATAPACK_BASE_PATH_QUESTS2             "/quests/"
    #define DATAPACK_BASE_PATH_CRAFTING            "crafting/"
    #define DATAPACK_BASE_PATH_SHOP1               "map/main/"
    #define DATAPACK_BASE_PATH_SHOP2               "/shop/"

    #if defined(__linux__) || defined(__CYGWIN__)
        #include <byteswap.h>
        #include <endian.h>

        #ifdef __BYTE_ORDER
            #if __BYTE_ORDER == __BIG_ENDIAN //if both are not defined it is TRUE!
                #define be16toh(a) a
                #define be32toh(a) a
                #define be64toh(a) a
                #define htobe16(a) a
                #define htobe32(a) a
                #define htobe64(a) a
            #elif __BYTE_ORDER == __LITTLE_ENDIAN
            #elif __BYTE_ORDER == __PDP_ENDIAN
            #else
                #error "Endian determination failed"
            #endif
        #else
            #error "Endian determination failed"
        #endif
    #else
        #include "PortableEndian.h"
    #endif

    //server cluster
    #define CATCHCHALLENGER_SERVER_MINIDBLOCK     20
    #define CATCHCHALLENGER_SERVER_MAXIDBLOCK     50
    #define CATCHCHALLENGER_SERVER_MINCLANIDBLOCK 1
    #define CATCHCHALLENGER_SERVER_MAXCLANIDBLOCK 5

#endif // GENERALVARIABLE_H
